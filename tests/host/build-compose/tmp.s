	.file	1 "/home/kai-luni/git/future-cop-lapd-ps1/tests/host/compose_harness.c"

 # -G value = 0, Cpu = 3000, ISA = 1
 # GNU C version cygnus-2.7.2-970404 SN32.3.7.0004 (SonyPSX) compiled by CC.
 # options passed:  -G0 -O1
 # options enabled:  -fdefer-pop -fomit-frame-pointer -fthread-jumps
 # -fpeephole -ffunction-cse -finline -fkeep-static-consts
 # -fpcc-struct-return -fdelayed-branch -fcommon -fverbose-asm -fgnu-linker
 # -mgas -msoft-float -mcpu=3000

gcc2_compiled.:
__gnu_compiled_c:
	.text
	.align	2
	.globl	__main
	.ent	__main
__main:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	j	$31
	.end	__main
	.align	2
	.ent	clamp
clamp:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	bgtz	$5,$L8
	li	$2,32767			# 0x00007fff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$5,$0,$L9
	slt	$2,$5,-1
	.set	macro
	.set	reorder

	li	$2,32767			# 0x00007fff
	sltu	$2,$2,$4
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L7
	li	$2,32767			# 0x00007fff
	.set	macro
	.set	reorder

	slt	$2,$5,-1
$L9:
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L8
	li	$2,-32768			# 0xffff8000
	.set	macro
	.set	reorder

	li	$2,-1			# 0xffffffff
	.set	noreorder
	.set	nomacro
	bne	$5,$2,$L8
	move	$2,$4
	.set	macro
	.set	reorder

	sltu	$2,$4,-32768
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L7
	li	$2,-32768			# 0xffff8000
	.set	macro
	.set	reorder

	move	$2,$4
$L7:
$L8:
	j	$31
	.end	clamp
	.align	2
	.globl	func_80010820
	.ent	func_80010820
func_80010820:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	move	$7,$0
	lui	$2,%hi(r) # high
	addiu	$10,$2,%lo(r) # low
	li	$9,1			# 0x00000001
$L14:
	move	$6,$0
	sll	$2,$7,1
	addu	$8,$2,$7
	sll	$2,$6,1
$L24:
	addu	$2,$2,$6
	addu	$2,$2,$7
	sll	$2,$2,2
	addu	$2,$2,$4
	lw	$3,0($2)
	addu	$2,$8,$6
	sll	$2,$2,1
	.set	noreorder
	.set	nomacro
	bne	$7,$9,$L19
	addu	$5,$2,$10
	.set	macro
	.set	reorder

	subu	$2,$0,$3
	.set	noreorder
	.set	nomacro
	j	$L23
	sra	$2,$2,4
	.set	macro
	.set	reorder

$L19:
	sra	$2,$3,4
$L23:
	sh	$2,0($5)
	addu	$6,$6,1
	slt	$2,$6,3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L24
	sll	$2,$6,1
	.set	macro
	.set	reorder

	addu	$7,$7,1
	slt	$2,$7,3
	bne	$2,$0,$L14
	j	$31
	.end	func_80010820
	.align	2
	.globl	func_8001006C
	.ent	func_8001006C
func_8001006C:
	.frame	$sp,56,$31		# vars= 0, regs= 10/0, args= 16, extra= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,56
	sw	$31,52($sp)
	sw	$fp,48($sp)
	sw	$23,44($sp)
	sw	$22,40($sp)
	sw	$21,36($sp)
	sw	$20,32($sp)
	sw	$19,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	sw	$4,56($sp)
	move	$fp,$5
	move	$23,$6
	.set	noreorder
	.set	nomacro
	blez	$23,$L27
	move	$17,$0
	.set	macro
	.set	reorder

	lui	$2,%hi(r) # high
	addiu	$21,$2,%lo(r) # low
	lui	$2,%hi(r+4) # high
	addiu	$22,$2,%lo(r+4) # low
	sll	$3,$17,1
$L40:
	addu	$3,$3,$17
	sll	$3,$3,2
	lw	$11,56($sp)
	#nop
	addu	$3,$3,$11
	lw	$2,0($3)
	#nop
	sll	$2,$2,12
	sra	$20,$2,16
	lw	$2,4($3)
	#nop
	sll	$2,$2,12
	sra	$19,$2,16
	lw	$2,8($3)
	#nop
	sll	$2,$2,12
	sra	$18,$2,16
	move	$16,$0
	sll	$10,$16,1
