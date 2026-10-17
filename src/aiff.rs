//! Audio file decoding for AIFF and WAV formats.
//! Supports classic 16-bit mono AIFF (the original Th0nk format) and modern WAV/AIFF files.

use std::fs::File;
use std::io::{BufReader, Read, Seek, SeekFrom};
use std::path::Path;

/// Loaded audio buffer containing 32-bit floating point samples normalized to [-1.0, 1.0].
#[derive(Clone)]
pub struct AudioBufferData {
    pub name: String,
    pub sample_rate: f64,
    pub channels: usize,
    pub samples: Vec<f32>, // Mono or mixed down to mono for granular processing
    /// The same material as 16-bit PCM: thOnk_0+2 only ever read 16-bit mono AIFF.
    pub pcm16: Vec<i16>,
}

impl AudioBufferData {
    fn new(name: String, sample_rate: f64, samples: Vec<f32>) -> Self {
        let pcm16 = samples
            .iter()
            .map(|&s| (s * 32768.0).round().clamp(-32768.0, 32767.0) as i16)
            .collect();
        Self {
            name,
            sample_rate,
            channels: 1,
            samples,
            pcm16,
        }
    }

    /// Creates a default built-in synthetic test sound (rich harmonic chime/burst).
    pub fn default_chime(sample_rate: f64) -> Self {
        let duration_secs = 2.5;
        let total_samples = (sample_rate * duration_secs) as usize;
        let mut samples = Vec::with_capacity(total_samples);

        for i in 0..total_samples {
            let t = i as f64 / sample_rate;
            let decay = (-t * 2.2).exp();
            // Complex metallic FM chime with multiple transitions
            let f0 = 220.0 + (t * 40.0).sin() * 30.0;
            let mod_sig = (t * 330.0 * std::f64::consts::TAU).sin() * 2.5;
            let sig = (t * (f0 + mod_sig * 110.0) * std::f64::consts::TAU).sin() * 0.4
                + (t * 440.0 * 1.5 * std::f64::consts::TAU).sin() * 0.25
                + (t * 880.0 * 2.1 * std::f64::consts::TAU).sin() * 0.15
                + (t * 1760.0 * 0.7 * std::f64::consts::TAU).sin() * 0.1;
            samples.push((sig * decay) as f32);
        }

        Self::new("Default Antiorp Metallic Burst".to_string(), sample_rate, samples)
    }

    /// Loads an audio file (WAV or AIFF) from disk.
    pub fn load_file(path: &Path) -> Result<Self, String> {
        let ext = path
            .extension()
            .and_then(|e| e.to_str())
            .unwrap_or("")
            .to_lowercase();

        let filename = path
            .file_name()
            .and_then(|n| n.to_str())
            .unwrap_or("Audio")
            .to_string();

        if ext == "wav" {
            Self::load_wav(path, filename)
        } else if ext == "aif" || ext == "aiff" {
            Self::load_aiff(path, filename)
        } else {
            // Try WAV first, then AIFF
            Self::load_wav(path, filename.clone())
                .or_else(|_| Self::load_aiff(path, filename))
        }
    }

    fn load_wav(path: &Path, name: String) -> Result<Self, String> {
        let mut reader = hound::WavReader::open(path)
            .map_err(|e| format!("Failed to open WAV: {e}"))?;
        let spec = reader.spec();
        let sample_rate = spec.sample_rate as f64;
        let channels = spec.channels as usize;

        let samples: Vec<f32> = match spec.sample_format {
            hound::SampleFormat::Float => reader
                .samples::<f32>()
                .filter_map(Result::ok)
                .collect(),
            hound::SampleFormat::Int => {
                let max_val = (1i64 << (spec.bits_per_sample - 1)) as f32;
                reader
                    .samples::<i32>()
                    .filter_map(Result::ok)
                    .map(|s| s as f32 / max_val)
                    .collect()
            }
        };

        // If stereo or multi-channel, mix down to mono
        let mono_samples = if channels > 1 {
            let mut mono = Vec::with_capacity(samples.len() / channels);
            for chunk in samples.chunks(channels) {
                let sum: f32 = chunk.iter().sum();
                mono.push(sum / channels as f32);
            }
            mono
        } else {
            samples
        };

        if sample_rate <= 0.0 || !sample_rate.is_finite() {
            return Err("Invalid sample rate".to_string());
        }
        Ok(Self::new(name, sample_rate, mono_samples))
    }

