	.file	1 "/home/kai-luni/git/future-cop-lapd-ps1/tests/host/joints_probe.c"

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
$L6:
	move	$6,$0
	sll	$2,$7,1
	addu	$8,$2,$7
	sll	$2,$6,1
$L16:
	addu	$2,$2,$6
	addu	$2,$2,$7
	sll	$2,$2,2
	addu	$2,$2,$4
	lw	$3,0($2)
	addu	$2,$8,$6
	sll	$2,$2,1
	.set	noreorder
	.set	nomacro
	bne	$7,$9,$L11
	addu	$5,$2,$10
	.set	macro
	.set	reorder

	subu	$2,$0,$3
	.set	noreorder
	.set	nomacro
	j	$L15
	sra	$2,$2,4
	.set	macro
	.set	reorder

$L11:
	sra	$2,$3,4
$L15:
	sh	$2,0($5)
	addu	$6,$6,1
	slt	$2,$6,3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L16
	sll	$2,$6,1
	.set	macro
	.set	reorder

	addu	$7,$7,1
	slt	$2,$7,3
	bne	$2,$0,$L6
	j	$31
	.end	func_80010820
	.align	2
	.globl	func_8001006C
	.ent	func_8001006C
func_8001006C:
	.frame	$sp,40,$31		# vars= 8, regs= 8/0, args= 0, extra= 0
	.mask	0x00ff0000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,40
	sw	$23,36($sp)
	sw	$22,32($sp)
	sw	$21,28($sp)
	sw	$20,24($sp)
	sw	$19,20($sp)
	sw	$18,16($sp)
	sw	$17,12($sp)
	sw	$16,8($sp)
	move	$22,$4
	move	$21,$5
	move	$20,$6
	.set	noreorder
	.set	nomacro
	blez	$20,$L19
	move	$12,$0
	.set	macro
	.set	reorder

	lui	$2,%hi(r) # high
	addiu	$18,$2,%lo(r) # low
	lui	$2,%hi(r+4) # high
	addiu	$19,$2,%lo(r+4) # low
	sll	$3,$12,1
$L36:
	addu	$3,$3,$12
	sll	$3,$3,2
	addu	$3,$3,$22
	lw	$2,0($3)
	#nop
	sll	$2,$2,12
	sra	$24,$2,16
	lw	$2,4($3)
	#nop
	sll	$2,$2,12
	sra	$15,$2,16
	lw	$2,8($3)
	#nop
	sll	$2,$2,12
	sra	$14,$2,16
	move	$11,$0
	sra	$17,$24,31
	sra	$16,$15,31
	sra	$25,$14,31
	sll	$13,$12,1
	addu	$13,$13,$12
	sll	$10,$11,1
$L35:
	addu	$10,$10,$11
	sll	$10,$10,1
	addu	$2,$10,$18
	lhu	$2,0($2)
	#nop
	sll	$2,$2,16
	sra	$4,$2,16
	sra	$5,$2,31
	move	$2,$24
	move	$3,$17
	multu	$4,$2
	mfhi	$9
	mflo	$8
	#nop
	#nop
	mult	$4,$3
	mflo	$7
	#nop
	#nop
	addu	$9,$9,$7
	mult	$2,$5
	mflo	$2
	#nop
	#nop
	addu	$9,$9,$2
	addu	$2,$18,$10
	lhu	$2,2($2)
	#nop
	sll	$2,$2,16
	sra	$6,$2,16
	sra	$7,$2,31
	move	$4,$15
	move	$5,$16
	multu	$6,$4
	mfhi	$3
	mflo	$2
	#nop
	#nop
	mult	$6,$5
	mflo	$23
	#nop
	#nop
	sw	$23,0($sp)
	addu	$3,$3,$23
	mult	$4,$7
	mflo	$7
	#nop
	#nop
	addu	$3,$3,$7
	addu	$8,$8,$2
	sltu	$4,$8,$2
	addu	$9,$9,$3
	addu	$9,$9,$4
	addu	$10,$10,$19
	lhu	$2,0($10)
	#nop
	sll	$2,$2,16
	sra	$6,$2,16
	sra	$7,$2,31
	move	$4,$14
	move	$5,$25
	multu	$6,$4
	mfhi	$3
	mflo	$2
	#nop
	#nop
	mult	$6,$5
	mflo	$10
	#nop
	#nop
	addu	$3,$3,$10
	mult	$4,$7
	mflo	$7
	#nop
	#nop
	addu	$3,$3,$7
	addu	$8,$8,$2
	sltu	$4,$8,$2
	addu	$9,$9,$3
	addu	$9,$9,$4
	srl	$8,$8,12
	sll	$2,$9,20
	or	$8,$8,$2
	sra	$9,$9,12
	li	$2,32767			# 0x00007fff
	slt	$2,$2,$8
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L33
	slt	$2,$8,-32768
	.set	macro
	.set	reorder

	li	$8,32767			# 0x00007fff
	slt	$2,$8,-32768
