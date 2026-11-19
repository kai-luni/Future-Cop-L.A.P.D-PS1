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
extern const struct Packed9 gCardSlotZeroLabel __asm__("D_80010AE8");
extern const struct Packed9 gCardFileSecondaryLabel __asm__("D_80010AF4");
extern const struct Packed9 gCardOtherSlotLabel __asm__("D_80010B00");
extern const struct Packed6 gCardEmptySlotLabel __asm__("D_8009F260");

extern void gCopyMemory(void *destination, const void *source, s32 length) __asm__("func_8001E318");

void gParseCardDirectory(void) __asm__("func_800207CC");

/**
 * @brief Parses 15 buffered memory-card directory frames and rebuilds the slot
 * state and names.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Initializes slot 0 and walks 0x80-byte frames for slots 1 through 15. XORs
 * the first 127 bytes and overwrites the type with 0x53 on checksum mismatch.
 * Type 0x51 copies a 20-byte filename, while types 0xA0, 0xA1, and 0xA3 set
 * free state 2. Other types receive the fallback label.
 *
 * @par Globals
 * Reads gOperationState and the four labels gCardSlotZeroLabel,
 * gCardFileSecondaryLabel, gCardOtherSlotLabel, and gCardEmptySlotLabel.
 * Writes slot state, both label buffers, and any invalid frame type through
 * the manager.
 *
 * @par Callers
 * Direct calls in the retail listing are from gFindCardFileSlot,
 * gReadCardSlotState, gFindFreeCardBlockRun.
 *
 * @par Role and path
 * Card lookup and allocation paths refresh the in-memory slot summary from the
 * directory frames.
 *
 * @par Unknown
 * The parser assigns generic labels and does not interpret file payloads.
 */
void gParseCardDirectory(void)
{
    struct CardDirectoryFrame *frame;
    struct CardManager *initial;
    u32 slot;

    initial = gOperationState;
    gOperationState->state[0] = 1;
    *gOperationState->primary[0] = gCardSlotZeroLabel;
    __asm__("" ::: "memory");
    frame = initial->directory;
    *gOperationState->secondary[0] = gCardSlotZeroLabel;
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
            *(struct Packed6 *)gOperationState->primary[slot] = gCardEmptySlotLabel;
            __asm__("" ::: "memory");
            *(struct Packed6 *)gOperationState->secondary[slot] = gCardEmptySlotLabel;
            break;
        }
        case 0x51:
            gOperationState->state[slot] = 1;
            gCopyMemory(gOperationState->primary[slot], &frame->bytes[10], 20);
            __asm__("" ::: "memory");
            *gOperationState->secondary[slot] = gCardFileSecondaryLabel;
            break;
        default:
            gOperationState->state[slot] = 1;
            *gOperationState->primary[slot] = gCardOtherSlotLabel;
            __asm__("" ::: "memory");
            *gOperationState->secondary[slot] = gCardOtherSlotLabel;
            break;
        }
        frame++;
    }
}
