// Categories: Game Engine > State
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

struct Item { u32 flags; u8 a, b, c, d; u8 pad[4]; };
struct Data { s32 count; struct Item items[1]; };
struct Range { u8 pad[3]; u8 start, count; u8 rest[15]; };
struct Info { u8 pad[0x14]; struct Range *ranges; u8 pad2[12]; struct Data *data; };
struct State { u8 pad[0xa8]; u32 bits[1]; };

/**
 *  @brief Clears enabled item bits for items outside a selected range.
 *
 *  Uses the indexed range's inclusive start and count to filter item fields.
 *  Fields a, b, and c are always checked; d is checked for flag type 4.
 *  @param self State containing the bit array to filter.
 *  @param info Source of the range and item tables.
 *  @param index Index of the 20-byte range record.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Walks items forward while the bit index counts down from data->count to 1.
 *  Only set bits are tested and cleared when an applicable field is out of range.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Clears selected bits in self->bits; it never sets a bit.
 *  @par Callers
 *  Confirmed: func_8003B144.
 */
void func_8003A960(struct State *self, struct Info *info, s32 index)
{
    register s32 offset __asm__("$2") = index * 20;
    register struct Range *range __asm__("$3");
    register struct Data *data __asm__("$6");
    struct Item *item;
    s32 i, start, end;
    __asm__("" : "=r"(offset) : "0"(offset));
    range = info->ranges;
    data = info->data;
    __asm__("" : "=r"(range), "=r"(data) : "0"(range), "1"(data));
    range = (struct Range *)((u8 *)range + offset);
    item = data->items;
    i = data->count;
    start = range->start;
    end = start + range->count - 1;
    for (; i > 0; item++, i--) {
        u32 *word = &self->bits[i / 32];
        u32 value = *word;
        if (value & (1 << (i & 31))) {
            if (item->a < start || item->a > end ||
                item->b < start || item->b > end ||
                item->c < start || item->c > end ||
                (((item->flags >> 8) & 15) == 4 && (item->d < start || item->d > end))) {
                *word = value & ~(1 << (i & 31));
            }
        }
    }
}
