	.file	1 "tmp_inline.c"
	.set	nobopt

 # -G value = 0, Cpu = 3000, ISA = 1
 # GNU C version cygnus-2.7.2-970404 SN32.3.7.0004 (SonyPSX) compiled by CC.
 # options passed:  -msoft-float -mgas -G0 -fgnu-linker -O2 -mmips-as -O2
 # options enabled:  -fdefer-pop -fomit-frame-pointer -fcse-follow-jumps
 # -fcse-skip-blocks -fexpensive-optimizations -fthread-jumps
 # -fstrength-reduce -fpeephole -fforce-mem -ffunction-cse -finline
 # -fkeep-static-consts -fcaller-saves -fpcc-struct-return -fdelayed-branch
 # -frerun-cse-after-loop -fschedule-insns -fschedule-insns2 -fcommon
 # -fverbose-asm -fgnu-linker -msoft-float -mcpu=3000

gcc2_compiled.:
__gnu_compiled_c:
	.text
	.align	2
	.globl	func_80082EF0
	.ent	func_80082EF0
func_80082EF0:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	move	$3,$4
	lw	$8,D_8009F9F0
	lw	$2,D_8009FA28
	#nop
	sltu	$2,$8,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L5
	addu	$6,$3,$6
	.set	macro
	.set	reorder

	addu	$2,$8,52
	sw	$2,D_8009F9F0
$L5:
	li	$2,5			# 0x00000005
	sb	$2,3($8)
	li	$2,40			# 0x00000028
	sb	$2,7($8)
	lw	$2,20($sp)
	#nop
	sb	$2,4($8)
	lw	$2,24($sp)
	#nop
	sb	$2,5($8)
	lw	$2,28($sp)
	addu	$7,$7,$5
	sh	$3,8($8)
	sh	$3,16($8)
	lw	$3,0($8)
	li	$4,16711680			# 0x00ff0000
	sh	$5,10($8)
	sh	$6,12($8)
	sh	$5,14($8)
	sh	$7,18($8)
	sh	$6,20($8)
	sh	$7,22($8)
	sb	$2,6($8)
	li	$2,-16777216			# 0xff000000
	and	$3,$3,$2
	lw	$2,16($sp)
	ori	$4,$4,0xffff
	and	$2,$2,$4
	or	$3,$3,$2
	.set	noreorder
	.set	nomacro
	j	$31
	sw	$3,0($8)
	.set	macro
	.set	reorder

	.end	func_80082EF0
