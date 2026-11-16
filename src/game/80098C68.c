// Categories: Platform > Device Control
typedef unsigned int u32;
typedef unsigned short u16;
typedef struct { volatile u16 a, b, c, d; } Ports;
typedef struct { char pad[0x188]; u16 a, b, c, d; } Device;
extern volatile u32 D_8009EE90, D_8009EA5C, D_8009EA58;
extern u32 D_8009EA30;
extern Ports D_800A7028;
extern Device *D_8009EEA4;

/**
 *  @brief Enables or disables selected device bits using one of two register paths.
 *
 *  Only the low 24 mask bits are used, split across low and high 16-bit fields.
 *  The selected path also maintains a corresponding software mask.
 *  @param mode 0 disables bits, 1 enables bits; other values perform no register update.
 *  @param mask Bit mask of device bits to change.
 *  @par Globals
 *  D_8009EE90 selects the path; D_800A7028 holds one register set and D_8009EEA4 points to the other.
 *  D_8009EA5C and D_8009EA58 track updates on the first path; D_8009EA30 tracks the second path.
 *  @par Control flow
 *  Mode selects enable or disable, then the path flag selects direct port or device-pointer writes.
 *  On the first path, conflicting bits in the opposite port fields are cleared when present.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes device registers and updates the selected software tracking globals.
 *  @par Callers
 *  Confirmed: func_80021E1C.
 */
void func_80098C68(int mode, u32 mask)
{
    register u32 result __asm__("$2");
    register u32 inv __asm__("$3");
    register u32 m __asm__("$5") = mask;
    register u32 high __asm__("$6");
    register Ports *port __asm__("$4");
    m &= 0xffffff;
    high = m >> 16;
    if (mode == 0) goto off;
    if (mode == 1) {
        if (D_8009EE90 & 1) {
            __asm__("lui %0,%%hi(D_800A7028)\n\taddiu %0,%0,%%lo(D_800A7028)" : "=r"(port));
            port->a = m;
            port->b = high;
            D_8009EA5C |= 1;
            D_8009EA58 |= m;
            if (port->c & m) port->c &= ~m;
            if (port->d & high) ((u16 *)port)[3] = port->d & ~high;
        } else {
            D_8009EEA4->a = m;
            D_8009EEA4->b = high;
            result = D_8009EA30 | m;
            goto store;
        }
    } else {
        goto done;
    }
    goto done;
off:
    {
        if (D_8009EE90 & 1) {
            __asm__("lui %0,%%hi(D_800A7028)\n\taddiu %0,%0,%%lo(D_800A7028)" : "=r"(port));
            port->c = m;
            port->d = high;
            D_8009EA5C |= 1;
            __asm__("" : "+r"(m));
            inv = ~m;
            D_8009EA58 &= inv;
            if (port->a & m) port->a &= inv;
            if (port->b & high) ((u16 *)port)[1] = port->b & ~high;
        } else {
            D_8009EEA4->c = m;
            D_8009EEA4->d = high;
            __asm__("" : "+r"(m) : : "memory");
            result = D_8009EA30 & ~m;
            goto store;
        }
    }
    goto done;
store:
    D_8009EA30 = result;
done:
    __asm__("" : : "r"(m), "r"(high));
}
