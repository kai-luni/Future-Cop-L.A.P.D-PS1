// Categories: Game Engine > Data Access
typedef unsigned char u8;

typedef struct {
    u8 pad00[0x28];
    u8 *data0;
    u8 *data1;
    u8 pad30[4];
    u8 count0;
    u8 count1;
    u8 mode;
    u8 pad37[0xe];
    u8 index;
    u8 pad46[0x11];
    u8 enabled[6];
} Object;
int gReadPadTxByte(Object *obj) __asm__("func_8008C07C");


/**
 *  @brief Reads a mode-dependent byte from an object's data arrays.
 *
 *  Subtracts three from the stored index and selects an array, enable check,
 *  and out-of-range fallback according to the object's mode.
 *  @param obj Object containing the mode, index, arrays, counts, and enable bytes.
 *  @return The selected byte, or 0xFF for an out-of-range index in mode 0x4D; otherwise 0 on rejection.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  Mode zero checks enable bytes for indices below six; mode 0x4D uses data1.
 *  Other modes also use data1; the code does not reject negative adjusted indices.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None; reads in modes zero and 0x4D use a volatile-qualified pointer.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Computes index = obj->index - 3 and selects a byte according to obj->mode:
 * mode 0 checks obj->enabled[index] for indices below 6 and reads
 * obj->data0[index] when within obj->count0, mode 0x4D reads
 * obj->data1[index] or returns 0xFF when out of range, and default reads
 * obj->data1[index] or returns 0. Bound to gPadTxByteHook (D_8009C730) by
 * func_8008BDB0.
 */
int gReadPadTxByte(Object *obj)
{
    int index = obj->index - 3;
    volatile u8 *p;
    switch (obj->mode) {
    case 0:
        if (index < 6 && !obj->enabled[index]) return 0;
        if (index < obj->count0) {
            p = obj->data0 + index;
load:
            return *p;
        }
        return 0;
    case 0x4d:
        if (index < obj->count1) {
            p = obj->data1 + index;
            goto load;
        }
        return 0xff;
    default:
        if (index < obj->count1) return obj->data1[index];
        return 0;
    }
}
