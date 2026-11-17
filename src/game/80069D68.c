// Categories: Gameplay > Entity Script | Game Engine > State Machine | Gameplay > Entity Update
#include "game/types.h"

struct Entity {
    struct Entity *next;
    s32 (*update)(s32 message, struct Entity *entity, s32 arg2, s32 arg3);
};

extern u8 gScriptVmDisabled __asm__("D_8009F840");
extern s32 D_800A3320[];
extern s32 *D_8009F9A4;
extern s32 *D_8009F9F8;
extern s32 *gGameSession __asm__("D_8009FA94");
extern s32 *D_8009FAD4;
extern s8 D_800A8540[];

extern s32 func_80019668(void);
extern s32 func_80029A60(s32, struct Entity *, s32);
extern void func_800298EC(s32);
extern void func_80029974(s32);
extern s32 func_8004DE80(s32, s32);
extern void func_8004DA84(struct Entity *, s32, s32, s32);
extern void func_8004DC64(s32, s32, s32);
extern void func_8004DD68(s32, s32, s32);
extern void func_8004DDF4(s32, s32, s32);
extern void func_8004F0F0(s32, s32, s32);
extern void func_8006158C(s32, s32, s32);
extern void func_80061600(s32, s32, s32);
extern void func_800841C4(s32, s32, s32);


