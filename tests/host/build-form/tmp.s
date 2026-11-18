	.file	1 "/home/kai-luni/git/future-cop-lapd-ps1/tests/host/form_harness.c"

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
	.globl	func_800449F8
	.ent	func_800449F8
func_800449F8:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	j	$31
	.end	func_800449F8
	.align	2
	.globl	func_800448D8
	.ent	func_800448D8
func_800448D8:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lui	$2,%hi(clips) # high
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$2,$2,%lo(clips) # low
	.set	macro
	.set	reorder

	.end	func_800448D8
	.align	2
	.globl	func_80044AE8
	.ent	func_80044AE8
func_80044AE8:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	j	$31
	.end	func_80044AE8
	.align	2
	.globl	func_80019668
	.ent	func_80019668
func_80019668:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	$31
	move	$2,$0
	.set	macro
	.set	reorder

	.end	func_80019668
	.align	2
	.globl	memset
	.ent	memset
memset:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	move	$2,$4
	move	$7,$2
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L8
	addu	$3,$6,-1
	.set	macro
	.set	reorder

	li	$6,-1			# 0xffffffff
$L9:
	sb	$5,0($7)
	addu	$3,$3,-1
	.set	noreorder
	.set	nomacro
	bne	$3,$6,$L9
	addu	$7,$7,1
	.set	macro
	.set	reorder

$L8:
	j	$31
	.end	memset
	.align	2
	.globl	memcpy
	.ent	memcpy
memcpy:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	move	$7,$4
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L13
	addu	$3,$6,-1
	.set	macro
	.set	reorder

	li	$6,-1			# 0xffffffff
$L14:
	lbu	$2,0($5)
	#nop
	sb	$2,0($7)
	addu	$5,$5,1
	addu	$3,$3,-1
	.set	noreorder
	.set	nomacro
	bne	$3,$6,$L14
	addu	$7,$7,1
	.set	macro
	.set	reorder

$L13:
	.set	noreorder
	.set	nomacro
	j	$31
	move	$2,$4
	.set	macro
	.set	reorder

	.end	memcpy
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
	beq	$4,$0,$L18
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L21
	addu	$2,$2,-48
	.set	macro
	.set	reorder

$L18:
	addu	$2,$3,-97
	andi	$2,$2,0x00ff
	sltu	$2,$2,6
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L19
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L21
	addu	$2,$2,-87
	.set	macro
	.set	reorder

$L19:
	addu	$2,$3,-65
	andi	$2,$2,0x00ff
	sltu	$2,$2,6
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L20
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L21
	li	$2,-1			# 0xffffffff
	.set	macro
	.set	reorder

$L20:
	addu	$2,$2,-55
$L21:
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
	beq	$2,$0,$L24
	move	$7,$0
	.set	macro
	.set	reorder

	addu	$7,$7,1
$L27:
	addu	$2,$6,$7
	lbu	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L27
	addu	$7,$7,1
	.set	macro
	.set	reorder

	addu	$7,$7,-1
$L24:
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
	.align	2
	.ent	num
num:
	.frame	$sp,40,$31		# vars= 16, regs= 1/0, args= 16, extra= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	subu	$sp,$sp,40
	sw	$31,32($sp)
	move	$5,$4
	srl	$8,$5,31
	.set	noreorder
	.set	nomacro
	beq	$8,$0,$L29
	move	$7,$0
	.set	macro
	.set	reorder

	subu	$5,$0,$5
$L29:
	.set	noreorder
	.set	nomacro
	bne	$5,$0,$L30
	addu	$3,$sp,$7
	.set	macro
	.set	reorder

	li	$2,48			# 0x00000030
	sb	$2,16($3)
	addu	$7,$7,1
$L30:
	blez	$5,$L32
	addu	$6,$sp,16
	li	$9,1717960704			# 0x66660000
	ori	$9,$9,0x6667
$L33:
	addu	$4,$6,$7
	mult	$5,$9
	mfhi	$10
	#nop
	#nop
	sra	$3,$10,2
	sra	$2,$5,31
	subu	$3,$3,$2
	sll	$2,$3,2
	addu	$2,$2,$3
	sll	$2,$2,1
	subu	$2,$5,$2
	addu	$2,$2,48
	sb	$2,0($4)
	move	$5,$3
	.set	noreorder
	.set	nomacro
	bgtz	$5,$L33
	addu	$7,$7,1
	.set	macro
	.set	reorder

$L32:
	.set	noreorder
	.set	nomacro
	beq	$8,$0,$L35
	addu	$3,$sp,$7
	.set	macro
	.set	reorder

	li	$2,45			# 0x0000002d
	sb	$2,16($3)
	addu	$7,$7,1
