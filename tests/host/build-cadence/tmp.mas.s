
.file	1 "/home/kai-luni/git/future-cop-lapd-ps1/tests/host/cadence_harness.c"


gcc2_compiled.:
__gnu_compiled_c:
.globl	D_8009FA40
.section .data
.align	2
	.align	2
D_8009FA40:
.word	0
.text
.align	2
.globl	func_8006AAEC
.ent	func_8006AAEC
.set	noreorder
func_8006AAEC:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
lui	$2,%hi(record) # high
j	$31
addiu	$2,$2,%lo(record) # low

.end	func_8006AAEC
.align	2
.globl	func_8006ACB0
.ent	func_8006ACB0
.set	noreorder
func_8006ACB0:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
lui	$3,%hi(fired) # high
ori	$2,$zero,1
j	$31
sw	$2,%lo(fired)($3)

.end	func_8006ACB0
.align	2
.globl	func_8006B23C
.ent	func_8006B23C
.set	noreorder
func_8006B23C:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
j	$31
addu	$2,$0,$zero

.end	func_8006B23C
.align	2
.globl	func_80046C84
.ent	func_80046C84
.set	noreorder
func_80046C84:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
j	$31
addu	$2,$0,$zero

.end	func_80046C84
.align	2
.globl	func_8004E5F0
.ent	func_8004E5F0
.set	noreorder
func_8004E5F0:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
j	$31
addu	$2,$0,$zero

.end	func_8004E5F0
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
.globl	memcpy
.ent	memcpy
.set	noreorder
memcpy:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
addu	$7,$4,$zero
beq	$6,$0,$L9
addu	$3,$6,-1

addiu	$6,$zero,-1
$L10:
lbu	$2,0($5)
nop # DEBUG: Reuse of '$2'. 'sb	$2,0($7)' does not use $at
sb	$2,0($7)
addu	$5,$5,1
addu	$3,$3,-1
bne	$3,$6,$L10
addu	$7,$7,1

$L9:
j	$31
addu	$2,$4,$zero

.end	memcpy
.align	2
.globl	memset
.ent	memset
.set	noreorder
memset:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
addu	$2,$4,$zero
addu	$7,$2,$zero
beq	$6,$0,$L14
addu	$3,$6,-1

addiu	$6,$zero,-1
$L15:
sb	$5,0($7)
addu	$3,$3,-1
bne	$3,$6,$L15
addu	$7,$7,1

$L14:
j	$31
nop  # DEBUG: branch/jump
.end	memset
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
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L20' does not use $at
beq	$2,$0,$L20
addu	$7,$0,$zero

addu	$7,$7,1
$L23:
addu	$2,$6,$7
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L23' does not use $at
bne	$2,$0,$L23
addu	$7,$7,1

addu	$7,$7,-1
$L20:
ori	$4,$zero,4004
jal	sys3
ori	$5,$zero,1

lw	$31,16($sp)
#nop # DEBUG: 'addu	$sp,$sp,24' does not load from $31
addu	$sp,$sp,24
j	$31
nop  # DEBUG: branch/jump
.end	say
.align	2
.ent	num
.set	noreorder
num:
.frame	$sp,40,$31		# vars= 16, regs= 1/0, args= 16, extra= 0
.mask	0x80000000,-8
.fmask	0x00000000,0
subu	$sp,$sp,40
sw	$31,32($sp)
addu	$5,$4,$zero
srl	$8,$5,31
beq	$8,$0,$L25
addu	$7,$0,$zero

subu	$5,$0,$5
$L25:
bne	$5,$0,$L26
addu	$3,$sp,$7

ori	$2,$zero,48
sb	$2,16($3)
addu	$7,$7,1
$L26:
blez	$5,$L28
nop  # DEBUG: branch/jump
addu	$6,$sp,16
lui	$9,(1717960704 >> 16) & 0xFFFF
ori	$9,$9,0x6667
$L29:
addu	$4,$6,$7
mult	$5,$9
mfhi	$10
sra	$3,$10,2
sra	$2,$5,31
subu	$3,$3,$2
sll	$2,$3,2
addu	$2,$2,$3
sll	$2,$2,1
subu	$2,$5,$2
addu	$2,$2,48
sb	$2,0($4)
addu	$5,$3,$zero
bgtz	$5,$L29
addu	$7,$7,1

