	.file	1 "/home/kai-luni/git/future-cop-lapd-ps1/tests/host/shot_harness.c"

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
	.globl	D_80036624
	.ent	D_80036624
D_80036624:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	j	$31
	.end	D_80036624
	.align	2
	.globl	func_8004D444
	.ent	func_8004D444
func_8004D444:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lui	$2,%hi(recorded) # high
	addiu	$7,$2,%lo(recorded) # low
	lw	$6,8($4)
	#nop
	addu	$8,$6,96
$L3:
	lw	$2,0($6)
	lw	$3,4($6)
	lw	$4,8($6)
	lw	$5,12($6)
	sw	$2,0($7)
	sw	$3,4($7)
	sw	$4,8($7)
	sw	$5,12($7)
	addu	$6,$6,16
	.set	noreorder
	.set	nomacro
	bne	$6,$8,$L3
	addu	$7,$7,16
	.set	macro
	.set	reorder

	lui	$3,%hi(recorded_flag) # high
	li	$2,1			# 0x00000001
	sw	$2,%lo(recorded_flag)($3)
	.set	noreorder
	.set	nomacro
	j	$31
	move	$2,$0
	.set	macro
	.set	reorder

	.end	func_8004D444
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
	beq	$6,$0,$L7
	addu	$3,$6,-1
	.set	macro
	.set	reorder

	li	$6,-1			# 0xffffffff
$L8:
	lbu	$2,0($5)
	#nop
	sb	$2,0($7)
	addu	$5,$5,1
	addu	$3,$3,-1
	.set	noreorder
	.set	nomacro
	bne	$3,$6,$L8
	addu	$7,$7,1
	.set	macro
	.set	reorder

$L7:
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
	beq	$6,$0,$L12
	addu	$3,$6,-1
	.set	macro
	.set	reorder

	li	$6,-1			# 0xffffffff
$L13:
	sb	$5,0($7)
	addu	$3,$3,-1
	.set	noreorder
	.set	nomacro
	bne	$3,$6,$L13
	addu	$7,$7,1
	.set	macro
	.set	reorder

$L12:
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
	beq	$2,$0,$L18
	move	$7,$0
	.set	macro
	.set	reorder

	addu	$7,$7,1
$L21:
	addu	$2,$6,$7
	lbu	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L21
	addu	$7,$7,1
	.set	macro
	.set	reorder

	addu	$7,$7,-1
$L18:
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
	beq	$8,$0,$L23
	move	$7,$0
	.set	macro
	.set	reorder

	subu	$5,$0,$5
$L23:
	.set	noreorder
	.set	nomacro
	bne	$5,$0,$L24
	addu	$3,$sp,$7
	.set	macro
	.set	reorder

	li	$2,48			# 0x00000030
	sb	$2,16($3)
	addu	$7,$7,1
$L24:
	blez	$5,$L26
	addu	$6,$sp,16
	li	$9,1717960704			# 0x66660000
	ori	$9,$9,0x6667
$L27:
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
	bgtz	$5,$L27
	addu	$7,$7,1
	.set	macro
	.set	reorder

$L26:
	.set	noreorder
	.set	nomacro
	beq	$8,$0,$L29
	addu	$3,$sp,$7
	.set	macro
	.set	reorder

	li	$2,45			# 0x0000002d
	sb	$2,16($3)
	addu	$7,$7,1
$L29:
	sra	$2,$7,31
	move	$3,$2
	subu	$2,$7,$2
	sra	$2,$2,1
	.set	noreorder
	.set	nomacro
	blez	$2,$L31
	move	$5,$0
	.set	macro
	.set	reorder

	addu	$8,$sp,16
	subu	$2,$7,$3
	sra	$9,$2,1
$L32:
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
	bne	$2,$0,$L32
	sb	$6,0($3)
	.set	macro
	.set	reorder

$L31:
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
	beq	$4,$0,$L35
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L38
	addu	$2,$2,-48
	.set	macro
	.set	reorder