    fn load_aiff(path: &Path, name: String) -> Result<Self, String> {
        let file = File::open(path).map_err(|e| format!("Failed to open AIFF: {e}"))?;
        let mut reader = BufReader::new(file);

        let mut magic = [0u8; 4];
        reader.read_exact(&mut magic).map_err(|e| e.to_string())?;
        if &magic != b"FORM" {
            return Err("Not a valid AIFF (missing FORM chunk)".to_string());
        }

        let mut form_size_bytes = [0u8; 4];
        reader.read_exact(&mut form_size_bytes).map_err(|e| e.to_string())?;

        let mut form_type = [0u8; 4];
        reader.read_exact(&mut form_type).map_err(|e| e.to_string())?;
        if &form_type != b"AIFF" && &form_type != b"AIFC" {
            return Err("Not a valid AIFF form type".to_string());
        }

        let mut channels = 1usize;
        let mut sample_rate = 44100.0f64;
        let mut bits_per_sample = 16usize;
        let mut raw_pcm: Option<Vec<u8>> = None;

        // Iterate chunks
        while let Ok(()) = reader.read_exact(&mut magic) {
            let mut chunk_size_bytes = [0u8; 4];
            if reader.read_exact(&mut chunk_size_bytes).is_err() {
                break;
            }
            let chunk_size = u32::from_be_bytes(chunk_size_bytes) as usize;

            match &magic {
                b"COMM" => {
                    let mut num_channels_b = [0u8; 2];
                    reader.read_exact(&mut num_channels_b).map_err(|e| e.to_string())?;
                    channels = u16::from_be_bytes(num_channels_b) as usize;

                    let mut num_frames_b = [0u8; 4];
                    reader.read_exact(&mut num_frames_b).map_err(|e| e.to_string())?;

                    let mut sample_size_b = [0u8; 2];
                    reader.read_exact(&mut sample_size_b).map_err(|e| e.to_string())?;
                    bits_per_sample = u16::from_be_bytes(sample_size_b) as usize;

                    let mut ext_sr = [0u8; 10];
                    reader.read_exact(&mut ext_sr).map_err(|e| e.to_string())?;
                    sample_rate = decode_extended_80(&ext_sr);

                    // Skip remainder of COMM chunk if any
                    let read_so_far = 2 + 4 + 2 + 10;
                    if chunk_size > read_so_far {
                        reader.seek(SeekFrom::Current((chunk_size - read_so_far) as i64))
                            .map_err(|e| e.to_string())?;
                    }
                }
                b"SSND" => {
                    let mut offset_b = [0u8; 4];
                    reader.read_exact(&mut offset_b).map_err(|e| e.to_string())?;
                    let offset = u32::from_be_bytes(offset_b) as usize;

                    let mut block_size_b = [0u8; 4];
                    reader.read_exact(&mut block_size_b).map_err(|e| e.to_string())?;

                    if offset > 0 {
                        reader.seek(SeekFrom::Current(offset as i64))
                            .map_err(|e| e.to_string())?;
                    }

                    let data_len = chunk_size.saturating_sub(8 + offset);
                    let mut data = vec![0u8; data_len];
                    reader.read_exact(&mut data).map_err(|e| e.to_string())?;
                    raw_pcm = Some(data);
                }
                _ => {
                    // Skip unknown chunk
                    let pad = chunk_size % 2;
                    reader.seek(SeekFrom::Current((chunk_size + pad) as i64))
                        .map_err(|e| e.to_string())?;
                }
            }
        }

        let pcm = raw_pcm.ok_or_else(|| "Missing SSND chunk in AIFF".to_string())?;
        let bytes_per_sample = bits_per_sample / 8;
        let mut float_samples = Vec::with_capacity(pcm.len() / bytes_per_sample);

        if bits_per_sample == 16 {
            for chunk in pcm.chunks_exact(2) {
                let s = i16::from_be_bytes([chunk[0], chunk[1]]);
                float_samples.push(s as f32 / 32768.0);
            }
        } else if bits_per_sample == 24 {
            for chunk in pcm.chunks_exact(3) {
                let s = i32::from_be_bytes([chunk[0], chunk[1], chunk[2], 0]) >> 8;
                float_samples.push(s as f32 / 8388608.0);
            }
        } else if bits_per_sample == 8 {
            for &b in &pcm {
                let s = b as i8;
                float_samples.push(s as f32 / 128.0);
            }
        } else {
            return Err(format!("Unsupported bit depth: {bits_per_sample}"));
        }

        // Mix down to mono
        let mono_samples = if channels > 1 {
            let mut mono = Vec::with_capacity(float_samples.len() / channels);
            for chunk in float_samples.chunks(channels) {
                let sum: f32 = chunk.iter().sum();
                mono.push(sum / channels as f32);
            }
            mono
        } else {
            float_samples
        };

        if sample_rate <= 0.0 || !sample_rate.is_finite() {
            return Err("Invalid sample rate".to_string());
        }
        Ok(Self::new(name, sample_rate, mono_samples))
    }
}

/// Decodes Apple IEEE 754 80-bit extended precision float to f64.
fn decode_extended_80(bytes: &[u8; 10]) -> f64 {
    let sign = (bytes[0] & 0x80) != 0;
    let exponent = u16::from_be_bytes([bytes[0] & 0x7f, bytes[1]]) as i32;
    let mantissa = u64::from_be_bytes([
        bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7], bytes[8], bytes[9],
    ]);

    if exponent == 0 && mantissa == 0 {
        return 0.0;
    }

    let normalized_mantissa = (mantissa as f64) / (1u64 << 63) as f64;
    let factor = (exponent - 16383) as f64;
    let value = normalized_mantissa * 2.0f64.powf(factor);

    if sign { -value } else { value }
}

