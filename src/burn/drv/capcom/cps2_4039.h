#ifndef CPS2_4039_H
#define CPS2_4039_H

// FBNeo 4039 - extensao para Marvel vs. Capcom (mvsc, CPS-2).
// Todas as funcoes retornam imediatamente quando o jogo carregado nao e "mvsc",
// entao nao alteram o comportamento de nenhum outro jogo.

void Cps4039Reset();
void Cps4039FrameStart();          // chamada em Cps2Frame(), apos a leitura das entradas
void Cps4039CharacterSelectDraw(); // chamada em DoDraw(), apos o desenho das camadas
int  Cps4039KaelVirtualP1();
int  Cps4039KaelVirtualP2();

#endif
