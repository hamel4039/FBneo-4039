// FBNeo 4039 - extensao para Marvel vs. Capcom (mvsc, CPS-2).
// Isolada neste arquivo: os drivers originais so recebem 3 chamadas de gancho
// (ver patches/0001-cps2-4039-hooks.patch).

#include "cps.h"
#include "cps2_4039.h"
#include <string.h>

namespace {

// Offsets dentro da Work RAM do 68000 (0xFF0000 + off)
const UINT32 P1_CURSOR     = 0x2000;
const UINT32 P2_CURSOR     = 0x2002;
const UINT16 WOLVERINE_POS = 5;

bool  sKaelVirtualP1 = false;
bool  sKaelVirtualP2 = false;
UINT8 sPrevP1 = 0;
UINT8 sPrevP2 = 0;

bool IsMvsc4039()
{
	const char* name = BurnDrvGetTextA(DRV_NAME);
	return name && strcmp(name, "mvsc") == 0;
}

// O FBNeo guarda a RAM do 68000 em palavras de 16 bits na ordem nativa do host
// (em little-endian os bytes ficam trocados, por isso o acesso a byte usa addr ^ 1).
// Ler/escrever a palavra inteira com BURN_ENDIAN_SWAP_INT16 funciona em ARM64 e big-endian.
UINT16 ReadRamWord(UINT32 off)
{
	return BURN_ENDIAN_SWAP_INT16(*((UINT16*)(CpsRamFF + off)));
}

void WriteRamWord(UINT32 off, UINT16 value)
{
	*((UINT16*)(CpsRamFF + off)) = BURN_ENDIAN_SWAP_INT16(value);
}

bool Rising(UINT8 now, UINT8 previous, UINT8 mask)
{
	return (now & mask) && !(previous & mask);
}

void DrawPixel(INT32 x, INT32 y, UINT32 colour)
{
	if (x < 0 || y < 0 || x >= nCpsScreenWidth || y >= nCpsScreenHeight) return;
	PutPix(pBurnDraw + y * nBurnPitch + x * nBurnBpp, colour);
}

void DrawBox(INT32 x, INT32 y, INT32 width, INT32 height, UINT32 colour)
{
	for (INT32 i = 0; i < width; i++) {
		DrawPixel(x + i, y, colour);
		DrawPixel(x + i, y + height - 1, colour);
	}
	for (INT32 i = 0; i < height; i++) {
		DrawPixel(x, y + i, colour);
		DrawPixel(x + width - 1, y + i, colour);
	}
}

} // namespace

void Cps4039Reset()
{
	sKaelVirtualP1 = false;
	sKaelVirtualP2 = false;
	sPrevP1 = 0;
	sPrevP2 = 0;
}

void Cps4039FrameStart()
{
	if (!IsMvsc4039() || CpsRamFF == NULL) return;

	if (CpsReset) {
		Cps4039Reset();
	}

	// CpsInp001 = P1, CpsInp000 = P2 (bit 0 = direita, bit 1 = esquerda), como em d_cps2.cpp
	const UINT8 p1 = CpsInp001[0];
	const UINT8 p2 = CpsInp000[0];
	const bool p1Right = Rising(p1, sPrevP1, 0x01);
	const bool p1Left  = Rising(p1, sPrevP1, 0x02);
	const bool p2Right = Rising(p2, sPrevP2, 0x01);
	const bool p2Left  = Rising(p2, sPrevP2, 0x02);

	const UINT16 p1pos = ReadRamWord(P1_CURSOR);
	const UINT16 p2pos = ReadRamWord(P2_CURSOR);

	if (!sKaelVirtualP1 && p1Right && p1pos == WOLVERINE_POS) sKaelVirtualP1 = true;
	else if (sKaelVirtualP1 && p1Left)                         sKaelVirtualP1 = false;

	if (!sKaelVirtualP2 && p2Right && p2pos == WOLVERINE_POS) sKaelVirtualP2 = true;
	else if (sKaelVirtualP2 && p2Left)                         sKaelVirtualP2 = false;

	if (sKaelVirtualP1) WriteRamWord(P1_CURSOR, WOLVERINE_POS);
	if (sKaelVirtualP2) WriteRamWord(P2_CURSOR, WOLVERINE_POS);

	sPrevP1 = p1;
	sPrevP2 = p2;
}

void Cps4039CharacterSelectDraw()
{
	if (!pBurnDraw || !IsMvsc4039()) return;

	const UINT32 colour = BurnHighCol(255, 255, 255, 0); // branco
	const INT32 x = 352;
	const INT32 y = 192;
	const INT32 size = 32;

	if (sKaelVirtualP1) DrawBox(x, y, size, size, colour);
	if (sKaelVirtualP2) DrawBox(x + 2, y + 2, size, size, colour);
}

int Cps4039KaelVirtualP1() { return sKaelVirtualP1 ? 1 : 0; }
int Cps4039KaelVirtualP2() { return sKaelVirtualP2 ? 1 : 0; }
