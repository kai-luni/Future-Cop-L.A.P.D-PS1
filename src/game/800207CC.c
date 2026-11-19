// Categories: Platform > Memory Card | Resources > Save Data
#include "game/types.h"

struct Packed6 {
    u8 bytes[6];
};

struct Packed9 {
    u8 bytes[9];
};

struct CardDirectoryFrame {
    u8 bytes[0x80];
};

struct CardManager {
    s32 unk00;
    s32 state[16];
    struct Packed9 *primary[16];
    struct Packed9 *secondary[16];
    u8 unk0C4[0x96C];
    struct CardDirectoryFrame directory[15];
};

extern struct CardManager *gOperationState __asm__("D_8009AB08");
extern const struct Packed9 D_80010AE8;
extern const struct Packed9 D_80010AF4;
extern const struct Packed9 D_80010B00;
extern const struct Packed6 D_8009F260;

extern void gCopyMemory(void *destination, const void *source, s32 length) __asm__("func_8001E318");

/**
 * @brief Parses the 15 memory-card directory frames in D_8009AB08 into slot state and names.
 *
 * Sets slot 0 state = 1 with default label D_80010AE8, then iterates slots 1..15 over D_8009AB08->directory[0..14]: verifies the 0x7F-byte XOR checksum (forcing type 0x53 on mismatch), marks 0xA0/0xA1/0xA3 free slots as state 2 (D_8009F260), copies 20-byte save names for 0x51 slots (state 1, D_80010AF4), and sets D_80010B00 otherwise.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009AB08 - CardManager struct (state[16], primary/secondary[16], directory[15]); D_80010AE8/D_80010AF4/D_80010B00/D_8009F260 - status label strings.
 *
 * @par Control flow
 * Outer 15-slot loop containing a 127-byte XOR checksum loop, checksum mismatch override, and a 3-way switch on frame->bytes[0].
 *
 * @par Calls
 * func_8001E318 - copies 20-byte filename from frame->bytes[10] into primary[slot] for 0x51 entries.
 *
 * @par Side effects
 * Mutates corrupted frame->bytes[0] to 0x53 and updates D_8009AB08 state, primary, and secondary slot labels.
 *
 * @par Callers
 * Confirmed: func_80020C90 (0x80020CC0), func_80020DF8 (0x80020E2C), func_80020E68 (0x80020E90) / Likely: none / Unknown: none.
 */
void func_800207CC(void)
{
    struct CardDirectoryFrame *frame;
    struct CardManager *initial;
    u32 slot;

    initial = gOperationState;
    gOperationState->state[0] = 1;
    *gOperationState->primary[0] = D_80010AE8;
    __asm__("" ::: "memory");
    frame = initial->directory;
    *gOperationState->secondary[0] = D_80010AE8;
    for (slot = 1; slot < 16; slot++) {
        u8 checksum;
        u8 *cursor;
        u32 i;

        cursor = frame->bytes;
        checksum = 0;
        i = 0;
        do {
            checksum ^= *cursor++;
            i++;
        } while (i < 0x7F);
        if (*cursor != checksum) {
            frame->bytes[0] = 0x53;
        }

        switch (frame->bytes[0]) {
        case 0xA0:
        case 0xA1:
        case 0xA3: {
            gOperationState->state[slot] = 2;
            *(struct Packed6 *)gOperationState->primary[slot] = D_8009F260;
            __asm__("" ::: "memory");
            *(struct Packed6 *)gOperationState->secondary[slot] = D_8009F260;
            break;
        }
        case 0x51:
            gOperationState->state[slot] = 1;
            gCopyMemory(gOperationState->primary[slot], &frame->bytes[10], 20);
            __asm__("" ::: "memory");
            *gOperationState->secondary[slot] = D_80010AF4;
            break;
        default:
            gOperationState->state[slot] = 1;
            *gOperationState->primary[slot] = D_80010B00;
            __asm__("" ::: "memory");
            *gOperationState->secondary[slot] = D_80010B00;
            break;
        }
        frame++;
    }
}
