# Th0nk 0.3.1 — Jardim Sonoro

Versão de correção da 0.3.0, focada em estabilidade de áudio em tempo real. **Recomendada para todos os usuários da 0.3.0.** O som não muda: renders offline com o mesmo seed saem byte a byte idênticos aos da 0.3.0.

### 🩹 Correções
- **Estalos (xrun) em blocos pequenos.** Com buffers pequenos no DAW (por exemplo, 128 amostras a 48 kHz), o agendador podia processar várias dezenas de milissegundos de grãos num único bloco. Na Classic Hectic, um bloco chegou a levar 14,9 ms para um orçamento de 2,7 ms. Agora o trabalho é distribuído por igual entre os blocos, e o pior bloco medido fica em torno de 0,4 ms.
- **Novo "Thonk" mais leve.** Em vez de zerar o buffer inteiro de 8 MB, só o trecho já mixado à frente da reprodução é limpo. O custo cai de ~1 ms para poucos microssegundos, mais a geração da partitura.
- **Gerador de números aleatórios do Mac OS ~2× mais rápido**, sem divisão e com a mesma sequência (verificado em 1 milhão de passos). A geração da partitura Flash caiu de 1,4 ms para 0,6 ms.

### ✅ Verificação
- O Audio Unit passa no `auval` da Apple (render de 11 kHz a 192 kHz, blocos irregulares, parâmetros e agendamento).
- 14 testes automáticos, mais um benchmark de tempo real (`cargo test --release --lib rt_budget -- --ignored --nocapture`). Medido a 48 kHz com blocos de 128 amostras: CPU média de ~0,8% por instância e nenhum bloco acima de 50% do orçamento.

### 📦 Formatos e instalação
Os mesmos da 0.3.0: AU v2, VST3, CLAP e App Standalone (universais arm64 + x86_64), mais o CLI `thonk-render` no `.zip`.
- **`thonk-0.3.1-macos.pkg`**: instalador.
- **`thonk-0.3.1-macos-universal.zip`**: instalação manual (veja o `README.txt`).

Os binários têm assinatura ad-hoc e não são notarizados. Se o macOS bloquear, use `xattr -dr com.apple.quarantine <arquivo>`. Para o Logic carregar o AU novo: `killall -9 AudioComponentRegistrar`.
