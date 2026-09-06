#include "cps.h"
#include "cps2_4039.h"
#include <string.h>

namespace {
static const UINT32 P1_CURSOR = 0x2000;
static const UINT32 P2_CURSOR = 0x2002;
static const UINT16 WOLVERINE_POS = 5;
static bool sKaelVirtualP1 = false;
static bool sKaelVirtualP2 = false;
static UINT8 sPrevP1 = 0;
static UINT8 sPrevP2 = 0;

static bool IsMvsc4039()
{
\treturn strcmp(BurnDrvGetTextA(DRV_NAME), "mvsc") == 0;
}

static UINT16 ReadRamWord(UINT32 off)
{
\treturn ((UINT16)CpsRamFF[off] << 8) | CpsRamFF[off + 1];
}

static void WriteRamWord(UINT32 off, UINT16 value)
{
\tCpsRamFF[off] = (UINT8)(value >> 8);
\tCpsRamFF[off + 1] = (UINT8)value;
}

static bool Rising(UINT8 now, UINT8 previous, UINT8 mask)
{
\treturn (now & mask) && !(previous & mask);
}

static void DrawPixel(INT32 x, INT32 y, UINT32 colour)
{
\tif (!pBurnDraw) return;
\tif (x < 0 || y < 0 || x >= nCpsScreenWidth || y >= nCpsScreenHeight) return;
\tPutPix(pBurnDraw + y * nBurnPitch + x * nBurnBpp, colour);
}

static void DrawBox(INT32 x, INT32 y, INT32 width, INT32 height, UINT32 colour)
{
\tINT32 i;
\tfor (i = 0; i < width; i++) {
\t\tDrawPixel(x + i, y, colour);
\t\tDrawPixel(x + i, y + height - 1, colour);
\t}
\tfor (i = 0; i < height; i++) {
\t\tDrawPixel(x, y + i, colour);
\t\tDrawPixel(x + width - 1, y + i, colour);
\t}
}
}

void Cps4039Reset()
{
\tsKaelVirtualP1 = false;
\tsKaelVirtualP2 = false;
\tsPrevP1 = 0;
\tsPrevP2 = 0;
}

void Cps4039FrameStart()
{
\tif (!IsMvsc4039()) return;

\tif (CpsReset) {
\t\tCps4039Reset();
\t}

\tconst UINT8 p1 = CpsInp001[0];
\tconst UINT8 p2 = CpsInp000[0];
\tconst bool p1Right = Rising(p1, sPrevP1, 0x01);
\tconst bool p1Left  = Rising(p1, sPrevP1, 0x02);
\tconst bool p2Right = Rising(p2, sPrevP2, 0x01);
\tconst bool p2Left  = Rising(p2, sPrevP2, 0x02);
\n\tconst UINT16 p1pos = ReadRamWord(P1_CURSOR);
\tconst UINT16 p2pos = ReadRamWord(P2_CURSOR);
\n\tif (!sKaelVirtualP1 && p1Right && p1pos == WOLVERINE_POS) {
\t\tsKaelVirtualP1 = true;
\t}
\tif (sKaelVirtualP1 && p1Left) {
\t\tsKaelVirtualP1 = false;
\t}
\tif (!sKaelVirtualP2 && p2Right && p2pos == WOLVERINE_POS) {
\t\tsKaelVirtualP2 = true;
\t}
\tif (sKaelVirtualP2 && p2Left) {
\t\tsKaelVirtualP2 = false;
\t}
\n\tif (sKaelVirtualP1) WriteRamWord(P1_CURSOR, WOLVERINE_POS);
\tif (sKaelVirtualP2) WriteRamWord(P2_CURSOR, WOLVERINE_POS);
\n\tsPrevP1 = p1;
\tsPrevP2 = p2;
}

void Cps4039CharacterSelectDraw()
{
\tif (!IsMvsc4039() || !pBurnDraw) return;
\n\tconst UINT32 colour = BurnHighCol(255, 255, 255, 0);
\tconst INT32 x = 352;
\tconst INT32 y = 192;
\tconst INT32 size = 32;
\n\tif (sKaelVirtualP1) {
\t\tDrawBox(x, y, size, size, colour);
\t}
\tif (sKaelVirtualP2) {
\t\tDrawBox(x + 2, y + 2, size, size, colour);
\t}
}

int Cps4039KaelVirtualP1() { return sKaelVirtualP1 ? 1 : 0; }
int Cps4039KaelVirtualP2() { return sKaelVirtualP2 ? 1 : 0; }
