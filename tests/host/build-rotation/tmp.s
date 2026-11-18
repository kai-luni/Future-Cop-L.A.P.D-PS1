	.file	1 "/home/kai-luni/git/future-cop-lapd-ps1/tests/host/rotation_harness.c"

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
	beq	$4,$0,$L4
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L7
	addu	$2,$2,-48
	.set	macro
	.set	reorder

$L4:
	addu	$2,$3,-97
	andi	$2,$2,0x00ff
	sltu	$2,$2,6
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L5
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L7
	addu	$2,$2,-87
	.set	macro
	.set	reorder

$L5:
	addu	$2,$3,-65
	andi	$2,$2,0x00ff
	sltu	$2,$2,6
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L6
	andi	$2,$3,0x00ff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L7
	li	$2,-1			# 0xffffffff
	.set	macro
	.set	reorder

$L6:
	addu	$2,$2,-55
$L7:
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
	beq	$2,$0,$L10
	move	$7,$0
	.set	macro
	.set	reorder

	addu	$7,$7,1
$L13:
	addu	$2,$6,$7
	lbu	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L13
	addu	$7,$7,1
	.set	macro
	.set	reorder

	addu	$7,$7,-1
$L10:
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

	.lcomm	data.10,32768
	.rdata
	.align	2
$LC0:
	.ascii	"rotation: missing fixture\n\000"
	.align	2
$LC1:
	.ascii	"rotation: cannot open fixture\n\000"
	.align	2
$LC2:
	.ascii	"rotation: read error\n\000"
	.align	2
$LC3:
	.ascii	"rotation: malformed fixture\n\000"
	.align	2
$LC4:
	.ascii	"rotation: extra fixture data\n\000"
	.align	2
$LC5:
	.ascii	"rotation: FAIL\n\000"
	.align	2
$LC6:
	.ascii	"rotation: all shared vectors PASS\n\000"
	.text
	.align	2
	.globl	main
	.ent	main
main:
	.frame	$sp,152,$31		# vars= 96, regs= 10/0, args= 16, extra= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,152
	sw	$31,148($sp)
	sw	$fp,144($sp)
	sw	$23,140($sp)
	sw	$22,136($sp)
	sw	$21,132($sp)
	sw	$20,128($sp)
	sw	$19,124($sp)
	sw	$18,120($sp)
	sw	$17,116($sp)
	sw	$16,112($sp)
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
	beq	$16,$2,$L15
	lui	$4,%hi($LC0) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L65
	addiu	$4,$4,%lo($LC0) # low
	.set	macro
	.set	reorder

$L15:
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
	bgez	$16,$L16
	move	$5,$16
	.set	macro
	.set	reorder

	lui	$4,%hi($LC1) # high
	.set	noreorder
	.set	nomacro
	j	$L65
	addiu	$4,$4,%lo($LC1) # low
	.set	macro
	.set	reorder

$L16:
	li	$4,4003			# 0x00000fa3
	lui	$6,%hi(data.10) # high
	addiu	$6,$6,%lo(data.10) # low
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
	bgez	$17,$L17
	lui	$4,%hi($LC2) # high
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L65
	addiu	$4,$4,%lo($LC2) # low
	.set	macro
	.set	reorder

$L17:
	lui	$4,%hi(data.10) # high
	addiu	$3,$4,%lo(data.10) # low
	addu	$2,$17,$3
	sb	$0,0($2)
	move	$16,$3
	sw	$0,104($sp)
	lbu	$2,%lo(data.10)($4)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L19
	move	$23,$0
	.set	macro
	.set	reorder

	li	$21,32			# 0x00000020
	li	$20,9			# 0x00000009
	li	$fp,10			# 0x0000000a
	addu	$22,$sp,16
$L66:
	lbu	$2,0($16)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$21,$L66
	addu	$16,$16,1
	.set	macro
	.set	reorder

	addu	$16,$16,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$20,$L66
	addu	$16,$16,1
	.set	macro
	.set	reorder

	addu	$16,$16,-1
	li	$8,13			# 0x0000000d
	.set	noreorder
	.set	nomacro
	beq	$2,$8,$L66
	addu	$16,$16,1
	.set	macro
	.set	reorder

	addu	$16,$16,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$fp,$L66
	addu	$16,$16,1
	.set	macro
	.set	reorder

	addu	$16,$16,-1
	lbu	$3,0($16)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L19
	li	$2,35			# 0x00000023
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L26
	move	$17,$0
	.set	macro
	.set	reorder

	lbu	$2,0($16)
