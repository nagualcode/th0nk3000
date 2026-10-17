//! The source sound as persisted plugin state.
//!
//! Hosts save `#[persist]` fields with the project and restore them on the host
//! thread (never the audio thread), which is the right place to (de)serialise
//! megabytes of audio. The audio thread only ever clones the `Arc`.

use crate::aiff::AudioBufferData;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::{Arc, RwLock};
use truce::core::custom_state::{PersistField, StateCursor};

/// Sounds up to this many samples are embedded in the project (about 3 minutes
/// at 48 kHz, 16 MB); longer ones are stored by path and reloaded from disk.
const EMBED_MAX_SAMPLES: usize = 8 * 1024 * 1024;
const FORMAT_VERSION: u8 = 1;

#[derive(Default)]
pub struct SourceSlot {
    current: RwLock<Option<Arc<AudioBufferData>>>,
    generation: AtomicU64,
}

impl SourceSlot {
    /// Replaces the sound (UI or host thread) and bumps the generation so the
    /// audio thread picks it up.
    pub fn set(&self, data: Option<Arc<AudioBufferData>>) {
        if let Ok(mut g) = self.current.write() {
            *g = data;
        }
        self.generation.fetch_add(1, Ordering::Release);
    }

    pub fn generation(&self) -> u64 {
        self.generation.load(Ordering::Acquire)
    }

    /// Non-blocking read for the audio thread (an `Arc` clone, no allocation).
    pub fn try_get(&self) -> Option<Option<Arc<AudioBufferData>>> {
        self.current.try_read().ok().map(|g| g.clone())
    }

    pub fn get(&self) -> Option<Arc<AudioBufferData>> {
        self.current.read().ok().and_then(|g| g.clone())
    }
}

fn put_str(buf: &mut Vec<u8>, s: &str) {
    buf.extend_from_slice(&(s.len() as u32).to_le_bytes());
    buf.extend_from_slice(s.as_bytes());
}

fn get_u32(c: &mut StateCursor) -> Option<u32> {
    Some(u32::from_le_bytes(c.read_bytes(4)?.try_into().ok()?))
}

fn get_str(c: &mut StateCursor) -> Option<String> {
    let n = get_u32(c)? as usize;
    String::from_utf8(c.read_bytes(n)?.to_vec()).ok()
}

impl PersistField for SourceSlot {
    fn persist_write(&self, buf: &mut Vec<u8>) {
        let data = self.get();
        buf.push(FORMAT_VERSION);
        let Some(d) = data else {
            buf.push(0);
            return;
        };
        buf.push(1);
        put_str(buf, &d.name);
        put_str(buf, &d.path);
        buf.extend_from_slice(&d.sample_rate.to_le_bytes());
        let embed = d.pcm16.len() <= EMBED_MAX_SAMPLES;
        let n = if embed { d.pcm16.len() } else { 0 };
        buf.extend_from_slice(&(n as u32).to_le_bytes());
        buf.reserve(n * 2);
        for s in &d.pcm16[..n] {
            buf.extend_from_slice(&s.to_le_bytes());
        }
    }

    fn persist_read(&self, c: &mut StateCursor) {
        let Some(&[version, present]) = c.read_bytes(2).map(|b| [b[0], b[1]]).as_ref() else {
            return;
        };
        if version != FORMAT_VERSION {
            return;
        }
        if present == 0 {
            self.set(None);
            return;
        }
        let parsed = (|| {
            let name = get_str(c)?;
            let path = get_str(c)?;
            let sample_rate = f64::from_le_bytes(c.read_bytes(8)?.try_into().ok()?);
            let n = get_u32(c)? as usize;
            let raw = c.read_bytes(n * 2)?;
            let pcm16 = raw
                .chunks_exact(2)
                .map(|b| i16::from_le_bytes([b[0], b[1]]))
                .collect::<Vec<_>>();
            Some((name, path, sample_rate, pcm16))
        })();
        let Some((name, path, sample_rate, pcm16)) = parsed else {
            return;
        };
        let data = if !pcm16.is_empty() && sample_rate > 0.0 {
            Some(AudioBufferData {
                name,
                path,
                sample_rate,
                pcm16,
            })
        } else if !path.is_empty() {
            // Too long to embed: reload from disk, if it is still there.
            AudioBufferData::load_file(std::path::Path::new(&path)).ok()
        } else {
            None
        };
        if let Some(d) = data {
            self.set(Some(Arc::new(d)));
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn roundtrip_embeds_audio() {
        let a = SourceSlot::default();
        a.set(Some(Arc::new(AudioBufferData::demo())));
        let mut buf = Vec::new();
        a.persist_write(&mut buf);

        let b = SourceSlot::default();
        let g0 = b.generation();
        b.persist_read(&mut StateCursor::new(&buf));
        assert!(b.generation() > g0);
        let (x, y) = (a.get().unwrap(), b.get().unwrap());
        assert_eq!(x.pcm16, y.pcm16);
        assert_eq!(x.sample_rate, y.sample_rate);
        assert_eq!(x.name, y.name);
    }

    #[test]
    fn empty_roundtrip_and_garbage_are_safe() {
        let a = SourceSlot::default();
        let mut buf = Vec::new();
        a.persist_write(&mut buf);
        let b = SourceSlot::default();
        b.persist_read(&mut StateCursor::new(&buf));
        assert!(b.get().is_none());
        b.persist_read(&mut StateCursor::new(&[1, 1, 255, 255]));
        assert!(b.get().is_none());
    }
}