$L35:
	addu	$2,$3,-97
	andi	$2,$2,0x00ff
	sltu	$2,$2,6
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L36
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L38
	addu	$2,$2,-87
	.set	macro
	.set	reorder

$L36:
	addu	$2,$3,-65
	andi	$2,$2,0x00ff
	sltu	$2,$2,6
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L37
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L38
	li	$2,-1			# 0xffffffff
	.set	macro
	.set	reorder

$L37:
	addu	$2,$2,-55
$L38:
	j	$31
	.end	hex
	.align	2
	.ent	next_word
next_word:
	.frame	$sp,40,$31		# vars= 0, regs= 5/0, args= 16, extra= 0
	.mask	0x800f0000,-8
	.fmask	0x00000000,0
	subu	$sp,$sp,40
	sw	$31,32($sp)
	sw	$19,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	move	$18,$4
	move	$19,$5
	lw	$16,0($18)
	li	$6,32			# 0x00000020
	li	$5,9			# 0x00000009
	li	$4,13			# 0x0000000d
	li	$3,10			# 0x0000000a
	li	$7,35			# 0x00000023
$L58:
	lbu	$2,0($16)
$L60:
	.set	noreorder
	.set	nomacro
	beq	$2,$6,$L58
	addu	$16,$16,1
	.set	macro
	.set	reorder

	addu	$16,$16,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$5,$L58
	addu	$16,$16,1
	.set	macro
	.set	reorder

	addu	$16,$16,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$4,$L58
	addu	$16,$16,1
	.set	macro
	.set	reorder

	addu	$16,$16,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L58
	addu	$16,$16,1
	.set	macro
	.set	reorder

	addu	$16,$16,-1
	lbu	$2,0($16)
	#nop
	bne	$2,$7,$L41
$L59:
	beq	$2,$3,$L58
	addu	$16,$16,1
	lbu	$2,0($16)
	#nop
	beq	$2,$0,$L60
	j	$L59
$L41:
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L53
	move	$17,$0
	.set	macro
	.set	reorder

	sw	$16,0($18)
	.set	noreorder
	.set	nomacro
	j	$L57
	move	$2,$0
	.set	macro
	.set	reorder

$L53:
$L54:
	lbu	$4,0($16)
	jal	hex
	move	$3,$2
	.set	noreorder
	.set	nomacro
	bltz	$3,$L55
	sll	$2,$17,4
	.set	macro
	.set	reorder

	or	$17,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L54
	addu	$16,$16,1
	.set	macro
	.set	reorder

$L55:
	sw	$16,0($18)
	sw	$17,0($19)
	li	$2,1			# 0x00000001
$L57:
	lw	$31,32($sp)
	lw	$19,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	addu	$sp,$sp,40
	j	$31
	.end	next_word
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
	bltz	$16,$L63
	li	$4,4003			# 0x00000fa3
	.set	macro
	.set	reorder

	move	$5,$16
	lui	$18,%hi(data) # high
	addiu	$6,$18,%lo(data) # low
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
	bltz	$17,$L63
	addiu	$2,$18,%lo(data) # low
	.set	macro
	.set	reorder

	addu	$2,$17,$2
	sb	$0,0($2)
	.set	noreorder
	.set	nomacro
	j	$L64
	move	$2,$0
	.set	macro
	.set	reorder

$L63:
	li	$2,-1			# 0xffffffff
$L64:
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	addu	$sp,$sp,32
	j	$31
	.end	load
	.rdata
	.align	2
$LC0:
	.ascii	"ALL PASS (\000"
	.align	2
$LC1:
	.ascii	" parity: \000"
	.align	2
$LC2:
	.ascii	")\n\000"
	.align	2
$LC3:
	.ascii	"FAILURES \000"
	.align	2
$LC4:
	.ascii	" of \000"
	.align	2
$LC5:
	.ascii	" (\000"
	.text
	.align	2
	.ent	report
