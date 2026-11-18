	.file	1 "/home/kai-luni/git/future-cop-lapd-ps1/build/us/fcq/work/func_80066F78-20261107-123906-55e54e38/candidate.c"

 # -G value = 4, Cpu = 3000, ISA = 1
 # GNU C version cygnus-2.7.2-970404 SN32.3.7.0004 (SonyPSX) compiled by CC.
 # options passed:  -G4 -O2 -mgpopt
 # options enabled:  -fdefer-pop -fomit-frame-pointer -fcse-follow-jumps
 # -fcse-skip-blocks -fexpensive-optimizations -fthread-jumps
 # -fstrength-reduce -fpeephole -fforce-mem -ffunction-cse -finline
 # -fkeep-static-consts -fcaller-saves -fpcc-struct-return -fdelayed-branch
 # -frerun-cse-after-loop -fschedule-insns -fschedule-insns2 -fcommon
 # -fverbose-asm -fgnu-linker -mgas -mgpOPT -mgpopt -msoft-float -mcpu=3000

gcc2_compiled.:
__gnu_compiled_c:
	.text
	.align	2
	.globl	func_80066F78

	.extern	D_8009F808, 4
	.extern	D_8009F80C, 4

	.text
	.text
	.ent	func_80066F78
func_80066F78:
	.frame	$sp,96,$31		# vars= 32, regs= 10/0, args= 24, extra= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,96
	move	$10,$4
	move	$11,$5
	sw	$31,92($sp)
	sw	$fp,88($sp)
	sw	$23,84($sp)
	sw	$22,80($sp)
	sw	$21,76($sp)
	sw	$20,72($sp)
	sw	$19,68($sp)
	sw	$18,64($sp)
	sw	$17,60($sp)
	sw	$16,56($sp)
	sw	$6,104($sp)
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L2
	sw	$7,108($sp)
	.set	macro
	.set	reorder

	sw	$0,0($6)
$L2:
	lw	$9,4($10)
	lw	$7,8($10)
	lw	$8,0($10)
	.set	noreorder
	.set	nomacro
	bltz	$7,$L41
	sra	$3,$7,17
	.set	macro
	.set	reorder

	lw	$6,D_8009F80C
	#nop
	lw	$5,4($6)
	#nop
	addu	$2,$5,-1
	slt	$2,$3,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L40
	move	$2,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bltz	$8,$L40
	sra	$4,$8,17
	.set	macro
	.set	reorder

	lw	$2,8($6)
	#nop
	addu	$2,$2,-1
	slt	$2,$4,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L41
	mult	$5,$4
	.set	macro
	.set	reorder

	sra	$4,$8,13
	andi	$4,$4,0x000f
	sll	$5,$4,4
	sra	$2,$7,13
	andi	$2,$2,0x000f
	addu	$5,$5,$2
	addu	$23,$9,512
	mflo	$14
	#nop
	#nop
	addu	$3,$3,$14
	sll	$3,$3,2
	addu	$3,$3,$6
	lw	$2,36($3)
	lw	$3,D_8009F808
	andi	$2,$2,0xfffc
	addu	$2,$2,$3
	lw	$21,0($2)
	sll	$2,$5,1
	addu	$2,$2,$21
	lhu	$3,972($2)
	addu	$4,$4,$5
	srl	$2,$3,4
	addu	$2,$21,$2
	addu	$19,$2,1488
	andi	$17,$3,0x000f
	sll	$2,$4,1
	addu	$2,$2,$4
	sll	$2,$2,1
	addu	$2,$21,$2
	.set	noreorder
	.set	nomacro
	beq	$17,$0,$L41
	addu	$22,$2,12
	.set	macro
	.set	reorder

	li	$18,1073741824			# 0x40000000
	li	$20,-1073741824			# 0xc0000000
	sll	$2,$7,3
	andi	$2,$2,0xffff
	sw	$2,36($sp)
	sll	$2,$8,3
	.set	noreorder
	.set	nomacro
	blez	$17,$L9
	andi	$fp,$2,0xffff
	.set	macro
	.set	reorder