$L39:
	addu	$10,$10,$16
	sll	$10,$10,1
	addu	$2,$10,$21
	lhu	$2,0($2)
	#nop
	sll	$2,$2,16
	sra	$6,$2,16
	sra	$7,$2,31
	move	$2,$20
	sra	$3,$20,31
	multu	$6,$2
	mfhi	$5
	mflo	$4
	#nop
	#nop
	mult	$6,$3
	mflo	$8
	#nop
	#nop
	addu	$5,$5,$8
	mult	$2,$7
	mflo	$3
	#nop
	#nop
	addu	$5,$5,$3
	addu	$2,$21,$10
	lhu	$2,2($2)
	#nop
	sll	$2,$2,16
	sra	$8,$2,16
	sra	$9,$2,31
	move	$6,$19
	sra	$7,$19,31
	multu	$8,$6
	mfhi	$3
	mflo	$2
	#nop
	#nop
	mult	$8,$7
	mflo	$12
	#nop
	#nop
	addu	$3,$3,$12
	mult	$6,$9
	mflo	$7
	#nop
	#nop
	addu	$3,$3,$7
	addu	$4,$4,$2
	sltu	$6,$4,$2
	addu	$5,$5,$3
	addu	$5,$5,$6
	addu	$10,$10,$22
	lhu	$2,0($10)
	#nop
	sll	$2,$2,16
	sra	$8,$2,16
	sra	$9,$2,31
	move	$6,$18
	sra	$7,$18,31
	multu	$8,$6
	mfhi	$3
	mflo	$2
	#nop
	#nop
	mult	$8,$7
	mflo	$10
	#nop
	#nop
	addu	$3,$3,$10
	mult	$6,$9
	mflo	$7
	#nop
	#nop
	addu	$3,$3,$7
	addu	$4,$4,$2
	sltu	$6,$4,$2
	addu	$5,$5,$3
	addu	$5,$5,$6
	srl	$4,$4,12
	sll	$2,$5,20
	or	$4,$4,$2
	.set	noreorder
	.set	nomacro
	jal	clamp
	sra	$5,$5,12
	.set	macro
	.set	reorder

	move	$4,$2
	sll	$2,$17,1
	addu	$2,$2,$17
	addu	$2,$2,$16
	sll	$2,$2,2
	addu	$3,$2,$fp
	li	$2,1			# 0x00000001
	.set	noreorder
	.set	nomacro
	bne	$16,$2,$L34
	subu	$2,$0,$4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L38
	sll	$2,$2,4
	.set	macro
	.set	reorder

$L34:
	sll	$2,$4,4
$L38:
	sw	$2,0($3)
	addu	$16,$16,1
	slt	$2,$16,3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L39
	sll	$10,$16,1
	.set	macro
	.set	reorder

	addu	$17,$17,1
	slt	$2,$17,$23
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L40
	sll	$3,$17,1
	.set	macro
	.set	reorder

$L27:
	lw	$31,52($sp)
	lw	$fp,48($sp)
	lw	$23,44($sp)
	lw	$22,40($sp)
	lw	$21,36($sp)
	lw	$20,32($sp)
	lw	$19,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	addu	$sp,$sp,56
	j	$31
	.end	func_8001006C
	.align	2
	.ent	sys3
sys3:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	move	$2,$4
	move	$4,$5
	move	$5,$6
	move	$6,$7
 #APP
	syscall
 #NO_APP
	j	$31
	.end	sys3
	.align	2
	.ent	hex
hex:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	move	$3,$4
	addu	$4,$4,-48
	andi	$4,$4,0x00ff
	sltu	$4,$4,10
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L43
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L46
	addu	$2,$2,-48
	.set	macro
	.set	reorder

$L43:
	addu	$2,$3,-97
	andi	$2,$2,0x00ff
	sltu	$2,$2,6
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L44
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L46
	addu	$2,$2,-87
	.set	macro
	.set	reorder

$L44:
	addu	$2,$3,-65
	andi	$2,$2,0x00ff
	sltu	$2,$2,6
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L45
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L46
	li	$2,-1			# 0xffffffff
	.set	macro
	.set	reorder

$L45:
	addu	$2,$2,-55
