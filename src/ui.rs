//! Editor: a device-strip layout in the spirit of Ableton Live 12, with the
//! original antiorp look as an alternative theme.
//!
//! ┌ transport · score · seed · length · render ──────────────────────────────┐
//! │ SAMPLE (waveform + live grains) │ SCORE lanes + playhead │ controls/meters │
//! └ status line · render progress ───────────────────────────────────────────┘

use crate::aiff::{AudioBufferData, ExportFormat};
use crate::engine::{offline_render_to_file, RenderError, RenderOptions};
use crate::params::{ThonkParams, ThonkParamsParamId as P, GRAIN_TRAIL};
use crate::score::{Lanes, Length, LengthMode, ScoreState, ScoreType};
use std::sync::atomic::Ordering;
use std::sync::{Arc, Mutex};
use truce::prelude::*;
use truce_core::editor::{PluginContext, PluginContextReadF32};
use truce_egui::widgets::{level_meter, param_knob};

pub const WINDOW_W: u32 = 1000;
pub const WINDOW_H: u32 = 420;
pub const MIN_W: u32 = 880;
pub const MIN_H: u32 = 380;
pub const MAX_W: u32 = 1900;
pub const MAX_H: u32 = 1100;

type Ctx = PluginContext<ThonkParams>;

// --- Themes ------------------------------------------------------------------

#[derive(Clone, Copy)]
struct Palette {
    bg: egui::Color32,
    panel: egui::Color32,
    header: egui::Color32,
    lane: egui::Color32,
    grid: egui::Color32,
    text: egui::Color32,
    dim: egui::Color32,
    accent: egui::Color32,
    accent_text: egui::Color32,
    curve: egui::Color32,
    voices: [egui::Color32; 4],
}

const MODERN: Palette = Palette {
    bg: egui::Color32::from_rgb(30, 30, 30),
    panel: egui::Color32::from_rgb(43, 43, 43),
    header: egui::Color32::from_rgb(54, 54, 54),
    lane: egui::Color32::from_rgb(36, 36, 36),
    grid: egui::Color32::from_rgb(52, 52, 52),
    text: egui::Color32::from_rgb(210, 210, 210),
    dim: egui::Color32::from_rgb(130, 130, 130),
    accent: egui::Color32::from_rgb(255, 150, 40),
    accent_text: egui::Color32::from_rgb(20, 20, 20),
    curve: egui::Color32::from_rgb(190, 190, 190),
    voices: [
        egui::Color32::from_rgb(255, 150, 40),
        egui::Color32::from_rgb(90, 180, 255),
        egui::Color32::from_rgb(150, 220, 90),
        egui::Color32::from_rgb(235, 110, 180),
    ],
};

const CLASSIC: Palette = Palette {
    bg: egui::Color32::from_rgb(10, 12, 11),
    panel: egui::Color32::from_rgb(20, 24, 22),
    header: egui::Color32::from_rgb(28, 36, 31),
    lane: egui::Color32::from_rgb(6, 8, 7),
    grid: egui::Color32::from_rgb(30, 45, 35),
    text: egui::Color32::from_rgb(0, 255, 102),
    dim: egui::Color32::from_rgb(0, 150, 70),
    accent: egui::Color32::from_rgb(0, 255, 102),
    accent_text: egui::Color32::BLACK,
    curve: egui::Color32::from_rgb(0, 210, 90),
    voices: [
        egui::Color32::from_rgb(0, 255, 102),
        egui::Color32::from_rgb(180, 255, 200),
        egui::Color32::from_rgb(0, 170, 255),
        egui::Color32::from_rgb(255, 60, 60),
    ],
};

fn visuals(p: &Palette) -> egui::Visuals {
    let mut v = egui::Visuals::dark();
    v.panel_fill = p.bg;
    v.window_fill = p.panel;
    v.extreme_bg_color = p.lane;
    v.faint_bg_color = p.panel;
    v.override_text_color = Some(p.text);
    v.selection.bg_fill = p.accent;
    v.selection.stroke = egui::Stroke::new(1.0_f32, p.accent_text);
    for (w, fill) in [
        (&mut v.widgets.noninteractive, p.panel),
        (&mut v.widgets.inactive, p.header),
        (&mut v.widgets.hovered, p.grid),
        (&mut v.widgets.active, p.accent),
        (&mut v.widgets.open, p.header),
    ] {
        w.bg_fill = fill;
        w.weak_bg_fill = fill;
        w.bg_stroke = egui::Stroke::NONE;
        w.corner_radius = egui::CornerRadius::same(3);
    }
    v.widgets.noninteractive.fg_stroke = egui::Stroke::new(1.0_f32, p.dim);
    v.widgets.inactive.fg_stroke = egui::Stroke::new(1.0_f32, p.text);
    v.widgets.hovered.fg_stroke = egui::Stroke::new(1.0_f32, p.text);
    // Also egui's "strong" text colour; lit buttons set their own text colour.
    v.widgets.active.fg_stroke = egui::Stroke::new(1.5_f32, p.text);
    v
}

