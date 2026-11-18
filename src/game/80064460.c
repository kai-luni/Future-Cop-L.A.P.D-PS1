// Categories: Gameplay > Map > Cell Sampling | Physics > Collision > Cell Grid | Graphics > HUD > Minimap
#include "game/types.h"
typedef struct { s16 x,y,z,w; } Vec;
typedef struct { s32 a,b; } VecW;
typedef struct { s32 mask; void *node; VecW pos; } Entry;
typedef struct { char pad[0x7c]; s32 x,y,z; } Camera;
typedef struct { s32 a,width; char pad[0x1c]; u32 offsets[1]; } Map;
extern Camera *gCamera __asm__("D_8009F9E4");
extern Map *gTileMapTable __asm__("D_8009F80C");
extern char *gTileBlockTable __asm__("D_8009F808");
void gSampleCellWindow(s32 arg0, s32 arg1, Entry *arg2) __asm__("func_80064460");

extern s32 func_8006429C(void *, Vec);
/**
 * @brief Samples the 5x5 cell window of the map attribute grid at a cell
 *        coordinate and fills 25 region entries with the node pointer, the
 *        packed mask and the cell's world position.
 *
 * @param arg0 Cell coordinate that becomes the world z axis.
 * @param arg1 Cell coordinate that becomes the world x axis.
 * @param arg2 Output block of 25 entries: mask word at +0, node pointer at +4
 *        and four s16 position components at +8; the same 400-byte layout that
 *        func_80066E98 draws and func_80084398 animates.
 * @return Nothing.
 *
 * @par Confirmed
 * D_8009F9E4 is the camera: bx = (arg1 << 17) - camera +0x7C, by is the negated
 * camera +0x80 and bz = (arg0 << 17) - camera +0x84, and all three are shifted
 * right by 4 to reach world units, so the two arguments are swapped between the
 * grid axis and the world axis. The grid entry is
 * D_8009F80C->offsets[(arg1 + j) * width + arg0 + i], masked with 0xFFFC and
 * added to the base D_8009F808, and the pointer stored there is the cell node
 * handed to func_8006429C together with the cell position, whose x comes from
 * the row axis (bx + (j << 17)) and whose z from the column axis (bz + (i << 17)).
 * The returned flags drive the mask: a zero mask is written for every entry and,
 * when bit 0 of the flags is set, the node and the four position halves are
 * stored too. The mask is a 16-bit permutation of the flag bits - four four-bit
 * groups taken from 0x20/0x40/0x200/0x400, 0x80/0x100/0x800/0x1000,
 * 0x2000/0x4000/0x20000/0x40000 and 0x8000/0x10000/0x80000/0x100000 - flag
 * bits 5 to 20 in order, the first group in the top nibble.
 *
 * @par Likely role
 * The region pre-pass of the minimap and cell renderers: it turns the current
 * camera position plus a cell coordinate into the 25 cells of one screen region,
 * asks the cell collision query what the local geometry says, and precomputes
 * both the world position and the layer mask so the three drawers behind it
 * only have to test masks. The same mask word is what func_80084398 ANDs with an
 * object's +0x4E band word, so the packing here defines the layer assignment of
 * the whole region pass.
 *
 * @par Unknown
 * The exact meaning of each flag bit before packing, and why the world axes are
 * swapped against the argument order.
 *
 * @par Callers
 * func_80066C9C, the per-view camera anchor update, with a block of
 * D_800AB3B8 at a derived offset.
 *
 * @par Environment path
 * Fills the 25 window entries (mask, node, world position) that the three
 * region layers then draw.
 */
void gSampleCellWindow(s32 arg0, s32 arg1, Entry *arg2)
{
    register s32 x __asm__("$22");
    register s32 y __asm__("$23");
    register Entry *out __asm__("$18");
    register s32 i __asm__("$16");
    register s32 j __asm__("$20");
    register s32 flags __asm__("$5");
    s32 a;
    Vec v;
    register s32 *cam __asm__("$3");
    volatile s32 bx,by,bz;
    register VecW *dst __asm__("$17");
    register void *node __asm__("$19");
    x=arg0; __asm__("" : "=r"(x) : "0"(x));
    y=arg1; __asm__("" : "=r"(y) : "0"(y));
    out=arg2; __asm__("" : "=r"(out) : "0"(out));
    j=0; __asm__("" : "=r"(j) : "0"(j));
    cam=(s32 *)gCamera;
    bx=(y<<17)-cam[0x7c/4];
    cam+=0x7c/4;
    __asm__("" : "=r"(cam) : "0"(cam) : "memory");
    by=-cam[1];
    bz=(x<<17)-cam[2];
    do {
        i=0;
        dst = &out->pos;
        do {
            register u32 off __asm__("$4");
            off = gTileMapTable->offsets[(y+j)*gTileMapTable->width+x+i];
            off &= 0xfffc;
            off += (u32)gTileBlockTable;
            node = *(void **)off;
            v.x=(bx+(j<<17))>>4;
            {
                s32 iz = i<<17;
                __asm__("" : "=m"(v.x) : "m"(v.x), "r"(iz));
                *(volatile s16 *)&v.z=(bz+iz)>>4;
            }
            *(volatile s16 *)&v.y=by>>4;
            flags=func_8006429C(node,v);
            out->mask=0;
            if(flags&1) {
                a=((flags&0x20)>>2)|((flags&0x40)>>4)|((flags&0x200)>>8)|((flags&0x400)>>10);
                a <<= 4;
                a |= ((flags&0x80)>>4)|((flags&0x100)>>6)|((flags&0x800)>>10)|((flags&0x1000)>>12);
                a <<= 4;
                a |= ((flags&0x2000)>>10)|((flags&0x4000)>>12)|(((u32)flags>>16)&2)|(((u32)flags>>18)&1);
                a <<= 4;
                __asm__("" : "=r"(flags) : "0"(flags));
                out->mask=a|(((flags&0x8000)>>12)|(((u32)flags>>14)&4)|(((u32)flags>>18)&2)|(((u32)flags>>20)&1));
                __asm__ volatile("" : "=r"(flags) : "0"(flags));
                ((void **)dst)[-1]=node;
                *dst=*(VecW *)&v;
            }
            dst=(VecW *)((char *)dst+16);
            i++;
            out++;
        } while (i<5);
        j++;
    } while (j<5);
}