$L28:
beq	$8,$0,$L31
addu	$3,$sp,$7

ori	$2,$zero,45
sb	$2,16($3)
addu	$7,$7,1
$L31:
sra	$2,$7,31
addu	$3,$2,$zero
subu	$2,$7,$2
sra	$2,$2,1
blez	$2,$L33
addu	$5,$0,$zero

addu	$8,$sp,16
subu	$2,$7,$3
sra	$9,$2,1
$L34:
addu	$4,$8,$5
lbu	$6,0($4)
#nop # DEBUG: 'addu	$5,$5,1' does not load from $6
addu	$5,$5,1
subu	$3,$7,$5
addu	$3,$8,$3
lbu	$2,0($3)
nop # DEBUG: Reuse of '$2'. 'sb	$2,0($4)' does not use $at
sb	$2,0($4)
slt	$2,$5,$9
bne	$2,$0,$L34
sb	$6,0($3)

$L33:
ori	$4,$zero,4004
ori	$5,$zero,1
jal	sys3
addu	$6,$sp,16

lw	$31,32($sp)
#nop # DEBUG: 'addu	$sp,$sp,40' does not load from $31
addu	$sp,$sp,40
j	$31
nop  # DEBUG: branch/jump
.end	num
.align	2
.ent	load
.set	noreorder
load:
.frame	$sp,32,$31		# vars= 0, regs= 4/0, args= 16, extra= 0
.mask	0x80070000,-4
.fmask	0x00000000,0
subu	$sp,$sp,32
sw	$31,28($sp)
sw	$18,24($sp)
sw	$17,20($sp)
sw	$16,16($sp)
addu	$5,$4,$zero
ori	$4,$zero,4005
addu	$6,$0,$zero
jal	sys3
addu	$7,$6,$zero

addu	$16,$2,$zero
bltz	$16,$L38
ori	$4,$zero,4003

addu	$5,$16,$zero
lui	$18,%hi(data) # high
addiu	$6,$18,%lo(data) # low
jal	sys3
ori	$7,$zero,16383

addu	$17,$2,$zero
ori	$4,$zero,4006
addu	$5,$16,$zero
addu	$6,$0,$zero
jal	sys3
addu	$7,$6,$zero

bltz	$17,$L38
addiu	$2,$18,%lo(data) # low

addu	$2,$17,$2
sb	$0,0($2)
j	$L39
addu	$2,$0,$zero

$L38:
addiu	$2,$zero,-1
$L39:
lw	$31,28($sp)
#nop # DEBUG: 'lw	$18,24($sp)' does not load from $31
lw	$18,24($sp)
#nop # DEBUG: 'lw	$17,20($sp)' does not load from $18
lw	$17,20($sp)
#nop # DEBUG: 'lw	$16,16($sp)' does not load from $17
lw	$16,16($sp)
#nop # DEBUG: 'addu	$sp,$sp,32' does not load from $16
addu	$sp,$sp,32
j	$31
nop  # DEBUG: branch/jump
.end	load
.align	2
.ent	skip_space
.set	noreorder
skip_space:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
lui	$4,%hi(p) # high
ori	$8,$zero,32
ori	$7,$zero,9
ori	$6,$zero,13
j	$L44
ori	$5,$zero,10

$L46:
lw	$2,%lo(p)($4) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$2'. 'addu	$2,$2,1' does not use $at
addu	$2,$2,1
sw	$2,%lo(p)($4)
$L44:
lw	$2,%lo(p)($4) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$2'. 'lbu	$2,0($2)' does not use $at
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$8,$L46' does not use $at
beq	$2,$8,$L46
nop  # DEBUG: branch/jump
beq	$2,$7,$L46
nop  # DEBUG: branch/jump
beq	$2,$6,$L46
nop  # DEBUG: branch/jump
beq	$2,$5,$L46
nop  # DEBUG: branch/jump
lw	$2,%lo(p)($4) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$2'. 'lbu	$3,0($2)' does not use $at
lbu	$3,0($2)
#nop # DEBUG: 'li	$2,35			# 0x00000023' does not load from $3
ori	$2,$zero,35
bne	$3,$2,$L40
nop  # DEBUG: branch/jump
$L53:
lw	$3,%lo(p)($4) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$3'. 'lbu	$2,0($3)' does not use $at
lbu	$2,0($3)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$5,$L44' does not use $at
beq	$2,$5,$L44
addu	$2,$3,1