$L33:
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L34
	addu	$2,$13,$11
	.set	macro
	.set	reorder

	li	$8,-32768			# 0xffff8000
$L34:
	sll	$2,$2,2
	addu	$3,$2,$21
	li	$2,1			# 0x00000001
	.set	noreorder
	.set	nomacro
	bne	$11,$2,$L28
	subu	$2,$0,$8
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L32
	sll	$2,$2,4
	.set	macro
	.set	reorder

$L28:
	sll	$2,$8,4
$L32:
	sw	$2,0($3)
	addu	$11,$11,1
	slt	$2,$11,3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L35
	sll	$10,$11,1
	.set	macro
	.set	reorder

	addu	$12,$12,1
	slt	$2,$12,$20
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L36
	sll	$3,$12,1
	.set	macro
	.set	reorder

$L19:
	lw	$23,36($sp)
	lw	$22,32($sp)
	lw	$21,28($sp)
	lw	$20,24($sp)
	lw	$19,20($sp)
	lw	$18,16($sp)
	lw	$17,12($sp)
	lw	$16,8($sp)
	addu	$sp,$sp,40
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
	.ent	hexval
hexval:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	move	$3,$4
	addu	$4,$4,-48
	andi	$4,$4,0x00ff
	sltu	$4,$4,10
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L39
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L42
	addu	$2,$2,-48
	.set	macro
	.set	reorder

$L39:
	addu	$2,$3,-97
	andi	$2,$2,0x00ff
	sltu	$2,$2,6
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L40
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L42
	addu	$2,$2,-87
	.set	macro
	.set	reorder

$L40:
	addu	$2,$3,-65
	andi	$2,$2,0x00ff
	sltu	$2,$2,6
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L41
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L42
	li	$2,-1			# 0xffffffff
	.set	macro
	.set	reorder

$L41:
	addu	$2,$2,-55
$L42:
	j	$31
	.end	hexval
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
	beq	$2,$0,$L45
	move	$7,$0
	.set	macro
	.set	reorder

	addu	$7,$7,1
$L48:
	addu	$2,$6,$7
	lbu	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L48
	addu	$7,$7,1
	.set	macro
	.set	reorder

	addu	$7,$7,-1
$L45:
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
	.rdata
	.align	2
$LC0:
	.ascii	"0\000"
	.text
	.align	2
	.ent	say_dec
say_dec:
	.frame	$sp,40,$31		# vars= 16, regs= 1/0, args= 16, extra= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	subu	$sp,$sp,40
	sw	$31,32($sp)
	move	$5,$4
	li	$6,11			# 0x0000000b
	.set	noreorder
	.set	nomacro
	bgtz	$5,$L56
	sb	$0,27($sp)
	.set	macro
	.set	reorder

	lui	$4,%hi($LC0) # high
	.set	noreorder
	.set	nomacro
	j	$L57
	addiu	$4,$4,%lo($LC0) # low
	.set	macro
	.set	reorder

$L56:
	addu	$7,$sp,16
	li	$8,1717960704			# 0x66660000
	ori	$8,$8,0x6667
$L55:
	.set	noreorder
	.set	nomacro
	blez	$6,$L52
	mult	$5,$8
	.set	macro
	.set	reorder

	addu	$6,$6,-1
	addu	$4,$7,$6
	mfhi	$9
	#nop
	#nop
	sra	$3,$9,2
	sra	$2,$5,31
	subu	$3,$3,$2
	sll	$2,$3,2
	addu	$2,$2,$3
	sll	$2,$2,1
	subu	$2,$5,$2
	addu	$2,$2,48
	move	$5,$3
	.set	noreorder
	.set	nomacro
	bgtz	$5,$L55
	sb	$2,0($4)
	.set	macro
	.set	reorder

$L52:
	addu	$4,$sp,16
	addu	$4,$4,$6
