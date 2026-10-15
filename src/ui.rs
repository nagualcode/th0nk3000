//! Antiorp / Mac OS 9 Cyberpunk Brutalist Editor for Th0nk.
//! Styled with phosphor green, monochrome contrasts, and classic terminal readouts.

use crate::aiff::AudioBufferData;
use crate::params::{ThonkParams, ThonkParamsParamId as P};
use std::sync::Arc;
use truce::prelude::*;
use truce_core::editor::PluginContext;
use truce_egui::widgets::{level_meter, param_dropdown, param_knob, param_toggle};

pub const WINDOW_W: u32 = 440;
pub const WINDOW_H: u32 = 580;
pub const MIN_W: u32 = 400;
pub const MIN_H: u32 = 520;
pub const MAX_W: u32 = 800;
pub const MAX_H: u32 = 900;

// Cyberpunk Antiorp Color Palette
const BG_COLOR: egui::Color32 = egui::Color32::from_rgb(14, 16, 15);
const PANEL_BG: egui::Color32 = egui::Color32::from_rgb(22, 26, 24);
const BORDER_COLOR: egui::Color32 = egui::Color32::from_rgb(45, 60, 50);
const PHOSPHOR_GREEN: egui::Color32 = egui::Color32::from_rgb(0, 255, 102);
const DIM_GREEN: egui::Color32 = egui::Color32::from_rgb(0, 180, 75);
const TEXT_MUTED: egui::Color32 = egui::Color32::from_rgb(140, 160, 145);
const ACCENT_RED: egui::Color32 = egui::Color32::from_rgb(255, 60, 60);