fn alpha(c: egui::Color32, a: f32) -> egui::Color32 {
    egui::Color32::from_rgba_unmultiplied(c.r(), c.g(), c.b(), (a.clamp(0.0, 1.0) * 255.0) as u8)
}

// --- Cached drawing data -----------------------------------------------------

#[derive(Default)]
struct Cache {
    wave_key: (usize, usize),
    wave: Vec<(f32, f32)>,
    lanes_key: (u32, u32, u32, u32, u32, usize),
    lanes: Option<Lanes>,
}

#[derive(Clone, Default)]
struct SharedCache(Arc<Mutex<Cache>>);

fn cache(ui: &egui::Ui) -> SharedCache {
    let id = egui::Id::new("thonk-ui-cache");
    ui.ctx()
        .data_mut(|d| d.get_temp_mut_or_default::<SharedCache>(id).clone())
}

// --- Helpers -----------------------------------------------------------------

pub fn fmt_time(secs: f64) -> String {
    let s = secs.max(0.0).round() as u64;
    if s >= 3600 {
        format!("{}:{:02}:{:02}", s / 3600, (s / 60) % 60, s % 60)
    } else {
        format!("{}:{:02}", s / 60, s % 60)
    }
}

fn normalized(ctx: &Ctx, id: P, plain: f64) -> f64 {
    let id: u32 = id.into();
    ctx.params()
        .param_infos()
        .iter()
        .find(|i| i.id == id)
        .map(|i| i.range.normalize(plain))
        .unwrap_or(0.0)
}

fn bump(counter: &std::sync::atomic::AtomicU32) {
    counter.fetch_add(1, Ordering::Relaxed);
}

fn section_label(ui: &mut egui::Ui, p: &Palette, text: &str) {
    ui.label(egui::RichText::new(text).size(10.0).color(p.dim).strong());
}

/// Small Live-style button: lit with the accent colour when `on`.
fn pill(ui: &mut egui::Ui, p: &Palette, text: &str, on: bool) -> egui::Response {
    let (fill, fg) = if on { (p.accent, p.accent_text) } else { (p.header, p.text) };
    ui.add(
        egui::Button::new(egui::RichText::new(text).size(11.0).color(fg))
            .fill(fill)
            .min_size(egui::vec2(0.0, 22.0)),
    )
}

/// A boolean parameter shown as a pill.
fn param_pill(ui: &mut egui::Ui, ctx: &Ctx, p: &Palette, id: P, text: &str) -> egui::Response {
    let on = ctx.get_param(id) > 0.5;
    let r = pill(ui, p, text, on);
    if r.clicked() {
        ctx.automate(id, if on { 0.0 } else { 1.0 });
    }
    r
}

/// Compact combo box bound to an enum parameter.
fn enum_combo<E: ParamEnum>(ui: &mut egui::Ui, ctx: &Ctx, id: P, current: usize, width: f32) {
    let names = E::variant_names();
    egui::ComboBox::from_id_salt(("thonk-enum", u32::from(id)))
        .width(width)
        .selected_text(egui::RichText::new(names[current.min(names.len() - 1)]).size(11.5))
        .show_ui(ui, |ui| {
            for (i, name) in names.iter().enumerate() {
                if ui.selectable_label(i == current, *name).clicked() && i != current {
                    ctx.automate(id, normalized(ctx, id, i as f64));
                }
            }
        });
}

