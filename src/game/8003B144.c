typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef int s32;
struct Part { s16 a,b,c,d,e,f,g,h,i; s8 j,k; };
struct Entity { u8 pad[0x28]; void *info; u8 pad2[0x75]; s8 count; s8 total; u8 pad3; s16 threshold; u8 pad4[0x22]; struct Part *parts; };
extern void func_8003A960(struct Entity *, void *, s32);
extern void func_8004D958(struct Entity *);
void func_8003B144(s32 unused, struct Entity *self, s32 delta)
{
    struct Part *p = self->parts;
    s32 i;
    i = 0;
    while (i < self->total) {
        if (p->i != 0) {
            if (p->b < self->threshold) {
                p->i = 0;
                self->count--;
                func_8003A960(self, self->info, i);
            } else {
                p->e += (p->i * delta) >> 8;
                p->a += (p->d * delta) >> 8;
                p->b += (p->e * delta) >> 8;
                p->c += (p->f * delta) >> 8;
                p->h += (p->k * delta) >> 6;
                p->g += (p->j * delta) >> 6;
            }
        }
        p++;
        i++;
    }
    if (self->count <= 0) func_8004D958(self);
}
