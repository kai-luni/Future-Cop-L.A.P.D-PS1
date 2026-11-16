// Categories: Graphics > Particles | Physics > Integration
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef int s32;
struct Part { s16 a,b,c,d,e,f,g,h,i; s8 j,k; };
struct Entity { u8 pad[0x28]; void *info; u8 pad2[0x75]; s8 count; s8 total; u8 pad3; s16 threshold; u8 pad4[0x22]; struct Part *parts; };
extern void func_8003A960(struct Entity *, void *, s32);
extern void func_8004D958(struct Entity *);

/**
 * @brief Animates the parts of a debris/particle object and retires it when empty.
 *
 * Every part whose active byte is set is checked against the object's life
 * threshold. A part below the threshold is deactivated and the object's part
 * count is reduced; the per-part cleanup func_8003A960 is called with the part
 * index. The remaining parts are integrated with the frame delta: velocity and
 * the two rising values use >> 8, the two angular values use >> 6, which keeps
 * the whole update in fixed point. When the count reaches zero the object is
 * retired.
 *
 * @param unused Unused first argument of the message signature.
 * @param self Object with the part array at 0xD2 and the count at 0x98.
 * @param delta Frame delta.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * One loop over all parts plus the final retire test.
 *
 * @par Calls
 * func_8003A960 - per-part cleanup. func_8004D958 - retires the object.
 *
 * @par Side effects
 * Advances the part state and reduces the part count.
 *
 * @par Callers
 * Confirmed: func_8003B2D0.
 */
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