/**
 * @brief Executes a game-specific bytecode script using a stack-based VM.
 *
 * Interprets a sequence of bytecode instructions that perform arithmetic,
 * comparisons, variable access, conditional branching, and game-engine
 * operations. Scripts can read and modify global state, generate random
 * values, and invoke callbacks on game entities.
 *
 * The virtual machine uses D_800A3320 as a global operand stack.
 * Instructions are encoded as single-byte opcodes:
 *
 * - 0x80-0xFF: Push an immediate value (0-127).
 * - 0x01-0x05: Load constants or the supplied argument.
 * - 0x08, 0x14: Unconditional and conditional forward jumps.
 * - 0x09-0x0F: Logical operations, lookups, and engine calls.
 * - 0x10-0x1C: Read, increment, or decrement global variables.
 * - 0x1D-0x20: Write global variables.
 * - 0x21-0x2D: Comparisons, arithmetic, and boolean operations.
 * - 0x2E-0x2F: Conditional return and engine/entity calls.
 * - 0x30-0x37: Add or subtract values from global variables.
 * - 0x38-0x3E: Execute game-engine operations with three operands.
 *
 * Opcodes 0x09-0x1C consume one stack operand, 0x1D-0x37
 * consume two, and 0x38-0x7F consume three.
 * Some instructions push a result back onto the stack.
 *
 * Execution stops when a zero byte is encountered or an explicit
 * conditional return is executed. The interpreter does not perform
 * explicit stack bounds checking or bytecode validation.
 *
 * @param script Pointer to the zero-terminated bytecode sequence.
 * @param arg1 Value accessible to the script through opcode 0x05.
 * @param arg2 Optional initial stack value (pushed if nonnegative).
 * @param entity Entity context used by script-driven engine calls.
 *
 * @return 1 if the script begins with a zero byte.
 * @return 0 if script execution is disabled by D_8009F840.
 * @return Otherwise, the final stack value or the value returned
 *         by opcode 0x2E.
 *
 * @note Uses shared global VM state and may modify game variables
 *       or trigger entity callbacks and other engine-side effects.
 *
 * @see D_800A3320
 *
 * @param script Bytecode stream: a zero first byte is the empty script, and the
 *        stream is read as a NUL-terminated sequence of one-byte opcodes with
 *        inline immediates.
 * @param arg1 Caller-supplied operand returned by opcode 0x05; most call sites
 *        pass the sign bit of a state word, so it reads as a one-bit mode.
 * @param arg2 Optional initial stack value: pushed when non-negative, otherwise
 *        the operand stack starts empty.
 * @param entity Entity record used by the entity opcodes; only +0x00 (next
 *        pointer) and +0x04 (the update handler) are ever read.
 *
 * @return 1 for an empty script, 0 when D_8009F840 (the VM inhibit byte) is
 *         non-zero, the value of the 0x2E conditional return, otherwise the
 *         value left on top of the operand stack.
 *
 * @par Confirmed
 * The stack is the global word array D_800A3320. Its element 0 receives the
 * base index (1, or 2 when arg2 was pushed) and the operands live above it, so
 * after the final byte the function decrements the depth and returns
 * D_800A3320[depth]. Opcodes 0x80..0xFF push their low seven bits. 0x01 pushes
 * the complement of the next byte, 0x02 that byte plus 0x80, 0x03 a big-endian
 * halfword and 0x04 a big-endian word; 0x05 pushes arg1. Operand counts follow
 * three thresholds applied before the switch: opcodes 0x09 and above pop one
 * value, 0x1D and above pop two, 0x38 and above pop three, and values are
 * pushed back by the shared push label.
 * Loads: 0x10 and 0x13 read D_8009FA94[a] and D_8009FAD4[a]; 0x11 reads
 * through the word at D_8009FA94[a + 0x39] (a second, pointer-indexed bank);
 * 0x12 reads D_8009F9A4[a] / 5, so that bank stores its values multiplied by
 * five. 0x1D..0x20 are the matching stores (again * 5 for D_8009F9A4), 0x15..0x18 and 0x19..0x1C increment and decrement the four banks, and 0x30..0x37
 * add or subtract the second operand. 0x0A reads the signed byte table
 * D_800A8540. 0x0B is the random opcode: a non-negative operand indexes the
 * 32-entry table D_8009F9F8, a negative one yields func_80019668() modulo the
 * magnitude. 0x09 turns the operand into a 0/1 test, 0x08 jumps forward by the
 * following byte and 0x14 does the same only when the operand is zero (both
 * skip the byte otherwise). 0x21..0x2D are equality, inequality, the four
 * orderings with b as the left operand, addition, b - a, multiplication, b / a,
 * b % a, and, or. 0x2E returns a immediately when b is zero.
 * Entity opcodes: 0x0C/0x0D queue a command through func_800298EC /
 * func_80029974, 0x0E/0x0F send one synchronously through
 * func_80029A60(a, entity, 0/1). 0x2F evaluates entity->update(0xC, entity, a, 0)
 * when b is zero and func_8004DE80(b, a) otherwise. 0x38 applies
 * func_8004DA84(entity, b, a, 1) when c is zero, else func_8004DC64(c, b, a);
 * 0x39..0x3E are func_8004DD68, func_8004DDF4, func_8004F0F0, func_8006158C,
 * func_80061600 and func_800841C4 with the same (c, b, a) order. Every other
 * opcode is a no-op.
 *
 * @par Likely role
 * The interpreter behind every scripted entity: the handlers call it once per
 * message with the script pointer kept in their own state block, and the third
 * argument is usually an entity flags value masked with 0x7FFF, i.e. a handle
 * the script can pass on to the entity opcodes. The four variable banks look
 * like per-level script globals: WiFi-free, they are seeded by func_80069B58
 * (32 random entries in D_8009F9F8, D_8009F9A4 cleared to -10, D_8009FAD4
 * cleared) and released again by func_80069D58, which sets D_8009F840 to stop
 * the machine.
 *
 * @par Unknown
 * The exact type of the values behind the D_8009FA94 pointer bank (a word
 * holding an address that the opcode then dereferences), what each of the
 * engine-call opcodes actually does downstream, and why some call sites pass
 * only three arguments, leaving the entity pointer undefined.
 *
 * @par Callers
 * func_80028BCC, func_8002A940, func_8002AA1C, func_8002D1A0, func_800306A0,
 * func_80030C60, func_80031004, func_80031120, func_80031C74, func_800324E0,
 * func_80049564, func_8004D958, func_8004DEE0, func_80058378, func_80058714,
 * func_800600E8, func_80060238, func_8006125C.
 */
