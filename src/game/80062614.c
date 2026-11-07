typedef unsigned char u8;
typedef struct Node { void *object; struct Node *next; } Node;
typedef struct { int x, y, width, height; } Grid;
extern Grid D_800A2970;
extern Node D_800A2990[];
extern u8 *D_8009F820;
extern void func_800623FC(Node *);
extern void func_80062504(Node *, Node *);
void func_80062614(void)
{
    register Node *node __asm__("$16");
    register u8 *next __asm__("$17");
    register u8 *cur __asm__("$18");
    register int cols __asm__("$19");
    Node *base;
    register int rows __asm__("$21");
    register int width __asm__("$22");
    register int value __asm__("$5");
    register int index __asm__("$2");
    register int height __asm__("$3");
    height = D_800A2970.height;
    cur = D_8009F820;
    width = D_800A2970.width;
    __asm__("" : "=r"(height) : "0"(height), "r"(cur), "r"(width));
    rows = height - 1;
    __asm__ volatile ("" : : : "memory");
    __asm__("" : "=r"(index));
    base = D_800A2990;
    if (rows > 0) {
        __asm__("" : : "r"(index));
        next = cur + width;
        do {
            index = *cur;
            __asm__("" : "=r"(index) : "0"(index));
            if (index) {
                node = &base[index];
                func_800623FC(node);
                if ((value = cur[1])) func_80062504(node, &base[value]);
                if ((value = next[0])) func_80062504(node, &base[value]);
                if ((value = next[1])) func_80062504(node, &base[value]);
            }
            next++; cur++;
            cols = width - 2;
            if (cols > 0) {
                do {
                    index = *cur;
            __asm__("" : "=r"(index) : "0"(index));
            if (index) {
                        node = &base[index];
                        func_800623FC(node);
                        if ((value = cur[1])) func_80062504(node, &base[value]);
                        if ((value = next[0])) func_80062504(node, &base[value]);
                        if ((value = next[1])) func_80062504(node, &base[value]);
                        if ((value = next[-1])) func_80062504(node, &base[value]);
                    }
                    next++; cur++; cols--;
                } while (cols > 0);
            }
            index = *cur;
            __asm__("" : "=r"(index) : "0"(index));
            if (index) {
                node = &base[index];
                func_800623FC(node);
                if ((value = next[0])) func_80062504(node, &base[value]);
                if ((value = next[-1])) func_80062504(node, &base[value]);
            }
            next++; cur++; rows--;
        } while (rows > 0);
    }
    cols = width - 1;
    if (cols > 0) {
        do {
            index = *cur;
            __asm__("" : "=r"(index) : "0"(index));
            if (index) {
                node = &base[index];
                func_800623FC(node);
                if ((value = cur[1])) func_80062504(node, &base[value]);
            }
            cur++; cols--;
        } while (cols > 0);
    }
    if ((index = *cur)) func_800623FC(&base[index]);
}
