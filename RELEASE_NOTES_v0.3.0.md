# Th0nk 0.3.0 — Jardim Sonoro

**Motor reconstruído a partir do binário original.** Nesta versão, o mecanismo de síntese e as 11 partituras do thOnk_0+2 foram reescritos como tradução direta do executável PowerPC. Constantes, pesos e laços foram lidos instrução por instrução. O processo completo está documentado em [`reverse_engineering/README.md`](https://github.com/nagualcode/th0nk3000/blob/main/reverse_engineering/README.md).

### 🔬 Fidelidade ao original
- **Partituras exatas.** Antes, só a Classic Hectic estava próxima do original; Flowing, Flash, Slack, Shepard, Pentathon e os Stretches tinham valores inventados. Agora todas usam as constantes do binário:
  - Flowing, Flash, Slack e Dis são variações do mesmo código da Hectic, com contagens e faixas próprias.
  - Shepard é a construção real: vozes que entram no agudo, descem e reaparecem em silêncio, ficando mais graves e mais lentas a cada ciclo.
  - Pentathon tem duas vozes em registros diferentes, com glides de 2,5 s e notas de duração aleatória.
  - Octaves Stretch mistura 0, −24 e +12 semitons.
  - Plain Stretch tem a imagem estéreo larga do original.
- **Agendador original** (`BreakPointsToSoundFile`): densidade avaliada a cada 10 ms e um relógio de grãos independente. A lista "GFRnd" é jitter de início do grão em amostras, não ruído na taxa.
- **Grão inteiro como no original:** interpolação linear truncada em 16 bits, tabelas de envelope cosseno de 1000 pontos, attack/decay de até 100 % do grão, pan com canais na orientação correta e leitura que nunca passa do fim do arquivo (antes dava a volta e gerava cliques).
- **Wrap de 16 bits bit-idêntico.** Os grãos são somados em 32 bits e convertidos para 16 bits na saída, o que equivale exatamente ao `short + short` do original.
- **Sem limite artificial de 600 grãos.** O "600 layers" do manual era o máximo teórico, não um teto.
- **Taxa de amostragem correta.** A síntese roda na taxa do arquivo de entrada, como no original; só a saída é reamostrada para a taxa do host. Antes, um arquivo a 48 kHz saía ~1,5 semitom agudo e o render saía sempre a 44,1 kHz.
- Descrições das partituras restauradas a partir do texto original do programa.

### ✨ Novidades
- **Seed:** 0 gera uma composição nova a cada Thonk, como o original fazia com o relógio. **LOCK SEED** fixa a composição que está tocando, para recall no DAW.
- **Render em WAV ou AIFF** de verdade, conforme a extensão. Antes, um arquivo `.aiff` recebia conteúdo WAV.
- **`thonk-render`**: renderizador de linha de comando, incluído no `.zip` (pasta `CLI`).
- A audição encadeia uma nova composição ao fim de cada partitura.

### ⚙️ Desempenho e tempo real
- Nenhuma alocação na thread de áudio: a partitura é regenerada em memória pré-alocada e o buffer de áudio antigo é liberado na thread da interface.
- Grãos renderizados inteiros, à frente da reprodução (~2 ns por amostra de grão). Uma partitura densa de 20 minutos renderiza offline entre 110× e 260× o tempo real no Apple Silicon.
- 13 testes de regressão verificam as partituras contra as constantes do binário, o gerador de números aleatórios do Mac OS, as tabelas de envelope e a equivalência do wrap.

### ⚠️ Comportamentos autênticos que podem surpreender
- Algumas partituras (sobretudo Classic Flowing) podem ficar **vários minutos em silêncio** e passar dos 20 minutos. É o relógio de grãos do original saltando à frente num vale de densidade.
- Os níveis são os do original: com material de entrada quente, o wrap aparece.

---

### 📦 Formatos (binários universais: Apple Silicon arm64 + Intel x86_64)
- **Audio Unit v2** (`Th0nk.component`), **VST3** (`Th0nk.vst3`), **CLAP** (`Th0nk.clap`)
- **App Standalone** (`Th0nk.app`)
- **CLI** `thonk-render` (só no `.zip`)

### 🚀 Instalação
- **`thonk-0.3.0-macos.pkg`**: instalador; escolha os formatos.
- **`thonk-0.3.0-macos-universal.zip`**: instalação manual:
  - `AU/Th0nk.component` ➔ `~/Library/Audio/Plug-Ins/Components/`
  - `VST3/Th0nk.vst3` ➔ `~/Library/Audio/Plug-Ins/VST3/`
  - `CLAP/Th0nk.clap` ➔ `~/Library/Audio/Plug-Ins/CLAP/`
  - `Standalone/Th0nk.app` ➔ `/Applications/`

Os binários têm assinatura ad-hoc e não são notarizados. Se o macOS bloquear, use clique direito → Abrir, ou `xattr -dr com.apple.quarantine <arquivo>`. Se o Logic continuar carregando a versão antiga do AU, force a nova varredura com `killall -9 AudioComponentRegistrar` e reabra o Logic.

> Projetos salvos na 0.2.0 abrem normalmente. A partitura tocada será a nova versão, fiel ao original, e não a aproximação anterior.