/// Play / stop icon button.
fn transport_button(ui: &mut egui::Ui, p: &Palette, playing: bool) -> egui::Response {
    let (rect, resp) = ui.allocate_exact_size(egui::vec2(30.0, 22.0), egui::Sense::click());
    let fill = if playing { p.accent } else if resp.hovered() { p.grid } else { p.header };
    let fg = if playing { p.accent_text } else { p.text };
    let painter = ui.painter_at(rect);
    painter.rect_filled(rect, 3.0, fill);
    let c = rect.center();
    if playing {
        painter.rect_filled(egui::Rect::from_center_size(c, egui::vec2(9.0, 9.0)), 1.0, fg);
    } else {
        painter.add(egui::Shape::convex_polygon(
            vec![
                c + egui::vec2(-4.0, -6.0),
                c + egui::vec2(6.0, 0.0),
                c + egui::vec2(-4.0, 6.0),
            ],
            fg,
            egui::Stroke::NONE,
        ));
    }
    resp.on_hover_text(if playing { "Stop (rewind)" } else { "Play" })
}

// --- Editor ------------------------------------------------------------------

pub fn thonk_ui(ui: &mut egui::Ui, ctx: &Ctx) {
    let params: &Arc<ThonkParams> = ctx.params();
    let classic = params.theme.read().map(|t| *t == 1).unwrap_or(false);
    let p = if classic { CLASSIC } else { MODERN };
    let v = visuals(&p);
    // `set_visuals` applies from the next frame; style the current one too.
    ui.ctx().set_visuals(v.clone());
    ui.style_mut().visuals = v;

    // Free a source the audio thread swapped out.
    if let Ok(mut retired) = params.retired.try_write() {
        retired.take();
    }

    egui::Panel::top("thonk-top")
        .exact_size(40.0)
        .frame(egui::Frame::NONE.fill(p.panel).inner_margin(egui::Margin::symmetric(10, 8)))
        .show_inside(ui, |ui| top_bar(ui, ctx, &p, classic));

    egui::Panel::bottom("thonk-status")
        .exact_size(26.0)
        .frame(egui::Frame::NONE.fill(p.panel).inner_margin(egui::Margin::symmetric(10, 4)))
        .show_inside(ui, |ui| status_bar(ui, ctx, &p));

    egui::Panel::left("thonk-sample")
        .exact_size(260.0)
        .resizable(false)
        .frame(egui::Frame::NONE.fill(p.bg).inner_margin(egui::Margin::same(10)))
        .show_inside(ui, |ui| sample_panel(ui, ctx, &p));

    egui::Panel::right("thonk-controls")
        .exact_size(214.0)
        .resizable(false)
        .frame(egui::Frame::NONE.fill(p.bg).inner_margin(egui::Margin::same(10)))
        .show_inside(ui, |ui| controls_panel(ui, ctx, &p));

    egui::CentralPanel::default()
        .frame(egui::Frame::NONE.fill(p.bg).inner_margin(egui::Margin::symmetric(4, 10)))
        .show_inside(ui, |ui| score_panel(ui, ctx, &p));

    ui.ctx().request_repaint_after(std::time::Duration::from_millis(33));
}

fn top_bar(ui: &mut egui::Ui, ctx: &Ctx, p: &Palette, classic: bool) {
    let params = ctx.params();
    let st = &params.status;
    let playing = st.playing.load(Ordering::Relaxed);
    let synced = st.synced.load(Ordering::Relaxed);

    ui.horizontal_centered(|ui| {
        ui.spacing_mut().item_spacing = egui::vec2(6.0, 0.0);
        ui.label(egui::RichText::new("th0nk").size(15.0).strong().color(p.accent));
        ui.add_space(6.0);

        let tb = transport_button(ui, p, playing);
        if tb.clicked() && !synced {
            if playing {
                bump(&params.stop_counter);
            } else {
                bump(&params.play_counter);
            }
        }
        if pill(ui, p, "THONK", false)
            .on_hover_text("New composition (new seed unless locked) and play")
            .clicked()
        {
            bump(&params.trigger_counter);
        }

        ui.separator();
        enum_combo::<ScoreType>(ui, ctx, P::Score, params.score.value().to_index(), 190.0);

        ui.separator();
        let seed = st.seed.load(Ordering::Relaxed);
        let locked = params.seed.value() != 0;
        ui.label(egui::RichText::new("Seed").size(11.0).color(p.dim));
        ui.label(egui::RichText::new(format!("{seed}")).size(11.0).monospace());
        if pill(ui, p, if locked { "Locked" } else { "Lock" }, locked)
            .on_hover_text("Keep this composition (recalled with the project)")
            .clicked()
        {
            let target = if locked { 0.0 } else { normalized(ctx, P::Seed, seed as f64) };
            ctx.automate(P::Seed, target);
        }

        ui.separator();
        ui.label(egui::RichText::new("Length").size(11.0).color(p.dim));
        enum_combo::<LengthMode>(ui, ctx, P::LengthMode, params.length_mode.value().to_index(), 78.0);
        let mode = params.length_mode.value();
        let mut secs = params.length.value() as f64;
        let native = st.total_secs();
        if mode == LengthMode::Original {
            ui.label(
                egui::RichText::new(fmt_time(native))
                    .size(11.0)
                    .monospace()
                    .color(p.dim),
            )
            .on_hover_text("The score's own length");
        } else {
            let speed = secs * 0.01;
            let resp = ui.add(
                egui::DragValue::new(&mut secs)
                    .range(10.0..=3600.0)
                    .speed(speed)
                    .custom_formatter(|v, _| fmt_time(v))
                    .custom_parser(parse_time),
            );
            if resp.changed() {
                ctx.automate(P::Length, normalized(ctx, P::Length, secs));
            }
            resp.on_hover_text("Composition length (drag, or type m:ss)");
        }

        ui.with_layout(egui::Layout::right_to_left(egui::Align::Center), |ui| {
            if pill(ui, p, if classic { "Classic" } else { "Modern" }, false)
                .on_hover_text("Switch theme")
                .clicked()
            {
                if let Ok(mut t) = params.theme.write() {
                    *t = if classic { 0 } else { 1 };
                }
            }
            render_controls(ui, ctx, p);
        });
    });
}

