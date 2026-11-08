typedef unsigned char u8;
typedef unsigned short u16;
typedef signed int s32;
struct State { u8 a[4]; volatile u16 status; u8 b[2]; volatile u16 x; volatile u16 flags; u8 c[2]; volatile u16 y; };
struct Obj { u8 a[12]; u8 *data; u8 b[0x2C]; u8 *p; u8 c[0x10]; u8 v50; u8 d[0x97]; u8 ve8; };
extern struct State *D_8009C78C;
extern s32 D_8009C768;
extern s32 D_8009C780[];
extern void (*D_8009C748)(void *);
extern void (*D_8009C74C)(void *);
extern s32 *D_8009C788;
extern void func_8008C890(s32);
extern void func_8008AE44(void);
s32 func_8008A6CC(struct Obj *obj)
{
    s32 result;
    s32 n;
    register s32 *base __asm__("$17");
    register s32 idx __asm__("$2");
    register struct State *state __asm__("$4");
    register s32 flags __asm__("$3");
    D_8009C78C->flags = 0x40;
    D_8009C78C->flags = 0;
    D_8009C78C->x = 13;
    D_8009C78C->y = 0x88;
    func_8008C890(obj->ve8 == 8 ? 0x50 : 0x91);
    idx = D_8009C768;
    state = D_8009C78C;
    flags = 0x1003;
    if (idx) flags = 0x3003;
    idx <<= 2;
    __asm__("" : "+r"(idx));
    state->flags = flags;
    {
        register u8 *arr __asm__("$3");
        idx = *(s32 *)((s32)D_8009C780 + idx);
        __asm__("" : "+r"(idx));
        arr = (u8 *)0x800a0000;
        __asm__("" : "+r"(arr));
        arr -= 0x3880;
        if (idx >= 0) {
            if (idx > 0) {
                base = (s32 *)arr;
                __asm__("" : "+r"(base));
                do {
                    n = --base[D_8009C768];
                    D_8009C748(obj->data + n * 240);
                } while (base[D_8009C768] > 0);
                __asm__ volatile ("" ::: "memory");
            }
            {
                register s32 index __asm__("$2");
                register u8 *arr2 __asm__("$3");
                index = D_8009C768;
                __asm__("" : "+r"(index));
                arr2 = (u8 *)0x800a0000;
                __asm__("" : "+r"(arr2));
                arr2 -= 0x3880;
                if (((s32 *)arr2)[index] == 0) {
                    ((s32 *)arr2)[index] = -1;
                    D_8009C748(obj);
                    D_8009C74C(obj);
                }
            }
        }
    }
    if (D_8009C78C->status & 0x200) {
        D_8009C78C->flags |= 0x10;
        if (D_8009C78C->status & 0x200) {
            *(volatile u8 *)D_8009C78C = 1;
            func_8008AE44();
            (void)*(volatile u8 *)D_8009C78C;
            return 0;
        }
        *D_8009C788 = -0x81;
    }
    if (obj->v50 && *((u8 *)obj + 0x36)) return 0;
    *obj->p = 0;
    return 1;
}
