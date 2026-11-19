// Categories: Platform > CD-ROM | Resources > File Access
#include "game/types.h"

extern u8 gDirRecordBuffer[] __asm__("D_800A5A80");
extern u8 gCdPathDirIdTable[] __asm__("D_800A4480");
extern u8 gCdPathParentTable[] __asm__("D_800A4484");
extern u8 gCdPathExtentTable[] __asm__("D_800A4488");
extern s32 gCdLogLevel __asm__("D_8009C8B0");
extern s32 gCdCachedDirIndex __asm__("D_8009CBC0");

extern u8 gCdPvdReadErrMsg[] __asm__("D_80012638");
extern u8 gCdIsoMagicStr[] __asm__("D_80012664");
extern u8 gCdBadSigErrMsg[] __asm__("D_8001266C");
extern u8 gCdPathReadErrFmt[] __asm__("D_8001269C");
extern u8 gCdPathTblLogMsg[] __asm__("D_800126C0");
extern u8 gCdPathEntryFmt[] __asm__("D_800126E0");
extern u8 gCdPathCountFmt[] __asm__("D_800126F4");

extern s32 gReadCdSectorsSync(s32, s32, u8 *) __asm__("func_8009081C");
extern void func_8008E000();
extern s32 func_80090890(u8 *, u8 *, s32);
extern void func_8008CD10(u8 *, u8 *, s32);
extern void *memcpy(void *, const void *, u32);

#define LOADX(S, o, lo) ({ s32 v_; __asm__ volatile("lui %0,%%hi(" #S ")\n\taddu %0,%0,%1\n\tlw %0,%%lo(" #S ")(%0)" : "=&r"(v_) : "r"(o)); v_; })

#define PRINT_STR(S) __asm__ volatile(".set\tnoreorder\n\tlui $4,%%hi(" #S ")\n\tjal func_8008E000\n\taddiu $4,$4,%%lo(" #S ")\n\t.set\treorder" \
    : : : "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "$31", "memory")
s32 gCdReadPathTable(void) __asm__("func_80090218");


/**
 * @brief Reads the ISO-9660 PVD and path table into D_800A4480..D_800A4488 (CD_newmedia).
 *
 * Reads sector 0x10 into D_800A5A80, checks signature "CD001" (D_80012664), reads the path-table sector at buf+0x8C, walks up to 128 path-table records storing directory id, parent id, extent sector, and NUL-terminated name in parallel 44-byte stride tables, and resets cached directory D_8009CBC0 = 0.
 *
 * @param void No parameters.
 * @return 1 on success, 0 on PVD read error, bad volume signature, or path-table read error.
 *
 * @par Globals
 * D_800A5A80 - sector buffer; D_800A4480/D_800A4484/D_800A4488 - 44-byte-stride path table arrays; D_8009CBC0 - cached dir reset; D_8009C8B0 - debug verbosity.
 *
 * @par Control flow
 * Three early error exits (sector 0x10 read, "CD001" compare, path-table read) followed by a bounded loop over <= 128 path records.
 *
 * @par Calls
 * func_8009081C - reads 1 CD sector; func_80090890 - BIOS strncmp checks "CD001"; func_8008CD10 - copies directory name; func_8008E000 - logs progress/errors.
 *
 * @par Side effects
 * Populates D_800A5A80 and D_800A4480..D_800A4488, and zeroes D_8009CBC0.
 *
 * @par Callers
 * Confirmed: func_8008FF20 (0x8008FF60, CD path lookup) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Reads ISO-9660 primary volume descriptor sector 0x10 into D_800A5A80 via
 * func_8009081C, verifies the CD001 signature in D_80012664 using
 * func_80090890, and reads the path-table sector referenced at offset 0x8C.
 * It then parses up to 128 path-table records into 44-byte-stride tables
 * D_800A4480, D_800A4484, and D_800A4488 using func_8008CD10, resets cached
 * directory index D_8009CBC0 to 0, and returns 1 on success. Driven by
 * func_8008FF20.
 */
s32 gCdReadPathTable(void)
{
    register u8 *buf __asm__("$16");
    register u8 *ptr __asm__("$17");
    u8 *end;
    u8 *end2;
    u8 *name;
    register u8 *extent_base __asm__("$20");
    u8 *name_base;
    s32 i;
    s32 off;
    s32 res;
    u32 len;
    u32 pad;
    s32 kind;
    u8 sector[4];

    __asm__("lui %0,%%hi(D_800A5A80)\n\taddiu %0,%0,%%lo(D_800A5A80)" : "=r"(buf));
    res = gReadCdSectorsSync(1, 0x10, buf);
    if (res != 1) {
        if (gCdLogLevel > 0) {
            PRINT_STR(D_80012638);
        }
        return 0;
    }

    if (func_80090890(buf + 1, gCdIsoMagicStr, 5) != 0) {
        if (gCdLogLevel > 0) {
            PRINT_STR(D_8001266C);
        }
        return 0;
    }

    memcpy(sector, buf + 0x8C, 4);

    if (gReadCdSectorsSync(1, *(s32 *)sector, buf) != res) {
        if (gCdLogLevel > 0) {
            register s32 a1 __asm__("$5") = *(s32 *)sector;
            __asm__ volatile(".set\tnoreorder\n\tlui $4,%%hi(D_8001269C)\n\tjal func_8008E000\n\taddiu $4,$4,%%lo(D_8001269C)\n\t.set\treorder"
                : "=r"(a1) : "0"(a1) : "$2", "$3", "$4", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "$31", "memory");
        }
        return 0;
    }

    ptr = buf;
    if (gCdLogLevel >= 2) {
        PRINT_STR(D_800126C0);
    }

    end = ptr + 0x800;
    i = 0;

    if (ptr >= end) goto finish_loop;
    extent_base = gCdPathExtentTable;
    name_base = extent_base + 4;
    end2 = end;
loop:
            if (ptr[0] == 0) goto finish_loop;

            off = i * 44;
            memcpy((u8 *)(off + (s32)extent_base), ptr + 2, 4);
            name = (u8 *)(off + (s32)name_base);
            kind = ptr[6];
            *(s32 *)(gCdPathDirIdTable + off) = i + 1;
            *(s32 *)(gCdPathParentTable + off) = kind;

            func_8008CD10(name, ptr + 8, ptr[0]);
            name[ptr[0]] = 0;

            len = ptr[0];
            pad = (len & 1) + 8;
            ptr += len + pad;

            if (gCdLogLevel >= 2) {
                s32 a_, b_, c_;
                a_ = LOADX(D_800A4488, off, 0x4488);
                b_ = LOADX(D_800A4480, off, 0x4480);
                c_ = LOADX(D_800A4484, off, 0x4484);
                func_8008E000(gCdPathEntryFmt, a_, b_, c_, name);
            }

            i++;
    if (i >= 0x80) goto after_loop;
    if (ptr < end2) goto loop;
finish_loop:
    if (i < 0x80) {
        *(s32 *)(gCdPathParentTable + i * 44) = 0;
    }

after_loop:
    gCdCachedDirIndex = 0;
    if (gCdLogLevel >= 2) {
        func_8008E000(gCdPathCountFmt, i);
        __asm__ volatile(".set\tnoreorder\n\t.set reorder");
    }

    return 1;
}
