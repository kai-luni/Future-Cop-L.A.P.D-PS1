
.file	1 "/home/kai-luni/git/future-cop-lapd-ps1/tests/host/rotation_harness.c"


gcc2_compiled.:
__gnu_compiled_c:
.text
.align	2
.globl	__main
.ent	__main
.set	noreorder
__main:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
j	$31
nop  # DEBUG: branch/jump
.end	__main
.align	2
.ent	sys3
.set	noreorder
sys3:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
addu	$2,$4,$zero
addu	$4,$5,$zero
addu	$5,$6,$zero
addu	$6,$7,$zero
syscall
j	$31
nop  # DEBUG: branch/jump
.end	sys3
.align	2
.ent	hex
.set	noreorder
hex:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
addu	$3,$4,$zero
addu	$4,$4,-48
andi	$4,$4,0x00ff
sltu	$4,$4,10
beq	$4,$0,$L4
andi	$2,$3,0x00ff

j	$L7
addu	$2,$2,-48

$L4:
addu	$2,$3,-97
andi	$2,$2,0x00ff
sltu	$2,$2,6
beq	$2,$0,$L5
andi	$2,$3,0x00ff

j	$L7
addu	$2,$2,-87

$L5:
addu	$2,$3,-65
andi	$2,$2,0x00ff
sltu	$2,$2,6
bne	$2,$0,$L6
andi	$2,$3,0x00ff

j	$L7
addiu	$2,$zero,-1

$L6:
addu	$2,$2,-55
$L7:
j	$31
nop  # DEBUG: branch/jump
.end	hex
.align	2
.ent	say
.set	noreorder
say:
.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, extra= 0
.mask	0x80000000,-8
.fmask	0x00000000,0
subu	$sp,$sp,24
sw	$31,16($sp)
addu	$6,$4,$zero
lbu	$2,0($6)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L10' does not use $at
beq	$2,$0,$L10
addu	$7,$0,$zero

addu	$7,$7,1
$L13:
addu	$2,$6,$7
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L13' does not use $at
bne	$2,$0,$L13
addu	$7,$7,1

addu	$7,$7,-1
$L10:
ori	$4,$zero,4004
jal	sys3
ori	$5,$zero,1

lw	$31,16($sp)
#nop # DEBUG: 'addu	$sp,$sp,24' does not load from $31
addu	$sp,$sp,24
j	$31
nop  # DEBUG: branch/jump
.end	say

.section .rodata
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
.set	noreorder
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
addu	$16,$4,$zero
jal	__main
addu	$17,$5,$zero

ori	$2,$zero,2
beq	$16,$2,$L15
lui	$4,%hi($LC0) # high

j	$L65
addiu	$4,$4,%lo($LC0) # low

$L15:
ori	$4,$zero,4005
lw	$5,4($17)
#nop # DEBUG: 'move	$6,$0' does not load from $5
addu	$6,$0,$zero
jal	sys3
addu	$7,$6,$zero

addu	$16,$2,$zero
bgez	$16,$L16
addu	$5,$16,$zero

lui	$4,%hi($LC1) # high
j	$L65
addiu	$4,$4,%lo($LC1) # low

$L16:
ori	$4,$zero,4003
lui	$6,%hi(data.10) # high
addiu	$6,$6,%lo(data.10) # low
jal	sys3
ori	$7,$zero,32767

addu	$17,$2,$zero
ori	$4,$zero,4006
addu	$5,$16,$zero
addu	$6,$0,$zero
jal	sys3
addu	$7,$6,$zero

bgez	$17,$L17
lui	$4,%hi($LC2) # high

j	$L65
addiu	$4,$4,%lo($LC2) # low

$L17:
lui	$4,%hi(data.10) # high
addiu	$3,$4,%lo(data.10) # low
addu	$2,$17,$3
sb	$0,0($2)
addu	$16,$3,$zero
sw	$0,104($sp)
lbu	$2,%lo(data.10)($4) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L19' does not use $at
beq	$2,$0,$L19
addu	$23,$0,$zero

ori	$21,$zero,32
ori	$20,$zero,9
ori	$fp,$zero,10
addu	$22,$sp,16
$L66:
lbu	$2,0($16)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$21,$L66' does not use $at
beq	$2,$21,$L66
addu	$16,$16,1

addu	$16,$16,-1
beq	$2,$20,$L66
addu	$16,$16,1

addu	$16,$16,-1
ori	$8,$zero,13
beq	$2,$8,$L66
addu	$16,$16,1

addu	$16,$16,-1
beq	$2,$fp,$L66
addu	$16,$16,1

addu	$16,$16,-1
lbu	$3,0($16)
nop # DEBUG: Reuse of '$3'. 'beq	$3,$0,$L19' does not use $at
beq	$3,$0,$L19
ori	$2,$zero,35

bne	$3,$2,$L26
addu	$17,$0,$zero

lbu	$2,0($16)
$L69:
nop # DEBUG: Reuse of '$2'. 'beq	$2,$fp,$L18' does not use $at
# $L69:  # DEBUG: skipped
beq	$2,$fp,$L18
nop  # DEBUG: branch/jump
addu	$16,$16,1
lbu	$2,0($16)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L19' does not use $at
beq	$2,$0,$L19
nop  # DEBUG: branch/jump
j	$L69
nop  # DEBUG: branch/jump
$L26:
$L67:
lbu	$2,0($16)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$21,$L67' does not use $at
beq	$2,$21,$L67
addu	$16,$16,1

