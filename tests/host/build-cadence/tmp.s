	.file	1 "/home/kai-luni/git/future-cop-lapd-ps1/tests/host/cadence_harness.c"

 # -G value = 0, Cpu = 3000, ISA = 1
 # GNU C version cygnus-2.7.2-970404 SN32.3.7.0004 (SonyPSX) compiled by CC.
 # options passed:  -G0 -O1
 # options enabled:  -fdefer-pop -fomit-frame-pointer -fthread-jumps
 # -fpeephole -ffunction-cse -finline -fkeep-static-consts
 # -fpcc-struct-return -fdelayed-branch -fcommon -fverbose-asm -fgnu-linker
 # -mgas -msoft-float -mcpu=3000

gcc2_compiled.:
__gnu_compiled_c:
	.globl	D_8009FA40
	.data
	.align	2
D_8009FA40:
	.word	0
	.text
	.align	2
	.globl	func_8006AAEC
	.ent	func_8006AAEC
func_8006AAEC:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lui	$2,%hi(record) # high
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$2,$2,%lo(record) # low
	.set	macro
	.set	reorder

	.end	func_8006AAEC
	.align	2
	.globl	func_8006ACB0
	.ent	func_8006ACB0
func_8006ACB0:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lui	$3,%hi(fired) # high
	li	$2,1			# 0x00000001
	.set	noreorder
	.set	nomacro
	j	$31
	sw	$2,%lo(fired)($3)
	.set	macro
	.set	reorder

	.end	func_8006ACB0
	.align	2
	.globl	func_8006B23C
	.ent	func_8006B23C
func_8006B23C:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	$31
	move	$2,$0
	.set	macro
	.set	reorder

	.end	func_8006B23C
	.align	2
	.globl	func_80046C84
	.ent	func_80046C84
func_80046C84:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	$31
	move	$2,$0
	.set	macro
	.set	reorder

	.end	func_80046C84
	.align	2
	.globl	func_8004E5F0
	.ent	func_8004E5F0
func_8004E5F0:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	$31
	move	$2,$0
	.set	macro
	.set	reorder

	.end	func_8004E5F0
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
	.globl	memcpy
	.ent	memcpy
memcpy:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	move	$7,$4
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L9
	addu	$3,$6,-1
	.set	macro
	.set	reorder

	li	$6,-1			# 0xffffffff
$L10:
	lbu	$2,0($5)
	#nop
	sb	$2,0($7)
	addu	$5,$5,1
	addu	$3,$3,-1
	.set	noreorder
	.set	nomacro
	bne	$3,$6,$L10
	addu	$7,$7,1
	.set	macro
	.set	reorder

$L9:
	.set	noreorder
	.set	nomacro
	j	$31
	move	$2,$4
	.set	macro
	.set	reorder

	.end	memcpy
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
	beq	$6,$0,$L14
	addu	$3,$6,-1
	.set	macro
	.set	reorder

	li	$6,-1			# 0xffffffff
$L15:
	sb	$5,0($7)
	addu	$3,$3,-1
	.set	noreorder
	.set	nomacro
	bne	$3,$6,$L15
	addu	$7,$7,1
	.set	macro
	.set	reorder

$L14:
	j	$31
	.end	memset
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
	beq	$2,$0,$L20
	move	$7,$0
	.set	macro
	.set	reorder

	addu	$7,$7,1
$L23:
	addu	$2,$6,$7
	lbu	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L23
	addu	$7,$7,1
	.set	macro
	.set	reorder

	addu	$7,$7,-1
$L20:
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
	beq	$8,$0,$L25
	move	$7,$0
	.set	macro
	.set	reorder

	subu	$5,$0,$5
$L25:
	.set	noreorder
	.set	nomacro
	bne	$5,$0,$L26
	addu	$3,$sp,$7
	.set	macro
	.set	reorder

	li	$2,48			# 0x00000030
	sb	$2,16($3)
	addu	$7,$7,1
$L26:
	blez	$5,$L28
	addu	$6,$sp,16
	li	$9,1717960704			# 0x66660000
	ori	$9,$9,0x6667
$L29:
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
	bgtz	$5,$L29
	addu	$7,$7,1
	.set	macro
	.set	reorder

$L28:
	.set	noreorder
	.set	nomacro
	beq	$8,$0,$L31
	addu	$3,$sp,$7
	.set	macro
	.set	reorder

	li	$2,45			# 0x0000002d
	sb	$2,16($3)
	addu	$7,$7,1
