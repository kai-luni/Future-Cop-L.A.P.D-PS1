// Categories: Game Engine > State
typedef unsigned char u8;
typedef struct { u8 pad[9]; u8 x, y, z; } Inner;
typedef struct { u8 pad[0x6c]; Inner *inner; u8 pad2[0x1b]; u8 x, y, z; } State;

/**
 *  @brief Adjusts three inner byte fields toward their corresponding targets.
 *
 *  Each nonzero signed increment is added to the current field and clamped
 *  against the target in the direction of that increment.
 *  @param s State containing the inner fields and target values.
 *  @param x Signed increment for the x field.
 *  @param y Signed increment for the y field.
 *  @param z Signed increment for the z field.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Each axis independently branches on the increment's sign; zero leaves it unchanged.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes the affected byte fields in s->inner.
 *  @par Callers
 *  Confirmed: func_80040A00.
 */
void func_80040600(State *s, int x, int y, int z)
{
    if (x > 0) {
        x += s->inner->x;
        if (x >= s->x) s->inner->x = s->x;
        else s->inner->x = x;
    } else if (x < 0) {
        x += s->inner->x;
        if (x <= s->x) s->inner->x = s->x;
        else s->inner->x = x;
    }
    if (y > 0) {
        y += s->inner->y;
        if (y >= s->y) s->inner->y = s->y;
        else s->inner->y = y;
    } else if (y < 0) {
        y += s->inner->y;
        if (y <= s->y) s->inner->y = s->y;
        else s->inner->y = y;
    }
    if (z > 0) {
        z += s->inner->z;
        if (z >= s->z) s->inner->z = s->z;
        else s->inner->z = z;
    } else if (z < 0) {
        z += s->inner->z;
        if (z <= s->z) s->inner->z = s->z;
        else s->inner->z = z;
    }
}