sw	$2,%lo(p)($4)
lbu	$2,1($3)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L44' does not use $at
beq	$2,$0,$L44
nop  # DEBUG: branch/jump
j	$L53
nop  # DEBUG: branch/jump
$L40:
j	$31
nop  # DEBUG: branch/jump
.end	skip_space
.align	2
.ent	next_int
.set	noreorder
next_int:
.frame	$sp,32,$31		# vars= 0, regs= 4/0, args= 16, extra= 0
.mask	0x80070000,-4
.fmask	0x00000000,0
subu	$sp,$sp,32
sw	$31,28($sp)
sw	$18,24($sp)
sw	$17,20($sp)
sw	$16,16($sp)
addu	$18,$4,$zero
addu	$16,$0,$zero
jal	skip_space
addu	$17,$16,$zero

lui	$2,%hi(p) # high
lw	$2,%lo(p)($2) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$2'. 'lbu	$2,0($2)' does not use $at
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L55' does not use $at
bne	$2,$0,$L55
lui	$5,%hi(p) # high

j	$L62
addu	$2,$16,$zero

$L55:
lw	$4,%lo(p)($5) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$4'. 'lbu	$3,0($4)' does not use $at
lbu	$3,0($4)
#nop # DEBUG: 'li	$2,45			# 0x0000002d' does not load from $3
ori	$2,$zero,45
bne	$3,$2,$L63
lui	$2,%hi(p) # high

ori	$17,$zero,1
addu	$2,$4,1
sw	$2,%lo(p)($5)
lui	$2,%hi(p) # high
$L63:
lw	$2,%lo(p)($2) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$2'. 'lbu	$2,0($2)' does not use $at
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'addu	$2,$2,-48' does not use $at
addu	$2,$2,-48
sltu	$2,$2,10
beq	$2,$0,$L58
sll	$2,$16,2

lui	$5,%hi(p) # high
$L64:
addu	$2,$2,$16
sll	$2,$2,1
addu	$2,$2,-48
lw	$4,%lo(p)($5) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$4'. 'lbu	$3,0($4)' does not use $at
lbu	$3,0($4)
nop # DEBUG: Reuse of '$3'. 'addu	$16,$2,$3' does not use $at
addu	$16,$2,$3
addu	$2,$4,1
sw	$2,%lo(p)($5)
lbu	$2,1($4)
nop # DEBUG: Reuse of '$2'. 'addu	$2,$2,-48' does not use $at
addu	$2,$2,-48
sltu	$2,$2,10
bne	$2,$0,$L64
sll	$2,$16,2

$L58:
beq	$17,$0,$L61
sw	$16,0($18)

subu	$2,$0,$16
sw	$2,0($18)
$L61:
ori	$2,$zero,1
$L62:
lw	$31,28($sp)
#nop # DEBUG: 'lw	$18,24($sp)' does not load from $31
lw	$18,24($sp)
#nop # DEBUG: 'lw	$17,20($sp)' does not load from $18
lw	$17,20($sp)
#nop # DEBUG: 'lw	$16,16($sp)' does not load from $17
lw	$16,16($sp)
#nop # DEBUG: 'addu	$sp,$sp,32' does not load from $16
addu	$sp,$sp,32
j	$31
nop  # DEBUG: branch/jump
.end	next_int
.align	2
.ent	next_name
.set	noreorder
next_name:
.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, extra= 0
.mask	0x80010000,-4
.fmask	0x00000000,0
subu	$sp,$sp,24
sw	$31,20($sp)
sw	$16,16($sp)
jal	skip_space
addu	$16,$0,$zero

lui	$2,%hi(p) # high
lw	$2,%lo(p)($2) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$2'. 'lbu	$2,0($2)' does not use $at
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L66' does not use $at
bne	$2,$0,$L66
lui	$5,%hi(p) # high