$L31:
	sra	$2,$7,31
	move	$3,$2
	subu	$2,$7,$2
	sra	$2,$2,1
	.set	noreorder
	.set	nomacro
	blez	$2,$L33
	move	$5,$0
	.set	macro
	.set	reorder

	addu	$8,$sp,16
	subu	$2,$7,$3
	sra	$9,$2,1
$L34:
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
	bne	$2,$0,$L34
	sb	$6,0($3)
	.set	macro
	.set	reorder

$L33:
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
	.ent	load
load:
	.frame	$sp,32,$31		# vars= 0, regs= 4/0, args= 16, extra= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,32
	sw	$31,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	move	$5,$4
	li	$4,4005			# 0x00000fa5
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
	bltz	$16,$L38
	li	$4,4003			# 0x00000fa3
	.set	macro
	.set	reorder

	move	$5,$16
	lui	$18,%hi(data) # high
	addiu	$6,$18,%lo(data) # low
	.set	noreorder
	.set	nomacro
	jal	sys3
	li	$7,16383			# 0x00003fff
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
	bltz	$17,$L38
	addiu	$2,$18,%lo(data) # low
	.set	macro
	.set	reorder

	addu	$2,$17,$2
	sb	$0,0($2)
	.set	noreorder
	.set	nomacro
	j	$L39
	move	$2,$0
	.set	macro
	.set	reorder

$L38:
	li	$2,-1			# 0xffffffff
$L39:
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	addu	$sp,$sp,32
	j	$31
	.end	load
	.align	2
	.ent	skip_space
skip_space:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lui	$4,%hi(p) # high
	li	$8,32			# 0x00000020
	li	$7,9			# 0x00000009
	li	$6,13			# 0x0000000d
	.set	noreorder
	.set	nomacro
	j	$L44
	li	$5,10			# 0x0000000a
	.set	macro
	.set	reorder

$L46:
	lw	$2,%lo(p)($4)
	#nop
	addu	$2,$2,1
	sw	$2,%lo(p)($4)
$L44:
	lw	$2,%lo(p)($4)
	#nop
	lbu	$2,0($2)
	#nop
	beq	$2,$8,$L46
	beq	$2,$7,$L46
	beq	$2,$6,$L46
	beq	$2,$5,$L46
	lw	$2,%lo(p)($4)
	#nop
	lbu	$3,0($2)
	li	$2,35			# 0x00000023
	bne	$3,$2,$L40
$L53:
	lw	$3,%lo(p)($4)
	#nop
	lbu	$2,0($3)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$5,$L44
	addu	$2,$3,1
	.set	macro
	.set	reorder

	sw	$2,%lo(p)($4)
	lbu	$2,1($3)
	#nop
	beq	$2,$0,$L44
	j	$L53
$L40:
	j	$31
	.end	skip_space
	.align	2
	.ent	next_int
next_int:
	.frame	$sp,32,$31		# vars= 0, regs= 4/0, args= 16, extra= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,32
	sw	$31,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	move	$18,$4
	move	$16,$0
	.set	noreorder
	.set	nomacro
	jal	skip_space
	move	$17,$16
	.set	macro
	.set	reorder

	lui	$2,%hi(p) # high
	lw	$2,%lo(p)($2)
	#nop
	lbu	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L55
	lui	$5,%hi(p) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L62
	move	$2,$16
	.set	macro
	.set	reorder

$L55:
	lw	$4,%lo(p)($5)
	#nop
	lbu	$3,0($4)
	li	$2,45			# 0x0000002d
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L63
	lui	$2,%hi(p) # high
	.set	macro
	.set	reorder

	li	$17,1			# 0x00000001
	addu	$2,$4,1
	sw	$2,%lo(p)($5)
	lui	$2,%hi(p) # high
$L63:
	lw	$2,%lo(p)($2)
	#nop
	lbu	$2,0($2)
	#nop
	addu	$2,$2,-48
	sltu	$2,$2,10
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L58
	sll	$2,$16,2
	.set	macro
	.set	reorder

	lui	$5,%hi(p) # high
$L64:
	addu	$2,$2,$16
	sll	$2,$2,1
	addu	$2,$2,-48
	lw	$4,%lo(p)($5)
	#nop
	lbu	$3,0($4)
	#nop
	addu	$16,$2,$3
	addu	$2,$4,1
	sw	$2,%lo(p)($5)
	lbu	$2,1($4)
	#nop
	addu	$2,$2,-48
	sltu	$2,$2,10
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L64
	sll	$2,$16,2
	.set	macro
	.set	reorder

