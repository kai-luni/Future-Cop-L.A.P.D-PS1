// Categories: Platform > Memory Card | Resources > Save Data
#include "game/types.h"

struct Unk8009AB08 {
    u32 unk0000;
    u8 unk0004[0x8F4];
    s32 unk8F8;
    u8 unk8FC[0x10];
    s32 unk90C;
    s32 unk910;
    u8 unk914[0x80];
    s32 unk994;
    s32 unk998;
    s32 unk99C;
    u8 unk9A0[4];
    s32 unk9A4;
    u8 unk9A8[0x120C];
    s32 unk1BB4;
};

extern struct Unk8009AB08 *D_8009AB08;
extern char D_80010B0C[];

extern s32 func_8008CD30(char *dst, char *fmt, u32 arg2, u32 arg3);
extern s32 func_80020C90(void *arg0);
extern void func_8001F24C(void *arg0, s32 arg1, s32 arg2, s32 arg3);
extern s32 func_80020E68(s32 arg0);
extern void func_80020F34(char *text, s32 index);
extern void func_8001F2E0(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
extern u32 func_8001F37C(void);

/**
 * @brief Queues a memory-card file write (create or overwrite) and returns 0x2000 when the transfer was queued, 0 when refused.
 *
 * @param arg0 File/title name string; the lookup key and the name-packet title.
 * @param arg1 Data pointer stored into the card manager's request word unk910.
 * @param arg2 Transfer size in bytes; rounded up to 128-byte units for the request and to 0x2000-byte blocks for allocation.
 * @return 0x2000 if the request was queued (manager idle, unk9A4 still 0), 0 if the unit was busy or no block run was free.
 *
 * @par Globals
 * D_8009AB08 - card manager: unk0000>>4 feeds the debug format, state[16]/primary[16] are walked by the lookup, unk9A4 is the pending flag read back at the end.
 *
 * @par Control flow
 * Lookup first: an existing name updates in place; otherwise allocate a free block run, build the name packet and queue a new entry; allocation failure returns 0 early. The final flag is (unk9A4 < 1) << 13.
 *
 * @par Calls
 * func_8008CD30 - sprintf of the debug trace "bu0%1ld:%s" into a stack buffer that is not read afterwards. func_80020C90 - find the slot whose state is 1 and name matches. func_8001F24C - requeue the directory request for an existing slot. func_80020E68 - allocate ceil(size/0x2000) consecutive state-2 blocks. func_80020F34 - build the 0x80-byte name packet (header, title, XOR checksum). func_8001F2E0 - queue the new entry. func_8001F37C - read back unk9A4.
 *
 * @par Side effects
 * Via the queue helpers: card manager request words unk998/99C/910/90C, command unk8F8 = 3 and pending flag unk9A4; name packet built for the slot.
 *
 * @par Callers
 * Confirmed: func_80080B70 at call site 0x8008118C (profile-image save loop, passes name D_800120A4, size 0x678). The caller listed in the package work list does not exist in the unit catalog; no such call edge was found.
 */
s32 func_80020FDC(u32 arg0, u32 arg1, u32 arg2)
{
    register u32 p0 __asm__("$18") = arg0;
    register u32 p1 __asm__("$21") = arg1;
    register u32 p2 __asm__("$20") = arg2;
    register s32 result __asm__("$17") = 0;
    register u32 idx __asm__("$19");
    register u32 slot __asm__("$16");
    register u32 tmp __asm__("$2");
    register u32 a0 __asm__("$4");
    char buf[0x100];

    __asm__("" : "=r"(p0) : "0"(p0));
    __asm__("" : "=r"(p1) : "0"(p1));
    __asm__("" : "=r"(p2) : "0"(p2));
    __asm__("" : "=r"(result) : "0"(result));

    func_8008CD30(buf, D_80010B0C, D_8009AB08->unk0000 >> 4, p0);
    a0 = p0;
    __asm__("" : "=r"(a0) : "0"(a0));
    tmp = p2 + 0x7F;
    idx = tmp >> 7;
    slot = func_80020C90(a0);
    if (slot != 0) {
        func_8001F24C(slot << 6, idx, p1, result);
        goto tail;
    }
    slot = func_80020E68(p2);
    if (slot == 0) {
        return result;
    }
    func_80020F34(p0, slot);
    func_8001F2E0(slot << 6, idx, p1, 0);
tail:
    result = (func_8001F37C() < 1) << 13;
    __asm__("" : "=r"(result) : "0"(result));
    return result;
}
