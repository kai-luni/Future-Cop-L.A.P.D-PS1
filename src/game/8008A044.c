// Categories: Input > DualShock | Platform > Controller
typedef unsigned char u8;
struct Entry { u8 a, b, c, d, e; };
struct Data { void *pad; struct Entry *entries; u8 pad08[0xE1]; u8 count; };
extern struct Data *(*D_8009C73C)(int);

/**
 * @brief Queries actuator count or a single capability field for a controller port.
 *
 * Confirmed: resolves port via D_8009C73C(arg0); if index < 0 returns data->count (+0xE9); if index < data->count, indexes 5-byte entry data->entries[index] (+0x04) and returns byte field a..e for kind 1..5, else 0.
 * Category follows PadCapabilityRecord layout populated by func_8008B210 step 3 and DualShock rumble initialization in func_8001EC94.
 * Likely: PSX pad library PadInfoAct wrapper returning actuator count (index < 0) or descriptor field (kind 1=func, 2=subfunc, 3=size, 4=current, 5=high_bit); Unknown: none.
 *
 * @param arg0 Controller port/slot index passed to D_8009C73C.
 * @param index Actuator index (< 0 queries total actuator count at +0xE9).
 * @param kind Descriptor field selector 1..5 (entries[index].a..e).
 * @return Actuator count when index < 0, selected u8 descriptor field for valid index and kind 1..5, or 0 otherwise.
 *
 * @par Globals
 * D_8009C73C - indirect port lookup function pointer (bound to func_8008C3EC in func_8008BDB0).
 * @par Control flow
 * Early return for index < 0; bounds guard index < data->count followed by a 5-case switch on kind (1..5), defaulting to 0.
 * @par Calls
 * Indirect call via D_8009C73C(arg0) to obtain the port Data pointer.
 * @par Side effects
 * None observed (read-only query).
 * @par Callers
 * Confirmed: func_8001EC94 (0x8001EE00 with index=-1 to get count; 0x8001EE38 with kind=3 to inspect actuator payload type) / Likely: none / Unknown: none.
 */
int func_8008A044(int arg0, int index, int kind)
{
    struct Data *data = D_8009C73C(arg0);
    struct Entry *entry;
    if (index < 0) return data->count;
    if (index < data->count) {
        entry = &data->entries[index];
        switch (kind) {
        case 1: return entry->a;
        case 2: return entry->b;
        case 3: return entry->c;
        case 4: return entry->d;
        case 5: return entry->e;
        }
    }
    __asm__ volatile (".set reorder");
    return 0;
}
