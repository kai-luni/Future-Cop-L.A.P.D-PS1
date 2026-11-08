typedef unsigned int u32;
typedef int s32;

struct Entry {
    u32 addr;
    u32 size;
};

extern s32 D_8009EA3C;
extern s32 D_8009EA40;
extern s32 D_8009EECC;
extern s32 D_8009EF04;
extern s32 D_8009EF08;
extern struct Entry *D_8009EF0C;

extern void func_80097DF8(void);

s32 func_800980F8(s32 a0, u32 a1, s32 a2)
{
    register s32 a0_reg __asm__("$10") = a0;
    register u32 mask __asm__("$4");
    register s32 a2_reg __asm__("$8") = a2;
    register u32 s0 __asm__("$16");
    register u32 entry_addr __asm__("$11");
    register s32 entry_size __asm__("$12");
    register s32 t1 __asm__("$9");
    register u32 index __asm__("$2");
    s32 v1;
    register s32 avail __asm__("$7");
    register s32 temp __asm__("$2");
    s32 limit;
    s32 count;

    mask = 0x0FFFFFFF;
    entry_addr = D_8009EF0C[a0_reg].addr;
    entry_size = D_8009EF0C[a0_reg].size;
    s0 = a1;
    t1 = s0 - (entry_addr & mask);

    if (!D_8009EA3C) {
        v1 = 0;
    } else {
        v1 = (0x10000 - D_8009EA40) << D_8009EECC;
    }

    if (entry_addr & 0x40000000) {
        temp = entry_size - t1;
        __asm__("" : "+r"(temp));
        avail = temp - v1;
    } else {
        avail = entry_size - t1;
    }

    if (avail < a2_reg) {
        return -1;
    }

    if (entry_addr & 0x40000000) {
        if (t1 > 0) {
            if (D_8009EF08 > D_8009EF04 - 2) {
                return -1;
            }
            D_8009EF0C[D_8009EF08].size = t1;
            D_8009EF0C[D_8009EF08].addr = (D_8009EF0C[D_8009EF08].addr & 0x0FFFFFFF) | 0x80000000;
            D_8009EF08++;
            D_8009EF0C[D_8009EF08].addr = s0;
            D_8009EF0C[D_8009EF08].size = a2_reg;
        } else {
            if (D_8009EF08 > D_8009EF04 - 1) {
                return -1;
            }
            D_8009EF0C[D_8009EF08].size = a2_reg;
            D_8009EF0C[D_8009EF08].addr &= 0x0FFFFFFF;
        }
        {
            register u32 tail_addr __asm__("$2");
            register s32 tail_size __asm__("$2");
            tail_addr = (entry_addr & 0x0FFFFFFF) + t1 + a2_reg;
            __asm__("" : "+r"(tail_addr));
            D_8009EF08++;
            D_8009EF0C[D_8009EF08].addr = tail_addr | 0x40000000;
            __asm__("");
            tail_size = entry_size - t1;
            __asm__("" : "+r"(tail_size));
            D_8009EF0C[D_8009EF08].size = tail_size - a2_reg;
        }
    } else {
        if (t1 > 0) {
            register struct Entry *base __asm__("$5");
            register struct Entry *e0 __asm__("$2");
            register u32 old_a __asm__("$3");
            register u32 old_s __asm__("$4");
            register s32 next_i __asm__("$6");

            if (avail == a2_reg) {
                limit = D_8009EF04;
                count = D_8009EF08;
                __asm__("" : "+r"(limit), "+r"(count));
                limit -= 2;
            } else {
                limit = D_8009EF04;
                count = D_8009EF08;
                __asm__("" : "+r"(limit), "+r"(count));
                limit -= 1;
            }
            if (limit < count) {
                return -1;
            }
            index = a0_reg << 3;
            __asm__("" : "+r"(index));
            base = D_8009EF0C;
            a0_reg = D_8009EF08;
            __asm__("" : "+r"(base), "+r"(a0_reg));
            e0 = (struct Entry *)(index + (u32)base);
            e0->size = t1;
            index = a0_reg << 3;
            __asm__("" : "+r"(index));
            e0 = (struct Entry *)(index + (u32)base);
            old_a = e0->addr;
            old_s = e0->size;
            __asm__("" : "+r"(old_a), "+r"(old_s));
            next_i = a0_reg + 1;
            __asm__("" : "+r"(next_i));
            e0->addr = s0;
            e0->size = a2_reg;
            D_8009EF08 = next_i;
            e0[1].addr = old_a;
            e0[1].size = old_s;
            __asm__("" : : "m"(e0[1].addr), "m"(e0[1].size));

            if (avail >= a2_reg) {
                register u32 new_a __asm__("$2");
                register struct Entry *e1 __asm__("$3");
                register u32 hi_bit __asm__("$4");
                register u32 old_a2 __asm__("$5");
                register u32 old_s2 __asm__("$4");

                new_a = 0x0FFFFFFF;
                __asm__("" : "+r"(new_a));
                e1 = (struct Entry *)(next_i << 3);
                __asm__("" : "+r"(e1));
                e1 = (struct Entry *)((u32)e1 + (u32)base);
                __asm__("" : "+r"(e1));
                new_a = (entry_addr & new_a) + t1 + a2_reg;
                hi_bit = 0x80000000;
                __asm__("" : "+r"(new_a), "+r"(hi_bit));
                new_a |= hi_bit;
                old_a2 = e1->addr;
                old_s2 = e1->size;
                __asm__("" : "+r"(old_a2), "+r"(old_s2), "+r"(new_a));
                e1->addr = new_a;
                new_a = avail - a2_reg;
                __asm__("" : "+r"(new_a));
                e1->size = new_a;
                new_a = a0_reg + 2;
                __asm__("" : "+r"(new_a));
                D_8009EF08 = new_a;
                e1[1].addr = old_a2;
                e1[1].size = old_s2;
            }
        } else {
            register u32 m6 __asm__("$6");
            register struct Entry *base4 __asm__("$4");
            register struct Entry *e0 __asm__("$2");
            register u32 v_addr __asm__("$3");

            if (avail > a2_reg) {
                __asm__("" : "=r"(m6));
                if (D_8009EF08 > D_8009EF04 - 1) {
                    return -1;
                }
            }
            m6 = 0x0FFF0000;
            base4 = D_8009EF0C;
            __asm__("" : "+r"(m6), "+r"(base4));
            index = a0_reg << 3;
            __asm__("" : "+r"(index));
            e0 = (struct Entry *)(index + (u32)base4);
            __asm__("" : "+r"(e0));
            v_addr = e0->addr;
            __asm__("" : "+r"(v_addr));
            m6 |= 0xFFFF;
            __asm__("" : "+r"(m6));
            e0->size = a2_reg;
            __asm__("");
            v_addr &= m6;
            __asm__("" : "+r"(v_addr));
            e0->addr = v_addr;
            __asm__("" : : "m"(e0->addr));

            if (avail > a2_reg) {
                register u32 new_a __asm__("$2");
                register s32 idx5 __asm__("$5");
                register struct Entry *e1 __asm__("$3");
                register u32 hi_bit __asm__("$4");
                register u32 old_a2 __asm__("$6");
                register u32 old_s2 __asm__("$4");

                new_a = entry_addr & m6;
                __asm__("" : "+r"(new_a));
                idx5 = D_8009EF08;
                __asm__("" : "+r"(idx5));
                new_a += a2_reg;
                e1 = (struct Entry *)((idx5 << 3) + (u32)base4);
                hi_bit = 0x80000000;
                __asm__("" : "+r"(new_a), "+r"(e1), "+r"(hi_bit));
                old_a2 = e1->addr;
                __asm__("" : "+r"(old_a2), "+r"(new_a));
                new_a |= hi_bit;
                __asm__("" : "+r"(new_a));
                e1->addr = new_a;
                new_a = avail - a2_reg;
                old_s2 = e1->size;
                __asm__("" : "+r"(new_a), "+r"(old_s2));
                idx5 += 1;
                __asm__("" : "+r"(idx5), "+r"(new_a));
                e1->size = new_a;
                __asm__("");
                D_8009EF08 = idx5;
                e1[1].addr = old_a2;
                e1[1].size = old_s2;
            }
        }
    }

    func_80097DF8();
    return s0;
}