fn parse_time(s: &str) -> Option<f64> {
    let parts: Vec<&str> = s.trim().split(':').collect();
    let mut secs = 0.0;
    for part in &parts {
        secs = secs * 60.0 + part.trim().parse::<f64>().ok()?;
    }
    Some(secs)
}

fn render_controls(ui: &mut egui::Ui, ctx: &Ctx, p: &Palette) {
    let params = ctx.params();
    let active = params.render.active.load(Ordering::Relaxed);

    ui.menu_button(egui::RichText::new("Format…").size(11.0), |ui| {
        let mut fmt = params.export_format.read().map(|f| *f).unwrap_or(0);
        for f in [ExportFormat::Int16, ExportFormat::Int24, ExportFormat::Float32] {
            ui.radio_value(&mut fmt, f.to_u8(), f.label());
        }
        if let Ok(mut g) = params.export_format.write() {
            *g = fmt;
        }
        ui.separator();
        let mut norm = params.export_normalize.read().map(|n| *n).unwrap_or(false);
        ui.checkbox(&mut norm, "Normalize to -0.3 dBFS");
        if let Ok(mut g) = params.export_normalize.write() {
            *g = norm;
        }
        ui.label(
            egui::RichText::new("16-bit at 0 dB without normalizing\nis bit-exact to the original.")
                .size(10.0)
                .color(p.dim),
        );
    });

    let label = if active { "Rendering…" } else { "Render" };
    if ui
        .add_enabled(!active, egui::Button::new(egui::RichText::new(label).size(11.0)))
        .on_hover_text("Render the composition to a WAV / AIFF file")
        .clicked()
    {
        start_render(params.clone());
    }
}

fn start_render(params: Arc<ThonkParams>) {
    let Some(source) = params.audio.get() else {
        set_message(&params, "Load a sound first.");
        return;
    };
    let score = params.score.value();
    let seed = match params.seed.value() {
        0 => None,
        v => Some(v as u32),
    };
    let opts = RenderOptions {
        score,
        seed,
        wrapping: params.wrapping_distortion.value(),
        density: params.density.value(),
        gain_db: params.gain.value(),
        length: Length {
            mode: params.length_mode.value(),
            secs: params.length.value() as f64,
        },
        skip_silence: params.skip_silence.value(),
        format: ExportFormat::from_u8(params.export_format.read().map(|f| *f).unwrap_or(0)),
        normalize: params.export_normalize.read().map(|n| *n).unwrap_or(false),
    };
    let default_name = format!(
        "thonk-{}.wav",
        score.name().to_lowercase().replace(|c: char| !c.is_alphanumeric(), "-")
    );
    params.render.active.store(true, Ordering::Relaxed);
    std::thread::spawn(move || {
        let path = rfd::FileDialog::new()
            .add_filter("WAV", &["wav"])
            .add_filter("AIFF", &["aif", "aiff"])
            .set_file_name(&default_name)
            .set_title("Render thOnk composition")
            .save_file();
        let Some(path) = path else {
            params.render.active.store(false, Ordering::Relaxed);
            return;
        };
        params.render.cancel.store(false, Ordering::Relaxed);
        params.render.progress.store(0, Ordering::Relaxed);
        let p2 = params.clone();
        let res = offline_render_to_file(source, &opts, &path, &params.render.cancel, |prog, msg| {
            p2.render.progress.store((prog * 10000.0) as u32, Ordering::Relaxed);
            set_message(&p2, msg);
        });
        let file = path.file_name().unwrap_or_default().to_string_lossy().into_owned();
        match res {
            Ok(seed) => set_message(&params, &format!("Rendered {file} (seed {seed})")),
            Err(RenderError::Cancelled) => set_message(&params, "Render cancelled"),
            Err(e) => set_message(&params, &format!("Render failed: {e}")),
        }
        params.render.active.store(false, Ordering::Relaxed);
    });
}