$L10:
	lw	$16,0($19)
	#nop
	srl	$2,$16,15
	andi	$5,$2,0x007f
	slt	$2,$5,71
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L9
	addu	$19,$19,4
	.set	macro
	.set	reorder

	srl	$2,$16,13
	andi	$2,$2,0x0003
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L13
	sb	$2,32($sp)
	.set	macro
	.set	reorder

	addu	$2,$2,$21
	lbu	$2,880($2)
	#nop
	andi	$2,$2,0x0040
	bne	$2,$0,$L12
$L13:
	addu	$2,$sp,24
	move	$4,$22
	lw	$6,36($sp)
	move	$7,$fp
	sw	$2,16($sp)
	sw	$10,48($sp)
	.set	noreorder
	.set	nomacro
	jal	func_80067480
	sw	$11,52($sp)
	.set	macro
	.set	reorder

	lw	$10,48($sp)
	lw	$11,52($sp)
	beq	$2,$0,$L12
	lw	$3,24($sp)
	#nop
	slt	$2,$3,$18
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L45
	slt	$2,$3,$23
	.set	macro
	.set	reorder

	move	$18,$3
	sw	$16,44($sp)
$L45:
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L12
	slt	$2,$20,$3
	.set	macro
	.set	reorder

	beq	$2,$0,$L12
	move	$20,$3
	sw	$16,40($sp)
$L12:
	addu	$17,$17,-1
	bgtz	$17,$L10
$L9:
	li	$2,1073741824			# 0x40000000
	.set	noreorder
	.set	nomacro
	bne	$18,$2,$L18
	li	$2,-1073741824			# 0xc0000000
	.set	macro
	.set	reorder

$L41:
	.set	noreorder
	.set	nomacro
	j	$L40
	move	$2,$0
	.set	macro
	.set	reorder

$L18:
	bne	$20,$2,$L19
	move	$20,$18
	lw	$14,44($sp)
	#nop
	sw	$14,40($sp)
