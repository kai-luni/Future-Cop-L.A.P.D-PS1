// Categories: Graphics > HUD > Gauge | Gameplay > Player
#include "game/types.h"

struct Unk5A190Gauge {
    s32 unk00;
    s32 value;
    s32 unk08;
    s32 limit;
    u8 state;
    u8 unk11[3];
};

struct Unk5A190 {
    u8 unk00[0x54];
    struct Unk5A190Gauge gauges[1];
};

extern void gPlayGaugeEdgeSound(struct Unk5A190 *object, struct Unk5A190Gauge *gauge, s32 rising) __asm__("func_8005A154");
void gSetEntityGaugeValue(struct Unk5A190 *object, s32 index, s32 value) __asm__("func_8005A190");


/**
 * @brief Sets a gauge value and drives its three-state fill machine.
 *
 * @param object Gauge owner; +0x54 the gauge array.
 * @param index Gauge index.
 * @param value New value (clamped to >= 0).
 * @return Nothing.
 *
 * @par Confirmed
 * value >= limit sets state 2 (full). Coming down from state 2 sets state
 * 1 and fires func_8005A154(object, gauge, 1) (rising edge); from state 1
 * at value <= 0 sets state 0 and fires func_8005A154(object, gauge, 0).
 *
 * @par Likely role
 * The health/energy gauge updater of the player handlers (spawn, tick,
 * retire and func_8002FC90): the edge callbacks likely trigger the HUD
 * flash or the low-energy warning sound.
 *
 * @par Unknown
 * What the rising flag selects in func_8005A154.
 *
 * @par Callers
 * func_8002FC90, func_80058714, func_800588D4, func_80058D78.
 */
void gSetEntityGaugeValue(struct Unk5A190 *object, s32 index, s32 value)
{
    struct Unk5A190Gauge *gauge;

    if (value < 0) {
        value = 0;
    }
    gauge = &object->gauges[index];
    gauge->value = value;
    if (value >= gauge->limit) {
        gauge->state = 2;
        return;
    }
    if (gauge->state == 2) {
        gauge->state = 1;
        gPlayGaugeEdgeSound(object, gauge, 1);
    } else if (gauge->state == 1 && value <= 0) {
        gauge->state = 0;
        gPlayGaugeEdgeSound(object, gauge, 0);
    }
}
