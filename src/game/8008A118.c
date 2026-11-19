// Categories: Resources > Data Access
typedef unsigned char u8;

struct Entry {
    u8 a;
    u8 pad01[3];
    u8 *ptr;
};

struct Data {
    void *pad;
    void *pad4;
    struct Entry *entries;
    u8 pad0C[0xDE];
    u8 count;
};

extern struct Data *(*gPadLookupPortFn)(int) __asm__("D_8009C73C");
int gGetPadCombInfo(int arg0, int index, int kind) __asm__("func_8008A118");


/**
 *  @brief Queries a count or byte from a callback-provided data table.
 *
 *  A negative index requests the entry count; a negative kind requests the
 *  selected entry's byte count, otherwise kind selects a byte in its data.
 *  @param arg0 Argument passed to the data-provider callback.
 *  @param index Entry index, or a negative value to request the entry count.
 *  @param kind Byte index, or a negative value to request the entry's count.
 *  @return The requested count or byte, or 0 for an out-of-range index.
 *  @par Globals
 *  D_8009C73C holds the callback that supplies the data object.
 *  @par Control flow
 *  Checks the index and kind in order and returns zero when either is too large.
 *  @par Calls
 *  The D_8009C73C callback retrieves the data object for arg0.
 *  @par Side effects
 *  No direct writes; the callback may have side effects.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Resolves the port Data struct via D_8009C73C(arg0) and returns data->count
 * when index is negative. For a valid index, returns entry->a when kind is
 * negative or entry->ptr[kind] when kind is less than entry->a, otherwise
 * returning 0. No direct callers are present in the codebase.
 */
int gGetPadCombInfo(int arg0, int index, int kind)
{
    struct Data *data = gPadLookupPortFn(arg0);
    struct Entry *entry;
    if (index < 0) return data->count;
    if (index < data->count) {
        entry = &data->entries[index];
        if (kind < 0) return entry->a;
        if (kind < entry->a) return entry->ptr[kind];
        return 0;
    }
    return 0;
}