report:
	.frame	$sp,32,$31		# vars= 0, regs= 4/0, args= 16, extra= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,32
	sw	$31,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	move	$18,$4
	move	$16,$6
	.set	noreorder
	.set	nomacro
	bne	$16,$0,$L66
	move	$17,$5
	.set	macro
	.set	reorder

	lui	$4,%hi($LC0) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC0) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	say
	move	$4,$18
	.set	macro
	.set	reorder

	lui	$4,%hi($LC1) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC1) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	num
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L69
	lui	$4,%hi($LC2) # high
	.set	macro
	.set	reorder

$L66:
	lui	$4,%hi($LC3) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC3) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	num
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$4,%hi($LC4) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC4) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	num
	move	$4,$17
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
	jal	say
	move	$4,$18
	.set	macro
	.set	reorder

	lui	$4,%hi($LC2) # high
$L69:
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC2) # low
	.set	macro
	.set	reorder

	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	addu	$sp,$sp,32
	j	$31
	.end	report
	.rdata
	.align	2
$LC6:
	.ascii	"shot: cannot open spawn fixture\n\000"
	.align	2
$LC7:
	.ascii	"shot: short row\n\000"
	.align	2
$LC8:
	.ascii	"case \000"
	.align	2
$LC9:
	.ascii	": spawner did not call the queue\n\000"
	.align	2
$LC10:
	.ascii	": field \000"
	.align	2
$LC11:
	.ascii	" got \000"
	.align	2
$LC12:
	.ascii	" expected \000"
	.align	2
$LC13:
	.ascii	"\n\000"
	.align	2
$LC14:
	.ascii	": position not copied\n\000"
	.align	2
$LC15:
	.ascii	"shot-spawn\000"
	.text
	.align	2
	.ent	run_spawn
run_spawn:
	.frame	$sp,464,$31		# vars= 376, regs= 10/0, args= 48, extra= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,464
	sw	$31,460($sp)
	sw	$fp,456($sp)
	sw	$23,452($sp)
	sw	$22,448($sp)
	sw	$21,444($sp)
	sw	$20,440($sp)
	sw	$19,436($sp)
	sw	$18,432($sp)
	sw	$17,428($sp)
	sw	$16,424($sp)
	lui	$2,%hi(data) # high
	addiu	$2,$2,%lo(data) # low
	sw	$2,208($sp)
	move	$22,$0
	.set	noreorder
	.set	nomacro
	jal	load
	move	$19,$22
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L71
	lui	$4,%hi($LC6) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L99
	addiu	$4,$4,%lo($LC6) # low
	.set	macro
	.set	reorder

$L98:
	lui	$4,%hi($LC7) # high
	addiu	$4,$4,%lo($LC7) # low
$L99:
	jal	say
	.set	noreorder
	.set	nomacro
	j	$L97
	li	$2,2			# 0x00000002
	.set	macro
	.set	reorder

$L71:
	addu	$20,$sp,48
	addu	$fp,$sp,216
	addu	$23,$sp,240
	addu	$21,$sp,280
	lui	$2,%hi(recorded) # high
	addiu	$18,$2,%lo(recorded) # low
$L72:
	addu	$4,$sp,208
$L100:
	.set	noreorder
	.set	nomacro
	jal	next_word
	addu	$5,$sp,48
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L73
	lui	$8,%hi(recorded) # high
	.set	macro
	.set	reorder

	addiu	$17,$8,%lo(recorded) # low
	li	$16,1			# 0x00000001
	sll	$5,$16,2
