typedef unsigned char u8;
typedef signed char s8;
struct Entry { char *name; s8 type; u8 value; char pad[2]; };
extern u8 D_8009F3E5;
extern s8 D_8009F3F0;
extern struct Entry D_8009B42C[];
extern char D_8009F2E4[];
extern int D_8009F3E0, D_8009F3D8, D_8009F238;
extern int D_8009F9C4;
extern void func_8008CD30(char *, char *, int, char *);
extern void func_8001E110(char *, int, void **, int *, int);
extern void func_800861E4(void *, int, int, int);
extern void func_800181D4(void *);
extern void func_8002549C(int);
extern void func_8001B4A8(int);

int func_80025370(void)
{
    char buffer[40];
    void *data = 0;
    int size;
    struct Entry *entry;
    int saved;
    if (!D_8009F3E5 && D_8009F3F0 < 23) {
        entry = &D_8009B42C[D_8009F3F0];
        {
            int arg;
            if (entry->type == 1)
                arg = D_8009F3E0;
            else
                arg = D_8009F3D8;
            func_8008CD30(buffer, D_8009F2E4, arg, entry->name);
        }
        func_8001E110(buffer, D_8009F238, &data, &size, 0);
        if (data) {
            saved = D_8009F9C4;
            D_8009F9C4 = 1;
            func_800861E4((char *)data + 8, entry->value, 320, 0);
            {
                register char *base __asm__("$2");
                base = (char *)data;
                func_800861E4(base + (*(int *)base + 8), entry->value, 320, 256);
            }
            D_8009F9C4 = saved;
            func_800181D4(data);
            func_8002549C(0);
            func_8001B4A8(1);
        }
    }
    D_8009F3E5 = 0;
    return data != 0;
}
