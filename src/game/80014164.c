typedef short s16;
typedef int s32;
struct Input { s16 x, y, z; char pad[6]; s16 a, b; };
struct Matrix { char data[0x24]; s32 x, y, z; };
extern void func_80018DC0(s32, s32, s32, struct Matrix *);
extern void func_80010820(struct Matrix *);
extern void func_8001006C(struct Matrix *, struct Matrix *, s32);
void func_80014164(struct Input *input, struct Matrix *source, struct Matrix *dest, struct Matrix *parent)
{
    struct Matrix first;
    struct Matrix second;
    func_80018DC0(input->a, input->b, 0, &first);
    first.x = input->x << 4;
    first.y = input->y << 4;
    first.z = input->z << 4;
    func_80010820(&first);
    func_8001006C(source, &second, 4);
    second.x += first.x;
    second.y += first.y;
    second.z += first.z;
    func_80010820(parent);
    func_8001006C(&second, dest, 4);
    dest->x += parent->x;
    dest->y += parent->y;
    dest->z += parent->z;
}
