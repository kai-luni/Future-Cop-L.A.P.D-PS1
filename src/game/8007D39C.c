typedef unsigned char u8;
typedef struct { u8 value; u8 pad[5]; } Entry;
typedef struct { Entry *entries; int unused; u8 count; } List;
u8 func_8007D39C(List *list, u8 value)
{
    Entry *entries = list->entries;
    u8 i = 0;
    while (i < list->count) {
        if (entries[i].value == value)
            break;
        i++;
    }
    if (i == list->count)
        i = 0;
    return i;
}