$L58:
	.set	noreorder
	.set	nomacro
	beq	$17,$0,$L61
	sw	$16,0($18)
	.set	macro
	.set	reorder

	subu	$2,$0,$16
	sw	$2,0($18)
$L61:
	li	$2,1			# 0x00000001
$L62:
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	addu	$sp,$sp,32
	j	$31
	.end	next_int
	.align	2
	.ent	next_name
next_name:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, extra= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,24
	sw	$31,20($sp)
	sw	$16,16($sp)
	.set	noreorder
	.set	nomacro
	jal	skip_space
	move	$16,$0
	.set	macro
	.set	reorder

	lui	$2,%hi(p) # high
	lw	$2,%lo(p)($2)
	#nop
	lbu	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L66
	lui	$5,%hi(p) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L72
	move	$2,$16
	.set	macro
	.set	reorder

$L66:
	li	$9,32			# 0x00000020
	li	$8,9			# 0x00000009
	li	$7,10			# 0x0000000a
	lui	$2,%hi(namebuf) # high
	.set	noreorder
	.set	nomacro
	j	$L67
	addiu	$6,$2,%lo(namebuf) # low
	.set	macro
	.set	reorder

$L71:
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L68
	addu	$4,$16,$6
	.set	macro
	.set	reorder

	lw	$3,%lo(p)($5)
	#nop
	lbu	$2,0($3)
	#nop
	sb	$2,0($4)
	addu	$3,$3,1
	sw	$3,%lo(p)($5)
	addu	$16,$16,1
$L67:
	lw	$2,%lo(p)($5)
	#nop
	lbu	$2,0($2)
	#nop
	beq	$2,$0,$L68
	beq	$2,$9,$L68
	beq	$2,$8,$L68
	.set	noreorder
	.set	nomacro
	bne	$2,$7,$L71
	slt	$2,$16,63
	.set	macro
	.set	reorder

$L68:
	lui	$2,%hi(namebuf) # high
	addiu	$2,$2,%lo(namebuf) # low
	addu	$2,$16,$2
	sb	$0,0($2)
	li	$2,1			# 0x00000001
$L72:
	lw	$31,20($sp)
	lw	$16,16($sp)
	addu	$sp,$sp,24
	j	$31
	.end	next_name
	.rdata
	.align	2
$LC0:
	.ascii	"cadence: usage <vectors>\n\000"
	.align	2
$LC1:
	.ascii	"cadence: cannot open fixture\n\000"
	.align	2
$LC2:
	.ascii	"cadence: short row\n\000"
	.align	2
$LC3:
	.ascii	"cadence: row too large\n\000"
	.align	2
$LC4:
	.ascii	"cadence: short expectation list\n\000"
	.align	2
$LC5:
	.ascii	": \000"
	.align	2
$LC6:
	.ascii	" shots, expected \000"
	.align	2
$LC7:
	.ascii	"\n\000"
	.align	2
$LC8:
	.ascii	": shot \000"
	.align	2
$LC9:
	.ascii	" at frame \000"
	.align	2
$LC10:
	.ascii	" expected \000"
	.align	2
$LC11:
	.ascii	"ALL PASS (cadence parity: \000"
	.align	2
$LC12:
	.ascii	")\n\000"
	.align	2
$LC13:
	.ascii	"FAILURES \000"
	.align	2
$LC14:
	.ascii	" of \000"
	.align	2
$LC15:
	.ascii	" cases\n\000"
	.text
	.align	2
	.globl	main
	.ent	main
main:
	.frame	$sp,768,$31		# vars= 720, regs= 8/0, args= 16, extra= 0
	.mask	0x807f0000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,768
	sw	$31,764($sp)
	sw	$22,760($sp)
	sw	$21,756($sp)
	sw	$20,752($sp)
	sw	$19,748($sp)
	sw	$18,744($sp)
	sw	$17,740($sp)
	sw	$16,736($sp)
	move	$16,$4
	.set	noreorder
	.set	nomacro
	jal	__main
	move	$17,$5
	.set	macro
	.set	reorder

	move	$21,$0
	li	$2,2			# 0x00000002
	.set	noreorder
	.set	nomacro
	beq	$16,$2,$L74
	move	$20,$21
	.set	macro
	.set	reorder

	lui	$4,%hi($LC0) # high
	.set	noreorder
	.set	nomacro
	j	$L111
	addiu	$4,$4,%lo($LC0) # low
	.set	macro
	.set	reorder

