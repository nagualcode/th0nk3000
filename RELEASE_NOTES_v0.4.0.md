# Th0nk 0.4.0 — Jardim Sonoro (Apple Silicon)

**Nova interface, controle de duração, sincronia com o DAW e render em segundo plano.** O motor fiel ao original da 0.3.x continua o mesmo: no modo de duração "Original", os renders saem byte a byte idênticos aos da 0.3.1.

![th0nk3000](https://raw.githubusercontent.com/nagualcode/th0nk3000/main/docs/screenshot.png)

### 🎛️ Nova interface (inspirada no Ableton Live)
- Layout em faixa horizontal, como um dispositivo do Live, redimensionável.
- **Barra superior:** ▶/■, THONK, partitura, seed com **Lock**, duração e Render.
- **Painel SAMPLE:** forma de onda do som, com pontos mostrando ao vivo de onde os grãos estão lendo e para que lado do estéreo vão.
- **Painel SCORE:** a partitura desenhada como faixas de automação (densidade com mapa de calor das camadas, as quatro vozes de afinação, pan e posição na fonte). A cabeça de leitura é clicável e arrastável.
- **Barra de status:** estado do transporte, partitura que está tocando, posição/duração e progresso do render.
- Tema **Classic** (o verde fósforo antiorp) disponível como alternativa.

### ⏱️ Duração da obra
- **Original** (padrão): duração própria da partitura e final autêntico.
- **Stretch:** a partitura inteira é comprimida ou esticada para a duração escolhida (10 s a 60 min). O arco narrativo inteiro cabe no tempo pedido.
- **Cut:** a partitura toca no ritmo próprio e para na duração escolhida, como encerrar o original antes do fim.
- Nos modos Stretch e Cut, o arquivo termina **exatamente** na duração, com fade de 30 ms.
- **Skip silences** (opcional, fora do original): evita os silêncios de vários minutos que o relógio de grãos do original produz. Na Classic Flowing de teste, 391 s de silêncio caíram para zero.

### 🎚️ Uso no DAW
- **Abre parado e em silêncio.** O som que tocava ao abrir a 0.3.x era o som sintético de demonstração disparando automaticamente; agora ele só toca se você clicar em **Demo**.
- **Host Sync:** toca com o transporte do DAW, e o tempo da composição segue a linha do tempo (compasso 1 = 0:00). Bounces a partir da mesma posição e do mesmo seed saem idênticos.
- **O som fica salvo no projeto.** Embutido até ~3 min a 48 kHz; acima disso, guardado pelo caminho e recarregado do disco. Na 0.3.x, reabrir um projeto perdia o som.
- No standalone, a **barra de espaço** toca e para.

### 💾 Render
- Roda em segundo plano, com progresso e **Cancel**. Um render cancelado não deixa arquivo pela metade.
- **16-bit, 24-bit ou 32-bit float**, em WAV ou AIFF/AIFC, com **normalização** opcional a −0,3 dBFS.
- 16-bit com wrap a 0 dB e sem normalizar continua **bit-exato** ao original.
- O CLI `thonk-render` ganhou `--length`, `--mode`, `--skip-silence`, `--format` e `--normalize`.

### ✅ Qualidade
- **Validação:**
  - AU v2 aprovado no `auval` da Apple;
  - VST3 aprovado no `pluginval` da Tracktion;
  - CLAP aprovado no `clap-validator`. Três testes de reprodutibilidade de estado falham do mesmo jeito nos exemplos oficiais do framework Truce v6.3.0, então ficam excluídos.
- **CI no GitHub** (`.github/workflows/ci.yml`): testes mais os três validadores a cada push.
- **Testes:** 23 automáticos.
- **Tempo real:** ~0,8% de CPU por instância a 48 kHz com blocos de 128 amostras; nenhum bloco passa de metade do orçamento.

### 📦 Formatos e instalação (Apple Silicon / arm64)
- **`thonk-0.4.0-macos.pkg`**: instalador; escolha os formatos.
- **`thonk-0.4.0-macos-arm64.zip`**: AU, VST3, CLAP, App Standalone e CLI `thonk-render`. Veja o `README.txt`.

Os binários têm assinatura ad-hoc e não são notarizados. Se o macOS bloquear, use `xattr -dr com.apple.quarantine <arquivo>`. Para o Logic carregar o AU novo: `killall -9 AudioComponentRegistrar`. Em Macs Intel, compile a partir do código (instruções no README) ou use a 0.3.1 universal.

> **Compatibilidade:** projetos da 0.3.x abrem com os mesmos valores de parâmetros. O som precisa ser recarregado uma única vez, porque versões anteriores não o salvavam.
