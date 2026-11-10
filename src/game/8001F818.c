typedef struct State {
    char pad0[0x8F4];
    volatile unsigned int mode;
    volatile int active;
    volatile int handle;
    volatile int code;
    volatile int other;
    char pad908[0xC];
    char data[0x84];
    volatile int value;
    int pad99c;
    volatile int timer;
    volatile int done;
    char pad9a8[0x1208];
    volatile int field1bb0;
} State;

extern State *D_8009AB08;
extern void func_8008D640(int);
extern void func_8008D670(int, int, void *);
extern void func_8008D680(int, int, void *);

int func_8001F818(void)
{
    register State *p asm("$6") = D_8009AB08;
    p->field1bb0 = 0;
    switch (p->mode) {
    case 2:
        if (p->timer > 0) {
            p->timer--;
            func_8008D680(p->handle, p->value, p->data);
        } else {
            p->done = 1;
            p->active = 0;
            p->mode = 0;
            p->other = 0;
            p->code = -3;
            func_8008D640(p->handle);
        }
        break;
    case 3:
    case 6:
        p = D_8009AB08;
        if (p->timer != 0) {
            p->timer--;
            func_8008D670(p->handle, p->value, p->data);
        } else {
            p->done = 1;
            p->active = 0;
            p->mode = 0;
            p->other = 0;
            p->code = -3;
            func_8008D640(p->handle);
        }
        break;
    default: {
        State *q = D_8009AB08;
        q->done = 1;
        q->active = 0;
        q->mode = 0;
        func_8008D640(q->handle);
        break;
    }
    }
    return 0;
}
