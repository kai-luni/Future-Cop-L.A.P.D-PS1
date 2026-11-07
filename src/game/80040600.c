typedef unsigned char u8;
typedef struct { u8 pad[9]; u8 x, y, z; } Inner;
typedef struct { u8 pad[0x6c]; Inner *inner; u8 pad2[0x1b]; u8 x, y, z; } State;

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
