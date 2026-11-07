typedef unsigned char u8;
typedef short s16;
void func_8006C768(u8 *faces, s16 rows, int cols, s16 count)
{
    int index = 0;
    int first = 1;
    int second = first;
    int i;
    for (i = 0; i < rows - 1; i++) {
        faces[index * 5 + 2] = second++;
        faces[index * 5] = 3;
        faces[index * 5 + 1] = 0;
        faces[index * 5 + 3] = second;
        index++;
    }
    second = first + rows;
    while (second < count) {
        for (i = 0; i < rows - 1; i++) {
            faces[index * 5 + 1] = first++;
            faces[index * 5 + 2] = second++;
            faces[index * 5] = 4;
            faces[index * 5 + 3] = second;
            faces[index * 5 + 4] = first;
            index++;
        }
        second++;
        first++;
    }
}