s32 func_80069D68(u8 *script, s32 arg1, s32 arg2, struct Entity *entity)
{
    u8 *code;
    s32 *stack;
    s32 depth;
    s32 a;
    s32 b;
    s32 c;
    struct Entity *ent;
    s32 saved;
    s32 op;
    s32 rnd;

    code = script;
    saved = arg1;
    ent = entity;

    if (*code == 0) {
        return 1;
    }
    if (gScriptVmDisabled != 0) {
        return 0;
    }

    depth = 1;
    D_800A3320[0] = depth;
    if (arg2 >= 0) {
        D_800A3320[1] = arg2;
        depth = 2;
    }
    stack = &D_800A3320[depth];

    do {
        op = *code++;
        if (op & 0x80) {
            a = op & 0x7F;
            goto push;
        }
        if (op >= 9) {
            a = *--stack;
            depth--;
            if (op >= 0x1D) {
                b = *--stack;
                depth--;
                if (op >= 0x38) {
                    stack--;
                    depth--;
                    c = *stack;
                }
            }
        }
        switch (op) {
        case 1:
            a = *code++;
            a = ~a;
            goto push;
        case 2:
            a = *code++;
            a += 0x80;
            goto push;
        case 3:
            a = *code++;
            a = (a << 8) | *code++;
            goto push;
        case 4:
            a = *code++;
            a = (a << 8) | *code++;
            a = (a << 8) | *code++;
            a = (a << 8) | *code++;
            goto push;
        case 5:
            a = saved;
            goto push;
        case 16:
            a = gGameSession[a];
            goto push;
        case 17:
            a = *(s32 *)(gGameSession[a + 0x39]);
            goto push;
        case 18:
            a = D_8009F9A4[a] / 5;
            goto push;
        case 19:
            a = D_8009FAD4[a];
            goto push;
        case 29:
            gGameSession[a] = b;
            break;
        case 30:
            *(s32 *)(gGameSession[a + 0x39]) = b;
            break;
        case 31:
            D_8009F9A4[a] = b * 5;
            break;
        case 32:
            D_8009FAD4[a] = b;
            break;
        case 21:
            gGameSession[a]++;
            goto push;
        case 22:
            gGameSession[a + 0x39]++;
            goto push;
        case 23:
            D_8009F9A4[a] += 5;
            goto push;
        case 24:
            D_8009FAD4[a]++;
            goto push;
        case 25:
            gGameSession[a]--;
            goto push;
        case 26:
            gGameSession[a + 0x39]--;
            goto push;
        case 27:
            D_8009F9A4[a] -= 5;
            goto push;
        case 28:
            D_8009FAD4[a]--;
            goto push;
        case 48:
            gGameSession[a] += b;
            goto push;
        case 49:
            gGameSession[a + 0x39] += b;
            goto push;
        case 50:
            D_8009F9A4[a] += b * 5;
            goto push;
        case 51:
            D_8009FAD4[a] += b;
            goto push;
        case 52:
            gGameSession[a] -= b;
            goto push;
        case 53:
            gGameSession[a + 0x39] -= b;
            goto push;
        case 54:
            D_8009F9A4[a] -= b * 5;
            goto push;
        case 55:
            D_8009FAD4[a] -= b;
            goto push;
        case 9:
            a = (a == 0);
            goto push;
        case 10:
            a = D_800A8540[a];
            goto push;
        case 11:
            if (a >= 0) {
                a = a % 32;
                a = D_8009F9F8[a];
            } else {
                b = func_80019668();
                op = b >> 31;
                b = (op + b) ^ op;
                a = b % (-a);
            }
            goto push;
        case 15:
            a = func_80029A60(a, ent, 1);
            goto push;
        case 14:
            a = func_80029A60(a, ent, 0);
            goto push;
        case 13:
            func_80029974(a);
            break;
        case 12:
            func_800298EC(a);
            break;
        case 20:
            if (a == 0) {
        case 8:
                code += *code;
            } else {
                code++;
            }
            break;
        case 33:
            a = (b == a);
            goto push;
        case 34:
            a = (b != a);
            goto push;
        case 35:
            a = (a < b);
            goto push;
        case 36:
            a = (a <= b);
            goto push;
        case 37:
            a = (b < a);
            goto push;
        case 38:
            a = (a >= b);
            goto push;
        case 39:
            a = a + b;
            goto push;
        case 40:
            a = b - a;
            goto push;
        case 41:
            a = a * b;
            goto push;
        case 42:
            a = b / a;
            goto push;
        case 43:
            a = b % a;
            goto push;
        case 44:
            a = (b && a);
            goto push;
        case 45:
            a = (b || a);
            goto push;
        case 46:
            if (b == 0) {
                return a;
            }
            break;
        case 47:
            if (b == 0) {
                a = ent->update(0xC, ent, a, 0);
            } else {
                a = func_8004DE80(b, a);
            }
            goto push;
        case 56:
            if (c == 0) {
                func_8004DA84(ent, b, a, 1);
            } else {
                func_8004DC64(c, b, a);
            }
            break;
        case 58:
            func_8004DDF4(c, b, a);
            break;
        case 57:
            func_8004DD68(c, b, a);
            break;
        case 59:
            func_8004F0F0(c, b, a);
            break;
        case 60:
            func_8006158C(c, b, a);
            break;
        case 61:
            func_80061600(c, b, a);
            break;
        case 62:
            func_800841C4(c, b, a);
            break;
        default:
            break;
        }
        continue;
    push:
        *stack = a;
        stack++;
        depth++;
    } while (*code != 0);

    depth--;
    return D_800A3320[depth];
}