/// Streaming 16-bit stereo writer: AIFF when the path ends in .aif/.aiff
/// (big-endian, 80-bit extended sample rate, as thOnk_0+2 wrote), WAV otherwise.
pub enum Pcm16Writer {
    Wav(hound::WavWriter<std::io::BufWriter<File>>),
    Aiff {
        out: std::io::BufWriter<File>,
        sample_rate: f64,
        frames: u32,
    },
}

impl Pcm16Writer {
    pub fn create(path: &Path, sample_rate: f64) -> Result<Self, String> {
        let ext = path
            .extension()
            .and_then(|e| e.to_str())
            .unwrap_or("")
            .to_lowercase();
        if ext == "aif" || ext == "aiff" {
            let file = File::create(path).map_err(|e| e.to_string())?;
            let mut w = Self::Aiff {
                out: std::io::BufWriter::new(file),
                sample_rate,
                frames: 0,
            };
            w.write_aiff_header()?;
            Ok(w)
        } else {
            let spec = hound::WavSpec {
                channels: 2,
                sample_rate: sample_rate.round() as u32,
                bits_per_sample: 16,
                sample_format: hound::SampleFormat::Int,
            };
            hound::WavWriter::create(path, spec)
                .map(Self::Wav)
                .map_err(|e| e.to_string())
        }
    }

    /// Writes interleaved L/R frames.
    pub fn write_frames(&mut self, interleaved: &[i16]) -> Result<(), String> {
        use std::io::Write;
        match self {
            Self::Wav(w) => {
                let mut w16 = w.get_i16_writer(interleaved.len() as u32);
                for &s in interleaved {
                    w16.write_sample(s);
                }
                w16.flush().map_err(|e| e.to_string())
            }
            Self::Aiff { out, frames, .. } => {
                let mut buf = Vec::with_capacity(interleaved.len() * 2);
                for &s in interleaved {
                    buf.extend_from_slice(&s.to_be_bytes());
                }
                *frames += (interleaved.len() / 2) as u32;
                out.write_all(&buf).map_err(|e| e.to_string())
            }
        }
    }

    pub fn finalize(self) -> Result<(), String> {
        use std::io::Write;
        match self {
            Self::Wav(w) => w.finalize().map_err(|e| e.to_string()),
            mut aiff @ Self::Aiff { .. } => {
                aiff.write_aiff_header()?;
                if let Self::Aiff { mut out, .. } = aiff {
                    out.flush().map_err(|e| e.to_string())?;
                }
                Ok(())
            }
        }
    }

    /// (Re)writes FORM/COMM/SSND headers at the start of the file.
    fn write_aiff_header(&mut self) -> Result<(), String> {
        use std::io::Write;
        let Self::Aiff {
            out,
            sample_rate,
            frames,
        } = self
        else {
            return Ok(());
        };
        let data_bytes = *frames * 4;
        let mut h = Vec::with_capacity(54);
        h.extend_from_slice(b"FORM");
        h.extend_from_slice(&(4 + 26 + 16 + data_bytes).to_be_bytes());
        h.extend_from_slice(b"AIFF");
        h.extend_from_slice(b"COMM");
        h.extend_from_slice(&18u32.to_be_bytes());
        h.extend_from_slice(&2u16.to_be_bytes());
        h.extend_from_slice(&frames.to_be_bytes());
        h.extend_from_slice(&16u16.to_be_bytes());
        h.extend_from_slice(&encode_extended_80(*sample_rate));
        h.extend_from_slice(b"SSND");
        h.extend_from_slice(&(8 + data_bytes).to_be_bytes());
        h.extend_from_slice(&0u32.to_be_bytes());
        h.extend_from_slice(&0u32.to_be_bytes());
        let pos = out.stream_position().map_err(|e| e.to_string())?;
        out.seek(SeekFrom::Start(0)).map_err(|e| e.to_string())?;
        out.write_all(&h).map_err(|e| e.to_string())?;
        if pos > h.len() as u64 {
            out.seek(SeekFrom::Start(pos)).map_err(|e| e.to_string())?;
        }
        Ok(())
    }
}

/// Encodes a positive f64 as Apple's 80-bit IEEE 754 extended float.
fn encode_extended_80(value: f64) -> [u8; 10] {
    let mut out = [0u8; 10];
    if value <= 0.0 || !value.is_finite() {
        return out;
    }
    let exp = value.log2().floor() as i32;
    let mantissa = (value / 2f64.powi(exp) * (1u64 << 63) as f64) as u64;
    let biased = (exp + 16383) as u16;
    out[..2].copy_from_slice(&biased.to_be_bytes());
    out[2..].copy_from_slice(&mantissa.to_be_bytes());
    out
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn extended_80_roundtrip() {
        for sr in [8000.0, 22050.0, 44100.0, 48000.0, 96000.0, 192000.0] {
            assert_eq!(decode_extended_80(&encode_extended_80(sr)), sr);
        }
    }
}