$L69:
	beq	$2,$fp,$L18
	addu	$16,$16,1
	lbu	$2,0($16)
	#nop
	beq	$2,$0,$L19
	j	$L69
$L26:
$L67:
	lbu	$2,0($16)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$21,$L67
	addu	$16,$16,1
	.set	macro
	.set	reorder

	addu	$16,$16,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$20,$L67
	addu	$16,$16,1
	.set	macro
	.set	reorder

	addu	$16,$16,-1
	sll	$2,$17,2
	addu	$2,$2,$22
	sw	$0,0($2)
	move	$18,$0
	sll	$2,$17,2
	addu	$19,$2,$22
$L43:
	lbu	$4,0($16)
	.set	noreorder
	.set	nomacro
	jal	hex
	addu	$16,$16,1
	.set	macro
	.set	reorder

	move	$3,$2
	.set	noreorder
	.set	nomacro
	bltz	$3,$L63
	addu	$18,$18,1
	.set	macro
	.set	reorder

	lw	$2,0($19)
	#nop
	sll	$2,$2,4
	or	$2,$2,$3
	sw	$2,0($19)
	slt	$2,$18,8
	bne	$2,$0,$L43
	addu	$17,$17,1
	slt	$2,$17,12
	bne	$2,$0,$L67
$L68:
	lbu	$2,0($16)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$21,$L68
	addu	$16,$16,1
	.set	macro
	.set	reorder

	addu	$16,$16,-1
	.set	noreorder
	.set	nomacro
	beq	$2,$20,$L68
	addu	$16,$16,1
	.set	macro
	.set	reorder

	addu	$16,$16,-1
	li	$8,13			# 0x0000000d
	.set	noreorder
	.set	nomacro
	beq	$2,$8,$L68
	addu	$16,$16,1
	.set	macro
	.set	reorder

	addu	$16,$16,-1
	lbu	$2,0($16)
	#nop
	beq	$2,$0,$L51
	.set	noreorder
	.set	nomacro
	bne	$2,$fp,$L64
	lui	$4,%hi($LC4) # high
	.set	macro
	.set	reorder

$L51:
	lw	$4,16($sp)
	lw	$5,20($sp)
	lw	$6,24($sp)
	.set	noreorder
	.set	nomacro
	jal	func_80018DC0
	addu	$7,$sp,64
	.set	macro
	.set	reorder

	move	$17,$0
	sll	$2,$17,2
$L70:
	addu	$2,$2,$22
	lw	$3,48($2)
	lw	$2,12($2)
	#nop
	beq	$3,$2,$L54
	.set	noreorder
	.set	nomacro
	j	$L53
	addu	$23,$23,1
	.set	macro
	.set	reorder

$L54:
	addu	$17,$17,1
	slt	$2,$17,9
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L70
	sll	$2,$17,2
	.set	macro
	.set	reorder

$L53:
	lw	$8,104($sp)
	#nop
	addu	$8,$8,1
	sw	$8,104($sp)
$L18:
	lbu	$2,0($16)
	#nop
	bne	$2,$0,$L66
$L19:
	lw	$8,104($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$8,$0,$L71
	lui	$4,%hi($LC5) # high
	.set	macro
	.set	reorder

	beq	$23,$0,$L59
$L71:
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC5) # low
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L61
	li	$2,1			# 0x00000001
	.set	macro
	.set	reorder

$L63:
	lui	$4,%hi($LC3) # high
	.set	noreorder
	.set	nomacro
	j	$L65
	addiu	$4,$4,%lo($LC3) # low
	.set	macro
	.set	reorder

$L64:
	addiu	$4,$4,%lo($LC4) # low
$L65:
	jal	say
	.set	noreorder
	.set	nomacro
	j	$L61
	li	$2,2			# 0x00000002
	.set	macro
	.set	reorder

$L59:
	lui	$4,%hi($LC6) # high
	.set	noreorder
	.set	nomacro
	jal	say
	addiu	$4,$4,%lo($LC6) # low
	.set	macro
	.set	reorder

	move	$2,$0
$L61:
	lw	$31,148($sp)
	lw	$fp,144($sp)
	lw	$23,140($sp)
	lw	$22,136($sp)
	lw	$21,132($sp)
	lw	$20,128($sp)
	lw	$19,124($sp)
	lw	$18,120($sp)
	lw	$17,116($sp)
	lw	$16,112($sp)
	addu	$sp,$sp,152
	j	$31
	.end	main
