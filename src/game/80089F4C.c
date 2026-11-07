typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
struct Data {
    u16 *table;
    u8 pad04[0x48];
    u32 field4c;
    u8 pad50[0x93];
    u8 count;
    u8 fieldE4;
    u8 padE5;
    u16 fieldE6;
    u8 fieldE8;
};
extern struct Data *(*D_8009C73C)(int);
int func_80089F4C(int arg0, int kind, int index)
{
    struct Data *data = D_8009C73C(arg0);
    switch (kind) {
    case 1: return data->fieldE8;
    case 2: return data->fieldE6;
    case 3: return data->fieldE4;
    case 4:
        if (index < 0) return data->count;
        if (index < data->count) return data->table[index];
        return 0;
    case 100: return data->field4c;
    }
    return 0;
}