addu	$16,$16,-1
beq	$2,$20,$L67
addu	$16,$16,1

addu	$16,$16,-1
sll	$2,$17,2
addu	$2,$2,$22
sw	$0,0($2)
addu	$18,$0,$zero
sll	$2,$17,2
addu	$19,$2,$22
$L43:
lbu	$4,0($16)
#nop # DEBUG: 'jal	hex' does not load from $4
jal	hex
addu	$16,$16,1

addu	$3,$2,$zero
bltz	$3,$L63
addu	$18,$18,1

lw	$2,0($19)
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,4' does not use $at
sll	$2,$2,4
or	$2,$2,$3
sw	$2,0($19)
slt	$2,$18,8
bne	$2,$0,$L43
nop  # DEBUG: branch/jump
addu	$17,$17,1
slt	$2,$17,12
bne	$2,$0,$L67
nop  # DEBUG: branch/jump
$L68:
lbu	$2,0($16)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$21,$L68' does not use $at
beq	$2,$21,$L68
addu	$16,$16,1

addu	$16,$16,-1
beq	$2,$20,$L68
addu	$16,$16,1

addu	$16,$16,-1
ori	$8,$zero,13
beq	$2,$8,$L68
addu	$16,$16,1

addu	$16,$16,-1
lbu	$2,0($16)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L51' does not use $at
beq	$2,$0,$L51
nop  # DEBUG: branch/jump
bne	$2,$fp,$L64
lui	$4,%hi($LC4) # high

$L51:
lw	$4,16($sp)
#nop # DEBUG: 'lw	$5,20($sp)' does not load from $4
lw	$5,20($sp)
#nop # DEBUG: 'lw	$6,24($sp)' does not load from $5
lw	$6,24($sp)
#nop # DEBUG: 'jal	func_80018DC0' does not load from $6
jal	func_80018DC0
addu	$7,$sp,64

addu	$17,$0,$zero
sll	$2,$17,2
$L70:
addu	$2,$2,$22
lw	$3,48($2)
#nop # DEBUG: 'lw	$2,12($2)' does not load from $3
lw	$2,12($2)
nop # DEBUG: Reuse of '$2'. 'beq	$3,$2,$L54' does not use $at
beq	$3,$2,$L54
nop  # DEBUG: branch/jump
j	$L53
addu	$23,$23,1

$L54:
addu	$17,$17,1
slt	$2,$17,9
bne	$2,$0,$L70
sll	$2,$17,2

$L53:
lw	$8,104($sp)
nop # DEBUG: Reuse of '$8'. 'addu	$8,$8,1' does not use $at
addu	$8,$8,1
sw	$8,104($sp)
$L18:
lbu	$2,0($16)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L66' does not use $at
bne	$2,$0,$L66
nop  # DEBUG: branch/jump
$L19:
lw	$8,104($sp)
nop # DEBUG: Reuse of '$8'. 'beq	$8,$0,$L71' does not use $at
beq	$8,$0,$L71
lui	$4,%hi($LC5) # high

beq	$23,$0,$L59
nop  # DEBUG: branch/jump
$L71:
jal	say
addiu	$4,$4,%lo($LC5) # low

j	$L61
ori	$2,$zero,1

$L63:
lui	$4,%hi($LC3) # high
j	$L65
addiu	$4,$4,%lo($LC3) # low

$L64:
addiu	$4,$4,%lo($LC4) # low
$L65:
jal	say
nop  # DEBUG: branch/jump
j	$L61
ori	$2,$zero,2

$L59:
lui	$4,%hi($LC6) # high
jal	say
addiu	$4,$4,%lo($LC6) # low

addu	$2,$0,$zero
$L61:
lw	$31,148($sp)
#nop # DEBUG: 'lw	$fp,144($sp)' does not load from $31
lw	$fp,144($sp)
#nop # DEBUG: 'lw	$23,140($sp)' does not load from $fp
lw	$23,140($sp)
#nop # DEBUG: 'lw	$22,136($sp)' does not load from $23
lw	$22,136($sp)
#nop # DEBUG: 'lw	$21,132($sp)' does not load from $22
lw	$21,132($sp)
#nop # DEBUG: 'lw	$20,128($sp)' does not load from $21
lw	$20,128($sp)
#nop # DEBUG: 'lw	$19,124($sp)' does not load from $20
lw	$19,124($sp)
#nop # DEBUG: 'lw	$18,120($sp)' does not load from $19
lw	$18,120($sp)
#nop # DEBUG: 'lw	$17,116($sp)' does not load from $18
lw	$17,116($sp)
#nop # DEBUG: 'lw	$16,112($sp)' does not load from $17
lw	$16,112($sp)
#nop # DEBUG: 'addu	$sp,$sp,152' does not load from $16
addu	$sp,$sp,152
j	$31
nop  # DEBUG: branch/jump
.end	main
.section .bss
	.globl data.10
	.align	2
data.10:
	.space 32768
