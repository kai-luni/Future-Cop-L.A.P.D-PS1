// Categories: Game Engine > Data Access
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
struct Data {
    u16 *table;
    u8 pad04[0x48];
    u32 field4c;
    u8 pad50[0x93];
    u8 count;
    u8 fieldE4;
    u8 padE5;
    u16 fieldE6;
    u8 fieldE8;
};
extern struct Data *(*gPadLookupPortFn)(int) __asm__("D_8009C73C");
int gQueryPadInfo(int arg0, int kind, int index) __asm__("func_80089F4C");


/**
 *  @brief Retrieves a selected field or table entry from a data record.
 *
 *  Resolves the record through a global function pointer and selects by kind.
 *  Kind 4 returns the table count for a negative index or an in-range entry.
 *  @param arg0 Argument used to resolve the data record.
 *  @param kind Field selector: 1, 2, 3, 4, or 100.
 *  @param index Table index for kind 4; negative requests the count.
 *  @return The selected value, or zero for an unsupported kind or out-of-range index.
 *  @par Globals
 *  D_8009C73C points to the function used to obtain the data record.
 *  @par Control flow
 *  Dispatches by kind and checks the index bounds for table access.
 *  @par Calls
 *  Invokes the function pointer in D_8009C73C with arg0.
 *  @par Side effects
 *  No direct writes; the resolver may have side effects.
 *  @par Callers
 *  Confirmed: func_8001EC94.
 *
 * @par Role and path
 * Resolves a pad port Data struct via D_8009C73C(arg0) and returns fieldE8
 * (kind 1), fieldE6 (kind 2), fieldE4 (kind 3), field4c (kind 100), or for
 * kind 4 either count (index < 0) or table[index] when in bounds. Called by
 * func_8001EC94.
 */
int gQueryPadInfo(int arg0, int kind, int index)
{
    struct Data *data = gPadLookupPortFn(arg0);
    switch (kind) {
    case 1: return data->fieldE8;
    case 2: return data->fieldE6;
    case 3: return data->fieldE4;
    case 4:
        if (index < 0) return data->count;
        if (index < data->count) return data->table[index];
        return 0;
    case 100: return data->field4c;
    }
    return 0;
}