$L101:
	addu	$4,$sp,208
	.set	noreorder
	.set	nomacro
	jal	next_word
	addu	$5,$20,$5
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L98
	addu	$16,$16,1
	.set	macro
	.set	reorder

	slt	$2,$16,39
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L101
	sll	$5,$16,2
	.set	macro
	.set	reorder

	move	$4,$fp
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,20			# 0x00000014
	.set	macro
	.set	reorder

	move	$4,$23
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,40			# 0x00000028
	.set	macro
	.set	reorder

	lhu	$2,52($sp)
	#nop
	sh	$2,222($sp)
	lhu	$2,56($sp)
	#nop
	sh	$2,224($sp)
	lhu	$2,60($sp)
	#nop
	sh	$2,216($sp)
	lhu	$2,64($sp)
	#nop
	sh	$2,226($sp)
	lhu	$2,68($sp)
	#nop
	sh	$2,218($sp)
	lbu	$2,72($sp)
	#nop
	sb	$2,230($sp)
	lbu	$2,76($sp)
	#nop
	sb	$2,234($sp)
	lhu	$2,80($sp)
	#nop
	sh	$2,260($sp)
	lbu	$2,84($sp)
	#nop
	sb	$2,262($sp)
	lbu	$2,88($sp)
	#nop
	sb	$2,263($sp)
	lbu	$2,92($sp)
	#nop
	sb	$2,264($sp)
	lw	$2,96($sp)
	#nop
	sw	$2,276($sp)
	lw	$2,100($sp)
	#nop
	sw	$2,280($sp)
	lw	$2,104($sp)
	#nop
	sw	$2,284($sp)
	lw	$2,108($sp)
	#nop
	sw	$2,288($sp)
	lui	$2,%hi(recorded_flag) # high
	sw	$0,%lo(recorded_flag)($2)
	sw	$23,16($sp)
	lw	$2,136($sp)
	#nop
	beq	$2,$0,$L81
	.set	noreorder
	.set	nomacro
	j	$L82
	sw	$17,20($sp)
	.set	macro
	.set	reorder

$L81:
	sw	$0,20($sp)
$L82:
	lw	$2,112($sp)
	#nop
	sw	$2,24($sp)
	lbu	$2,116($sp)
	#nop
	sw	$2,28($sp)
	lbu	$2,120($sp)
	#nop
	sw	$2,32($sp)
	lhu	$2,124($sp)
	#nop
	sw	$2,36($sp)
	lbu	$2,128($sp)
	#nop
	sw	$2,40($sp)
	lw	$2,132($sp)
	#nop
	sw	$2,44($sp)
	move	$4,$fp
	move	$5,$21
	move	$6,$21
	.set	noreorder
	.set	nomacro
	jal	func_8005E47C
	move	$7,$21
	.set	macro
	.set	reorder

	lui	$2,%hi(recorded_flag) # high
	lw	$2,%lo(recorded_flag)($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L83
	addu	$22,$22,1
	.set	macro
	.set	reorder

	addu	$19,$19,1
	lui	$4,%hi($LC8) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC8) # low
	.set	macro
	.set	reorder

	lw	$4,48($sp)
	jal	num
	lui	$4,%hi($LC9) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC9) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L100
	addu	$4,$sp,208
	.set	macro
	.set	reorder

$L83:
	lui	$8,%hi(recorded) # high
	lw	$2,%lo(recorded)($8)
	#nop
	sw	$2,296($sp)
	lw	$2,20($18)
	#nop
	sw	$2,300($sp)
	lhu	$2,24($18)
	#nop
	sw	$2,304($sp)
	lhu	$2,26($18)
	#nop
	sw	$2,308($sp)
	lw	$2,44($18)
	#nop
	sw	$2,312($sp)
	lw	$2,60($18)
	#nop
	sw	$2,316($sp)
	lw	$2,68($18)
	#nop
	sw	$2,320($sp)
	lw	$2,64($18)
	#nop
	sw	$2,324($sp)
	lhu	$2,72($18)
	#nop
	sw	$2,328($sp)
	lh	$2,74($18)
	#nop
	sw	$2,332($sp)
	lbu	$2,76($18)
	#nop
	sw	$2,336($sp)
	lbu	$2,77($18)
	#nop
	sw	$2,340($sp)
	lhu	$2,78($18)
	#nop
	sw	$2,344($sp)
	lbu	$2,80($18)
	#nop
	sw	$2,348($sp)
	lbu	$2,81($18)
	#nop
	sw	$2,352($sp)
	lw	$2,84($18)
	#nop
	sw	$2,356($sp)
	move	$16,$0
	sll	$2,$16,2