$L35:
	sra	$2,$7,31
	move	$3,$2
	subu	$2,$7,$2
	sra	$2,$2,1
	.set	noreorder
	.set	nomacro
	blez	$2,$L37
	move	$5,$0
	.set	macro
	.set	reorder

	addu	$8,$sp,16
	subu	$2,$7,$3
	sra	$9,$2,1
$L38:
	addu	$4,$8,$5
	lbu	$6,0($4)
	addu	$5,$5,1
	subu	$3,$7,$5
	addu	$3,$8,$3
	lbu	$2,0($3)
	#nop
	sb	$2,0($4)
	slt	$2,$5,$9
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L38
	sb	$6,0($3)
	.set	macro
	.set	reorder

$L37:
	li	$4,4004			# 0x00000fa4
	li	$5,1			# 0x00000001
	.set	noreorder
	.set	nomacro
	jal	sys3
	addu	$6,$sp,16
	.set	macro
	.set	reorder

	lw	$31,32($sp)
	addu	$sp,$sp,40
	j	$31
	.end	num
	.align	2
	.ent	setClip
setClip:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	sb	$0,0($4)
	li	$2,3			# 0x00000003
	sb	$2,1($4)
	sb	$0,2($4)
	sb	$0,3($4)
	sh	$5,4($4)
	sh	$6,6($4)
	sb	$0,8($4)
	sb	$0,9($4)
	sh	$0,10($4)
	li	$2,10000			# 0x00002710
	.set	noreorder
	.set	nomacro
	j	$31
	sw	$2,12($4)
	.set	macro
	.set	reorder

	.end	setClip
	.align	2
	.ent	setTrack
setTrack:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	sw	$5,0($4)
	sw	$5,4($4)
	sw	$0,8($4)
	sh	$0,12($4)
	sh	$0,14($4)
	li	$2,1			# 0x00000001
	sb	$2,16($4)
	sb	$0,17($4)
	sb	$2,18($4)
	.set	noreorder
	.set	nomacro
	j	$31
	sb	$0,19($4)
	.set	macro
	.set	reorder

	.end	setTrack

	.lcomm	data.28,32768
	.rdata
	.align	2
$LC0:
	.ascii	"mech-form: missing fixture\n\000"
	.align	2
$LC1:
	.ascii	"mech-form: cannot open fixture\n\000"
	.align	2
$LC2:
	.ascii	"mech-form: read error\n\000"
	.align	2
$LC3:
	.ascii	"mech-form: short TABLE\n\000"
	.align	2
$LC4:
	.ascii	"mech-form: malformed TABLE\n\000"
	.align	2
$LC5:
	.ascii	"mech-form: malformed row\n\000"
	.align	2
$LC6:
	.ascii	"mech-form: short row\n\000"
	.align	2
$LC7:
	.ascii	"case \000"
	.align	2
$LC8:
	.ascii	": progress \000"
	.align	2
$LC9:
	.ascii	" main \000"
	.align	2
$LC10:
	.ascii	" != \000"
	.align	2
$LC11:
	.ascii	" companion \000"
	.align	2
$LC12:
	.ascii	"\n\000"
	.align	2
$LC13:
	.ascii	"ALL PASS (mech-form parity: \000"
	.align	2
$LC14:
	.ascii	")\n\000"
	.align	2
$LC15:
	.ascii	"FAILURES \000"
	.align	2
$LC16:
	.ascii	" of \000"
	.text
	.align	2
	.globl	main
	.ent	main
main:
	.frame	$sp,96,$31		# vars= 40, regs= 10/0, args= 16, extra= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,96
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
	move	$16,$4
	.set	noreorder
	.set	nomacro
	jal	__main
	move	$17,$5
	.set	macro
	.set	reorder

	sw	$0,48($sp)
	sw	$0,52($sp)
	addu	$4,$sp,16
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,4			# 0x00000004
	.set	macro
	.set	reorder

	li	$2,29			# 0x0000001d
	sh	$2,24($sp)
	sh	$2,26($sp)
	li	$2,2			# 0x00000002
	.set	noreorder
	.set	nomacro
	beq	$16,$2,$L43
	lui	$4,%hi($LC0) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L110
	addiu	$4,$4,%lo($LC0) # low
	.set	macro
	.set	reorder

$L43:
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
	bgez	$16,$L44
	move	$5,$16
	.set	macro
	.set	reorder

	lui	$4,%hi($LC1) # high
	.set	noreorder
	.set	nomacro
	j	$L110
	addiu	$4,$4,%lo($LC1) # low
	.set	macro
	.set	reorder