j	$L72
addu	$2,$16,$zero

$L66:
ori	$9,$zero,32
ori	$8,$zero,9
ori	$7,$zero,10
lui	$2,%hi(namebuf) # high
j	$L67
addiu	$6,$2,%lo(namebuf) # low

$L71:
beq	$2,$0,$L68
addu	$4,$16,$6

lw	$3,%lo(p)($5) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$3'. 'lbu	$2,0($3)' does not use $at
lbu	$2,0($3)
nop # DEBUG: Reuse of '$2'. 'sb	$2,0($4)' does not use $at
sb	$2,0($4)
addu	$3,$3,1
sw	$3,%lo(p)($5)
addu	$16,$16,1
$L67:
lw	$2,%lo(p)($5) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$2'. 'lbu	$2,0($2)' does not use $at
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L68' does not use $at
beq	$2,$0,$L68
nop  # DEBUG: branch/jump
beq	$2,$9,$L68
nop  # DEBUG: branch/jump
beq	$2,$8,$L68
nop  # DEBUG: branch/jump
bne	$2,$7,$L71
slt	$2,$16,63

$L68:
lui	$2,%hi(namebuf) # high
addiu	$2,$2,%lo(namebuf) # low
addu	$2,$16,$2
sb	$0,0($2)
ori	$2,$zero,1
$L72:
lw	$31,20($sp)
#nop # DEBUG: 'lw	$16,16($sp)' does not load from $31
lw	$16,16($sp)
#nop # DEBUG: 'addu	$sp,$sp,24' does not load from $16
addu	$sp,$sp,24
j	$31
nop  # DEBUG: branch/jump
.end	next_name
.section .rodata
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
.set	noreorder
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
addu	$16,$4,$zero
jal	__main
addu	$17,$5,$zero

addu	$21,$0,$zero
ori	$2,$zero,2
beq	$16,$2,$L74
addu	$20,$21,$zero

lui	$4,%hi($LC0) # high
j	$L111
addiu	$4,$4,%lo($LC0) # low

$L74:
lw	$4,4($17)
#nop # DEBUG: 'jal	load' does not load from $4
jal	load
nop  # DEBUG: branch/jump
beq	$2,$0,$L75
lui	$4,%hi($LC1) # high

j	$L111
addiu	$4,$4,%lo($LC1) # low

$L75:
lui	$2,%hi(p) # high
lui	$3,%hi(data) # high
addiu	$3,$3,%lo(data) # low
sw	$3,%lo(p)($2)
lui	$2,%hi(expect) # high
addiu	$22,$2,%lo(expect) # low
$L76:
jal	next_name
nop  # DEBUG: branch/jump
beq	$2,$0,$L77
nop  # DEBUG: branch/jump
jal	next_int
addu	$4,$sp,704

beq	$2,$0,$L113
lui	$4,%hi($LC2) # high

jal	next_int
addu	$4,$sp,708

beq	$2,$0,$L113
lui	$4,%hi($LC2) # high

jal	next_int
addu	$4,$sp,712

beq	$2,$0,$L113
lui	$4,%hi($LC2) # high

jal	next_int
addu	$4,$sp,716

beq	$2,$0,$L113
lui	$4,%hi($LC2) # high

jal	next_int
addu	$4,$sp,720

beq	$2,$0,$L113
lui	$4,%hi($LC2) # high

jal	next_int
addu	$4,$sp,724

beq	$2,$0,$L113
lui	$4,%hi($LC2) # high

jal	next_int
addu	$4,$sp,728

bne	$2,$0,$L79
lui	$4,%hi($LC2) # high

$L113:
j	$L111
addiu	$4,$4,%lo($LC2) # low

$L110:
addu	$20,$20,1
lui	$4,%hi(namebuf) # high
jal	say
addiu	$4,$4,%lo(namebuf) # low

lui	$4,%hi($LC8) # high
jal	say
addiu	$4,$4,%lo($LC8) # low

jal	num
addu	$4,$16,$zero

lui	$4,%hi($LC9) # high
jal	say
addiu	$4,$4,%lo($LC9) # low

lw	$4,0($17)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC10) # high
jal	say
addiu	$4,$4,%lo($LC10) # low