$L57:
	jal	say
	lw	$31,32($sp)
	addu	$sp,$sp,40
	j	$31
	.end	say_dec
	.align	2
	.ent	hexword
hexword:
	.frame	$sp,40,$31		# vars= 0, regs= 5/0, args= 16, extra= 0
	.mask	0x800f0000,-8
	.fmask	0x00000000,0
	subu	$sp,$sp,40
	sw	$31,32($sp)
	sw	$19,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	move	$17,$4
	move	$19,$5
	lw	$2,0($17)
	#nop
	lbu	$3,0($2)
	.set	noreorder
	.set	nomacro
	j	$L70
	move	$18,$0
	.set	macro
	.set	reorder

$L61:
	lw	$2,0($17)
	#nop
	addu	$3,$2,1
	sw	$3,0($17)
	lbu	$3,1($2)
$L70:
	li	$2,32			# 0x00000020
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L61
	li	$2,9			# 0x00000009
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L61
	li	$2,13			# 0x0000000d
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L61
	move	$16,$0
	.set	macro
	.set	reorder

$L66:
	lw	$2,0($17)
	#nop
	lbu	$4,0($2)
	jal	hexval
	move	$3,$2
	.set	noreorder
	.set	nomacro
	bltz	$3,$L69
	li	$2,-1			# 0xffffffff
	.set	macro
	.set	reorder

	sll	$2,$18,4
	or	$18,$2,$3
	lw	$2,0($17)
	#nop
	addu	$2,$2,1
	sw	$2,0($17)
	addu	$16,$16,1
	slt	$2,$16,8
	bne	$2,$0,$L66
	sw	$18,0($19)
	move	$2,$0
$L69:
	lw	$31,32($sp)
	lw	$19,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	addu	$sp,$sp,40
	j	$31
	.end	hexword

	.lcomm	data.18,65536

	.lcomm	table.19,128

	.lcomm	rec.20,160
	.rdata
	.align	2
$LC1:
	.ascii	"joints: missing fixture\n\000"
	.align	2
$LC2:
	.ascii	"joints: cannot open fixture\n\000"
	.align	2
$LC3:
	.ascii	"joints: read error\n\000"
	.align	2
$LC4:
	.ascii	"joints: malformed TABLE\n\000"
	.align	2
$LC5:
	.ascii	"joints: empty TABLE\n\000"
	.align	2
$LC6:
	.ascii	"joints: row too long\n\000"
	.align	2
$LC7:
	.ascii	"joints: malformed fixture\n\000"
	.align	2
$LC8:
	.ascii	"joints: TABLE directive missing\n\000"
	.align	2
$LC9:
	.ascii	"joints: short row\n\000"
	.align	2
$LC10:
	.ascii	"joints: bad record count\n\000"
	.align	2
$LC11:
	.ascii	"joints: row length\n\000"
	.align	2
$LC12:
	.ascii	"joints: no terminating 0x80 before the record array ends"
	.ascii	"\n\000"
	.align	2
$LC13:
	.ascii	"joints: FAIL\n\000"
	.align	2
$LC14:
	.ascii	"joints: all shared vectors PASS (\000"
	.align	2
$LC15:
	.ascii	" cases, GTE software model)\n\000"
	.text
	.align	2
	.globl	main
	.ent	main
main:
	.frame	$sp,792,$31		# vars= 736, regs= 10/0, args= 16, extra= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,792
	sw	$31,788($sp)
	sw	$fp,784($sp)
	sw	$23,780($sp)
	sw	$22,776($sp)
	sw	$21,772($sp)
	sw	$20,768($sp)
	sw	$19,764($sp)
	sw	$18,760($sp)
	sw	$17,756($sp)
	sw	$16,752($sp)
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
	beq	$16,$2,$L72
	lui	$4,%hi($LC1) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L167
	addiu	$4,$4,%lo($LC1) # low
	.set	macro
	.set	reorder

$L72:
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
	bgez	$16,$L73
	move	$5,$16
	.set	macro
	.set	reorder

	lui	$4,%hi($LC2) # high
	.set	noreorder
	.set	nomacro
	j	$L167
	addiu	$4,$4,%lo($LC2) # low
	.set	macro
	.set	reorder