$L44:
	li	$4,4003			# 0x00000fa3
	lui	$6,%hi(data.28) # high
	addiu	$6,$6,%lo(data.28) # low
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
	bgez	$17,$L45
	lui	$4,%hi($LC2) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L110
	addiu	$4,$4,%lo($LC2) # low
	.set	macro
	.set	reorder

$L45:
	lui	$4,%hi(data.28) # high
	addiu	$3,$4,%lo(data.28) # low
	addu	$2,$17,$3
	sb	$0,0($2)
	lbu	$2,%lo(data.28)($4)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L47
	move	$18,$3
	.set	macro
	.set	reorder

	li	$22,32			# 0x00000020
	li	$21,9			# 0x00000009
	li	$20,13			# 0x0000000d
	li	$23,10			# 0x0000000a
	lui	$2,%hi(tracks) # high
	addiu	$fp,$2,%lo(tracks) # low
$L111:
	lbu	$2,0($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$22,$L111
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$21,$L111
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$20,$L111
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$23,$L111
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	lbu	$3,0($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L47
	li	$2,35			# 0x00000023
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L54
	li	$2,84			# 0x00000054
	.set	macro
	.set	reorder

	lbu	$2,0($18)
$L114:
	beq	$2,$23,$L46
	addu	$18,$18,1
	lbu	$2,0($18)
	#nop
	beq	$2,$0,$L47
	j	$L114
$L54:
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L115
	move	$19,$0
	.set	macro
	.set	reorder

	lbu	$3,1($18)
	li	$2,65			# 0x00000041
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L113
	move	$17,$0
	.set	macro
	.set	reorder

	lbu	$3,2($18)
	li	$2,66			# 0x00000042
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L113
	li	$2,76			# 0x0000004c
	.set	macro
	.set	reorder

	lbu	$3,3($18)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L113
	li	$2,69			# 0x00000045
	.set	macro
	.set	reorder

	lbu	$3,4($18)
	#nop
	bne	$3,$2,$L113
	addu	$18,$18,5
$L112:
	lbu	$2,0($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$22,$L112
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$21,$L112
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$20,$L112
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	lbu	$2,0($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$23,$L117
	lui	$4,%hi($LC3) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L69
	move	$16,$0
	.set	macro
	.set	reorder

	lui	$4,%hi($LC3) # high
$L117:
	.set	noreorder
	.set	nomacro
	j	$L110
	addiu	$4,$4,%lo($LC3) # low
	.set	macro
	.set	reorder

$L69:
$L74:
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
	bltz	$3,$L107
	sll	$2,$17,4
	.set	macro
	.set	reorder

	or	$17,$2,$3
	addu	$16,$16,1
	slt	$2,$16,8
	bne	$2,$0,$L74
	.set	noreorder
	.set	nomacro
	bne	$19,$0,$L77
	li	$2,1			# 0x00000001
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L63
	sh	$17,16($sp)
	.set	macro
	.set	reorder

$L77:
	.set	noreorder
	.set	nomacro
	bne	$19,$2,$L79
	li	$2,2			# 0x00000002
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L63
	sh	$17,24($sp)
	.set	macro
	.set	reorder

$L79:
	bne	$19,$2,$L81
	.set	noreorder
	.set	nomacro
	j	$L63
	sh	$17,18($sp)
	.set	macro
	.set	reorder

$L81:
	sh	$17,26($sp)
$L63:
	addu	$19,$19,1
	slt	$2,$19,4
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L112
	move	$17,$0
	.set	macro
	.set	reorder

	lui	$4,%hi(clips) # high
	lh	$5,16($sp)
	lh	$6,24($sp)
	.set	noreorder
	.set	nomacro
	jal	setClip
	addiu	$4,$4,%lo(clips) # low
	.set	macro
	.set	reorder

	lui	$4,%hi(clips+16) # high
	lh	$5,18($sp)
	lh	$6,26($sp)
	.set	noreorder
	.set	nomacro
	jal	setClip
	addiu	$4,$4,%lo(clips+16) # low
	.set	macro
	.set	reorder

	j	$L46
$L115:
	move	$17,$0
$L113:
	lbu	$2,0($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$22,$L113
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$21,$L113
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$20,$L113
	addu	$18,$18,1
	.set	macro
	.set	reorder

	addu	$18,$18,-1
	lbu	$2,0($18)
	#nop
	beq	$2,$23,$L85
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L116
	slt	$2,$19,4
	.set	macro
	.set	reorder

	move	$16,$0
$L97:
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
	bltz	$3,$L108
	sll	$2,$17,4
	.set	macro
	.set	reorder

	or	$17,$2,$3
	addu	$16,$16,1
	slt	$2,$16,8
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L97
	sll	$2,$19,2
	.set	macro
	.set	reorder

	addu	$2,$sp,$2
	sw	$17,32($2)
	addu	$19,$19,1
	slt	$2,$19,4
	bne	$2,$0,$L115
$L85:
	slt	$2,$19,4
$L116:
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L109
	lui	$5,%hi(clips) # high
	.set	macro
	.set	reorder

	lw	$8,52($sp)
	#nop
	addu	$8,$8,1
	sw	$8,52($sp)
	lui	$8,%hi(tracks) # high
	addiu	$4,$8,%lo(tracks) # low
	.set	noreorder
	.set	nomacro
	jal	setTrack
	addiu	$5,$5,%lo(clips) # low
	.set	macro
	.set	reorder

	lui	$16,%hi(tracks+20) # high
	addiu	$4,$16,%lo(tracks+20) # low
	lui	$5,%hi(clips+16) # high
	.set	noreorder
	.set	nomacro
	jal	setTrack
	addiu	$5,$5,%lo(clips+16) # low
	.set	macro
	.set	reorder

	move	$4,$0
	lui	$8,%hi(tracks) # high
	addiu	$5,$8,%lo(tracks) # low
	lw	$6,36($sp)
	.set	noreorder
	.set	nomacro
	jal	func_80044B34
	move	$7,$4
	.set	macro
	.set	reorder

	move	$4,$0
	addiu	$5,$16,%lo(tracks+20) # low
	lw	$6,36($sp)
	.set	noreorder
	.set	nomacro
	jal	func_80044B34
	move	$7,$4
	.set	macro
	.set	reorder

	lh	$3,12($fp)
	lh	$2,40($sp)
	#nop
	bne	$3,$2,$L103
	lh	$3,32($fp)
	lh	$2,44($sp)
	#nop
	beq	$3,$2,$L46
$L103:
	lw	$8,48($sp)
	#nop
	addu	$8,$8,1
	sw	$8,48($sp)
	lui	$4,%hi($LC7) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC7) # low
	.set	macro
	.set	reorder

	lw	$4,32($sp)
	.set	noreorder
	.set	nomacro
	jal	num
	lui	$16,%hi($LC10) # high
	.set	macro
	.set	reorder

	lui	$4,%hi($LC8) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC8) # low
	.set	macro
	.set	reorder

	lw	$4,36($sp)
	jal	num
	lui	$4,%hi($LC9) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC9) # low
	.set	macro
	.set	reorder

	lh	$4,12($fp)
	jal	num
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$16,%lo($LC10) # low
	.set	macro
	.set	reorder

	lh	$4,40($sp)
	jal	num
	lui	$4,%hi($LC11) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC11) # low
	.set	macro
	.set	reorder

	lh	$4,32($fp)
	jal	num
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$16,%lo($LC10) # low
	.set	macro
	.set	reorder

	lh	$4,44($sp)
	jal	num
	lui	$4,%hi($LC12) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC12) # low
	.set	macro
	.set	reorder