lw	$4,0($18)
#nop # DEBUG: 'j	$L112' does not load from $4
j	$L112
nop  # DEBUG: branch/jump
$L79:
lw	$2,728($sp)
nop # DEBUG: Reuse of '$2'. 'slt	$2,$2,129' does not use $at
slt	$2,$2,129
beq	$2,$0,$L114
lui	$4,%hi($LC3) # high

lw	$2,724($sp)
nop # DEBUG: Reuse of '$2'. 'slt	$2,$2,4097' does not use $at
slt	$2,$2,4097
bne	$2,$0,$L81
nop  # DEBUG: branch/jump
$L114:
j	$L111
addiu	$4,$4,%lo($LC3) # low

$L81:
lw	$2,728($sp)
nop # DEBUG: Reuse of '$2'. 'blez	$2,$L84' does not use $at
blez	$2,$L84
addu	$16,$0,$zero

sll	$4,$16,2
$L115:
jal	next_int
addu	$4,$4,$22

beq	$2,$0,$L109
addu	$16,$16,1

lw	$2,728($sp)
nop # DEBUG: Reuse of '$2'. 'slt	$2,$16,$2' does not use $at
slt	$2,$16,$2
bne	$2,$0,$L115
sll	$4,$16,2

$L84:
lui	$16,%hi(record) # high
addiu	$4,$16,%lo(record) # low
addu	$5,$0,$zero
jal	memset
ori	$6,$zero,84

