#include "game/types.h"
typedef struct { s16 x,y,z,w; } Vec;
typedef struct { s32 a,b; } VecW;
typedef struct { s32 mask; void *node; VecW pos; } Entry;
typedef struct { char pad[0x7c]; s32 x,y,z; } Camera;
typedef struct { s32 a,width; char pad[0x1c]; u32 offsets[1]; } Map;
extern Camera *D_8009F9E4;
extern Map *D_8009F80C;
extern char *D_8009F808;
extern s32 func_8006429C(void *, Vec);
void func_80064460(s32 arg0, s32 arg1, Entry *arg2)
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
    cam=(s32 *)D_8009F9E4;
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
            off = D_8009F80C->offsets[(y+j)*D_8009F80C->width+x+i];
            off &= 0xfffc;
            off += (u32)D_8009F808;
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
