// Categories: Resources > Chunk Loading
#include "game/types.h"

typedef struct Chunk { u32 type; u32 size; s32 count; } Chunk;
typedef struct Entry { u8 a; u8 b; u8 kind; u8 c; s16 value; u16 d; u8 *p; u8 *q; } Entry;
typedef struct Section { u8 *a; u8 *b; u8 *c; u8 pad[0x363]; s8 index; u8 pad2[0x5a]; s16 n; } Section;
typedef struct List { s8 count; u8 pad[3]; u32 values[1]; } List;
extern Section **gTileBlockTable __asm__("D_8009F808");
extern Chunk *gMapTileAnimList __asm__("D_8009FA04");
extern List gTileDefTable __asm__("D_800A6F38");
extern u8 gSlfxFlagByte[] __asm__("D_800A2931");

/**
 *  @brief Processes section, animation, and effects chunks in a data buffer.
 *
 *  Section chunks establish pointers used by later chunks; animation offsets are relocated in place.
 *  Effects values are deduplicated in a global list and assigned to the current section.
 *  @param data Start of the chunk buffer.
 *  @param size Size of the chunk buffer in bytes.
 *  @param id Index at which to store the section pointer.
 *  @par Globals
 *  D_8009F808 stores sections; D_8009FA04 links animation chunks; D_800A6F38 stores effects values; D_800A2931 records a flag.
 *  @par Control flow
 *  Advances by each chunk's declared size, dispatches recognized tags, and loops over animation entries or effects values.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Rewrites chunk fields and pointers, updates global tables and flags, and modifies the current section.
 *  @par Callers
 *  Confirmed: func_80063C20.
 */
void func_800639D4(u8 *data, u32 size, s32 id)
{
    register u8 *end __asm__("$14") = data + size;
    register Section *section __asm__("$9") = 0;
    while (data < end) {
        register u32 tag __asm__("$5") = 0x53634c41;
        register Chunk *chunk __asm__("$4");
        u32 type;
        register s32 high __asm__("$2");
        u32 chunk_size;
        __asm__("" : "+r"(tag));
        chunk = (Chunk *)data;
        chunk_size = ((Chunk *)data)->size;
        __asm__("" : "+r"(chunk));
        type = chunk->type;
        data += chunk_size;
        if (type == tag) goto next_chunk;
        high = type > tag;
        __asm__("" : "+r"(high));
        if (high) goto high_tag;
        if (type == 0x534c4658) goto effects;
        goto next_chunk;
    high_tag:
        if (type == 0x53635441) goto animations;
        if (type != 0x53656374) goto next_chunk;
        {
            register s32 counts __asm__("$3");
            register u32 low __asm__("$4");
            register u8 *base __asm__("$2");
            section = (Section *)chunk;
            counts = (s32)section->c;
            low = counts & 0xffff;
            counts >>= 16;
            __asm__("" : "+r"(counts), "+r"(low));
            base = (u8 *)section + 0x5d0 + section->n * 4;
            section->c = base;
            section->a = base + counts * 2;
            section->b = section->a + low * 2;
            gTileBlockTable[id] = section;
            section->index = -1;
            goto next_chunk;
        }
    animations:
        {
            Entry *entry;
            u8 *base;
            register s32 i __asm__("$7");
            s32 count;
            register Chunk *owner __asm__("$11") = chunk;

            count = chunk->count;
            entry = (Entry *)(chunk + 1);
            base = (u8 *)entry + count * 16;
            {
                Chunk *previous = gMapTileAnimList;
                gMapTileAnimList = chunk;
                *(Chunk **)chunk = previous;
            }
            i = 0;
            if (count > 0) do {
                register u8 *p __asm__("$2") = entry->p;
                register u8 *q __asm__("$4") = entry->q;
                register u32 kind __asm__("$3") = entry->kind;
                __asm__("" : "+r"(p), "+r"(q), "+r"(kind));
                entry->p = base + (u32)p;
                entry->q = section->c + (u32)q;
                if (kind >= 16)
                    entry->value = 0x20000 / (entry->value * entry->a);
                i++;
                entry++;
            } while (i < owner->count);
            goto next_chunk;
        }
    effects:
        {
            register u32 value __asm__("$4") = chunk->count;
            register s32 i __asm__("$7");
            if ((value >> 28) == 1) {
                gSlfxFlagByte[0] = 1;
                value &= ~0xfc;
            }
            {
                register u32 *scan __asm__("$3");
                register s32 n __asm__("$2") = gTileDefTable.count;
                register s32 offset __asm__("$2");
                __asm__("" : "+r"(n));
                i = n - 1;
                __asm__("" : "+r"(i));
                if (i >= 0) {
                    offset = i * 4;
                    __asm__("" : "+r"(offset));
                    scan = (u32 *)((u8 *)&gTileDefTable + offset);
                scan_loop:
                    if (scan[1] == value) goto scan_done;
                    i--;
                    scan--;
                    if (i >= 0) goto scan_loop;
                }
            scan_done:;
            }
            if (i < 0) {
                register u32 raw __asm__("$3") = *(u8 *)&gTileDefTable.count;
                register s32 shifted __asm__("$2");
                __asm__("" : "+r"(raw));
                shifted = raw << 24;
                __asm__("" : "+r"(shifted));
                i = shifted >> 24;
                gTileDefTable.count = raw + 1;
                __asm__("" : "+r"(i));
                if ((value >> 28) == 1) value &= ~0xfc;
                gTileDefTable.values[i] = value;
            }
            section->index = i;
        }
    next_chunk:;
    }
}