$L73:
	li	$4,4003			# 0x00000fa3
	lui	$6,%hi(data.18) # high
	addiu	$6,$6,%lo(data.18) # low
	.set	noreorder
	.set	nomacro
	jal	sys3
	li	$7,65535			# 0x0000ffff
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
	bgez	$17,$L74
	move	$23,$0
	.set	macro
	.set	reorder

	lui	$4,%hi($LC3) # high
	.set	noreorder
	.set	nomacro
	j	$L167
	addiu	$4,$4,%lo($LC3) # low
	.set	macro
	.set	reorder

$L74:
	lui	$4,%hi(data.18) # high
	addiu	$3,$4,%lo(data.18) # low
	addu	$2,$17,$3
	sb	$0,0($2)
	sw	$3,736($sp)
	sw	$0,744($sp)
	lbu	$2,%lo(data.18)($4)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L76
	move	$18,$23
	.set	macro
	.set	reorder

	li	$21,32			# 0x00000020
	li	$20,9			# 0x00000009
	li	$19,13			# 0x0000000d
	li	$22,10			# 0x0000000a
	addu	$fp,$sp,224
$L77:
	lw	$2,736($sp)
	#nop
	lbu	$2,0($2)
	j	$L168
$L80:
	lw	$3,736($sp)
	#nop
	addu	$2,$3,1
	sw	$2,736($sp)
	lbu	$2,1($3)
$L168:
	beq	$2,$21,$L80
	beq	$2,$20,$L80
	beq	$2,$19,$L80
	beq	$2,$22,$L80
	lw	$2,736($sp)
	#nop
	lbu	$3,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L76
	li	$2,35			# 0x00000023
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L83
	li	$2,84			# 0x00000054
	.set	macro
	.set	reorder

$L88:
	lw	$3,736($sp)
	#nop
	lbu	$2,0($3)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$22,$L75
	addu	$2,$3,1
	.set	macro
	.set	reorder

	sw	$2,736($sp)
	lbu	$2,1($3)
	#nop
	beq	$2,$0,$L76
	j	$L88
$L83:
	lw	$4,736($sp)
	#nop
	lbu	$3,0($4)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L104
	move	$16,$0
	.set	macro
	.set	reorder

	lbu	$3,1($4)
	li	$2,65			# 0x00000041
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L104
	li	$2,66			# 0x00000042
	.set	macro
	.set	reorder

	lbu	$3,2($4)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L104
	li	$2,76			# 0x0000004c
	.set	macro
	.set	reorder

	lbu	$3,3($4)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L104
	li	$2,69			# 0x00000045
	.set	macro
	.set	reorder

	lbu	$3,4($4)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L89
	addu	$2,$4,5
	.set	macro
	.set	reorder

	sw	$2,736($sp)
	lbu	$2,5($4)
	#nop
	beq	$2,$0,$L91
$L102:
	lw	$2,736($sp)
	#nop
	lbu	$2,0($2)
	#nop
	beq	$2,$22,$L91
	j	$L169
$L96:
	lw	$3,736($sp)
	#nop
	addu	$2,$3,1
	sw	$2,736($sp)
	lbu	$2,1($3)
$L169:
	beq	$2,$21,$L96
	beq	$2,$20,$L96
	beq	$2,$19,$L96
	lw	$2,736($sp)
	#nop
	lbu	$2,0($2)
	#nop
	beq	$2,$22,$L91
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L91
	addu	$4,$sp,736
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	hexword
	addu	$5,$sp,740
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L157
	slt	$2,$18,64
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L90
	sll	$2,$18,1
	.set	macro
	.set	reorder

	lui	$8,%hi(table.19) # high
	addiu	$8,$8,%lo(table.19) # low
	addu	$2,$2,$8
	lhu	$3,740($sp)
	#nop
	sh	$3,0($2)
	addu	$18,$18,1
$L90:
	lw	$2,736($sp)
	#nop
	lbu	$2,0($2)
	#nop
	bne	$2,$0,$L102
$L91:
	bne	$18,$0,$L75
	lui	$4,%hi($LC5) # high
	.set	noreorder
	.set	nomacro
	j	$L167
	addiu	$4,$4,%lo($LC5) # low
	.set	macro
	.set	reorder

$L165:
	.set	noreorder
	.set	nomacro
	j	$L142
	addu	$23,$23,1
	.set	macro
	.set	reorder

$L166:
	.set	noreorder
	.set	nomacro
	j	$L148
	addu	$23,$23,1
	.set	macro
	.set	reorder

$L89:
$L104:
	lw	$2,736($sp)
	#nop
	lbu	$2,0($2)
	j	$L170