fn set_message(params: &ThonkParams, msg: &str) {
    if let Ok(mut m) = params.render.message.write() {
        *m = Some(msg.to_string());
    }
}

fn status_bar(ui: &mut egui::Ui, ctx: &Ctx, p: &Palette) {
    let params = ctx.params();
    let st = &params.status;
    ui.horizontal_centered(|ui| {
        ui.spacing_mut().item_spacing = egui::vec2(10.0, 0.0);
        let playing = st.playing.load(Ordering::Relaxed);
        let synced = st.synced.load(Ordering::Relaxed);
        let (dot, text) = match (playing, synced) {
            (true, true) => (p.accent, "SYNCED TO HOST"),
            (true, false) => (p.accent, "PLAYING"),
            _ => (p.dim, "STOPPED"),
        };
        let (r, _) = ui.allocate_exact_size(egui::vec2(8.0, 8.0), egui::Sense::hover());
        ui.painter().circle_filled(r.center(), 4.0, dot);
        ui.label(egui::RichText::new(text).size(10.5).strong());
        let score = ScoreType::from_index(st.score.load(Ordering::Relaxed) as usize);
        ui.label(egui::RichText::new(score.name()).size(10.5).color(p.dim));
        ui.label(
            egui::RichText::new(format!(
                "{} / {}",
                fmt_time(st.position_secs()),
                fmt_time(st.total_secs())
            ))
            .size(10.5)
            .monospace(),
        );

        ui.with_layout(egui::Layout::right_to_left(egui::Align::Center), |ui| {
            let active = params.render.active.load(Ordering::Relaxed);
            if active {
                if pill(ui, p, "Cancel", false).clicked() {
                    params.render.cancel.store(true, Ordering::Relaxed);
                }
                let prog = params.render.progress.load(Ordering::Relaxed) as f32 / 10000.0;
                ui.add(
                    egui::ProgressBar::new(prog)
                        .desired_width(180.0)
                        .desired_height(12.0)
                        .fill(p.accent),
                );
            }
            if let Ok(m) = params.render.message.read() {
                if let Some(msg) = m.as_ref() {
                    ui.label(egui::RichText::new(msg).size(10.5).color(p.dim));
                }
            }
        });
    });
}