$L102:
	addu	$2,$2,$20
	lw	$3,92($2)
	#nop
	sw	$3,312($2)
	addu	$16,$16,1
	slt	$2,$16,16
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L102
	sll	$2,$16,2
	.set	macro
	.set	reorder

	move	$16,$0
	sll	$2,$16,2
$L103:
	addu	$17,$2,$20
	lw	$3,248($17)
	lw	$2,312($17)
	#nop
	beq	$3,$2,$L91
	addu	$19,$19,1
	lui	$4,%hi($LC8) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC8) # low
	.set	macro
	.set	reorder

	lw	$4,48($sp)
	jal	num
	lui	$4,%hi($LC10) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC10) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	num
	addu	$4,$16,16
	.set	macro
	.set	reorder

	lui	$4,%hi($LC11) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC11) # low
	.set	macro
	.set	reorder

	lw	$4,248($17)
	jal	num
	lui	$4,%hi($LC12) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC12) # low
	.set	macro
	.set	reorder

	lw	$4,312($17)
	jal	num
	lui	$4,%hi($LC13) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC13) # low
	.set	macro
	.set	reorder

$L91:
	addu	$16,$16,1
	slt	$2,$16,16
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L103
	sll	$2,$16,2
	.set	macro
	.set	reorder

	lw	$3,8($18)
	lw	$2,280($sp)
	#nop
	bne	$3,$2,$L96
	lw	$3,12($18)
	lw	$2,284($sp)
	#nop
	bne	$3,$2,$L96
	lw	$3,16($18)
	lw	$2,288($sp)
	#nop
	beq	$3,$2,$L72
$L96:
	addu	$19,$19,1
	lui	$4,%hi($LC8) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC8) # low
	.set	macro
	.set	reorder

	lw	$4,48($sp)
	jal	num
	lui	$4,%hi($LC14) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC14) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L100
	addu	$4,$sp,208
	.set	macro
	.set	reorder

$L73:
	lui	$4,%hi($LC15) # high
	addiu	$4,$4,%lo($LC15) # low
	move	$5,$22
	.set	noreorder
	.set	nomacro
	jal	report
	move	$6,$19
	.set	macro
	.set	reorder

	sltu	$2,$0,$19
$L97:
	lw	$31,460($sp)
	lw	$fp,456($sp)
	lw	$23,452($sp)
	lw	$22,448($sp)
	lw	$21,444($sp)
	lw	$20,440($sp)
	lw	$19,436($sp)
	lw	$18,432($sp)
	lw	$17,428($sp)
	lw	$16,424($sp)
	addu	$sp,$sp,464
	j	$31
	.end	run_spawn
	.rdata
	.align	2
$LC16:
	.ascii	"shot: cannot open move fixture\n\000"
	.align	2
$LC17:
	.ascii	"shot: short move row\n\000"
	.align	2
$LC18:
	.ascii	"move case \000"
	.align	2
$LC19:
	.ascii	": component \000"
	.align	2
$LC20:
	.ascii	"shot-move\000"
	.text
	.align	2
	.ent	run_move
run_move:
	.frame	$sp,128,$31		# vars= 88, regs= 6/0, args= 16, extra= 0
	.mask	0x801f0000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,128
	sw	$31,124($sp)
	sw	$20,120($sp)
	sw	$19,116($sp)
	sw	$18,112($sp)
	sw	$17,108($sp)
	sw	$16,104($sp)
	lui	$2,%hi(data) # high
	addiu	$2,$2,%lo(data) # low
	sw	$2,64($sp)
	move	$20,$0
	.set	noreorder
	.set	nomacro
	jal	load
	move	$18,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L105
	lui	$4,%hi($LC16) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L123
	addiu	$4,$4,%lo($LC16) # low
	.set	macro
	.set	reorder

$L122:
	lui	$4,%hi($LC17) # high
	addiu	$4,$4,%lo($LC17) # low
$L123:
	jal	say
	.set	noreorder
	.set	nomacro
	j	$L121
	li	$2,2			# 0x00000002
	.set	macro
	.set	reorder

$L105:
	addu	$19,$sp,16
	addu	$4,$sp,64
