// Categories: Platform > GPU > Library Reset | Graphics > Display Setup
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct State {
    volatile u8 mode;
    u8 active;
    u8 field2;
    u8 pad3;
    u16 field4;
    u16 field6;
    u8 rest[0x78];
};
struct Entry {
    u16 value;
    u16 other;
};
struct Callbacks {
    u8 pad[0x34];
    int (*callback)(int);
};

extern u8 D_800129A0[];
extern u8 D_800129C0[];
extern u8 D_8009DDA8[];
extern struct State D_8009DDF0;
extern struct Entry D_8009DE70[];
extern struct Entry D_8009DE7C[];
extern struct Callbacks *D_8009DDE8;
extern void (*D_8009DDEC)(u8 *, int);
extern void func_8008E000(void *, void *, void *);
extern void func_80095830(unsigned char *, int, unsigned int);
extern void func_80091480(void);
extern void func_80095860(u32);
extern int func_80094F4C(int);

/**
 * @brief Initialises or resets the GPU library state for the given mode.
 *
 * @param input Mode bits; the low three bits select the reset variant.
 * @return The library state's mode word (or the callback result in the
 *         debug-level branch).
 *
 * @par Confirmed
 * func_8008E000(D_800129A0, D_8009DDA8, &D_8009DDF0) loads the defaults,
 * func_80095830 clears the 0x80-byte state, func_80091480 dispatches the callback,
 * func_80095860 masks the callback table address with 0xFFFFFF and func_80094F4C
 * runs with the argument. `switch (arg & 7)` treats 0/3/5 alike; the state block
 * (mode, active, field4/field6) is returned, and at debug level >= 2 the callback
 * returns instead.
 *
 * @par Likely role
 * The single entry point that resets the library between screens;
 * func_80092BF4 (queue reset) and func_80092A84 are its companions in
 * func_8001BE68.
 */
int func_80092910(int input)
{
    register int arg __asm__("$17") = input;
    register struct State *state __asm__("$16");
    register u32 index __asm__("$2");
    register u8 *dst __asm__("$4");
    register int fill __asm__("$5");
    switch (arg & 7) {
    case 0:
    case 3:
        __asm__ volatile ("" ::: "memory");
        func_8008E000(D_800129A0, D_8009DDA8, &D_8009DDF0);
    case 5:
        __asm__ volatile ("" ::: "memory");
        state = &D_8009DDF0;
        __asm__("" : "+r"(state));
        func_80095830((u8 *)state, 0, 0x80);
        func_80091480();
        func_80095860((u32)D_8009DDE8 & 0xffffff);
        index = func_80094F4C(arg);
        dst = (u8 *)state + 0x10;
        __asm__("" : "+r"(dst));
        state->mode = index;
        index = state->mode;
        __asm__("" : "+r"(index));
        state->active = 1;
        {
            register u32 base __asm__("$3");
            register u32 value __asm__("$3");
            register u32 base2 __asm__("$1");
            __asm__ volatile ("" ::: "memory");
            index <<= 2;
            __asm__("" : "+r"(index));
            base = 0x800A0000;
            __asm__("" : "+r"(base));
            base += index;
            __asm__("" : "+r"(base));
            value = *(u16 *)(base - 0x2190);
            __asm__("" : "+r"(value));
            index = state->mode;
            __asm__("" : "+r"(index));
            fill = -1;
            __asm__("" : "+r"(fill));
            index <<= 2;
            __asm__("" : "+r"(index));
            state->field4 = value;
            __asm__ volatile ("" ::: "memory");
            base2 = 0x800A0000;
            __asm__("" : "+r"(base2));
            base2 += index;
            __asm__("" : "+r"(base2));
            state->field6 = *(u16 *)(base2 - 0x2184);
        }
        func_80095830(dst, fill, 0x5c);
        func_80095830((u8 *)state + 0x6c, -1, 0x14);
        return state->mode;
    default:
        __asm__ volatile ("" ::: "memory");
        if (D_8009DDF0.field2 >= 2) {
            __asm__ volatile ("" ::: "memory");
            D_8009DDEC(D_800129C0, arg);
        }
        return D_8009DDE8->callback(1);
    }
}
