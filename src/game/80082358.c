#include "game/types.h"

struct Node {
    u8 *data;
    u8 pad04[0x14];
    u32 tag;
    u8 pad1C[4];
    s32 size;
};

extern s32 D_8009F464;
extern u8 *D_8009F92C;
extern u8 D_800120E0[];

extern void func_800822CC(u8 *arg0);
extern u8 *func_8001E318(u8 *destination, u8 *source, u32 count);
extern void func_800181D4(struct Node *node);

void func_80082358(struct Node *node)
{
    s32 has_pl16;
    u8 *dst;
    u8 *cursor;
    u8 *end;
    s32 pix_offset;
    s32 pix_count;
    u32 chunk_type;
    s32 chunk_size;
    u8 *chunk_data;
    s32 chunk_data_size;

    if (D_8009F464 != 0 && node->tag == 0x436D6963) {
        dst = D_8009F92C;
        dst[0] = 0x43;
        dst[2] = 0x13;
        dst[3] = 1;
        func_800822CC(D_800120E0);
        has_pl16 = 0;

        cursor = node->data;
        end = cursor + node->size;
        pix_count = 0;
        if (cursor < end) {
            pix_offset = 0x80;
            do {
                chunk_type = *(u32 *)cursor;
                chunk_size = *(s32 *)(cursor + 4);
                chunk_data = cursor + 8;
                chunk_data_size = chunk_size - 8;

                switch (chunk_type) {
                case 0x50495834:
                    if (pix_count != 3 && chunk_data_size == 0x80) {
                        func_8001E318(dst + pix_offset, chunk_data, 0x80);
                        pix_offset += 0x80;
                        pix_count++;
                    }
                    break;
                case 0x504C3136:
                    if (has_pl16 == 0 && chunk_data_size == 0x20) {
                        func_8001E318(dst + 0x60, chunk_data, 0x20);
                        has_pl16 = 1;
                    }
                    break;
                }
                cursor += *(s32 *)(cursor + 4);
            } while (cursor < end);
        }

        if (has_pl16 == 1 && pix_count == 3) {
            D_8009F92C[0x920] = has_pl16;
        }
    }
    func_800181D4(node);
}
