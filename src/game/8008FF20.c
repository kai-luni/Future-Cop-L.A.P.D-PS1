// Categories: Platform > CD-ROM | Resources > File Access
#include "game/types.h"

typedef struct Entry { s32 a, b; char name[16]; } Entry;
extern s32 gCdSeenResponseCount __asm__("D_8009CBC4");
extern s32 gCdResponseCount __asm__("D_8009C8BC");
extern s32 gCdLogLevel __asm__("D_8009C8B0");
extern Entry gCdDirEntryCache[] __asm__("D_800A3E80");
extern char gCachedDirName[] __asm__("D_800A3E88");
extern char gCdDepthErrFmt[] __asm__("D_800125AC");
extern char gCdBadPathErrFmt[] __asm__("D_800125C8");
extern char D_800125E0[];
extern char gCdSearchLogFmt[] __asm__("D_800125FC");
extern char gCdFoundLogFmt[] __asm__("D_8001261C");
extern char gCdNotFoundErrFmt[] __asm__("D_80012628");
extern s32 gCdReadPathTable(void) __asm__("func_80090218");
extern s32 gFindCdPathDir(s32, char *) __asm__("func_800904DC");
extern s32 gCacheCdDirSector(s32) __asm__("func_80090580");
extern s32 gMatchCdFilename(char *, char *) __asm__("func_800901F8");
extern void func_8008E000(char *, ...);
Entry *gCdSearchFile(Entry *out, char *path) __asm__("func_8008FF20");


/**
 * @brief Resolves a backslash-delimited ISO-9660 path to a CD directory entry (CdSearchFile).
 *
 * Refreshes the ISO path table via func_80090218 if D_8009CBC4 != D_8009C8BC, rejects paths not starting with '\\', walks up to 7 directory levels via func_800904DC, caches the leaf directory with func_80090580, scans up to 64 entries in D_800A3E80 with func_800901F8, and copies the matched 24-byte Entry to *out.
 *
 * @param out Destination Entry struct receiving location, size, and filename.
 * @param path NUL-terminated ISO path string starting with '\\'.
 * @return Pointer to matched entry in D_800A3E80, or 0 on media change failure, depth >= 8, invalid path, or missing file.
 *
 * @par Globals
 * D_8009CBC4/D_8009C8BC - media-change sync counters; D_800A3E80/D_800A3E88 - cached directory entries/names; D_8009C8B0 - debug verbosity.
 *
 * @par Control flow
 * Media-change guard, path-component parsing loop (< 8 components), directory load, and linear scan over <= 64 cached entries.
 *
 * @par Calls
 * func_80090218 - reads PVD/path table; func_800904DC - resolves dir id; func_80090580 - caches dir sector; func_800901F8 - 12-byte name match; func_8008E000 - debug logs.
 *
 * @par Side effects
 * Updates D_8009CBC4, refreshes ISO path/directory caches, and writes *out on match.
 *
 * @par Callers
 * Confirmed: func_8001D424 (0x8001D548, file open) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Resolves an ISO-9660 path string starting with a backslash into a 24-byte
 * directory Entry written to out, refreshing the path table via func_80090218
 * whenever D_8009CBC4 differs from D_8009C8BC. It walks up to 7 directory
 * levels with func_800904DC, caches the leaf directory sector with
 * func_80090580, and scans up to 64 entries in D_800A3E80 using func_800901F8
 * while logging errors to func_8008E000 according to D_8009C8B0. Driven by
 * func_8001D424.
 */
Entry *gCdSearchFile(Entry *out, char *path)
{
    char buf[32];
    register char *p __asm__("$16");
    register char *q __asm__("$17");
    register s32 count __asm__("$18");
    register s32 slash __asm__("$21");
    register s32 neg __asm__("$20");
    register char *orig __asm__("$19");
    register Entry *dest __asm__("$22");
    s32 dir;
    register char *base __asm__("$2");
    register s32 off __asm__("$17");
    register Entry *entry __asm__("$16");
    register char *name __asm__("$19");
    orig = path;
    if (gCdSeenResponseCount != gCdResponseCount) {
        if (!gCdReadPathTable()) return 0;
        gCdSeenResponseCount = gCdResponseCount;
    }
    dest = out;
    if (*(signed char *)orig != '\\') return 0;
    buf[0] = 0;
    __asm__ volatile("");
    dir = 1;
    p = orig;
    count = 0;
    slash = '\\';
    neg = -1;
    do {
        s32 ch = *(unsigned char *)p;
        register s32 inner_slash __asm__("$5");
        register s32 c2 __asm__("$2");
        q = buf;
        if ((c2 = *(signed char *)p) != slash) {
            inner_slash = '\\';
            do {
                if (!ch) goto end;
                p++;
                *q = ch;
                ch = *(unsigned char *)p;
                q++;
            } while (*(signed char *)p != inner_slash);
            __asm__ volatile("lb\t%0,0(%1)\n\tnop" : "=r"(c2) : "r"(p));
        }
        if (!c2) goto end;
        p++;
        *q = 0;
        dir = gFindCdPathDir(dir, buf);
        if (dir == neg) { buf[0] = 0; break; }
        count++;
    } while (count < 8);
end:
    if (count >= 8) {
        if (gCdLogLevel > 0) func_8008E000(gCdDepthErrFmt, orig, count);
        return 0;
    }
    if (!*(signed char *)buf) {
        if (gCdLogLevel > 0) {
            register char *a1 __asm__("$5") = orig;
            __asm__ volatile("" : "=r"(a1) : "0"(a1));
            {
                register char *a0 __asm__("$4") = (char *)0x80010000;
                __asm__ volatile("" : "=r"(a0) : "0"(a0));
                func_8008E000(a0 + 0x25C8, a1);
            }
            __asm__ volatile(".set reorder");
        }
        return 0;
    }
    *q = 0;
    if (!gCacheCdDirSector(dir)) {
        if (gCdLogLevel <= 0) return 0;
        __asm__ volatile(".set\tnoreorder\n\tlui $4,%%hi(D_800125E0)\n\tjal func_8008E000\n\taddiu $4,$4,%%lo(D_800125E0)\n\t.set\treorder"
            : : : "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "$31", "memory");
        return 0;
    }
    count = 0;
    if (gCdLogLevel >= 2) {
        func_8008E000(gCdSearchLogFmt, buf);
        __asm__ volatile("" : "=r"(count));
        count = 0;
    }
    base = gCachedDirName;
    __asm__("" : "=r"(base) : "0"(base));
    entry = (Entry *)(base - 8);
    name = base;
    off = 0;
    do {
        s32 hi = 0x800A0000;
        __asm__ volatile("" : "=r"(hi) : "0"(hi));
        if (!*(signed char *)(hi + off + 0x3E88)) break;
        if (gMatchCdFilename(name, buf)) {
            if (gCdLogLevel >= 2) {
                __asm__ volatile("");
                func_8008E000(gCdFoundLogFmt, buf);
            }
            *dest = *entry;
            return entry;
        }
        entry++;
        name += 24;
        count++;
        off += 24;
    } while (count < 64);
    if (gCdLogLevel > 0) {
        func_8008E000(gCdNotFoundErrFmt, buf);
        __asm__ volatile(".set reorder");
    }
    return 0;
}