$L46:
	j	$31
	.end	hex
	.align	2
	.ent	say
say:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, extra= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	subu	$sp,$sp,24
	sw	$31,16($sp)
	move	$6,$4
	lbu	$2,0($6)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L49
	move	$7,$0
	.set	macro
	.set	reorder

	addu	$7,$7,1
$L52:
	addu	$2,$6,$7
	lbu	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L52
	addu	$7,$7,1
	.set	macro
	.set	reorder

	addu	$7,$7,-1
$L49:
	li	$4,4004			# 0x00000fa4
	.set	noreorder
	.set	nomacro
	jal	sys3
	li	$5,1			# 0x00000001
	.set	macro
	.set	reorder

	lw	$31,16($sp)
	addu	$sp,$sp,24
	j	$31
	.end	say

	.lcomm	data.16,32768
	.rdata
	.align	2
$LC0:
	.ascii	"compose: missing fixture\n\000"
	.align	2
$LC1:
	.ascii	"compose: cannot open fixture\n\000"
	.align	2
$LC2:
	.ascii	"compose: read error\n\000"
	.align	2
$LC3:
	.ascii	"compose: malformed TABLE\n\000"
	.align	2
$LC4:
	.ascii	"compose: empty TABLE\n\000"
	.align	2
$LC5:
	.ascii	"compose: malformed fixture\n\000"
	.align	2
$LC6:
	.ascii	"compose: extra fixture data\n\000"
	.align	2
$LC7:
	.ascii	"compose: FAIL\n\000"
	.align	2
$LC8:
	.ascii	"compose: all shared vectors PASS (GTE software model)\n\000"
	.text
	.align	2
	.globl	main
	.ent	main
main:
	.frame	$sp,344,$31		# vars= 288, regs= 10/0, args= 16, extra= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,344
	sw	$31,340($sp)
	sw	$fp,336($sp)
	sw	$23,332($sp)
	sw	$22,328($sp)
	sw	$21,324($sp)
	sw	$20,320($sp)
	sw	$19,316($sp)
	sw	$18,312($sp)
	sw	$17,308($sp)
	sw	$16,304($sp)
	move	$16,$4
	.set	noreorder
	.set	nomacro
	jal	__main
	move	$17,$5
	.set	macro
	.set	reorder

	li	$2,2			# 0x00000002
	.set	noreorder
	.set	nomacro
	beq	$16,$2,$L54
	lui	$4,%hi($LC0) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L136
	addiu	$4,$4,%lo($LC0) # low
	.set	macro
	.set	reorder

$L54:
	li	$4,4005			# 0x00000fa5
	lw	$5,4($17)
	move	$6,$0
	.set	noreorder
	.set	nomacro
	jal	sys3
	move	$7,$6
	.set	macro
	.set	reorder

	move	$16,$2
	.set	noreorder
	.set	nomacro
	bgez	$16,$L55
	move	$5,$16
	.set	macro
	.set	reorder

	lui	$4,%hi($LC1) # high
	.set	noreorder
	.set	nomacro
	j	$L136
	addiu	$4,$4,%lo($LC1) # low
	.set	macro
	.set	reorder

$L55:
	li	$4,4003			# 0x00000fa3
	lui	$6,%hi(data.16) # high
	addiu	$6,$6,%lo(data.16) # low
	.set	noreorder
	.set	nomacro
	jal	sys3
	li	$7,32767			# 0x00007fff
	.set	macro
	.set	reorder

	move	$17,$2
	li	$4,4006			# 0x00000fa6
	move	$5,$16
	move	$6,$0
	.set	noreorder
	.set	nomacro
	jal	sys3
	move	$7,$6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bgez	$17,$L56
	lui	$2,%hi(data.16) # high
	.set	macro
	.set	reorder

	lui	$4,%hi($LC2) # high
	.set	noreorder
	.set	nomacro
	j	$L136
	addiu	$4,$4,%lo($LC2) # low
	.set	macro
	.set	reorder

$L56:
	addiu	$2,$2,%lo(data.16) # low
	addu	$3,$17,$2
	sb	$0,0($3)
	move	$18,$2
	sw	$0,296($sp)
	sw	$0,300($sp)
	move	$23,$0
	move	$17,$23
	addu	$4,$sp,16
	sll	$2,$17,1