$L109:
	lw	$3,736($sp)
	#nop
	addu	$2,$3,1
	sw	$2,736($sp)
	lbu	$2,1($3)
$L170:
	beq	$2,$21,$L109
	beq	$2,$20,$L109
	beq	$2,$19,$L109
	lw	$2,736($sp)
	#nop
	lbu	$2,0($2)
	#nop
	beq	$2,$0,$L105
	.set	noreorder
	.set	nomacro
	beq	$2,$22,$L105
	slt	$2,$16,128
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L158
	sll	$5,$16,2
	.set	macro
	.set	reorder

	addu	$4,$sp,736
	.set	noreorder
	.set	nomacro
	jal	hexword
	addu	$5,$fp,$5
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L159
	addu	$16,$16,1
	.set	macro
	.set	reorder

	j	$L104
$L105:
	beq	$16,$0,$L75
	.set	noreorder
	.set	nomacro
	beq	$18,$0,$L160
	slt	$2,$16,15
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L161
	lui	$4,%hi($LC9) # high
	.set	macro
	.set	reorder

	lw	$17,280($sp)
	#nop
	addu	$2,$17,-1
	sltu	$2,$2,5
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L162
	sll	$2,$17,3
	.set	macro
	.set	reorder

	addu	$2,$2,39
	.set	noreorder
	.set	nomacro
	bne	$16,$2,$L163
	slt	$2,$17,5
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L120
	sll	$2,$17,5
	.set	macro
	.set	reorder

	addu	$2,$sp,$2
	lbu	$2,256($2)
	#nop
	andi	$2,$2,0x0080
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L164
	lui	$4,%hi($LC12) # high
	.set	macro
	.set	reorder

$L120:
	move	$4,$0
	addu	$6,$sp,16
	addu	$5,$sp,232
	sll	$2,$4,2
$L171:
	addu	$3,$2,$6
	addu	$2,$5,$2
	lw	$2,0($2)
	#nop
	sw	$2,36($3)
	addu	$4,$4,1
	slt	$2,$4,12
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L171
	sll	$2,$4,2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	blez	$17,$L127
	move	$4,$0
	.set	macro
	.set	reorder

	lui	$2,%hi(rec.20) # high
	addiu	$7,$2,%lo(rec.20) # low
	sll	$2,$4,5
$L173:
	addu	$2,$2,60
	addu	$5,$fp,$2
	sll	$2,$4,2
	addu	$2,$2,$4
	sll	$2,$2,2
	addu	$2,$2,$7
	lbu	$3,0($5)
	#nop
	sb	$3,0($2)
	move	$3,$0
	sll	$2,$4,2
	addu	$2,$2,$4
	sll	$2,$2,2
	addu	$6,$2,$7
	addu	$2,$6,$3
$L172:
	sb	$0,1($2)
	addu	$3,$3,1
	slt	$2,$3,6
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L172
	addu	$2,$6,$3
	.set	macro
	.set	reorder

	sll	$2,$4,2
	addu	$2,$2,$4
	sll	$2,$2,2
	addu	$2,$2,$7
	lbu	$3,4($5)
	#nop
	sb	$3,7($2)
	lhu	$3,8($5)
	#nop
	sh	$3,8($2)
	lhu	$3,12($5)
	#nop
	sh	$3,10($2)
	lhu	$3,16($5)
	#nop
	sh	$3,12($2)
	lhu	$3,20($5)
	#nop
	sh	$3,14($2)
	lhu	$3,24($5)
	#nop
	sh	$3,16($2)
	lhu	$3,28($5)
	#nop
	sh	$3,18($2)
	addu	$4,$4,1
	slt	$2,$4,$17
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L173
	sll	$2,$4,5
	.set	macro
	.set	reorder

$L127:
	lui	$2,%hi(rec.20) # high
	addiu	$2,$2,%lo(rec.20) # low
	sw	$2,36($sp)
	la	$8,table.19
	sw	$8,40($sp)
	addu	$2,$sp,16
	sw	$2,48($sp)
	lhu	$2,228($sp)
	#nop
	sh	$2,120($sp)
	move	$4,$0
	addu	$5,$sp,16
	li	$3,305397760			# 0x12340000
	ori	$3,$3,0x5678
	sll	$2,$4,2
