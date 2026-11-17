// Categories: Platform > CD-ROM | Resources > File Access
typedef int s32;
typedef unsigned char u8;
typedef struct { volatile s32 state; volatile s32 pos; s32 value; s32 copy; s32 mode; s32 other; } Entry;
typedef struct { u8 loc[4]; s32 value; } File;
extern Entry D_800A6D78[];
extern char gFileOpenResult[] __asm__("D_800A6DD8");
extern File gFileTable[] __asm__("D_800A6E20");
extern s32 func_8001CE10(void);
extern s32 func_8001DC48(s32, s32, void *, char *);
extern s32 func_8001C478(s32);
extern s32 func_8008FF20(File *, char *);
extern s32 func_8001C588(File *);
/**
 * @brief Opens a file and reserves one of the four file slots.
 *
 * @param arg0 File name or index handed to func_8001DC48.
 * @param arg1 Open mode. The low nibble is normalised: 3 becomes 1 and 2
 *             becomes 1; negative values are made positive first. Only 1 and 17
 *             survive, every other mode returns -6.
 * @param arg2 Buffer/private pointer passed to the open call.
 *
 * @return The slot index 0..3 on success, or one of -2 (device not ready),
 *         -23 (a slot holds an invalid state), -10 (all four slots busy),
 *         -6 (unsupported mode), -5 (the file table lookup failed after five
 *         attempts).
 *
 * @par Confirmed flow
 * 1. func_8001CE10() < 0 means the device layer is not ready: -2.
 * 2. Scan the four slots D_800A6D78 (0x18 bytes each): the first one with
 *    state == 0 is used; a state whose low byte is negative or >= 4 is corrupt
 *    and returns -23; all four busy returns -10.
 * 3. func_8001DC48(mode, arg0, arg2, D_800A6DD8) opens the file; a negative
 *    result is returned unchanged.
 * 4. func_8001C478(mode) applies the mode, then the file table D_800A6E20 is
 *    searched for the entry with func_8008FF20 up to five times (a retry
 *    loop for "entry not yet visible").
 * 5. On a hit the slot is filled: pos = func_8001C588(table), value =
 *    table[0].value, copy = pos, mode = arg1, state = 1, and the slot index is
 *    returned.
 *
 * @par Globals
 * D_800A6D78 (four slots: state, pos, value, copy, mode, other),
 * D_800A6DD8 (name buffer), D_800A6E20 (open-file table).
 *
 * @par Callers
 * func_8001D5E4 (the name-directory search) and func_8001E110.
 */
s32 func_8001D424(s32 arg0, s32 arg1, void *arg2)
{
    s32 i, j, ret;
    register s32 mode __asm__("$4");
    register s32 argument __asm__("$5");
    register s32 three __asm__("$2");
    Entry *entry;
    register char *name __asm__("$20");
    if (func_8001CE10() < 0) return -2;
    i = 0;
    entry = D_800A6D78;
    __asm__ volatile ("" : "+r" (entry));
    while (i < 4) {
        if (!entry->state) break;
        if ((entry->state & 255) < 0 || (entry->state & 255) >= 4) return -23;
        i++;
        entry++;
    }
    if (i == 4) { three = 3; __asm__ volatile ("" : : "r" (three)); return -10; }
    __asm__ volatile ("" : "=r" (three));
    if (arg1 < 0) arg1 = -arg1;
    __asm__ volatile ("" : "+r" (arg1));
    if ((arg1 & 15) == three) arg1 = 1;
    mode = arg1;
    if ((arg1 & 15) == 2) { arg1 = 1; mode = arg1; }
    __asm__ volatile ("" : "+r" (arg1));
    argument = arg0;
    __asm__ volatile ("" : "+r" (argument));
    name = (char *)0x800a0000;
    __asm__ volatile ("" : "+r" (name));
    ret = func_8001DC48(mode, argument, arg2, name + 0x6dd8);
    if (ret < 0) return ret;
    if (arg1 != 1 && arg1 != 17) return -6;
    ret = func_8001C478(arg1);
    if (ret < 0) return ret;
    j = 0;
    do {
        if (func_8008FF20(gFileTable, name + 0x6dd8)) break;
        j++;
    } while (j < 5);
    if (j != 5) {
        register s32 value __asm__("$2");
        register s32 copy __asm__("$4");
        D_800A6D78[i].pos = func_8001C588(gFileTable);
        value = gFileTable[0].value;
        copy = D_800A6D78[i].pos;
        D_800A6D78[i].mode = arg1;
        D_800A6D78[i].value = value;
        D_800A6D78[i].copy = copy;
        {
            register s32 state __asm__("$2");
            state = 1;
            D_800A6D78[i].state = state;
        }
        __asm__ volatile ("" : : "r" (copy));
        return i;
    }
    __asm__ volatile ("");
    return -5;
}
