// Categories: Graphics > Display Setup | Game Engine > Frame
#include "game/types.h"

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
} Unk800A3578;

extern Unk800A3578 gSlotRecordTable[] __asm__("D_800A3578");
void gQueueStatusLineText(s32 arg0, s32 arg1, s32 arg2) __asm__("func_80078B34");


/**
 * @brief Queues a display id into one of the two HUD display slots.
 *
 * @param arg0 The display/text id to queue.
 * @param arg1 Slot index (0 or 1); >= 2 returns without effect.
 * @param arg2 Mode: 1 writes the secondary pair (0x55 marker), 2 the
 *        tertiary pair (0xEB marker), default the primary pair (0xA0 marker).
 * @return Nothing.
 *
 * @par Confirmed
 * The slot record D_800A3578[arg1] holds three {marker, id} pairs; the mode
 * selects which pair receives the id. The status pass func_80078B98 draws
 * the primary pair of both slots each frame.
 *
 * @par Likely role
 * The universal prompt/message queue of the front end and HUD: every screen
 * handler queues its "press start"/warning ids here instead of drawing them
 * directly, so the frame close renders them in a fixed order.
 *
 * @par Unknown
 * What the secondary/tertiary pairs (0x55/0xEB markers) render as.
 *
 * @par Callers
 * func_8007A2C4, func_8007B17C, func_8007B854, func_8007BCF4, func_8007C6F0,
 * func_8007CFA0, func_8007DD08, func_8007E67C, func_8007F1F8, func_80080038.
 */
void gQueueStatusLineText(s32 arg0, s32 arg1, s32 arg2)
{
    Unk800A3578 *p;
    register s32 temp __asm__("$7");

    temp = arg0;
    if (arg1 >= 2) {
        return;
    }
    p = &gSlotRecordTable[arg1];
    switch (arg2) {
    case 1:
        p->unk2 = 0x55;
        p->unk0 = arg0;
        break;
    case 2:
        p->unk6 = 0xEB;
        p->unk4 = arg0;
        break;
    default:
        p->unk2 = 0xA0;
        p->unk0 = temp;
        break;
    }
}
