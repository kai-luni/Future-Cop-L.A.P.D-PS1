typedef unsigned char u8;

struct Entry {
    u8 a;
    u8 pad01[3];
    u8 *ptr;
};

struct Data {
    void *pad;
    void *pad4;
    struct Entry *entries;
    u8 pad0C[0xDE];
    u8 count;
};

extern struct Data *(*D_8009C73C)(int);

int func_8008A118(int arg0, int index, int kind)
{
    struct Data *data = D_8009C73C(arg0);
    struct Entry *entry;
    if (index < 0) return data->count;
    if (index < data->count) {
        entry = &data->entries[index];
        if (kind < 0) return entry->a;
        if (kind < entry->a) return entry->ptr[kind];
        return 0;
    }
    return 0;
}
