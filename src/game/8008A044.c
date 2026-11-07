typedef unsigned char u8;
struct Entry { u8 a, b, c, d, e; };
struct Data { void *pad; struct Entry *entries; u8 pad08[0xE1]; u8 count; };
extern struct Data *(*D_8009C73C)(int);
int func_8008A044(int arg0, int index, int kind)
{
    struct Data *data = D_8009C73C(arg0);
    struct Entry *entry;
    if (index < 0) return data->count;
    if (index < data->count) {
        entry = &data->entries[index];
        switch (kind) {
        case 1: return entry->a;
        case 2: return entry->b;
        case 3: return entry->c;
        case 4: return entry->d;
        case 5: return entry->e;
        }
    }
    __asm__ volatile (".set reorder");
    return 0;
}