$L174:
	addu	$2,$2,$5
	sw	$3,112($2)
	sw	$3,160($2)
	addu	$4,$4,1
	slt	$2,$4,12
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L174
	sll	$2,$4,2
	.set	macro
	.set	reorder

	addu	$4,$sp,48
	addu	$5,$sp,128
	.set	noreorder
	.set	nomacro
	jal	func_80052960
	addu	$6,$sp,176
	.set	macro
	.set	reorder

	move	$4,$0
	addu	$7,$sp,16
	addu	$6,$sp,284
	sll	$5,$17,3
	sll	$3,$4,2
$L175:
	addu	$3,$3,$7
	addu	$2,$5,$4
	sll	$2,$2,2
	addu	$2,$6,$2
	lw	$3,112($3)
	lw	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L165
	addu	$4,$4,1
	.set	macro
	.set	reorder

	slt	$2,$4,12
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L175
	sll	$3,$4,2
	.set	macro
	.set	reorder

$L142:
	move	$4,$0
	addu	$7,$sp,16
	addu	$6,$sp,332
	sll	$5,$17,3
	sll	$3,$4,2
$L176:
	addu	$3,$3,$7
	addu	$2,$5,$4
	sll	$2,$2,2
	addu	$2,$6,$2
	lw	$3,160($3)
	lw	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L166
	addu	$4,$4,1
	.set	macro
	.set	reorder

	slt	$2,$4,12
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L176
	sll	$3,$4,2
	.set	macro
	.set	reorder

$L148:
	lw	$8,744($sp)
	#nop
	addu	$8,$8,1
	sw	$8,744($sp)
$L75:
	lw	$2,736($sp)
	#nop
	lbu	$2,0($2)
	#nop
	bne	$2,$0,$L77
$L76:
	lw	$8,744($sp)
	#nop
	beq	$8,$0,$L155
	.set	noreorder
	.set	nomacro
	beq	$23,$0,$L154
	lui	$4,%hi($LC14) # high
	.set	macro
	.set	reorder

$L155:
	lui	$4,%hi($LC13) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC13) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L156
	li	$2,1			# 0x00000001
	.set	macro
	.set	reorder

$L157:
	lui	$4,%hi($LC4) # high
	.set	noreorder
	.set	nomacro
	j	$L167
	addiu	$4,$4,%lo($LC4) # low
	.set	macro
	.set	reorder

$L158:
	lui	$4,%hi($LC6) # high
	.set	noreorder
	.set	nomacro
	j	$L167
	addiu	$4,$4,%lo($LC6) # low
	.set	macro
	.set	reorder

$L159:
	lui	$4,%hi($LC7) # high
	.set	noreorder
	.set	nomacro
	j	$L167
	addiu	$4,$4,%lo($LC7) # low
	.set	macro
	.set	reorder

$L160:
	lui	$4,%hi($LC8) # high
	.set	noreorder
	.set	nomacro
	j	$L167
	addiu	$4,$4,%lo($LC8) # low
	.set	macro
	.set	reorder

$L161:
	.set	noreorder
	.set	nomacro
	j	$L167
	addiu	$4,$4,%lo($LC9) # low
	.set	macro
	.set	reorder

$L162:
	lui	$4,%hi($LC10) # high
	.set	noreorder
	.set	nomacro
	j	$L167
	addiu	$4,$4,%lo($LC10) # low
	.set	macro
	.set	reorder

$L163:
	lui	$4,%hi($LC11) # high
	.set	noreorder
	.set	nomacro
	j	$L167
	addiu	$4,$4,%lo($LC11) # low
	.set	macro
	.set	reorder

$L164:
	addiu	$4,$4,%lo($LC12) # low
$L167:
	jal	say
	.set	noreorder
	.set	nomacro
	j	$L156
	li	$2,2			# 0x00000002
	.set	macro
	.set	reorder

$L154:
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC14) # low
	.set	macro
	.set	reorder

	lw	$4,744($sp)
	jal	say_dec
	lui	$4,%hi($LC15) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC15) # low
	.set	macro
	.set	reorder

	move	$2,$0
$L156:
	lw	$31,788($sp)
	lw	$fp,784($sp)
	lw	$23,780($sp)
	lw	$22,776($sp)
	lw	$21,772($sp)
	lw	$20,768($sp)
	lw	$19,764($sp)
	lw	$18,760($sp)
	lw	$17,756($sp)
	lw	$16,752($sp)
	addu	$sp,$sp,792
	j	$31
	.end	main

	.lcomm	r,18
