#ifndef GAME_CDROM_GLOBALS_H
#define GAME_CDROM_GLOBALS_H

/*
 * CD-ROM controller register pointers.
 *
 * 0x1F801800 INDEX/STATUS  write: bank select, read: status
 * 0x1F801801 DATA/FIFO     0x1F801802 INTERRUPT ENABLE/FLAG
 * 0x1F801803 REQUEST/ACK
 *
 * The bank select register decides which pair the other addresses expose, so
 * the driver saves and restores it around command sequences. Each pointer
 * keeps its retail assembler symbol through an asm label, so the machine code
 * is unchanged. Declare them where they are used:
 *
 *   extern volatile u8 *gCdromIndexStatusPtr __asm__(CDROM_INDEX_STATUS_ASM);
 */

#define CDROM_INDEX_STATUS_ASM "D_8009CB6C"

#endif /* GAME_CDROM_GLOBALS_H */
