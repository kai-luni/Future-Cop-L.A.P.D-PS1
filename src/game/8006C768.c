// Categories: Graphics > Mesh Generation | Graphics > Effects
typedef unsigned char u8;
typedef short s16;

/**
 * @brief Populates 5-byte triangle-fan and quad-strip face records for a dome-patch mesh.
 *
 * Writes (rows - 1) pole triangles (vertex count 3, apex vertex 0) followed by quad strips
 * (vertex count 4) over consecutive vertex columns until vertex index second reaches count.
 *
 * @param faces Output buffer of 5-byte face records (count byte plus up to 4 u8 vertex indices).
 * @param rows Number of vertices per radial rib (writes rows - 1 triangles/quads per column).
 * @param cols Unused column count parameter.
 * @param count Total vertex count bounding the outer quad-strip loop.
 *
 * @par Globals
 * None.
 * @par Control flow
 * Initial for loop (i < rows - 1) emitting pole triangles, then while (second < count) over inner for loop emitting quads.
 * @par Calls
 * None.
 * @par Side effects
 * Writes ((rows - 1) + quad_count) * 5 bytes into the faces buffer.
 * @par Callers
 * Confirmed: func_8006C84C (dome-patch mesh allocator/builder).
 */
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