pub fn thonk_ui(ui: &mut egui::Ui, ctx: &PluginContext<ThonkParams>) {
    let params: &Arc<ThonkParams> = ctx.params();

    // Top Header Banner
    egui::Panel::top("top_banner")
        .exact_size(42.0)
        .frame(egui::Frame::NONE.fill(PANEL_BG))
        .show_inside(ui, |ui| {
            ui.horizontal_centered(|ui| {
                ui.add_space(12.0);
                ui.vertical(|ui| {
                    ui.horizontal(|ui| {
                        ui.label(
                            egui::RichText::new("thOnk_0+2")
                                .size(16.0)
                                .color(PHOSPHOR_GREEN)
                                .strong(),
                        );
                        ui.label(
                            egui::RichText::new("// =cw4t7abs (antiorp) edition")
                                .size(11.0)
                                .color(TEXT_MUTED),
                        );
                    });
                    ui.label(
                        egui::RichText::new("GRANULAR AUDIO COLLIDER • AUDIO EASE 1995 • PUBLIC DOMAIN")
                            .size(9.0)
                            .color(DIM_GREEN),
                    );
                });
            });
        });

    // Central Area
    egui::CentralPanel::default()
        .frame(egui::Frame::central_panel(ui.style()).fill(BG_COLOR).inner_margin(12.0))
        .show_inside(ui, |ui| {
            ui.spacing_mut().item_spacing = egui::vec2(0.0, 8.0);

            // Philosophy quote
            ui.scope(|ui| {
                ui.label(
                    egui::RichText::new(
                        "\"Luck is not the hand of God. Luck is the way the wind swirls, and the dust settles, eons after God has passed by.\" (Kurt Vonnegut)",
                    )
                    .size(9.5)
                    .italics()
                    .color(TEXT_MUTED),
                );
            });

            ui.separator();

            // Brutalist Dual Buttons: Real-time Audition & Render to Disk
            ui.horizontal(|ui| {
                ui.spacing_mut().item_spacing = egui::vec2(6.0, 0.0);

                let thonk_btn = egui::Button::new(
                    egui::RichText::new("⚡ T H O N K (AUDITION)")
                        .size(12.5)
                        .color(egui::Color32::BLACK)
                        .strong(),
                )
                .fill(PHOSPHOR_GREEN)
                .min_size(egui::vec2(ui.available_width() * 0.5 - 3.0, 40.0));

                if ui.add(thonk_btn).clicked() {
                    params.trigger_counter.fetch_add(1, std::sync::atomic::Ordering::Relaxed);
                }

                let render_btn = egui::Button::new(
                    egui::RichText::new("💾 RENDER TO DISK")
                        .size(12.5)
                        .color(egui::Color32::BLACK)
                        .strong(),
                )
                .fill(DIM_GREEN)
                .min_size(egui::vec2(ui.available_width(), 40.0));

                if ui.add(render_btn).clicked() {
                    let params_clone = params.clone();
                    std::thread::spawn(move || {
                        if let Some(path) = rfd::FileDialog::new()
                            .add_filter("Audio File", &["wav", "aiff"])
                            .set_file_name("thonk_rendered.wav")
                            .set_title("Save ThOnk Rendered Sound (16-bit)")
                            .save_file()
                        {
                            let score_type = params_clone.score.value();
                            let wrapping = params_clone.wrapping_distortion.value();
                            let density = params_clone.density.read() / 100.0;
                            let gain = params_clone.gain.read();
                            let source = params_clone
                                .current_audio
                                .read()
                                .ok()
                                .and_then(|g| g.clone())
                                .unwrap_or_else(|| AudioBufferData::default_chime(44100.0));

                            let params_cb = params_clone.clone();
                            let res = crate::engine::offline_render_to_file(
                                source,
                                score_type,
                                wrapping,
                                density,
                                gain,
                                44100.0,
                                &path,
                                move |_prog, msg| {
                                    if let Ok(mut status) = params_cb.render_status.write() {
                                        *status = Some(msg.to_string());
                                    }
                                },
                            );

                            if let Ok(mut status) = params_clone.render_status.write() {
                                match res {
                                    Ok(_) => {
                                        *status = Some(format!(
                                            "COMPLETE: {}",
                                            path.file_name().unwrap_or_default().to_string_lossy()
                                        ));
                                    }
                                    Err(e) => {
                                        *status = Some(format!("ERROR: {e}"));
                                    }
                                }
                            }
                        }
                    });
                }
            });

            // Disk Render Progress Banner (if active)
            if let Ok(status_guard) = params.render_status.read() {
                if let Some(status) = status_guard.as_ref() {
                    ui.group(|ui| {
                        ui.set_width(ui.available_width());
                        ui.horizontal(|ui| {
                            ui.label(
                                egui::RichText::new("DISK RENDER: ")
                                    .size(9.5)
                                    .color(PHOSPHOR_GREEN)
                                    .strong(),
                            );
                            ui.label(
                                egui::RichText::new(status)
                                    .size(9.5)
                                    .color(egui::Color32::WHITE),
                            );
                        });
                    });
                }
            }

            ui.add_space(2.0);

            // File Loading Section
            ui.group(|ui| {
                ui.set_width(ui.available_width());
                ui.spacing_mut().item_spacing = egui::vec2(0.0, 4.0);

                ui.horizontal(|ui| {
                    let load_btn = egui::Button::new(
                        egui::RichText::new("📂  OPEN AUDIO FILE (AIFF / WAV)")
                            .size(11.0)
                            .color(PHOSPHOR_GREEN),
                    )
                    .fill(PANEL_BG)
                    .min_size(egui::vec2(210.0, 26.0));

                    if ui.add(load_btn).clicked() {
                        let params_clone = params.clone();
                        std::thread::spawn(move || {
                            if let Some(path) = rfd::FileDialog::new()
                                .add_filter("Audio File", &["wav", "aif", "aiff"])
                                .set_title("Open Sound for ThOnk Processing")
                                .pick_file()
                            {
                                if let Ok(buf) = AudioBufferData::load_file(&path) {
                                    if let Ok(mut pending) = params_clone.pending_file.write() {
                                        *pending = Some(buf.clone());
                                    }
                                    if let Ok(mut current) = params_clone.current_audio.write() {
                                        *current = Some(buf);
                                    }
                                    if let Ok(mut p) = params_clone.loaded_file_name.write() {
                                        *p = path
                                            .file_name()
                                            .unwrap_or_default()
                                            .to_string_lossy()
                                            .to_string();
                                    }
                                }
                            }
                        });
                    }

                    let fname = params
                        .loaded_file_name
                        .read()
                        .map(|g| g.clone())
                        .unwrap_or_else(|_| "Default Antiorp Sound".to_string());

                    ui.label(
                        egui::RichText::new(format!("Sample: {fname}"))
                            .size(10.5)
                            .color(TEXT_MUTED),
                    );
                });
            });

            // Method Selector
            ui.group(|ui| {
                ui.set_width(ui.available_width());
                ui.horizontal(|ui| {
                    ui.label(
                        egui::RichText::new("METHOD:")
                            .size(11.0)
                            .color(PHOSPHOR_GREEN)
                            .strong(),
                    );
                    param_dropdown(ui, ctx, P::Score, "Score", 1);
                });

                let score = params.score.value();
                ui.label(
                    egui::RichText::new(score.description())
                        .size(10.0)
                        .color(TEXT_MUTED),
                );
            });

            // Controls: Checkboxes and Knobs
            ui.horizontal(|ui| {
                ui.vertical(|ui| {
                    ui.set_width(ui.available_width() * 0.52);
                    ui.spacing_mut().item_spacing = egui::vec2(0.0, 6.0);

                    param_toggle(ui, ctx, P::WrappingDistortion, "16-Bit Wrap Clip");
                    ui.label(
                        egui::RichText::new("Authentic 1995 digital wrapping distortion (>0dBFS)")
                            .size(8.5)
                            .color(TEXT_MUTED),
                    );

                    ui.add_space(2.0);

                    param_toggle(ui, ctx, P::LiveInput, "Live Audio Input");
                    ui.label(
                        egui::RichText::new("Capture and collide incoming live DAW / mic audio")
                            .size(8.5)
                            .color(TEXT_MUTED),
                    );
                });

                ui.separator();

                // Knobs for Density and Gain
                ui.horizontal(|ui| {
                    ui.spacing_mut().item_spacing = egui::vec2(16.0, 0.0);
                    param_knob(ui, ctx, P::Density, "Density");
                    param_knob(ui, ctx, P::Gain, "Gain");
                });
            });

            ui.separator();

            // Real-Time Density & Output Meters
            ui.group(|ui| {
                ui.set_width(ui.available_width());
                ui.spacing_mut().item_spacing = egui::vec2(0.0, 4.0);

                let density_val = ctx.get_meter(P::MeterDensity);
                let active_grains = (density_val * 600.0) as usize;

                ui.horizontal(|ui| {
                    ui.label(
                        egui::RichText::new("ACTIVE LAYERS:")
                            .size(10.0)
                            .color(PHOSPHOR_GREEN)
                            .strong(),
                    );
                    ui.label(
                        egui::RichText::new(format!("{active_grains} / 600 max"))
                            .size(10.0)
                            .color(if active_grains > 500 {
                                ACCENT_RED
                            } else {
                                PHOSPHOR_GREEN
                            }),
                    );
                });

                let progress_bar = egui::ProgressBar::new(density_val.clamp(0.0, 1.0))
                    .show_percentage()
                    .fill(PHOSPHOR_GREEN);
                ui.add(progress_bar);

                ui.add_space(2.0);

                ui.horizontal(|ui| {
                    ui.label(egui::RichText::new("OUTPUT:").size(9.5).color(TEXT_MUTED));
                    level_meter(ui, ctx, &[P::MeterLeft, P::MeterRight], 30.0);
                });
            });

            // Console Log Terminal
            ui.scope(|ui| {
                egui::Frame::NONE
                    .fill(egui::Color32::BLACK)
                    .stroke(egui::Stroke::new(1.0f32, BORDER_COLOR))
                    .inner_margin(6.0)
                    .show(ui, |ui| {
                        ui.set_width(ui.available_width());
                        ui.horizontal(|ui| {
                            ui.label(
                                egui::RichText::new("[CONSOLE] ")
                                    .size(9.0)
                                    .color(DIM_GREEN)
                                    .strong(),
                            );
                            let wrap_str = if params.wrapping_distortion.value() {
                                "WRAPPING (CRUNCH)"
                            } else {
                                "FLOAT (CLEAN)"
                            };
                            let input_str = if params.live_input.value() {
                                "LIVE IN"
                            } else {
                                "BUFFER"
                            };
                            ui.label(
                                egui::RichText::new(format!(
                                    "State: COLLIDING | Mode: {wrap_str} | Source: {input_str} | Max 600 Layers"
                                ))
                                .size(9.0)
                                .color(TEXT_MUTED),
                            );
                        });
                    });
            });

            // Request continuous redraw for live meters
            ui.ctx().request_repaint();
        });
}
