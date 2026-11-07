typedef unsigned char u8;
typedef unsigned int u32;
struct State {
    u8 pad0[0x10];
    struct State *next;
    u8 pad14[0x1c];
    u8 *data;
    u32 flags;
    u8 busy;
    u8 pad39[0x10];
    u8 mode;
};
extern struct State *(*D_8009C73C)(void);
int func_80089E8C(void)
{
    struct State *s = D_8009C73C();
    if ((s->flags & 0xffff0000) || (s != s->next && s->busy) || s->data[0]) {
        switch (s->mode) {
        case 2: return 1;
        case 3: return 1;
        case 6: return 4;
        }
    }
    return s->mode;
}