$L74:
	lw	$4,4($17)
	jal	load
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L75
	lui	$4,%hi($LC1) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L111
	addiu	$4,$4,%lo($LC1) # low
	.set	macro
	.set	reorder

$L75:
	lui	$2,%hi(p) # high
	lui	$3,%hi(data) # high
	addiu	$3,$3,%lo(data) # low
	sw	$3,%lo(p)($2)
	lui	$2,%hi(expect) # high
	addiu	$22,$2,%lo(expect) # low
$L76:
	jal	next_name
	beq	$2,$0,$L77
	.set	noreorder
	.set	nomacro
	jal	next_int
	addu	$4,$sp,704
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L113
	lui	$4,%hi($LC2) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	next_int
	addu	$4,$sp,708
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L113
	lui	$4,%hi($LC2) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	next_int
	addu	$4,$sp,712
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L113
	lui	$4,%hi($LC2) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	next_int
	addu	$4,$sp,716
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L113
	lui	$4,%hi($LC2) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	next_int
	addu	$4,$sp,720
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L113
	lui	$4,%hi($LC2) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	next_int
	addu	$4,$sp,724
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L113
	lui	$4,%hi($LC2) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	next_int
	addu	$4,$sp,728
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L79
	lui	$4,%hi($LC2) # high
	.set	macro
	.set	reorder

$L113:
	.set	noreorder
	.set	nomacro
	j	$L111
	addiu	$4,$4,%lo($LC2) # low
	.set	macro
	.set	reorder

$L110:
	addu	$20,$20,1
	lui	$4,%hi(namebuf) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo(namebuf) # low
	.set	macro
	.set	reorder

	lui	$4,%hi($LC8) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC8) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	num
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$4,%hi($LC9) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC9) # low
	.set	macro
	.set	reorder

	lw	$4,0($17)
	jal	num
	lui	$4,%hi($LC10) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC10) # low
	.set	macro
	.set	reorder

	lw	$4,0($18)
	j	$L112
$L79:
	lw	$2,728($sp)
	#nop
	slt	$2,$2,129
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L114
	lui	$4,%hi($LC3) # high
	.set	macro
	.set	reorder

	lw	$2,724($sp)
	#nop
	slt	$2,$2,4097
	bne	$2,$0,$L81
$L114:
	.set	noreorder
	.set	nomacro
	j	$L111
	addiu	$4,$4,%lo($LC3) # low
	.set	macro
	.set	reorder

$L81:
	lw	$2,728($sp)
	#nop
	.set	noreorder
	.set	nomacro
	blez	$2,$L84
	move	$16,$0
	.set	macro
	.set	reorder

	sll	$4,$16,2
$L115:
	.set	noreorder
	.set	nomacro
	jal	next_int
	addu	$4,$4,$22
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L109
	addu	$16,$16,1
	.set	macro
	.set	reorder

	lw	$2,728($sp)
	#nop
	slt	$2,$16,$2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L115
	sll	$4,$16,2
	.set	macro
	.set	reorder

$L84:
	lui	$16,%hi(record) # high
	addiu	$4,$16,%lo(record) # low
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,84			# 0x00000054
	.set	macro
	.set	reorder

	addiu	$16,$16,%lo(record) # low
	lbu	$2,716($sp)
	#nop
	sb	$2,10($16)
	li	$2,3			# 0x00000003
	sb	$2,4($16)
	lbu	$2,704($sp)
	#nop
	sb	$2,10($16)
	lhu	$2,708($sp)
	#nop
	sh	$2,12($16)
	lhu	$2,712($sp)
	#nop
	sh	$2,14($16)
	addu	$4,$sp,504
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,196			# 0x000000c4
	.set	macro
	.set	reorder

	addu	$4,$sp,192
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,132			# 0x00000084
	.set	macro
	.set	reorder

	addu	$4,$sp,16
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,176			# 0x000000b0
	.set	macro
	.set	reorder

	move	$16,$0
	addu	$4,$sp,16
	li	$6,100			# 0x00000064
	addu	$5,$sp,651
	sll	$2,$16,2
