// Categories: Game Engine > Table Processing
typedef unsigned char u8;
typedef int s32;

struct Entry {
    u8 data[16];
    u8 count;
    u8 rest[11];
};

extern struct Entry D_800A7938[];
extern void func_800242B4(s32, s32);

/**
 *  @brief Dispatches each table entry's first data byte according to its count.
 *
 *  Scans 110 entries and passes the entry index and first data byte to the
 *  dispatch function once for each unit of the entry's count.
 *  @par Globals
 *  D_800A7938 supplies the entries, counts, and data bytes.
 *  @par Control flow
 *  An outer loop visits all entries; a nonzero count starts an inner loop.
 *  The data pointer is not advanced within that loop.
 *  @par Calls
 *  func_800242B4 receives the entry index and repeated data byte.
 *  @par Side effects
 *  Invokes the dispatch function; its effects are not shown here.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80023A24(void)
{
    register s32 i __asm__("$18");
    register s32 j __asm__("$16");
    register s32 count __asm__("$20");
    struct Entry *entry;
    register u8 *data __asm__("$19");
    i = 0;
    entry = D_800A7938;
    do {
        count = entry->count;
        j = 0;
        if (count != 0) {
            data = entry->data;
            __asm__("" : "+r" (data));
            do {
                j++;
                func_800242B4(i, *data);
            } while (j < count);
        }
        i++;
        entry++;
    } while (i < 110);
}
