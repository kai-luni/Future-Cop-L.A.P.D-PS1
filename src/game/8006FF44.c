typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef int s32;
struct Parent { u8 pad[0x34]; void *child; };
struct Obj { u8 pad00[0xE]; s8 index; u8 pad0f[0xB]; u16 flags; struct Parent *parent; u8 pad20[0x14]; u8 matrix[0x30]; u8 pad64[0x30]; u8 point[8]; };
extern s32 func_8003C7B8(s32, struct Obj *, s32, s32);
extern s32 func_8003C980(s32, struct Obj *, s32, s32);
extern void *func_8003D9F8(void *, u16, s8, s32, struct Parent *, void *);
extern void func_8001B148(void *, void *, void *, s32);
extern void func_8006F898(struct Obj *);
extern void func_800181D4(struct Obj *);
s32 func_8006FF44(s32 message, struct Obj *obj, s32 arg2, s32 arg3)
{
    struct Parent *parent;
    void *child;
    if (func_8003C7B8(message, obj, arg2, arg3)) goto ret0;
    if (message == 1) goto ret0;
    if (message == 0) goto case0;
    if (message == 6) goto ret1;
    if (message == 10) goto case10;
    goto dflt;
case0:
    if (obj->flags & 4) {
        parent = obj->parent;
        child = 0;
        if (parent) child = parent->child;
        func_8001B148(func_8003D9F8(child, obj->flags, obj->index, 1, parent, obj->matrix), obj->point, obj->point, 1);
    }
    func_8006F898(obj);
ret1:
    return 1;
case10:
    func_800181D4(obj);
    goto ret0;
dflt:
    return func_8003C980(message, obj, arg2, arg3);
ret0:
    return 0;
}
