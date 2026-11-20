// Categories: Graphics > Frame Submission | Game Engine > Frame End | Platform > GPU
#include "game/types.h"


extern volatile u8 gFrameBarrier __asm__("D_8009FA3C");
extern u8 gDisplayListHead[] __asm__("D_800A63E0");
extern u8 gVramUploadQueue[] __asm__("D_800A0520");
extern u8 gVramUploadRectFields[] __asm__("D_800A0528");
extern u8 gUploadRing[] __asm__("D_800A0660");
extern u32 gSingleShotQueue[] __asm__("D_8009AA68");
extern s32 gPlayerCount __asm__("D_8009FA0C");

extern u32 gPacketBufferBases[] __asm__("D_8009F5C8");
extern u32 gRenderNodeTable[] __asm__("D_8009F5D0");
extern u32 gOrderingTableBuffers[] __asm__("D_8009F5D8");

extern s32 gVramUploadEndIndex __asm__("D_8009F218");
extern s32 gVramUploadReclaimIndex __asm__("D_8009F21C");
extern s32 gVramUploadSubmitIndex __asm__("D_8009F220");
extern s32 gSubScreenActive __asm__("D_8009F5C0");
extern u32 gPacketPoolBase __asm__("D_8009F5E0");
extern u32 gNodePoolBase __asm__("D_8009F5E4");
extern s32 gUploadRingWriteIdx __asm__("D_8009F5E8");
extern s32 gMdecUploadReclaimIndex __asm__("D_8009F5EC");
extern s32 gMdecUploadSubmitIndex __asm__("D_8009F5F0");
extern s32 gFrameBlockCount __asm__("D_8009F5F4");
extern s32 gNodeCapacity __asm__("D_8009F5F8");
extern s32 gNodeTableLength __asm__("D_8009F5FC");
extern s32 gNodeTableIndex __asm__("D_8009F600");
extern u32 gPrimPoolNext __asm__("D_8009F9F0");
extern u32 gPacketCursor __asm__("D_8009FA90");
extern s32 gScreenOffsetValue __asm__("D_8009F9D4");
extern u32 gPacketLimit __asm__("D_8009F9C0");
extern u32 gPrimPoolEnd __asm__("D_8009FA28");

extern u8 gGetSelectedLayer(void) __asm__("func_8001C034");
extern s32 gGteLeadingZeros(s32 value) __asm__("func_80095C14");
extern void func_80029650(u32 arg0);
extern void gLoadImage(u32 arg0, u32 arg1) __asm__("func_80092F38");
extern void func_80092C8C(s32 arg0);
extern void gClearOrderingTableReverse(u32 arg0, s32 arg1) __asm__("func_80093178");
extern void func_80093280(u32 arg0);
extern void func_8009398C(u32 arg0, void *arg1);
extern void func_80093A0C(u32 arg0, void *arg1);
extern void gDecodeMdecFrameToVram(u32 arg0, u32 arg1, u32 arg2, u32 arg3) __asm__("func_800861E4");

void gSubmitRenderFrame(s32 arg0) __asm__("func_8001B4A8");

/**
 * @brief Builds and submits an ordering table, services deferred GPU work, and
 * swaps render buffers.
 *
 * @param arg0 Nonzero selects a 320-wide clip and raises the frame barrier
 *     after submission. Zero selects the conditional 160-wide path.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Waits for the frame barrier, then walks the two active pools backward at
 * strides 0x34 and 0x1C to find nonzero 24-bit depth extrema. Derives a bucket
 * shift with the leading-zero helper and relinks packet tags by depth.
 * Conditionally inserts clipping commands, drains upload and reclamation
 * cursors, and submits the ordering table. Toggles the buffer index and resets
 * the next frame's bases, cursors, and bounds.
 *
 * @par Globals
 * Reads gPacketBufferBases, gRenderNodeTable, gOrderingTableBuffers,
 * gFrameBlockCount, gNodeCapacity, gNodeTableLength, gSubScreenActive, and
 * D_8009FA0C. Reads or updates gFrameBarrier, gDisplayListHead,
 * gScreenOffsetValue, gPacketPoolBase, gNodePoolBase, gPrimPoolNext,
 * gPacketCursor, gPacketLimit, gPrimPoolEnd, and gNodeTableIndex. Services
 * gVramUploadQueue, gVramUploadRectFields, gVramUploadEndIndex,
 * gVramUploadReclaimIndex, gVramUploadSubmitIndex, gSingleShotQueue,
 * gUploadRing, gUploadRingWriteIdx, gMdecUploadReclaimIndex, and
 * gMdecUploadSubmitIndex.
 *
 * @par Callers
 * Direct calls in the retail listing are from gEndSubScreenFrame,
 * func_80025348, gShowLoadingScreen, gDrawScreenFadeQuad,
 * gPrepareAndSubmitFrame, func_80027EBC, gRunGameFrameLoop, func_80078DD0.
 *
 * @par Role and path
 * Frame-end callers use this path to turn emitted primitives into a GPU list
 * and advance the double buffer.
 *
 * @par Unknown
 * The full meanings of all values of the reserved word D_8009FA0C and the
 * exact ownership of deferred resources depend on their producers.
 */