$L46:
	lbu	$2,0($18)
	#nop
	bne	$2,$0,$L111
$L47:
	lw	$8,48($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$8,$0,$L105
	lui	$4,%hi($LC13) # high
	.set	macro
	.set	reorder

	lui	$4,%hi($LC15) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC15) # low
	.set	macro
	.set	reorder

	lw	$4,48($sp)
	jal	num
	lui	$4,%hi($LC16) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC16) # low
	.set	macro
	.set	reorder

	lw	$4,52($sp)
	jal	num
	lui	$4,%hi($LC12) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC12) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L106
	li	$2,1			# 0x00000001
	.set	macro
	.set	reorder

$L107:
	lui	$4,%hi($LC4) # high
	.set	noreorder
	.set	nomacro
	j	$L110
	addiu	$4,$4,%lo($LC4) # low
	.set	macro
	.set	reorder

$L108:
	lui	$4,%hi($LC5) # high
	.set	noreorder
	.set	nomacro
	j	$L110
	addiu	$4,$4,%lo($LC5) # low
	.set	macro
	.set	reorder

$L109:
	lui	$4,%hi($LC6) # high
	addiu	$4,$4,%lo($LC6) # low
$L110:
	jal	say
	.set	noreorder
	.set	nomacro
	j	$L106
	li	$2,2			# 0x00000002
	.set	macro
	.set	reorder

$L105:
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC13) # low
	.set	macro
	.set	reorder

	lw	$4,52($sp)
	jal	num
	lui	$4,%hi($LC14) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC14) # low
	.set	macro
	.set	reorder

	move	$2,$0
$L106:
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
	.end	main

	.comm	clips,32

	.comm	tracks,40