fn sample_panel(ui: &mut egui::Ui, ctx: &Ctx, p: &Palette) {
    let params = ctx.params();
    let source = params.audio.get();
    let live = params.live_input.value();

    section_label(ui, p, "SAMPLE");
    match (&source, live) {
        (_, true) => {
            ui.label(egui::RichText::new("Live input · last 5 s").size(12.0).strong());
        }
        (Some(s), false) => {
            ui.label(egui::RichText::new(&s.name).size(12.0).strong())
                .on_hover_text(&s.path);
            ui.label(
                egui::RichText::new(format!(
                    "{:.1} kHz · {}",
                    s.sample_rate / 1000.0,
                    fmt_time(s.duration_secs())
                ))
                .size(10.5)
                .color(p.dim),
            );
        }
        (None, false) => {
            ui.label(egui::RichText::new("No sound loaded").size(12.0).color(p.dim));
        }
    }
    ui.add_space(6.0);

    let h = (ui.available_height() - 40.0).max(60.0);
    let (rect, _) = ui.allocate_exact_size(egui::vec2(ui.available_width(), h), egui::Sense::hover());
    let painter = ui.painter_at(rect);
    painter.rect_filled(rect, 4.0, p.lane);
    painter.line_segment(
        [rect.left_center(), rect.right_center()],
        egui::Stroke::new(1.0_f32, p.grid),
    );

    if let (Some(s), false) = (&source, live) {
        draw_waveform(ui, &painter, rect, s, p);
    } else if source.is_none() && !live {
        painter.text(
            rect.center(),
            egui::Align2::CENTER_CENTER,
            "Open a WAV / AIFF file\nor try the demo sound",
            egui::FontId::proportional(12.0),
            p.dim,
        );
    }

    // Grains being read right now (x = source position, y = balance L..R).
    if params.status.playing.load(Ordering::Relaxed) && (source.is_some() || live) {
        for i in 0..GRAIN_TRAIL {
            let v = params.status.grains[i].load(Ordering::Relaxed);
            if v == 0 {
                continue;
            }
            let x = (v >> 16) as f32 / 65535.0;
            let bal = ((v >> 8) & 0xff) as f32 / 255.0;
            let age = i as f32 / GRAIN_TRAIL as f32; // oldest first
            let pos = egui::pos2(
                rect.left() + x * rect.width(),
                rect.top() + 6.0 + bal * (rect.height() - 12.0),
            );
            painter.circle_filled(pos, 1.5 + 2.0 * age, alpha(p.accent, 0.15 + 0.85 * age));
        }
    }

    ui.add_space(6.0);
    ui.horizontal(|ui| {
        ui.spacing_mut().item_spacing = egui::vec2(6.0, 0.0);
        if pill(ui, p, "Open…", false).clicked() {
            let params = params.clone();
            std::thread::spawn(move || {
                if let Some(path) = rfd::FileDialog::new()
                    .add_filter("Audio", &["wav", "aif", "aiff", "aifc"])
                    .set_title("Open a sound for thOnk")
                    .pick_file()
                {
                    match AudioBufferData::load_file(&path) {
                        Ok(buf) => params.audio.set(Some(Arc::new(buf))),
                        Err(e) => set_message(&params, &format!("Could not load: {e}")),
                    }
                }
            });
        }
        if pill(ui, p, "Demo", false)
            .on_hover_text("Built-in 2.5 s metallic burst")
            .clicked()
        {
            params.audio.set(Some(Arc::new(AudioBufferData::demo())));
        }
        param_pill(ui, ctx, p, P::LiveInput, "Live in")
            .on_hover_text("Granulate the last 5 s of the track / microphone input");
    });
}

fn draw_waveform(ui: &egui::Ui, painter: &egui::Painter, rect: egui::Rect, s: &AudioBufferData, p: &Palette) {
    let cols = rect.width().max(1.0) as usize;
    let shared = cache(ui);
    let mut c = shared.0.lock().unwrap_or_else(|e| e.into_inner());
    let key = (s.pcm16.as_ptr() as usize ^ s.pcm16.len(), cols);
    if c.wave_key != key {
        c.wave_key = key;
        c.wave.clear();
        let n = s.pcm16.len().max(1);
        for i in 0..cols {
            let a = i * n / cols;
            let b = ((i + 1) * n / cols).max(a + 1).min(n);
            let (mut lo, mut hi) = (0i16, 0i16);
            for &x in &s.pcm16[a.min(n - 1)..b] {
                lo = lo.min(x);
                hi = hi.max(x);
            }
            c.wave.push((lo as f32 / 32768.0, hi as f32 / 32768.0));
        }
    }
    let mid = rect.center().y;
    let half = rect.height() * 0.45;
    for (i, &(lo, hi)) in c.wave.iter().enumerate() {
        let x = rect.left() + i as f32 + 0.5;
        painter.line_segment(
            [egui::pos2(x, mid - hi * half), egui::pos2(x, mid - lo * half)],
            egui::Stroke::new(1.0_f32, alpha(p.curve, 0.55)),
        );
    }
}

