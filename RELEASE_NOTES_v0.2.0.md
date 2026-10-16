# Th0nk 0.2.0 — Jardim Sonoro

Recriação moderna em Rust (via Truce Audio Framework) do clássico aplicativo de síntese granular artística do Mac OS 9 da Audio Ease: **Th0nk** (1995–1997, Arjen van der Schoot & antiorp).

### 🏷️ Organização na DAW
Nas DAWs (Logic Pro, Ableton Live, Reaper, Cubase, Studio One, Bitwig, FL Studio), o plugin é identificado sob a publicação e pasta do fabricante:
- **Jardim Sonoro** ➔ **Th0nk**

---

### 📦 Formatos Inclusos (Binários Universais: Apple Silicon arm64 + Intel x86_64)
- **Audio Unit v2 (`Th0nk.component`)**: Logic Pro, GarageBand, Ableton Live, etc.
- **VST3 (`Th0nk.vst3`)**: Ableton Live, Cubase, Studio One, FL Studio, Reaper, etc.
- **CLAP (`Th0nk.clap`)**: Bitwig Studio, Reaper, etc.
- **Standalone App (`Th0nk.app`)**: Aplicativo independente para macOS.

---

### 🚀 Opções de Instalação no macOS

#### Opção 1: Instalador Automático (.pkg) — Recomendado
Baixe e execute **`thonk-0.2.0-macos.pkg`**. Ele permite escolher quais formatos instalar e instala automaticamente nas pastas padrão de áudio do sistema/usuário.

#### Opção 2: Instalação Manual (.zip)
Baixe **`thonk-0.2.0-macos-universal.zip`**, descompacte e copie para as pastas correspondentes:
- `AU/Th0nk.component` ➔ `~/Library/Audio/Plug-Ins/Components/`
- `VST3/Th0nk.vst3` ➔ `~/Library/Audio/Plug-Ins/VST3/`
- `CLAP/Th0nk.clap` ➔ `~/Library/Audio/Plug-Ins/CLAP/`
- `Standalone/Th0nk.app` ➔ `/Applications/`

---

### ✨ Recursos e Síntese
- **12 Partituras ("Scores") Originais**: Classic Flowing, Classic Hectic, Pentathon, Shepard, Dis, Octaves Stretch (1m/5m), Plain Stretch (1m/5m), Flash, Slack, Stochastic.
- **Distorção 16-bit Autêntica**: Recriação exata do overflow de aritmética de inteiros com sinal de 16 bits do Mac OS 9 de 1995.
- **Live Input**: Granulação contínua do sinal do microfone/entrada da DAW.
- **Suporte de Áudio**: Carregamento direto de arquivos WAV e AIFF (incluindo decodificação big-endian original).
- **Interface Gráfica Fiel**: UI minimalista retrô com medidores estéreo e indicador de densidade de grãos.
