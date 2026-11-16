#ifndef GAME_CDROM_GLOBALS_H
#define GAME_CDROM_GLOBALS_H

/*
 * CD-ROM controller register pointers.
 *
 * 0x1F801800 INDEX/STATUS  write: bank select, read: status
 * 0x1F801801 COMMAND/RESPONSE  0x1F801802 PARAMETER/FIFO
 * 0x1F801803 REQUEST/ACK
 *
 * The bank select register decides which pair the other addresses expose, so
 * the driver saves and restores it around command sequences. Each pointer
 * keeps its retail assembler symbol through an asm label, so the machine code
 * is unchanged. Declare them where they are used:
 *
 *   extern volatile u8 *gCdromIndexStatusPtr __asm__(CDROM_INDEX_STATUS_ASM);
 *   extern volatile u8 *gCdromCommandPtr __asm__(CDROM_COMMAND_ASM);
 *   extern volatile u8 *gCdromParameterPtr __asm__(CDROM_PARAMETER_ASM);
 *   extern volatile u8 *gCdromRequestStatusPtr __asm__(CDROM_REQUEST_ASM);
 */

#define CDROM_INDEX_STATUS_ASM "D_8009CB6C"
#define CDROM_ACCESS_DELAY_ASM "D_8009CBA0"
#define COM_ACCESS_DELAY_ASM "D_8009CB7C"
#define CDROM_COMMAND_ASM "D_8009CB70"
#define CDROM_PARAMETER_ASM "D_8009CB74"
#define CDROM_REQUEST_ASM "D_8009CB78"

#endif /* GAME_CDROM_GLOBALS_H */
