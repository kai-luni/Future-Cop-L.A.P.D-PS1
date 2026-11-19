// Categories: Resources > Texture Atlas | Graphics > 2D Sprites
#include "game/types.h"

typedef struct Unk824CCHeader {
    u16 type;
    u16 count;
    u32 magic;
} Unk824CCHeader;

typedef struct Unk824CCObject {
    Unk824CCHeader *header;
} Unk824CCObject;

extern Unk824CCObject *gSpriteAtlasBlock __asm__("D_8009F460");
extern Unk824CCHeader *gAtlasTable __asm__("D_8009F45C");

extern void gFreeBlock(Unk824CCObject *object) __asm__("func_800181D4");
void gValidateAtlasSpriteId(Unk824CCObject *object) __asm__("func_800825AC");


/**
 * @brief Registers and validates the loaded 2D texture atlas header resource.
 *
 * Frees any previously active atlas block, installs the new object and header into
 * the active atlas globals, and frees/clears both if type != 2 or magic != 0x35BFBC73.
 *
 * @param object Loaded resource wrapper whose first word points to an Unk824CCHeader.
 *
 * @par Globals
 * D_8009F460 - active atlas wrapper object; D_8009F45C - active atlas header pointer.
 *
 * @par Control flow
 * Frees prior D_8009F460 if non-null, installs new pointers, sets flag if type != 2
 * or magic != 0x35BFBC73, and frees/nulls both globals when flag is non-zero.
 *
 * @par Calls
 * func_800181D4 - frees the previous atlas object or the newly rejected object.
 *
 * @par Side effects
 * Updates D_8009F460 and D_8009F45C; may release allocated blocks via func_800181D4.
 *
 * @par Callers
 * Confirmed: func_80024EC8 (asset loader dispatch on tag 0x4374696E / "Ctin").
 *
 * @par Texture load path
 * Called by func_80024EC8 on tag `0x4374696E` ("Ctin"); only
 * validates (type == 2, magic == `0x35BFBC73`) and registers the 2D atlas
 * header in D_8009F45C / D_8009F460 with no VRAM pixel upload.
 *
 * @par Environment path
 * Registers and validates a loaded 2D texture atlas header resource from
 * object, freeing temporary data via gFreeBlock (called by
 * func_80024EC8:154).
 */
void gValidateAtlasSpriteId(Unk824CCObject *object)
{
    register s32 flag __asm__("$16") = 0;
    Unk824CCHeader *header;

    if (gSpriteAtlasBlock != 0) {
        gFreeBlock(gSpriteAtlasBlock);
    }
    header = object->header;
    gSpriteAtlasBlock = object;
    gAtlasTable = header;
    if (header->type != 2 || header->magic != 0x35BFBC73) {
        flag = 1;
    }
    if (flag != 0) {
        gFreeBlock(gSpriteAtlasBlock);
        gSpriteAtlasBlock = 0;
        gAtlasTable = 0;
    }
}
