#include "cps.h"
#include "cps2_4039.h"

namespace {
static const UINT32 P1_CURSOR = 0x2000;
static const UINT32 P2_CURSOR = 0x2002;
static const UINT8 WOLVERINE_POS = 5;
static bool sKaelVirtualP1 = false;
static bool sKaelVirtualP2 = false;
static UINT8 sPrevP1 = 0;
static UINT8 sPrevP2 = 0;

static bool IsMvsc4039()
{
    return strcmp(BurnDrvGetTextA(DRV_NAME), "mvsc") == 0;
}

static UINT16 ReadRamWord(UINT32 off)
{
    return ((UINT16)CpsRamFF[off] << 8) | CpsRamFF[off + 1];
}

static void WriteRamWord(UINT32 off, UINT16 v)
{
    CpsRamFF[off] = (UINT8)(v >> 8);
    CpsRamFF[off + 1] = (UINT8)v;
}

static bool Rising(UINT8 now, UINT8 prev, UINT8 mask)
{
    return (now & mask) && !(prev & mask);
}
}

void Cps4039Reset()
{
    sKaelVirtualP1 = false;
    sKaelVirtualP2 = false;
    sPrevP1 = 0;
    sPrevP2 = 0;
}

void Cps4039FrameStart()
{
    if (!IsMvsc4039()) return;
    if (CpsReset) Cps4039Reset();

    const UINT8 p1 = CpsInp001[0];
    const UINT8 p2 = CpsInp000[0];
    const bool p1Right = Rising(p1, sPrevP1, 0x01);
    const bool p1Left  = Rising(p1, sPrevP1, 0x02);
    const bool p2Right = Rising(p2, sPrevP2, 0x01);
    const bool p2Left  = Rising(p2, sPrevP2, 0x02);

    const UINT16 p1pos = ReadRamWord(P1_CURSOR);
    const UINT16 p2pos = ReadRamWord(P2_CURSOR);

    if (!sKaelVirtualP1 && p1Right && p1pos == WOLVERINE_POS) sKaelVirtualP1 = true;
    if (sKaelVirtualP1 && p1Left) sKaelVirtualP1 = false;
    if (!sKaelVirtualP2 && p2Right && p2pos == WOLVERINE_POS) sKaelVirtualP2 = true;
    if (sKaelVirtualP2 && p2Left) sKaelVirtualP2 = false;

    if (sKaelVirtualP1) WriteRamWord(P1_CURSOR, WOLVERINE_POS);
    if (sKaelVirtualP2) WriteRamWord(P2_CURSOR, WOLVERINE_POS);

    sPrevP1 = p1;
    sPrevP2 = p2;
}

void Cps4039CharacterSelectDraw()
{
    if (!IsMvsc4039() || !pBurnDraw) return;
    const int bpp = nBurnBpp;
    const int x = 352, y = 192, w = 32, h = 32;
    auto put = [&](int px, int py) {
        if (px < 0 || py < 0 || px >= nCpsScreenWidth || py >= nCpsScreenHeight) return;
        PutPix(pBurnDraw + py * nBurnPitch + px * bpp, 0xffff);
    };
    if (sKaelVirtualP1) {
        for (int i = 0; i < w; i++) { put(x+i,y); put(x+i,y+h-1); }
        for (int i = 0; i < h; i++) { put(x,y+i); put(x+w-1,y+i); }
    }
    if (sKaelVirtualP2) {
        for (int i = 0; i < w; i++) { put(x+i+2,y+2); put(x+i+2,y+h+1); }
        for (int i = 0; i < h; i++) { put(x+2,y+i+2); put(x+w+1,y+i+2); }
    }
}

int Cps4039KaelVirtualP1() { return sKaelVirtualP1 ? 1 : 0; }
int Cps4039KaelVirtualP2() { return sKaelVirtualP2 ? 1 : 0; }
