// Categories: Game Engine > State Update
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
struct Entity {
    u8 pad[0x6c];
    u8 bounds[12];
    u8 pad78;
    u8 value[3];
    u8 pad7c;
    u8 value2[3];
    s16 speed[6];
    u16 flags;
};
#define STEP(N, V, LO, HI) { \
    register u8 sum __asm__("$2"); \
    raw = e->speed[N] * dt; __asm__ volatile ("" : "=r" (raw) : "0" (raw)); delta = raw / 300; \
    if (delta > 0) { \
        if (e->V + delta >= e->bounds[HI]) { \
            e->V = e->bounds[HI]; \
            if (e->flags & 1) e->speed[N] = -e->speed[N]; \
        } else { sum = e->V + delta; e->V = sum; } \
    } else if (delta < 0) { \
        if (e->V + delta <= e->bounds[LO]) { \
            e->V = e->bounds[LO]; \
            if (e->flags & 1) e->speed[N] = -e->speed[N]; \
        } else { sum = e->V + delta; e->V = sum; } \
    } }
void gStepBoundedByteChannels(struct Entity *e, int dt) __asm__("func_8006FB7C");


/**
 *  @brief Advances six bounded byte values using their signed speeds.
 *
 *  Each value receives its corresponding speed multiplied by dt and divided by 300.
 *  Values reaching a bound are clamped, with optional speed reversal.
 *  @param e Entity containing the values, bounds, speeds, and reversal flag.
 *  @param dt Time scale used to calculate each integer delta.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  Six unrolled steps branch on delta sign and whether the relevant bound is reached.
 *  A zero delta leaves the corresponding value and speed unchanged.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates entity values and may negate speeds when flag bit 0 is set.
 *  @par Callers
 *  Confirmed: func_80070048.
 */
void gStepBoundedByteChannels(struct Entity *e, int dt)
{
    register int delta __asm__("$3");
    register int raw __asm__("$2");
    STEP(0, value[0], 0, 3)
    STEP(1, value[1], 1, 4)
    STEP(2, value[2], 2, 5)
    STEP(3, value2[0], 6, 9)
    STEP(4, value2[1], 7, 10)
    STEP(5, value2[2], 8, 11)
}
