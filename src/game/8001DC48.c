// Categories: Resources > File Access | Utilities > String
typedef unsigned char u8;
extern int func_8008CCC0(const u8 *);
extern void func_8008CCB0(u8 *, const u8 *);
extern void func_8008CC90(u8 *, const u8 *);
extern u8 func_8008CCE0(int);
extern int gCompletionFlag __asm__("D_8009FA68");
extern int gFileSlotState __asm__("D_8009F620");

/**
 * @brief Builds the on-disc path of a file request: joins directory and name,
 *        upper-cases, removes illegal characters and appends the version suffix.
 *
 * @param mode Access mode; only the low nibble is used. 1 enables the name
 *        conversion (separators, extension and version handling), any other value
 *        only joins, upper-cases and filters.
 * @param name File name, may be null.
 * @param dir Directory or volume prefix, may be null.
 * @param out Destination buffer; names that would need 64 characters or more are
 *        refused.
 * @return The index of the last character written, or -18 when the result does not
 *         fit the buffer and -19 for a malformed name.
 *
 * @par Confirmed
 * Both lengths come from func_8008CCC0 (BIOS strlen). Empty directory and name
 * write out[0] = 0 and return 0, and pos + namelen + 1 >= 64 returns -18.
 * The directory is copied with func_8008CCB0 (BIOS strcpy) and a separator is
 * appended unless the last directory character is one of backslash, ':', ';', '.'
 * or slash, or the first name character is one of them; mode 2 with a one-character
 * name uses ':' instead of a backslash. The name is appended with func_8008CC90
 * (BIOS strcat) and the whole string is then walked character by character through
 * func_8008CCE0 (BIOS toupper).
 * A character is dropped from the output when it lies in 0x01..0x2C except '!' and
 * '*', in 0x3D..0x40 except '?', or at 0x5B and above except backslash and '_';
 * every other character is copied (only if below 128) at the next output position.
 * In mode 1 the remaining cases are handled: ':' restarts the output after
 * rejecting a second colon, a colon after a dot or a colon after a directory
 * separator (-19); '/' becomes a backslash; a backslash only bumps the separator
 * count; a ';' at the end of the string is dropped, a ';' with no preceding dot
 * turns into a dot and sets the extension position, and a ';' after an extension
 * sets the version flag (a second one is -19); a second dot is -19; the terminator
 * with nothing written is -19, and otherwise a missing dot is appended followed by
 * ";1" when no version was seen.
 * The return value is the index of the last character stored, i.e. the length minus
 * one.
 *
 * @par Likely role
 * The single normaliser between the game's file names and the on-disc directory
 * format, used by the open path func_8001D424 before it looks the name up in the
 * open-file table. The ';1' suffix, the upper-casing and the 8.3 character filter
 * are the ISO9660 conventions, and a device prefix is stripped by restarting the
 * output at the colon.
 *
 * @par Unknown
 * - Why a ';' without a preceding dot is turned into a dot rather than rejected.
 * - Whether the character filter is exactly the ISO9660 d-character set; '<' and
 *   '>' are both kept, which that set does not allow.
 * - Why the length check uses 64 while the caller's buffer is described as a name
 *   buffer without a stated size.
 *
 * @par Callers
 * func_8001D424 (the open-file slot allocator), which passes mode 1 and the
 * 64-byte name buffer at 0x800A6DD8.
 */
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