fn controls_panel(ui: &mut egui::Ui, ctx: &Ctx, p: &Palette) {
    let params = ctx.params();
    section_label(ui, p, "GRAINS");
    ui.horizontal(|ui| {
        ui.spacing_mut().item_spacing = egui::vec2(10.0, 0.0);
        param_knob(ui, ctx, P::Density, "Density");
        param_knob(ui, ctx, P::Gain, "Gain");
    });
    ui.add_space(4.0);
    ui.horizontal_wrapped(|ui| {
        ui.spacing_mut().item_spacing = egui::vec2(6.0, 6.0);
        param_pill(ui, ctx, p, P::WrappingDistortion, "16-bit wrap")
            .on_hover_text("The original's wrap-around on overload. Off: clean float + soft clip");
        param_pill(ui, ctx, p, P::SkipSilence, "Skip silences")
            .on_hover_text("Not in the original: stops density valleys from becoming long silences");
        param_pill(ui, ctx, p, P::HostSync, "Host sync")
            .on_hover_text("Play with the DAW transport, from the project position");
    });

    ui.add_space(8.0);
    section_label(ui, p, "OUTPUT");
    ui.horizontal(|ui| {
        level_meter(ui, ctx, &[P::MeterLeft, P::MeterRight], 70.0);
        ui.add_space(10.0);
        ui.vertical(|ui| {
            let layers = params.status.layers.load(Ordering::Relaxed);
            ui.label(egui::RichText::new("Layers").size(10.0).color(p.dim));
            ui.label(egui::RichText::new(format!("{layers}")).size(18.0).strong());
            let frac = (layers as f32 / 600.0).clamp(0.0, 1.0);
            ui.add(
                egui::ProgressBar::new(frac)
                    .desired_width(90.0)
                    .desired_height(6.0)
                    .fill(p.accent),
            )
            .on_hover_text("Overlapping grains (the manual's maximum is 600)");
        });
    });
}