$L143:
	addu	$3,$2,$4
	addu	$2,$2,$17
	sll	$2,$2,3
	addu	$2,$2,$17
	sll	$2,$2,2
	addu	$2,$2,$17
	addu	$2,$2,-600
	sh	$2,128($3)
	addu	$17,$17,1
	slt	$2,$17,16
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L143
	sll	$2,$17,1
	.set	macro
	.set	reorder

	lbu	$2,0($18)
	#nop
	beq	$2,$0,$L63
	li	$22,32			# 0x00000020
	li	$21,9			# 0x00000009
	li	$fp,13			# 0x0000000d
	.set	noreorder
	.set	nomacro
	j	$L147
	addu	$20,$sp,16
	.set	macro
	.set	reorder

$L137:
	lbu	$2,0($18)
$L147:
	.set	noreorder
	.set	nomacro
	beq	$2,$22,$L137
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$21,$L137
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$fp,$L137
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	li	$8,10			# 0x0000000a
	.set	noreorder
	.set	nomacro
	beq	$2,$8,$L137
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	lbu	$3,0($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L63
	li	$2,35			# 0x00000023
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L70
	li	$2,84			# 0x00000054
	.set	macro
	.set	reorder

	lbu	$2,0($18)
	li	$8,10			# 0x0000000a
$L142:
	beq	$2,$8,$L62
	addu	$18,$18,1
	lbu	$2,0($18)
	#nop
	beq	$2,$0,$L63
	j	$L142
$L70:
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L140
	move	$17,$0
	.set	macro
	.set	reorder

	lbu	$3,1($18)
	li	$2,65			# 0x00000041
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L140
	li	$2,66			# 0x00000042
	.set	macro
	.set	reorder

	lbu	$3,2($18)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L140
	li	$2,76			# 0x0000004c
	.set	macro
	.set	reorder

	lbu	$3,3($18)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L140
	li	$2,69			# 0x00000045
	.set	macro
	.set	reorder

	lbu	$3,4($18)
	#nop
	bne	$3,$2,$L140
	addu	$18,$18,5
$L139:
	lbu	$2,0($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L78
	li	$8,10			# 0x0000000a
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$8,$L78
	move	$17,$0
	.set	macro
	.set	reorder

	j	$L138
$L144:
	lbu	$2,0($18)
$L138:
	.set	noreorder
	.set	nomacro
	beq	$2,$22,$L144
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$21,$L144
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$fp,$L144
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	lbu	$2,0($18)
	li	$8,10			# 0x0000000a
	beq	$2,$8,$L78
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L78
	move	$16,$0
	.set	macro
	.set	reorder

$L90:
	lbu	$4,0($18)
	.set	noreorder
	.set	nomacro
	jal	hex
	addu	$18,$18,1
	.set	macro
	.set	reorder

	move	$3,$2
	.set	noreorder
	.set	nomacro
	bltz	$3,$L132
	sll	$2,$17,4
	.set	macro
	.set	reorder

	or	$17,$2,$3
	addu	$16,$16,1
	slt	$2,$16,8
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L90
	slt	$2,$23,64
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L139
	sll	$2,$23,1
	.set	macro
	.set	reorder

	addu	$2,$2,$20
	sh	$17,128($2)
	.set	noreorder
	.set	nomacro
	j	$L139
	addu	$23,$23,1
	.set	macro
	.set	reorder

$L78:
	bne	$23,$0,$L62
	lui	$4,%hi($LC4) # high
	.set	noreorder
	.set	nomacro
	j	$L136
	addiu	$4,$4,%lo($LC4) # low
	.set	macro
	.set	reorder

$L135:
	lw	$8,300($sp)
	#nop
	addu	$8,$8,1
	.set	noreorder
	.set	nomacro
	j	$L122
	sw	$8,300($sp)
	.set	macro
	.set	reorder

$L140:
	lbu	$2,0($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$22,$L140
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$21,$L140
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	sll	$2,$17,2
	addu	$2,$2,$20
	sw	$0,0($2)
	move	$16,$0
	sll	$2,$17,2
	addu	$19,$2,$20
$L107:
	lbu	$4,0($18)
	.set	noreorder
	.set	nomacro
	jal	hex
	addu	$18,$18,1
	.set	macro
	.set	reorder

	move	$3,$2
	.set	noreorder
	.set	nomacro
	bltz	$3,$L133
	addu	$16,$16,1
	.set	macro
	.set	reorder

	lw	$2,0($19)
	#nop
	sll	$2,$2,4
	or	$2,$2,$3
	sw	$2,0($19)
	slt	$2,$16,8
	bne	$2,$0,$L107
	addu	$17,$17,1
	slt	$2,$17,32
	bne	$2,$0,$L140
$L141:
	lbu	$2,0($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$22,$L141
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$21,$L141
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$fp,$L141
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	lbu	$2,0($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L115
	li	$8,10			# 0x0000000a
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$8,$L134
	lui	$4,%hi($LC6) # high
	.set	macro
	.set	reorder

$L115:
	move	$17,$0
	lui	$2,%hi(D_800A0300) # high
	addiu	$4,$2,%lo(D_800A0300) # low
	sll	$2,$17,2
$L145:
	addu	$3,$2,$4
	addu	$2,$2,$20
	lw	$2,0($2)
	#nop
	sw	$2,0($3)
	addu	$17,$17,1
	slt	$2,$17,12
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L145
	sll	$2,$17,2
	.set	macro
	.set	reorder

	sh	$0,276($sp)
	sh	$0,274($sp)
	sh	$0,272($sp)
	sb	$0,278($sp)
	lbu	$2,64($sp)
	#nop
	sb	$2,279($sp)
	lw	$7,68($sp)
	lhu	$2,72($sp)
	#nop
	sh	$2,280($sp)
	lhu	$2,76($sp)
	#nop
	sh	$2,282($sp)
	lhu	$2,80($sp)
	#nop
	sh	$2,284($sp)
	lhu	$2,84($sp)
	#nop
	sh	$2,286($sp)
	lhu	$2,88($sp)
	#nop
	sh	$2,288($sp)
	lhu	$2,92($sp)
	#nop
	sh	$2,290($sp)
	li	$4,1			# 0x00000001
	addu	$5,$sp,272
	.set	noreorder
	.set	nomacro
	jal	func_80013CB8
	addu	$6,$sp,144
	.set	macro
	.set	reorder

	move	$4,$0
	lui	$2,%hi(D_800A0300) # high
	addiu	$5,$2,%lo(D_800A0300) # low
	sll	$2,$4,2
$L146:
	addu	$3,$2,$5
	addu	$2,$2,$20
	lw	$3,48($3)
	lw	$2,80($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L135
	addu	$4,$4,1
	.set	macro
	.set	reorder

	slt	$2,$4,12
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L146
	sll	$2,$4,2
	.set	macro
	.set	reorder

$L122:
	lw	$8,296($sp)
	#nop
	addu	$8,$8,1
	sw	$8,296($sp)
$L62:
	lbu	$2,0($18)
	#nop
	bne	$2,$0,$L137
$L63:
	lw	$8,296($sp)
	#nop
	beq	$8,$0,$L129
	lw	$8,300($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$8,$0,$L128
	lui	$4,%hi($LC8) # high
	.set	macro
	.set	reorder

$L129:
	lui	$4,%hi($LC7) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC7) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L130
	li	$2,1			# 0x00000001
	.set	macro
	.set	reorder

$L132:
	lui	$4,%hi($LC3) # high
	.set	noreorder
	.set	nomacro
	j	$L136
	addiu	$4,$4,%lo($LC3) # low
	.set	macro
	.set	reorder

$L133:
	lui	$4,%hi($LC5) # high
	.set	noreorder
	.set	nomacro
	j	$L136
	addiu	$4,$4,%lo($LC5) # low
	.set	macro
	.set	reorder

$L134:
	addiu	$4,$4,%lo($LC6) # low
$L136:
	jal	say
	.set	noreorder
	.set	nomacro
	j	$L130
	li	$2,2			# 0x00000002
	.set	macro
	.set	reorder

$L128:
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC8) # low
	.set	macro
	.set	reorder

	move	$2,$0
$L130:
	lw	$31,340($sp)
	lw	$fp,336($sp)
	lw	$23,332($sp)
	lw	$22,328($sp)
	lw	$21,324($sp)
	lw	$20,320($sp)
	lw	$19,316($sp)
	lw	$18,312($sp)
	lw	$17,308($sp)
	lw	$16,304($sp)
	addu	$sp,$sp,344
	j	$31
	.end	main

	.comm	D_800A0300,384

	.lcomm	r,18
