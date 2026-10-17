# Th0nk 0.4.1 — Jardim Sonoro (Apple Silicon)

<img src="https://raw.githubusercontent.com/nagualcode/th0nk3000/main/assets/icon-1024.png" width="128" alt="ícone th0nk3000">

### 🎨 Novo ícone
- O app standalone (`Th0nk.app`) tem um ícone próprio no Finder, no Dock, no Launchpad e no Spotlight.
- O ícone segue a grade de ícones do macOS (arte de 824 px numa tela de 1024 px, com cantos transparentes) e inclui todos os tamanhos, de 16 a 1024 px.

Nada mais mudou em relação à 0.4.0: mesmo motor, mesma interface e os mesmos formatos.

### 📦 Instalação (Apple Silicon / arm64)
- **`thonk-0.4.1-macos.pkg`**: instalador.
- **`thonk-0.4.1-macos-arm64.zip`**: AU, VST3, CLAP, App Standalone e CLI `thonk-render`.

Se o Dock ou o Finder ainda mostrarem o ícone antigo depois de atualizar, arraste o app novo para `/Applications` substituindo o antigo. O cache de ícones do macOS se atualiza sozinho, ou após `killall Dock`.

Os binários têm assinatura ad-hoc e não são notarizados. Se o macOS bloquear, use `xattr -dr com.apple.quarantine <arquivo>`.