addiu	$16,$16,%lo(record) # low
lbu	$2,716($sp)
nop # DEBUG: Reuse of '$2'. 'sb	$2,10($16)' does not use $at
sb	$2,10($16)
ori	$2,$zero,3
sb	$2,4($16)
lbu	$2,704($sp)
nop # DEBUG: Reuse of '$2'. 'sb	$2,10($16)' does not use $at
sb	$2,10($16)
lhu	$2,708($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,12($16)' does not use $at
sh	$2,12($16)
lhu	$2,712($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,14($16)' does not use $at
sh	$2,14($16)
addu	$4,$sp,504
addu	$5,$0,$zero
jal	memset
ori	$6,$zero,196

addu	$4,$sp,192
addu	$5,$0,$zero
jal	memset
ori	$6,$zero,132

addu	$4,$sp,16
addu	$5,$0,$zero
jal	memset
ori	$6,$zero,176

addu	$16,$0,$zero
addu	$4,$sp,16
ori	$6,$zero,100
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
nop # DEBUG: Reuse of '$2'. 'sb	$2,0($3)' does not use $at
sb	$2,0($3)
addu	$16,$16,1
slt	$2,$16,3
bne	$2,$0,$L116
sll	$2,$16,2

addu	$2,$sp,192
sw	$2,188($sp)
addu	$2,$sp,504
sw	$2,500($sp)
addu	$21,$21,1
addu	$17,$0,$zero
lw	$2,724($sp)
nop # DEBUG: Reuse of '$2'. 'blez	$2,$L95' does not use $at
blez	$2,$L95
addu	$16,$17,$zero

lui	$18,%hi(fired) # high
lui	$2,%hi(hits) # high
addiu	$19,$2,%lo(hits) # low
$L97:
sw	$0,%lo(fired)($18)
lw	$5,720($sp)
#nop # DEBUG: 'jal	func_8006C3E0' does not load from $5
jal	func_8006C3E0
addu	$4,$sp,16

lw	$2,312($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,624($sp)' does not use $at
sw	$2,624($sp)
addu	$4,$sp,328
jal	func_8006BA50
addu	$5,$0,$zero

lw	$2,624($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,312($sp)' does not use $at
sw	$2,312($sp)
lw	$2,%lo(fired)($18) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L96' does not use $at
beq	$2,$0,$L96
slt	$2,$17,128

beq	$2,$0,$L96
sll	$2,$17,2

addu	$2,$2,$19
sw	$16,0($2)
addu	$17,$17,1
$L96:
addu	$16,$16,1
lw	$2,724($sp)
nop # DEBUG: Reuse of '$2'. 'slt	$2,$16,$2' does not use $at
slt	$2,$16,$2
bne	$2,$0,$L97
nop  # DEBUG: branch/jump
$L95:
lw	$2,728($sp)
nop # DEBUG: Reuse of '$2'. 'beq	$17,$2,$L118' does not use $at
beq	$17,$2,$L118
lui	$4,%hi(namebuf) # high

addu	$20,$20,1
jal	say
addiu	$4,$4,%lo(namebuf) # low

lui	$4,%hi($LC5) # high
jal	say
addiu	$4,$4,%lo($LC5) # low

jal	num
addu	$4,$17,$zero

lui	$4,%hi($LC6) # high
jal	say
addiu	$4,$4,%lo($LC6) # low

lw	$4,728($sp)
#nop # DEBUG: 'jal	num' does not load from $4
$L112:
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC7) # high
jal	say
addiu	$4,$4,%lo($LC7) # low

j	$L76
nop  # DEBUG: branch/jump
$L118:
blez	$2,$L76
addu	$16,$0,$zero

lui	$2,%hi(hits) # high
addiu	$5,$2,%lo(hits) # low
lw	$4,728($sp)
#nop # DEBUG: 'sll	$2,$16,2' does not load from $4
sll	$2,$16,2
$L117:
addu	$17,$2,$5
addu	$18,$2,$22
lw	$3,0($17)
#nop # DEBUG: 'lw	$2,0($18)' does not load from $3
lw	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'bne	$3,$2,$L110' does not use $at
bne	$3,$2,$L110
nop  # DEBUG: branch/jump
addu	$16,$16,1
slt	$2,$16,$4
bne	$2,$0,$L117
sll	$2,$16,2

j	$L76
nop  # DEBUG: branch/jump
$L77:
beq	$20,$0,$L107
lui	$4,%hi($LC11) # high

lui	$4,%hi($LC13) # high
jal	say
addiu	$4,$4,%lo($LC13) # low

jal	num
addu	$4,$20,$zero

lui	$4,%hi($LC14) # high
jal	say
addiu	$4,$4,%lo($LC14) # low

jal	num
addu	$4,$21,$zero

lui	$4,%hi($LC15) # high
jal	say
addiu	$4,$4,%lo($LC15) # low

j	$L108
ori	$2,$zero,1

$L109:
lui	$4,%hi($LC4) # high
addiu	$4,$4,%lo($LC4) # low
$L111:
jal	say
nop  # DEBUG: branch/jump
j	$L108
ori	$2,$zero,2

$L107:
jal	say
addiu	$4,$4,%lo($LC11) # low

jal	num
addu	$4,$21,$zero

lui	$4,%hi($LC12) # high
jal	say
addiu	$4,$4,%lo($LC12) # low

addu	$2,$0,$zero
$L108:
lw	$31,764($sp)
#nop # DEBUG: 'lw	$22,760($sp)' does not load from $31
lw	$22,760($sp)
#nop # DEBUG: 'lw	$21,756($sp)' does not load from $22
lw	$21,756($sp)
#nop # DEBUG: 'lw	$20,752($sp)' does not load from $21
lw	$20,752($sp)
#nop # DEBUG: 'lw	$19,748($sp)' does not load from $20
lw	$19,748($sp)
#nop # DEBUG: 'lw	$18,744($sp)' does not load from $19
lw	$18,744($sp)
#nop # DEBUG: 'lw	$17,740($sp)' does not load from $18
lw	$17,740($sp)
#nop # DEBUG: 'lw	$16,736($sp)' does not load from $17
lw	$16,736($sp)
#nop # DEBUG: 'addu	$sp,$sp,768' does not load from $16
addu	$sp,$sp,768
j	$31
nop  # DEBUG: branch/jump
.end	main








.section .bss
	.globl D_8009FA8C
	.align	2
D_8009FA8C:
	.space 8
	.globl record
	.align	2
record:
	.space 84
	.globl fired
	.align	2
fired:
	.space 4
	.globl data
	.align	2
data:
	.space 16384
	.globl namebuf
	.align	2
namebuf:
	.space 64
	.globl expect
	.align	2
expect:
	.space 512
	.globl hits
	.align	2
hits:
	.space 512
	.globl p
	.align	2
p:
	.space 4