$L19:
	lw	$14,108($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$14,$0,$L20
	move	$4,$10
	.set	macro
	.set	reorder

	move	$5,$11
	addu	$6,$sp,28
	addu	$7,$sp,32
	.set	noreorder
	.set	nomacro
	jal	func_8006320C
	sw	$11,52($sp)
	.set	macro
	.set	reorder

	move	$3,$2
	lw	$11,52($sp)
	beq	$3,$0,$L21
	lw	$2,28($sp)
	#nop
	slt	$2,$2,$20
	bne	$2,$0,$L21
	beq	$11,$0,$L22
	lw	$2,56($3)
	#nop
	sw	$2,0($11)
	lw	$2,60($3)
	#nop
	sw	$2,4($11)
	lw	$2,64($3)
	#nop
	sw	$2,8($11)
$L22:
	lw	$14,108($sp)
	#nop
	sw	$3,0($14)
	lw	$14,104($sp)
	#nop
	beq	$14,$0,$L23
	lbu	$2,32($sp)
	#nop
	sw	$2,0($14)
$L23:
	lw	$2,28($sp)
	j	$L40
$L21:
	lw	$14,108($sp)
	#nop
	sw	$0,0($14)
$L20:
	lw	$14,104($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$14,$0,$L24
	subu	$2,$23,$20
	.set	macro
	.set	reorder

	slt	$2,$2,8192
	beq	$2,$0,$L24
	lw	$14,40($sp)
	#nop
	srl	$2,$14,13
	andi	$2,$2,0x0003
	addu	$2,$2,$21
	lbu	$2,880($2)
	lw	$14,104($sp)
	#nop
	sw	$2,0($14)
$L24:
	.set	noreorder
	.set	nomacro
	beq	$11,$0,$L26
	move	$12,$0
	.set	macro
	.set	reorder

	lw	$14,40($sp)
	li	$13,5			# 0x00000005
	srl	$2,$14,15
	andi	$5,$2,0x007f
	sll	$3,$5,2
	lui	$2,%hi(D_8009AFE8) # high
	addiu	$2,$2,%lo(D_8009AFE8) # low
	addu	$7,$3,$2
	lbu	$4,0($7)
	lbu	$2,1($7)
	lbu	$3,2($7)
	addu	$4,$22,$4
	addu	$2,$22,$2
	addu	$3,$22,$3
	lb	$8,0($4)
	lb	$2,0($2)
	lb	$3,0($3)
	subu	$9,$2,$8
	slt	$2,$5,60
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L27
	subu	$10,$3,$8
	.set	macro
	.set	reorder

	lw	$14,36($sp)
	#nop
	slt	$2,$fp,$14
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L35
	li	$4,32			# 0x00000020
	.set	macro
	.set	reorder

	move	$9,$10
	li	$6,5			# 0x00000005
	lbu	$2,3($7)
	move	$4,$6
	addu	$2,$22,$2
	lb	$2,0($2)
	li	$5,32			# 0x00000020
	.set	noreorder
	.set	nomacro
	j	$L30
	subu	$10,$2,$8
	.set	macro
	.set	reorder

$L27:
	lui	$2,%hi(D_8009B1A4) # high
	addiu	$2,$2,%lo(D_8009B1A4) # low
	addu	$2,$5,$2
	lbu	$2,0($2)
	#nop
	srl	$3,$2,3
	li	$2,1			# 0x00000001
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L33
	slt	$2,$3,2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L37
	li	$2,2			# 0x00000002
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L32
	li	$4,32			# 0x00000020
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L42
	li	$5,5			# 0x00000005
	.set	macro
	.set	reorder

$L37:
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L34
	li	$4,32			# 0x00000020
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L42
	li	$5,5			# 0x00000005
	.set	macro
	.set	reorder

$L32:
	li	$5,32			# 0x00000020
	.set	noreorder
	.set	nomacro
	j	$L42
	move	$4,$5
	.set	macro
	.set	reorder

$L33:
	li	$4,5			# 0x00000005
	li	$6,32			# 0x00000020
	move	$5,$4
	.set	noreorder
	.set	nomacro
	j	$L30
	li	$12,1			# 0x00000001
	.set	macro
	.set	reorder

$L34:
	li	$5,32			# 0x00000020
	li	$6,5			# 0x00000005
	.set	noreorder
	.set	nomacro
	j	$L30
	move	$4,$6
	.set	macro
	.set	reorder

$L35:
	li	$5,5			# 0x00000005
$L42:
	li	$6,5			# 0x00000005
$L30:
	sll	$2,$10,$4
	sll	$3,$9,$13
	subu	$2,$2,$3
	sw	$2,8($11)
	addu	$2,$13,$6
	li	$3,1			# 0x00000001
	.set	noreorder
	.set	nomacro
	beq	$12,$0,$L38
	sll	$7,$3,$2
	.set	macro
	.set	reorder

	subu	$9,$0,$9
	addu	$2,$5,$4
	sll	$2,$3,$2
	.set	noreorder
	.set	nomacro
	j	$L43
	addu	$2,$7,$2
	.set	macro
	.set	reorder

$L38:
	addu	$2,$5,$4
	sll	$2,$3,$2
	subu	$2,$7,$2
$L43:
	sw	$2,4($11)
	sll	$2,$9,$5
	sll	$3,$10,$6
	subu	$2,$2,$3
	sw	$2,0($11)
	.set	noreorder
	.set	nomacro
	jal	func_8001A180
	move	$4,$11
	.set	macro
	.set	reorder

$L26:
	move	$2,$20
$L40:
	lw	$31,92($sp)
	lw	$fp,88($sp)
	lw	$23,84($sp)
	lw	$22,80($sp)
	lw	$21,76($sp)
	lw	$20,72($sp)
	lw	$19,68($sp)
	lw	$18,64($sp)
	lw	$17,60($sp)
	lw	$16,56($sp)
	addu	$sp,$sp,96
	j	$31
	.end	func_80066F78