$L116:
	addu	$2,$2,$4
	sw	$0,608($2)
	sll	$2,$16,1
	addu	$2,$2,$4
	sh	$6,620($2)
	addu	$3,$5,$16
	lbu	$2,716($sp)
	#nop
	sb	$2,0($3)
	addu	$16,$16,1
	slt	$2,$16,3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L116
	sll	$2,$16,2
	.set	macro
	.set	reorder

	addu	$2,$sp,192
	sw	$2,188($sp)
	addu	$2,$sp,504
	sw	$2,500($sp)
	addu	$21,$21,1
	move	$17,$0
	lw	$2,724($sp)
	#nop
	.set	noreorder
	.set	nomacro
	blez	$2,$L95
	move	$16,$17
	.set	macro
	.set	reorder

	lui	$18,%hi(fired) # high
	lui	$2,%hi(hits) # high
	addiu	$19,$2,%lo(hits) # low
$L97:
	sw	$0,%lo(fired)($18)
	lw	$5,720($sp)
	.set	noreorder
	.set	nomacro
	jal	func_8006C3E0
	addu	$4,$sp,16
	.set	macro
	.set	reorder

	lw	$2,312($sp)
	#nop
	sw	$2,624($sp)
	addu	$4,$sp,328
	.set	noreorder
	.set	nomacro
	jal	func_8006BA50
	move	$5,$0
	.set	macro
	.set	reorder

	lw	$2,624($sp)
	#nop
	sw	$2,312($sp)
	lw	$2,%lo(fired)($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L96
	slt	$2,$17,128
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L96
	sll	$2,$17,2
	.set	macro
	.set	reorder

	addu	$2,$2,$19
	sw	$16,0($2)
	addu	$17,$17,1
$L96:
	addu	$16,$16,1
	lw	$2,724($sp)
	#nop
	slt	$2,$16,$2
	bne	$2,$0,$L97
$L95:
	lw	$2,728($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$17,$2,$L118
	lui	$4,%hi(namebuf) # high
	.set	macro
	.set	reorder

	addu	$20,$20,1
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo(namebuf) # low
	.set	macro
	.set	reorder

	lui	$4,%hi($LC5) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC5) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	num
	move	$4,$17
	.set	macro
	.set	reorder

	lui	$4,%hi($LC6) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC6) # low
	.set	macro
	.set	reorder

	lw	$4,728($sp)
$L112:
	jal	num
	lui	$4,%hi($LC7) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC7) # low
	.set	macro
	.set	reorder

	j	$L76
$L118:
	.set	noreorder
	.set	nomacro
	blez	$2,$L76
	move	$16,$0
	.set	macro
	.set	reorder

	lui	$2,%hi(hits) # high
	addiu	$5,$2,%lo(hits) # low
	lw	$4,728($sp)
	sll	$2,$16,2
$L117:
	addu	$17,$2,$5
	addu	$18,$2,$22
	lw	$3,0($17)
	lw	$2,0($18)
	#nop
	bne	$3,$2,$L110
	addu	$16,$16,1
	slt	$2,$16,$4
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L117
	sll	$2,$16,2
	.set	macro
	.set	reorder

	j	$L76
$L77:
	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L107
	lui	$4,%hi($LC11) # high
	.set	macro
	.set	reorder

	lui	$4,%hi($LC13) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC13) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	num
	move	$4,$20
	.set	macro
	.set	reorder

	lui	$4,%hi($LC14) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC14) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	num
	move	$4,$21
	.set	macro
	.set	reorder

	lui	$4,%hi($LC15) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC15) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L108
	li	$2,1			# 0x00000001
	.set	macro
	.set	reorder

$L109:
	lui	$4,%hi($LC4) # high
	addiu	$4,$4,%lo($LC4) # low
$L111:
	jal	say
	.set	noreorder
	.set	nomacro
	j	$L108
	li	$2,2			# 0x00000002
	.set	macro
	.set	reorder

$L107:
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC11) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	num
	move	$4,$21
	.set	macro
	.set	reorder

	lui	$4,%hi($LC12) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC12) # low
	.set	macro
	.set	reorder

	move	$2,$0
$L108:
	lw	$31,764($sp)
	lw	$22,760($sp)
	lw	$21,756($sp)
	lw	$20,752($sp)
	lw	$19,748($sp)
	lw	$18,744($sp)
	lw	$17,740($sp)
	lw	$16,736($sp)
	addu	$sp,$sp,768
	j	$31
	.end	main

	.comm	D_8009FA8C,8

	.lcomm	record,84

	.lcomm	fired,4

	.lcomm	data,16384

	.lcomm	namebuf,64

	.lcomm	expect,512

	.lcomm	hits,512

	.lcomm	p,4
