// Categories: Platform > Memory Card | Resources > Save Data
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

extern u8 *gOperationState __asm__("D_8009AB08");
extern void *gFillMemory(void *destination, int value, unsigned int count) __asm__("func_8001E4C0");
extern void gInitCardReplacementFrames(void *records) __asm__("func_80020B28");
extern s32 gIsMcManagerBusy(void) __asm__("func_8001F180");

int gPrepareCardFormat(void) __asm__("func_80020B64");

/**
 * @brief Builds a blank memory-card system area and stages its write when the
 * manager is idle.
 * @return Zero after staging the format request, or one if the manager is
 *     busy.
 *
 * @par Confirmed mechanics
 * Clears 0x1200 bytes, writes the MC marker and free-directory records, and
 * writes their XOR checksums. Initializes replacement frames. On an idle
 * result, fills sector bounds 1 through 0x23 and selects operation 5.
 *
 * @par Globals
 * Reads gOperationState and updates its system area and operation fields.
 *
 * @par Callers
 * Direct calls in the retail listing are from gQueueMemoryCardFormat.
 *
 * @par Role and path
 * The card management screen uses this routine to prepare a format request.
 *
 * @par Unknown
 * The later state machine controls completion of the write.
 */
int gPrepareCardFormat(void)
{
    u8 *records;
    u8 *record;
    u8 *end;
    u8 *p;
    u32 off;
    u32 i;
    u32 checksum;
    u32 b;
    u32 tt;
    u8 *g;
    u8 *base;
    u8 *first;
    register u32 h1 __asm__("$13");
    register u32 h2 __asm__("$12");
    register u32 mk __asm__("$11");
    register u32 ff __asm__("$10");

    base = gOperationState;
    records = base + 0x9B0;
    gFillMemory(records, 0, 0x1200);
    h1 = 0x4D;
    h2 = 0x43;
    mk = 0xA0;
    ff = 0xFFFF;
    __asm__ volatile("" : "=r"(h1), "=r"(h2), "=r"(mk), "=r"(ff) : "0"(h1), "1"(h2), "2"(mk), "3"(ff));
    off = 0;
    record = records;
    first = records;
    end = base + 0x11B0;
    do {
        if (record == first) {
            *(volatile u8 *)(gOperationState + 0x9B0) = h1;
            *(volatile u8 *)(gOperationState + 0x9B1) = h2;
        } else {
            record[0] = mk;
            *(u16 *)(record + 8) = ff;
        }
        checksum = 0;
        __asm__("" : "=r"(checksum) : "0"(checksum));
        p = records + off;
        for (i = 0; i < 0x7F; i++) {
            b = *p++;
            tt = checksum ^ b;
            checksum = tt;
        }
        *p = tt;
        off += 0x80;
        record += 0x80;
    } while (record < end);
    gInitCardReplacementFrames(records);
    if (gIsMcManagerBusy() != 0) {
        return 1;
    }
    g = gOperationState;
    *(s32 *)(g + 0x998) = 1;
    *(s32 *)(g + 0x99C) = 0x23;
    *(s32 *)(g + 0x910) = (s32)(g + 0xA30);
    *(s32 *)(g + 0x90C) = 0;
    *(s32 *)(g + 0x994) = 0;
    *(s32 *)(g + 0x9A4) = 0;
    *(s32 *)(g + 0x1BB4) = 0;
    *(s32 *)(g + 0x8F8) = 5;
    return 0;
}
