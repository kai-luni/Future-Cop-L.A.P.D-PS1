typedef signed int s32;
typedef unsigned int u32;
typedef signed short s16;
typedef unsigned short u16;
typedef struct { char p0[0x16]; s16 x,y,z; char p1[8]; s16 size; char p2[2]; s16 rate, scale, rate2; char p3[14]; u32 flags; char p4[12]; u16 angle, angle2; char p5[2]; u16 angle3, angle4; char p6[2]; s16 turn; u16 turnout; } Obj;
typedef struct { char p[0x50]; s32 x,y,z; } Vec;
#define MUL(o,a,b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(o) : "r"(a),"r"(b) : "$8","$9")
#define MULN(o,a,b) __asm__("nop\n\tmult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(o) : "r"(a),"r"(b) : "$8","$9")
#define MULU(o,a,b) __asm__("nop\n\tnop\n\tmultu %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(o) : "r"(a),"r"(b) : "$8","$9")
void func_8004FFD0(Obj *obj, s32 input, Vec *vec) {
 register Obj *p __asm__("$10") = obj;
 register s32 size __asm__("$7");
 register s32 absin __asm__("$2");
 register s32 d __asm__("$4");
 register s32 den __asm__("$3");
 register s32 r __asm__("$4");
 s32 v;
 register s32 q __asm__("$4");
 register s32 temp __asm__("$2");
 register s32 initial __asm__("$2") = p->size;
 register s32 turn __asm__("$3") = p->turn;
 __asm__("" : "+r"(initial));
 size = initial << 8;
 if (turn && ((p->flags >> 28) & 3)) {
  absin = ((input >> 31) + input) ^ (input >> 31);
  d = size - absin;
  if (d > 0) {
   if (size > 0) { den = size >> 1; goto pos; }
   den = -size;
   den >>= 1;
   goto neg;
  }
  d = absin - size;
  if (size > 0) {
   den = size >> 1;
 neg:
   MULU(temp, d, 0x80000000u / (u32)den); r = -temp;
   goto done;
  }
  den = -size; den >>= 1;
 pos:
  MULU(r, d, 0x80000000u / (u32)den);
 done:
  MUL(temp, p->turn << 16, r);
  p->turnout = (temp >> 16) & 0xfff;
 }
 if (p->rate) {
  if (!(p->flags & 0x1000)) {
   register s32 product __asm__("$2");
   register s32 angleq __asm__("$3");
   register s32 product2 __asm__("$4");
   register s32 sum __asm__("$2");
   product = (p->scale << 2) * p->rate;
   __asm__("" : "+r"(product));
   angleq = product >> 9;
   __asm__("" : "+r"(angleq));
   __asm__("" : "+r"(size));
   sum = size + input;
   __asm__("" : "+r"(sum));
   product2 = angleq * sum;
   angleq = product2 >> 16;
   __asm__("" : "+r"(angleq));
   p->angle2 = (p->angle + angleq) & 0xfff;
  }
  __asm__("" : "+r"(input));
  { register s32 sign __asm__("$3") = input >> 31;
  __asm__("" : "+r"(sign));
  absin = (sign + input) ^ sign; }
  MUL(q, p->scale << 10, size - absin);
  q >>= 2;
  q = q * q;
  __asm__("" : "+r"(q));
  q >>= 12;
  { register s32 vx __asm__("$3"); register s32 vy __asm__("$2");
  MULN(vx, p->x << 8, q); vec->x += vx;
  MUL(vy, p->y << 8, q); vec->y += vy;
  MUL(vy, p->z << 8, q); vec->z += vy; }
 }
 { register s32 rate2 __asm__("$6") = p->rate2;
 if (rate2) {
  register s32 sum __asm__("$3") = size + input;
  register s32 result __asm__("$3");
  register s32 scale __asm__("$2") = p->scale;
  __asm__("" : "+r"(scale));
  sum >>= 1;
  MUL(q, sum, scale << 10);
  __asm__("" : "+r"(rate2));
  scale = rate2 << 16;
  __asm__("" : "+r"(scale));
  MUL(result, scale, q);
  q = result >> 16;
  p->angle4 = (p->angle3 + q) & 0xfff;
 } }
}