void gSubmitRenderFrame(s32 arg0)
{
    register s32 max __asm__("$16");
    register u32 q __asm__("$17");
    register u32 p __asm__("$18");
    register s32 min __asm__("$19");
    register u32 limY __asm__("$20");
    register u32 limX __asm__("$21");
    register u32 h __asm__("$22");
    register u32 table __asm__("$23");
    s32 count;
    register s32 v __asm__("$2");
    register s32 xbase __asm__("$3");
    register s32 shift __asm__("$7");
    register u32 mask __asm__("$6");
    register s32 arg9 __asm__("$9");

    u32 pcur;
    u32 qcur;
    register u32 av __asm__("$5");
    register u32 bv __asm__("$4");
    register u32 val __asm__("$8");
    u32 ptr;
    s32 quot;
    s32 idx;
    s16 rect2[2];
    s16 rect[4];
    u32 e;
    u32 hnd;
    s32 dx;
    s32 dy;

    while (gFrameBarrier) {
    }

    __asm__("" : "+m"(arg0));
    h = gGetSelectedLayer();
    if (h != 0) {
        h = 0x100;
    }

    xbase = gPrimPoolNext;
    v = 0x100 - h;
    gScreenOffsetValue = v;
    v = gPacketCursor;
    limX = gPacketPoolBase;
    limY = gNodePoolBase;

    p = xbase - 0x34;
    pcur = p;
    q = v - 0x1C;
    qcur = q;

    if (p < limX && q < limY) {
        goto done;
    }

    max = 0xFFC00000;
    min = 0x400000;

    if (p >= limX) {
        do {
            v = *(s32 *)pcur;
            pcur -= 0x34;
            xbase = v & 0xFFFFFF;
            if (xbase != 0) {
                if (max < xbase) {
                    max = xbase;
                }
                if (xbase < min) {
                    min = xbase;
                }
            }
        } while (pcur >= limX);
    }

    if (qcur >= limY) {
        do {
            v = *(s32 *)qcur;
            qcur -= 0x1C;
            xbase = v & 0xFFFFFF;
            if (xbase != 0) {
                if (max < xbase) {
                    max = xbase;
                }
                if (xbase < min) {
                    min = xbase;
                }
            }
        } while (qcur >= limY);
    }

    v = max - 1;
    quot = (gNodeTableLength + v - min) / gNodeTableLength;
    shift = 1;
    if (quot != 0) {
        v = gGteLeadingZeros(quot);
        xbase = 0x20;
        shift = xbase - v;
    }

    if (max != 0xFFC00000) {
        v = max - min;
        v = v >> shift;
        count = v + 1;
    } else {
        count = 0;
    }

    mask = 0xFF0000;
    v = gNodeTableIndex;
    __asm__("" : "+r"(mask) : "r"(v));
    mask |= 0xFFFF;
    xbase = (s32)gOrderingTableBuffers;
    v = v << 2;
    v = v + xbase;
    table = *(u32 *)v;

    if (q >= limY) {
        do {
            v = *(u32 *)q;
            max = v & mask;
            v ^= max;
            if (max != 0) {
                max = ((max - min) >> shift) + 1;
            }
            xbase = max << 2;
            xbase = xbase + table;
            av = *(u32 *)xbase;
            bv = av & mask;
            av ^= bv;
            v = v | bv;
            *(u32 *)q = v;
            v = q & mask;
            q -= 0x1C;
            v = av | v;
            *(u32 *)xbase = v;
        } while (q >= limY);
    }

    if (p >= limX) {
        do {
            v = *(u32 *)p;
            max = v & mask;
            v ^= max;
            if (max != 0) {
                max = ((max - min) >> shift) + 1;
            }
            xbase = max << 2;
            xbase = xbase + table;
            av = *(u32 *)xbase;
            bv = av & mask;
            av ^= bv;
            v = v | bv;
            *(u32 *)p = v;
            v = p & mask;
            p -= 0x34;
            v = av | v;
            *(u32 *)xbase = v;
        } while (p >= limX);
    }

    if (gPlayerCount >= 2 || ({ arg9 = arg0; __asm__ volatile("" : "+r"(arg9)); arg9 != 0; })) {
        arg9 = arg0;
        limX = 0;
        if (arg9 == 0 && gSubScreenActive != 0) {
            limX = 0xA0;
        }

        rect[0] = limX;
        rect[1] = h;
        rect[2] = (arg9 != 0) ? 0x140 : 0xA0;
        rect[3] = 0xF0;

        q = (u32)gDisplayListHead;
        func_8009398C(q, rect);

        p = 0xFFFFFF;
        mask = q & p;
        __asm__ volatile("" : "+r"(q) : "r"(count));
        q += 0x1C;
        __asm__("" : "+r"(q) : "r"(count));
        bv = q;
        __asm__("" : : "r"(bv));
        max = (count << 2) + table;
        min = 0xFF000000;
        xbase = *(u32 *)gDisplayListHead;
        v = *(u32 *)(max + 4);
        *(u32 *)gDisplayListHead = (xbase & min) | (v & p);
        v = *(volatile u32 *)(max + 4);
        *(u32 *)(max + 4) = (v & min) | mask;

        rect2[0] = limX;
        rect2[1] = h;
        func_80093A0C(q, rect2);

        xbase = *(u32 *)q;
        v = *(u32 *)(max + 4);
        *(u32 *)q = (xbase & min) | (v & p);
        v = *(volatile u32 *)(max + 4);
        *(u32 *)(max + 4) = (v & min) | (q & p);
    }

    if (gVramUploadReclaimIndex != gVramUploadSubmitIndex) {
        do {
            v = (u32)gVramUploadQueue + (gVramUploadReclaimIndex << 4);
            hnd = *(u32 *)(v + 4);
            if (hnd != 0) {
                func_80029650(hnd);
            }
            gVramUploadReclaimIndex++;
            if (gVramUploadReclaimIndex >= 0x14) {
                gVramUploadReclaimIndex = 0;
            }
        } while (gVramUploadReclaimIndex != gVramUploadSubmitIndex);
    }

    if (gVramUploadSubmitIndex != gVramUploadEndIndex) {
        ptr = (u32)gVramUploadRectFields;
        q = ptr - 8;
        do {
            bv = gVramUploadSubmitIndex << 4;
            v = bv + q;
            av = *(u32 *)v;
            gLoadImage(bv + ptr, av);
            func_80092C8C(0);
            gVramUploadSubmitIndex++;
            if (gVramUploadSubmitIndex >= 0x14) {
                gVramUploadSubmitIndex = 0;
            }
        } while (gVramUploadSubmitIndex != gVramUploadEndIndex);
    }

    if (gSingleShotQueue[0] != 0) {
        gLoadImage((u32)gSingleShotQueue + 8, gSingleShotQueue[0]);
        func_80092C8C(0);
        if (gSingleShotQueue[1] != 0) {
            func_80029650(gSingleShotQueue[1]);
        }
        gSingleShotQueue[0] = 0;
    }

    if (({ arg9 = arg0; __asm__ volatile("" : "+r"(arg9)); arg9 != 0; }) || gSubScreenActive == 0 || gPlayerCount == 1) {
        if (gMdecUploadReclaimIndex != gMdecUploadSubmitIndex) {
            do {
                v = (u32)gUploadRing + gMdecUploadReclaimIndex * 20;
                hnd = *(u32 *)(v + 4);
                if (hnd != 0) {
                    func_80029650(hnd);
                }
                gMdecUploadReclaimIndex++;
                if (gMdecUploadReclaimIndex >= 4) {
                    gMdecUploadReclaimIndex = 0;
                }
            } while (gMdecUploadReclaimIndex != gMdecUploadSubmitIndex);
        }

        if (gMdecUploadSubmitIndex != gUploadRingWriteIdx) {
            do {
                xbase = (u32)gUploadRing + gMdecUploadSubmitIndex * 20;
                gDecodeMdecFrameToVram(*(u32 *)xbase, *(u8 *)(xbase + 8),
                              *(u32 *)(xbase + 0xC), *(u32 *)(xbase + 0x10));
                func_80092C8C(0);
                gMdecUploadSubmitIndex++;
                if (gMdecUploadSubmitIndex >= 4) {
                    gMdecUploadSubmitIndex = 0;
                }
            } while (gMdecUploadSubmitIndex != gUploadRingWriteIdx);
        }
    }

    max = count + 1;
    func_80092C8C(0);
    func_80093280(table + (max << 2));

    if (({ arg9 = arg0; __asm__ volatile("" : "+r"(arg9)); arg9 != 0; })) {
        gFrameBarrier = 1;
    }

    mask = 1;
    v = gNodeTableIndex;
    __asm__ volatile("" : "+r"(mask) : "r"(v));
    xbase = (s32)gRenderNodeTable;
    mask -= v;
    av = mask << 2;
    v = (s32)gOrderingTableBuffers + av;
    bv = *(u32 *)v;
    __asm__ volatile("" : : "r"(v));
    v = (s32)gPacketBufferBases + av;
    av += xbase;
    shift = *(u32 *)v;
    val = *(u32 *)av;
    __asm__("" : : "r"(av));
    av = gNodeTableLength;
    xbase = gFrameBlockCount;
    gNodeTableIndex = mask;
    v = xbase * 52;
    xbase = gNodeCapacity;
    __asm__("" : : "r"(xbase));
    v -= 0x34;
    gPacketPoolBase = shift;
    gPrimPoolNext = shift;
    shift += v;
    gNodePoolBase = val;
    gPacketCursor = val;
    gPrimPoolEnd = shift;
    v = xbase * 28;
    v -= 0x1C;
    val += v;
    gPacketLimit = val;
    gClearOrderingTableReverse(bv, av + 2);
done:
    return;
}
