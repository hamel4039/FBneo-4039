# FBNeo 4039

Extensão do [FBNeo (libretro)](https://github.com/libretro/FBNeo) para **Marvel vs. Capcom (`mvsc`, CPS-2)**, compilada como core libretro para **Android ARM64**.

## Estrutura

| Caminho | Função |
|---|---|
| `src/burn/drv/capcom/cps2_4039.cpp/.h` | Código novo do 4039 (isolado, só atua quando o jogo é `mvsc`) |
| `patches/0001-cps2-4039-hooks.patch` | Patch mínimo no FBNeo original: 1 include + 1 chamada em `cps_run.cpp` (`Cps2Frame`) e em `cps_draw.cpp` (`DoDraw`) |
| `.github/workflows/Android.yml` | Baixa o FBNeo oficial num commit fixo, aplica o 4039 e gera `fbneo_libretro_android.so` (arm64-v8a) |

Os arquivos originais do FBNeo **não são substituídos**: apenas recebem os ganchos do patch.

## Build

Actions → *Build FBNeo 4039 Android ARM64* → *Run workflow*. O artefato `FBNeo-4039-Android-ARM64` contém o `.so`.
Para atualizar a base, informe outro commit em `upstream_ref`; se o patch não aplicar mais, o build falha logo no passo "Aplicar".
