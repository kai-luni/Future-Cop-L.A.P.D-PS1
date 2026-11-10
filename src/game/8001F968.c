typedef struct {
    char pad0[0x8f4];
    volatile unsigned int state;
    volatile int field8f8;
    volatile int field8fc;
    volatile int field900;
    volatile int field904;
    char pad908[0xc];
    char field914[0x84];
    volatile int field998;
    char pad99c[4];
    volatile int counter;
    volatile int done;
    char pad9a8[0x1208];
    volatile int field1bb0;
} State;

extern State *volatile D_8009AB08;
extern void func_8008D640(int);
extern void func_8008D670(int, int, void *);
extern void func_8008D680(int, int, void *);

int func_8001F968(void)
{
    register State *p asm("$6") = D_8009AB08;
    p->field1bb0 = 0;
    switch (p->state) {
    case 2:
        if (p->counter > 0) {
            p->counter--;
            func_8008D680(p->field8fc, p->field998, p->field914);
        } else {
            p->done = 1;
            p->field8f8 = 0;
            p->state = 0;
            p->field904 = 0;
            p->field900 = -3;
            func_8008D640(p->field8fc);
        }
        break;
    case 3:
    case 6:
        p = D_8009AB08;
        if (p->counter > 0) {
            p->counter--;
            func_8008D670(p->field8fc, p->field998, p->field914);
        } else {
            p->done = 1;
            p->field8f8 = 0;
            p->state = 0;
            p->field904 = 0;
            p->field900 = -3;
            func_8008D640(p->field8fc);
        }
        break;
    default:
        {
            State *q = D_8009AB08;
            q->done = 1;
            q->field8f8 = 0;
            q->state = 0;
            func_8008D640(q->field8fc);
        }
        break;
    }
    return 0;
}