fn score_panel(ui: &mut egui::Ui, ctx: &Ctx, p: &Palette) {
    let params = ctx.params();
    let st = &params.status;
    let rect = ui.available_rect_before_wrap();
    let resp = ui.allocate_rect(rect, egui::Sense::click_and_drag());
    let painter = ui.painter_at(rect);

    let total = st.total_secs().max(1e-3);
    let ruler_h = 16.0;
    let label_w = 52.0;
    let area = egui::Rect::from_min_max(
        egui::pos2(rect.left() + label_w, rect.top() + ruler_h),
        rect.max,
    );

    // Lanes, cached per composition and width.
    let cols = (area.width().max(1.0) as usize).min(1600);
    let score_sel = params.score.value();
    let seed = st.seed.load(Ordering::Relaxed);
    let src_ms = st.source_ms.load(Ordering::Relaxed);
    let length = Length {
        mode: params.length_mode.value(),
        secs: params.length.value() as f64,
    };
    let key = (
        score_sel.to_index() as u32,
        seed,
        src_ms,
        length.mode.to_index() as u32,
        (length.secs * 10.0) as u32,
        cols,
    );
    let shared = cache(ui);
    let mut c = shared.0.lock().unwrap_or_else(|e| e.into_inner());
    if c.lanes_key != key || c.lanes.is_none() {
        c.lanes_key = key;
        let src_secs = (src_ms as f64 / 1000.0).max(0.1);
        let sr = params.audio.get().map(|a| a.sample_rate).unwrap_or(44100.0);
        let score = ScoreState::generate_with(score_sel, src_secs, seed, length);
        c.lanes = Some(score.lanes(cols, sr));
    }
    let Some(lanes) = c.lanes.as_ref() else { return };

    painter.rect_filled(rect, 4.0, p.panel);

    // Ruler.
    let step = nice_step(total, area.width());
    let mut t = 0.0;
    while t <= total + 1e-6 {
        let x = area.left() + (t / total) as f32 * area.width();
        painter.line_segment(
            [egui::pos2(x, rect.top() + ruler_h - 5.0), egui::pos2(x, rect.bottom())],
            egui::Stroke::new(1.0_f32, p.grid),
        );
        painter.text(
            egui::pos2(x + 3.0, rect.top() + 2.0),
            egui::Align2::LEFT_TOP,
            fmt_time(t),
            egui::FontId::monospace(9.5),
            p.dim,
        );
        t += step;
    }

    let names = ["Density", "Pitch", "Pan", "Source"];
    let gap = 4.0;
    let lane_h = (area.height() - gap * 3.0) / 4.0;
    let lane_rect = |i: usize| {
        egui::Rect::from_min_size(
            egui::pos2(area.left(), area.top() + i as f32 * (lane_h + gap)),
            egui::vec2(area.width(), lane_h),
        )
    };
    for (i, name) in names.iter().enumerate() {
        let r = lane_rect(i);
        painter.rect_filled(r, 2.0, alpha(p.lane, 0.9));
        painter.text(
            egui::pos2(rect.left() + 6.0, r.top() + 2.0),
            egui::Align2::LEFT_TOP,
            *name,
            egui::FontId::proportional(10.5),
            p.dim,
        );
    }
    let x_of = |i: usize| area.left() + i as f32 + 0.5;

    // Density: heat strip of estimated layers + log(grains/s) curve.
    let r = lane_rect(0);
    for (i, &layers) in lanes.layers.iter().enumerate() {
        let heat = (layers / 300.0).clamp(0.0, 1.0).sqrt();
        painter.line_segment(
            [egui::pos2(x_of(i), r.bottom() - 6.0), egui::pos2(x_of(i), r.bottom())],
            egui::Stroke::new(1.0_f32, alpha(p.accent, heat)),
        );
    }
    let gf_pts: Vec<egui::Pos2> = lanes
        .gf
        .iter()
        .enumerate()
        .map(|(i, &g)| {
            let v = ((g.max(1.0)).ln() / 6000f32.ln()).clamp(0.0, 1.0);
            egui::pos2(x_of(i), r.bottom() - 8.0 - v * (r.height() - 14.0))
        })
        .collect();
    painter.add(egui::Shape::line(gf_pts, egui::Stroke::new(1.2_f32, p.curve)));

    // Pitch: four transposer voices, opacity = probability (-60 .. +30 st).
    let r = lane_rect(1);
    let y_of = |st: f32| r.bottom() - 3.0 - ((st + 60.0) / 90.0).clamp(0.0, 1.0) * (r.height() - 6.0);
    painter.line_segment(
        [egui::pos2(r.left(), y_of(0.0)), egui::pos2(r.right(), y_of(0.0))],
        egui::Stroke::new(1.0_f32, p.grid),
    );
    for (i, v) in lanes.voices.iter().enumerate() {
        for (k, &(pitch, prob)) in v.iter().enumerate() {
            if prob > 0.01 {
                painter.circle_filled(
                    egui::pos2(x_of(i), y_of(pitch)),
                    1.2,
                    alpha(p.voices[k], 0.2 + 0.8 * prob),
                );
            }
        }
    }

    // Pan (L top, R bottom) and source position.
    for (lane, data, scale) in [(2usize, &lanes.balance, 0.5f32), (3, &lanes.source, 1.0)] {
        let r = lane_rect(lane);
        let pts: Vec<egui::Pos2> = data
            .iter()
            .enumerate()
            .map(|(i, &v)| egui::pos2(x_of(i), r.top() + 3.0 + (v * scale) * (r.height() - 6.0)))
            .collect();
        painter.add(egui::Shape::line(pts, egui::Stroke::new(1.2_f32, p.curve)));
    }

    // Playhead, and click / drag to seek.
    let synced = st.synced.load(Ordering::Relaxed);
    let pos = st.position_secs().min(total);
    let x = area.left() + (pos / total) as f32 * area.width();
    painter.line_segment(
        [egui::pos2(x, rect.top()), egui::pos2(x, rect.bottom())],
        egui::Stroke::new(1.5_f32, p.accent),
    );
    painter.add(egui::Shape::convex_polygon(
        vec![
            egui::pos2(x - 5.0, rect.top()),
            egui::pos2(x + 5.0, rect.top()),
            egui::pos2(x, rect.top() + 7.0),
        ],
        p.accent,
        egui::Stroke::NONE,
    ));
    if !synced && (resp.clicked() || resp.dragged()) {
        if let Some(m) = resp.interact_pointer_pos() {
            let frac = ((m.x - area.left()) / area.width()).clamp(0.0, 1.0) as f64;
            params
                .seek_ms
                .store((frac * total * 1000.0) as u32, Ordering::Relaxed);
            bump(&params.seek_counter);
        }
    }
    if resp.hovered() && !synced {
        resp.on_hover_text("Click or drag to move the playhead");
    }
}

fn nice_step(total: f64, width: f32) -> f64 {
    let target = total / (width as f64 / 70.0).max(1.0);
    for s in [1.0, 2.0, 5.0, 10.0, 15.0, 30.0, 60.0, 120.0, 300.0, 600.0, 900.0, 1800.0] {
        if s >= target {
            return s;
        }
    }
    3600.0
}
