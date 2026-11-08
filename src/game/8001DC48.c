typedef unsigned char u8;
extern int func_8008CCC0(const u8 *);
extern void func_8008CCB0(u8 *, const u8 *);
extern void func_8008CC90(u8 *, const u8 *);
extern u8 func_8008CCE0(int);
extern int D_8009FA68;
extern int D_8009F620;

int func_8001DC48(int mode, const u8 *name, const u8 *dir, u8 *out)
{
    u8 buf[64];
    int end;
    register int namelen __asm__("$18") = 0;
    register int pos __asm__("$16") = 0;
    register int slash __asm__("$23") = 0;
    register int version __asm__("$22") = 0;
    register int dot __asm__("$19") = -9;
    register int i __asm__("$18");
    u8 c;
    register int marker __asm__("$8");
    mode &= 15;
    if (name) namelen = func_8008CCC0(name);
    if (dir) pos = func_8008CCC0(dir);
    if (pos + namelen == 0) { out[0] = 0; return pos; }
    if (pos + namelen + 1 >= 64) return -18;
    if (pos) {
        func_8008CCB0(buf, dir);
        if (namelen > 0 && dir[pos-1] != '\\' && dir[pos-1] != ':' && dir[pos-1] != ';' && dir[pos-1] != '.' && dir[pos-1] != '/' && name[0] != '\\' && name[0] != ':' && name[0] != ';' && name[0] != '.' && name[0] != '/') {
            if (namelen == 1 && mode == 2) buf[pos] = ':';
            else buf[pos] = '\\';
            pos++;
        }
    }
    { register u8 *p __asm__("$2") = buf + pos;
      __asm__ volatile("" : "+r" (p));
      *p = 0; }
    if (namelen) func_8008CC90(buf, name);
    end = func_8008CCC0(buf) - 1;
    i = -1;
    pos = -1;
    while (1) {
        i++;
        c = func_8008CCE0(buf[i]);
        if (((u8)(c - 1) < 44 && c != '!' && c != '*') || ((u8)(c - 61) < 4 && c != '?') || (c >= 91 && c != '\\' && c != '_')) continue;
        if (mode == 1) {
            { int t = '\\' - slash; slash = '\\' - t; }
            switch (c) {
            case ':':
                marker = -9;
                __asm__ volatile("" : "+r" (marker));
                if (marker >= 0 || dot >= 0) return -19;
                __asm__ volatile("" : "+r" (marker));
                if (marker >= 0 || slash > 0) return -19;
                c = 255; pos = -1; break;
            case ';':
                if (pos == -1 || version == mode) return -19;
                { register int last __asm__("$8") = end;
                __asm__ volatile("" : "+r" (last));
                if (i == last) c = 255;
                else if (dot < 0) { dot = pos + 1; i--; c = '.'; }
                else version = 1; }
                break;
            case '.':
                if (dot >= 0) return -19;
                dot = pos + 1; break;
            case '/': c = '\\'; break;
            case '\\': slash++; break;
            case 0:
                if (pos < 0) return -19;
                if (dot < 0) out[++pos] = '.';
                if (!(u8)version) { out[++pos] = ';'; out[++pos] = '1'; }
                break;
            }
        }
        if (c < 128) out[++pos] = c;
        if (!c) break;
    }
    return pos;
}