$L124:
	.set	noreorder
	.set	nomacro
	jal	next_word
	addu	$5,$sp,16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L107
	li	$16,1			# 0x00000001
	.set	macro
	.set	reorder

	sll	$5,$16,2
$L125:
	addu	$4,$sp,64
	.set	noreorder
	.set	nomacro
	jal	next_word
	addu	$5,$19,$5
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L122
	addu	$16,$16,1
	.set	macro
	.set	reorder

	slt	$2,$16,11
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L125
	sll	$5,$16,2
	.set	macro
	.set	reorder

	lw	$2,20($sp)
	#nop
	sw	$2,72($sp)
	lw	$2,24($sp)
	#nop
	sw	$2,76($sp)
	lw	$2,28($sp)
	#nop
	sw	$2,80($sp)
	lw	$2,32($sp)
	#nop
	sw	$2,88($sp)
	lw	$2,36($sp)
	#nop
	sw	$2,92($sp)
	lw	$2,40($sp)
	#nop
	sw	$2,96($sp)
	addu	$7,$sp,72
	move	$4,$7
	lw	$6,44($sp)
	.set	noreorder
	.set	nomacro
	jal	func_8001ACA4
	addu	$5,$sp,88
	.set	macro
	.set	reorder

	addu	$20,$20,1
	move	$16,$0
	sll	$2,$16,2
$L126:
	addu	$17,$2,$19
	lw	$3,56($17)
	lw	$2,32($17)
	#nop
	beq	$3,$2,$L117
	addu	$18,$18,1
	lui	$4,%hi($LC18) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC18) # low
	.set	macro
	.set	reorder

	lw	$4,16($sp)
	jal	num
	lui	$4,%hi($LC19) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC19) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	num
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$4,%hi($LC11) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC11) # low
	.set	macro
	.set	reorder

	lw	$4,56($17)
	jal	num
	lui	$4,%hi($LC12) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC12) # low
	.set	macro
	.set	reorder

	lw	$4,32($17)
	jal	num
	lui	$4,%hi($LC13) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC13) # low
	.set	macro
	.set	reorder

$L117:
	addu	$16,$16,1
	slt	$2,$16,3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L126
	sll	$2,$16,2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L124
	addu	$4,$sp,64
	.set	macro
	.set	reorder

$L107:
	lui	$4,%hi($LC20) # high
	addiu	$4,$4,%lo($LC20) # low
	move	$5,$20
	.set	noreorder
	.set	nomacro
	jal	report
	move	$6,$18
	.set	macro
	.set	reorder

	sltu	$2,$0,$18
$L121:
	lw	$31,124($sp)
	lw	$20,120($sp)
	lw	$19,116($sp)
	lw	$18,112($sp)
	lw	$17,108($sp)
	lw	$16,104($sp)
	addu	$sp,$sp,128
	j	$31
	.end	run_move
	.rdata
	.align	2
$LC21:
	.ascii	"shot: usage <spawn-vectors> <move-vectors>\n\000"
	.text
	.align	2
	.globl	main
	.ent	main
main:
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, extra= 0
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	subu	$sp,$sp,32
	sw	$31,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	move	$16,$4
	.set	noreorder
	.set	nomacro
	jal	__main
	move	$17,$5
	.set	macro
	.set	reorder

	li	$2,3			# 0x00000003
	.set	noreorder
	.set	nomacro
	bne	$16,$2,$L128
	lui	$4,%hi($LC21) # high
	.set	macro
	.set	reorder

	lw	$4,4($17)
	jal	run_spawn
	lw	$4,8($17)
	.set	noreorder
	.set	nomacro
	jal	run_move
	move	$16,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L129
	or	$2,$16,$2
	.set	macro
	.set	reorder

$L128:
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC21) # low
	.set	macro
	.set	reorder

	li	$2,2			# 0x00000002
$L129:
	lw	$31,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	addu	$sp,$sp,32
	j	$31
	.end	main

	.lcomm	recorded,96

	.lcomm	recorded_flag,4

	.lcomm	data,32768
