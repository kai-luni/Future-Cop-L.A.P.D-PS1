typedef unsigned int u32;

extern u32 D_80076A0C[];
extern void func_80076878(void);
extern void func_8008C9C0(void);
extern void func_8008C9E0(void);

void func_80077340(void)
{
    u32 *src = (u32 *)0x80;
    u32 *dst = D_80076A0C;
    register u32 handler __asm__("$3");
    register u32 instruction __asm__("$2");
    register u32 first __asm__("$4");
    register u32 second __asm__("$4");
    int i;

    func_8008C9C0();
    for (i = 0; i < 8; i++) {
        *dst++ = *src++;
    }
    handler = (u32)func_80076878 + 0x4c;
    __asm__ volatile ("" : "+r" (handler));
    first = handler >> 16;
    __asm__ volatile ("" : "+r" (first));
    first |= 0x3c1a0000;
    handler &= 0xffff;
    __asm__ volatile ("" : "+r" (handler));
    instruction = 0x375a0000;
    __asm__ volatile ("" : "+r" (instruction));
    ((u32 *)0x80)[0] = first;
    second = handler | instruction;
    __asm__ volatile ("" : "+r" (second));
    __asm__ volatile ("" : : : "memory");
    instruction = 0x03400008;
    __asm__ volatile ("" : "+r" (instruction));
    ((u32 *)0x80)[1] = second;
    ((u32 *)0x80)[2] = instruction;
    ((u32 *)0x80)[3] = 0;
    func_8008C9E0();
}
