
.file	1 "/home/kai-luni/git/future-cop-lapd-ps1/tests/host/shot_harness.c"


gcc2_compiled.:
__gnu_compiled_c:
.text
.align	2
.globl	D_80036624
.ent	D_80036624
.set	noreorder
D_80036624:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
j	$31
nop  # DEBUG: branch/jump
.end	D_80036624
.align	2
.globl	func_8004D444
.ent	func_8004D444
.set	noreorder
func_8004D444:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
lui	$2,%hi(recorded) # high
addiu	$7,$2,%lo(recorded) # low
lw	$6,8($4)
nop # DEBUG: Reuse of '$6'. 'addu	$8,$6,96' does not use $at
addu	$8,$6,96
$L3:
lw	$2,0($6)
#nop # DEBUG: 'lw	$3,4($6)' does not load from $2
lw	$3,4($6)
#nop # DEBUG: 'lw	$4,8($6)' does not load from $3
lw	$4,8($6)
#nop # DEBUG: 'lw	$5,12($6)' does not load from $4
lw	$5,12($6)
#nop # DEBUG: 'sw	$2,0($7)' does not load from $5
sw	$2,0($7)
sw	$3,4($7)
sw	$4,8($7)
sw	$5,12($7)
addu	$6,$6,16
bne	$6,$8,$L3
addu	$7,$7,16

lui	$3,%hi(recorded_flag) # high
ori	$2,$zero,1
sw	$2,%lo(recorded_flag)($3)
j	$31
addu	$2,$0,$zero

.end	func_8004D444
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
beq	$6,$0,$L7
addu	$3,$6,-1

addiu	$6,$zero,-1
$L8:
lbu	$2,0($5)
nop # DEBUG: Reuse of '$2'. 'sb	$2,0($7)' does not use $at
sb	$2,0($7)
addu	$5,$5,1
addu	$3,$3,-1
bne	$3,$6,$L8
addu	$7,$7,1

$L7:
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
beq	$6,$0,$L12
addu	$3,$6,-1

addiu	$6,$zero,-1
$L13:
sb	$5,0($7)
addu	$3,$3,-1
bne	$3,$6,$L13
addu	$7,$7,1

$L12:
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
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L18' does not use $at
beq	$2,$0,$L18
addu	$7,$0,$zero

addu	$7,$7,1
$L21:
addu	$2,$6,$7
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L21' does not use $at
bne	$2,$0,$L21
addu	$7,$7,1

addu	$7,$7,-1
$L18:
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
beq	$8,$0,$L23
addu	$7,$0,$zero

subu	$5,$0,$5
$L23:
bne	$5,$0,$L24
addu	$3,$sp,$7

ori	$2,$zero,48
sb	$2,16($3)
addu	$7,$7,1
$L24:
blez	$5,$L26
nop  # DEBUG: branch/jump
addu	$6,$sp,16
lui	$9,(1717960704 >> 16) & 0xFFFF
ori	$9,$9,0x6667
$L27:
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
bgtz	$5,$L27
addu	$7,$7,1

$L26:
beq	$8,$0,$L29
addu	$3,$sp,$7

ori	$2,$zero,45
sb	$2,16($3)
addu	$7,$7,1
$L29:
sra	$2,$7,31
addu	$3,$2,$zero
subu	$2,$7,$2
sra	$2,$2,1
blez	$2,$L31
addu	$5,$0,$zero

addu	$8,$sp,16
subu	$2,$7,$3
sra	$9,$2,1
$L32:
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
bne	$2,$0,$L32
sb	$6,0($3)

$L31:
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
beq	$4,$0,$L35
andi	$2,$3,0x00ff

j	$L38
addu	$2,$2,-48

$L35:
addu	$2,$3,-97
andi	$2,$2,0x00ff
sltu	$2,$2,6
beq	$2,$0,$L36
andi	$2,$3,0x00ff

j	$L38
addu	$2,$2,-87

$L36:
addu	$2,$3,-65
andi	$2,$2,0x00ff
sltu	$2,$2,6
bne	$2,$0,$L37
andi	$2,$3,0x00ff

j	$L38
addiu	$2,$zero,-1

$L37:
addu	$2,$2,-55
$L38:
j	$31
nop  # DEBUG: branch/jump
.end	hex
.align	2
.ent	next_word
.set	noreorder
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
addu	$18,$4,$zero
addu	$19,$5,$zero
lw	$16,0($18)
#nop # DEBUG: 'li	$6,32			# 0x00000020' does not load from $16
ori	$6,$zero,32
ori	$5,$zero,9
ori	$4,$zero,13
ori	$3,$zero,10
ori	$7,$zero,35
$L58:
lbu	$2,0($16)
$L60:
nop # DEBUG: Reuse of '$2'. 'beq	$2,$6,$L58' does not use $at
# $L60:  # DEBUG: skipped
beq	$2,$6,$L58
addu	$16,$16,1

addu	$16,$16,-1
beq	$2,$5,$L58
addu	$16,$16,1

addu	$16,$16,-1
beq	$2,$4,$L58
addu	$16,$16,1

addu	$16,$16,-1
beq	$2,$3,$L58
addu	$16,$16,1

addu	$16,$16,-1
lbu	$2,0($16)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$7,$L41' does not use $at
bne	$2,$7,$L41
nop  # DEBUG: branch/jump
$L59:
beq	$2,$3,$L58
nop  # DEBUG: branch/jump
addu	$16,$16,1
lbu	$2,0($16)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L60' does not use $at
beq	$2,$0,$L60
nop  # DEBUG: branch/jump
j	$L59
nop  # DEBUG: branch/jump
$L41:
bne	$2,$0,$L53
addu	$17,$0,$zero

sw	$16,0($18)
j	$L57
addu	$2,$0,$zero

$L53:
$L54:
lbu	$4,0($16)
#nop # DEBUG: 'jal	hex' does not load from $4
jal	hex
nop  # DEBUG: branch/jump
addu	$3,$2,$zero
bltz	$3,$L55
sll	$2,$17,4

or	$17,$2,$3
j	$L54
addu	$16,$16,1

$L55:
sw	$16,0($18)
sw	$17,0($19)
ori	$2,$zero,1
$L57:
lw	$31,32($sp)
#nop # DEBUG: 'lw	$19,28($sp)' does not load from $31
lw	$19,28($sp)
#nop # DEBUG: 'lw	$18,24($sp)' does not load from $19
lw	$18,24($sp)
#nop # DEBUG: 'lw	$17,20($sp)' does not load from $18
lw	$17,20($sp)
#nop # DEBUG: 'lw	$16,16($sp)' does not load from $17
lw	$16,16($sp)
#nop # DEBUG: 'addu	$sp,$sp,40' does not load from $16
addu	$sp,$sp,40
j	$31
nop  # DEBUG: branch/jump
.end	next_word
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
bltz	$16,$L63
ori	$4,$zero,4003

addu	$5,$16,$zero
lui	$18,%hi(data) # high
addiu	$6,$18,%lo(data) # low
jal	sys3
ori	$7,$zero,32767

addu	$17,$2,$zero
ori	$4,$zero,4006
addu	$5,$16,$zero
addu	$6,$0,$zero
jal	sys3
addu	$7,$6,$zero

bltz	$17,$L63
addiu	$2,$18,%lo(data) # low

addu	$2,$17,$2
sb	$0,0($2)
j	$L64
addu	$2,$0,$zero

$L63:
addiu	$2,$zero,-1
$L64:
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
.section .rodata
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
.set	noreorder
report:
.frame	$sp,32,$31		# vars= 0, regs= 4/0, args= 16, extra= 0
.mask	0x80070000,-4
.fmask	0x00000000,0
subu	$sp,$sp,32
sw	$31,28($sp)
sw	$18,24($sp)
sw	$17,20($sp)
sw	$16,16($sp)
addu	$18,$4,$zero
addu	$16,$6,$zero
bne	$16,$0,$L66
addu	$17,$5,$zero

lui	$4,%hi($LC0) # high
jal	say
addiu	$4,$4,%lo($LC0) # low

jal	say
addu	$4,$18,$zero

lui	$4,%hi($LC1) # high
jal	say
addiu	$4,$4,%lo($LC1) # low

jal	num
addu	$4,$17,$zero

j	$L69
lui	$4,%hi($LC2) # high

$L66:
lui	$4,%hi($LC3) # high
jal	say
addiu	$4,$4,%lo($LC3) # low

jal	num
addu	$4,$16,$zero

lui	$4,%hi($LC4) # high
jal	say
addiu	$4,$4,%lo($LC4) # low

jal	num
addu	$4,$17,$zero

lui	$4,%hi($LC5) # high
jal	say
addiu	$4,$4,%lo($LC5) # low

jal	say
addu	$4,$18,$zero

lui	$4,%hi($LC2) # high
$L69:
jal	say
addiu	$4,$4,%lo($LC2) # low

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
.end	report
.section .rodata
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
.set	noreorder
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
addu	$22,$0,$zero
jal	load
addu	$19,$22,$zero

beq	$2,$0,$L71
lui	$4,%hi($LC6) # high

j	$L99
addiu	$4,$4,%lo($LC6) # low

$L98:
lui	$4,%hi($LC7) # high
addiu	$4,$4,%lo($LC7) # low
$L99:
jal	say
nop  # DEBUG: branch/jump
j	$L97
ori	$2,$zero,2

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
jal	next_word
addu	$5,$sp,48

beq	$2,$0,$L73
lui	$8,%hi(recorded) # high

addiu	$17,$8,%lo(recorded) # low
ori	$16,$zero,1
sll	$5,$16,2
$L101:
addu	$4,$sp,208
jal	next_word
addu	$5,$20,$5

beq	$2,$0,$L98
addu	$16,$16,1

slt	$2,$16,39
bne	$2,$0,$L101
sll	$5,$16,2

addu	$4,$fp,$zero
addu	$5,$0,$zero
jal	memset
ori	$6,$zero,20

addu	$4,$23,$zero
addu	$5,$0,$zero
jal	memset
ori	$6,$zero,40

lhu	$2,52($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,222($sp)' does not use $at
sh	$2,222($sp)
lhu	$2,56($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,224($sp)' does not use $at
sh	$2,224($sp)
lhu	$2,60($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,216($sp)' does not use $at
sh	$2,216($sp)
lhu	$2,64($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,226($sp)' does not use $at
sh	$2,226($sp)
lhu	$2,68($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,218($sp)' does not use $at
sh	$2,218($sp)
lbu	$2,72($sp)
nop # DEBUG: Reuse of '$2'. 'sb	$2,230($sp)' does not use $at
sb	$2,230($sp)
lbu	$2,76($sp)
nop # DEBUG: Reuse of '$2'. 'sb	$2,234($sp)' does not use $at
sb	$2,234($sp)
lhu	$2,80($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,260($sp)' does not use $at
sh	$2,260($sp)
lbu	$2,84($sp)
nop # DEBUG: Reuse of '$2'. 'sb	$2,262($sp)' does not use $at
sb	$2,262($sp)
lbu	$2,88($sp)
nop # DEBUG: Reuse of '$2'. 'sb	$2,263($sp)' does not use $at
sb	$2,263($sp)
lbu	$2,92($sp)
nop # DEBUG: Reuse of '$2'. 'sb	$2,264($sp)' does not use $at
sb	$2,264($sp)
lw	$2,96($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,276($sp)' does not use $at
sw	$2,276($sp)
lw	$2,100($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,280($sp)' does not use $at
sw	$2,280($sp)
lw	$2,104($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,284($sp)' does not use $at
sw	$2,284($sp)
lw	$2,108($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,288($sp)' does not use $at
sw	$2,288($sp)
lui	$2,%hi(recorded_flag) # high
sw	$0,%lo(recorded_flag)($2)
sw	$23,16($sp)
lw	$2,136($sp)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L81' does not use $at
beq	$2,$0,$L81
nop  # DEBUG: branch/jump
j	$L82
sw	$17,20($sp)

$L81:
sw	$0,20($sp)
$L82:
lw	$2,112($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,24($sp)' does not use $at
sw	$2,24($sp)
lbu	$2,116($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,28($sp)' does not use $at
sw	$2,28($sp)
lbu	$2,120($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,32($sp)' does not use $at
sw	$2,32($sp)
lhu	$2,124($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,36($sp)' does not use $at
sw	$2,36($sp)
lbu	$2,128($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,40($sp)' does not use $at
sw	$2,40($sp)
lw	$2,132($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,44($sp)' does not use $at
sw	$2,44($sp)
addu	$4,$fp,$zero
addu	$5,$21,$zero
addu	$6,$21,$zero
jal	func_8005E47C
addu	$7,$21,$zero

lui	$2,%hi(recorded_flag) # high
lw	$2,%lo(recorded_flag)($2) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L83' does not use $at
bne	$2,$0,$L83
addu	$22,$22,1

addu	$19,$19,1
lui	$4,%hi($LC8) # high
jal	say
addiu	$4,$4,%lo($LC8) # low

lw	$4,48($sp)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC9) # high
jal	say
addiu	$4,$4,%lo($LC9) # low

j	$L100
addu	$4,$sp,208

$L83:
lui	$8,%hi(recorded) # high
lw	$2,%lo(recorded)($8) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$2'. 'sw	$2,296($sp)' does not use $at
sw	$2,296($sp)
lw	$2,20($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,300($sp)' does not use $at
sw	$2,300($sp)
lhu	$2,24($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,304($sp)' does not use $at
sw	$2,304($sp)
lhu	$2,26($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,308($sp)' does not use $at
sw	$2,308($sp)
lw	$2,44($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,312($sp)' does not use $at
sw	$2,312($sp)
lw	$2,60($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,316($sp)' does not use $at
sw	$2,316($sp)
lw	$2,68($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,320($sp)' does not use $at
sw	$2,320($sp)
lw	$2,64($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,324($sp)' does not use $at
sw	$2,324($sp)
lhu	$2,72($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,328($sp)' does not use $at
sw	$2,328($sp)
lh	$2,74($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,332($sp)' does not use $at
sw	$2,332($sp)
lbu	$2,76($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,336($sp)' does not use $at
sw	$2,336($sp)
lbu	$2,77($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,340($sp)' does not use $at
sw	$2,340($sp)
lhu	$2,78($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,344($sp)' does not use $at
sw	$2,344($sp)
lbu	$2,80($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,348($sp)' does not use $at
sw	$2,348($sp)
lbu	$2,81($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,352($sp)' does not use $at
sw	$2,352($sp)
lw	$2,84($18)
nop # DEBUG: Reuse of '$2'. 'sw	$2,356($sp)' does not use $at
sw	$2,356($sp)
addu	$16,$0,$zero
sll	$2,$16,2
$L102:
addu	$2,$2,$20
lw	$3,92($2)
nop # DEBUG: Reuse of '$3'. 'sw	$3,312($2)' does not use $at
sw	$3,312($2)
addu	$16,$16,1
slt	$2,$16,16
bne	$2,$0,$L102
sll	$2,$16,2

addu	$16,$0,$zero
sll	$2,$16,2
$L103:
addu	$17,$2,$20
lw	$3,248($17)
#nop # DEBUG: 'lw	$2,312($17)' does not load from $3
lw	$2,312($17)
nop # DEBUG: Reuse of '$2'. 'beq	$3,$2,$L91' does not use $at
beq	$3,$2,$L91
nop  # DEBUG: branch/jump
addu	$19,$19,1
lui	$4,%hi($LC8) # high
jal	say
addiu	$4,$4,%lo($LC8) # low

lw	$4,48($sp)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC10) # high
jal	say
addiu	$4,$4,%lo($LC10) # low

jal	num
addu	$4,$16,16

lui	$4,%hi($LC11) # high
jal	say
addiu	$4,$4,%lo($LC11) # low

lw	$4,248($17)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC12) # high
jal	say
addiu	$4,$4,%lo($LC12) # low

lw	$4,312($17)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC13) # high
jal	say
addiu	$4,$4,%lo($LC13) # low

$L91:
addu	$16,$16,1
slt	$2,$16,16
bne	$2,$0,$L103
sll	$2,$16,2

lw	$3,8($18)
#nop # DEBUG: 'lw	$2,280($sp)' does not load from $3
lw	$2,280($sp)
nop # DEBUG: Reuse of '$2'. 'bne	$3,$2,$L96' does not use $at
bne	$3,$2,$L96
nop  # DEBUG: branch/jump
lw	$3,12($18)
#nop # DEBUG: 'lw	$2,284($sp)' does not load from $3
lw	$2,284($sp)
nop # DEBUG: Reuse of '$2'. 'bne	$3,$2,$L96' does not use $at
bne	$3,$2,$L96
nop  # DEBUG: branch/jump
lw	$3,16($18)
#nop # DEBUG: 'lw	$2,288($sp)' does not load from $3
lw	$2,288($sp)
nop # DEBUG: Reuse of '$2'. 'beq	$3,$2,$L72' does not use $at
beq	$3,$2,$L72
nop  # DEBUG: branch/jump
$L96:
addu	$19,$19,1
lui	$4,%hi($LC8) # high
jal	say
addiu	$4,$4,%lo($LC8) # low

lw	$4,48($sp)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC14) # high
jal	say
addiu	$4,$4,%lo($LC14) # low

j	$L100
addu	$4,$sp,208

$L73:
lui	$4,%hi($LC15) # high
addiu	$4,$4,%lo($LC15) # low
addu	$5,$22,$zero
jal	report
addu	$6,$19,$zero

sltu	$2,$0,$19
$L97:
lw	$31,460($sp)
#nop # DEBUG: 'lw	$fp,456($sp)' does not load from $31
lw	$fp,456($sp)
#nop # DEBUG: 'lw	$23,452($sp)' does not load from $fp
lw	$23,452($sp)
#nop # DEBUG: 'lw	$22,448($sp)' does not load from $23
lw	$22,448($sp)
#nop # DEBUG: 'lw	$21,444($sp)' does not load from $22
lw	$21,444($sp)
#nop # DEBUG: 'lw	$20,440($sp)' does not load from $21
lw	$20,440($sp)
#nop # DEBUG: 'lw	$19,436($sp)' does not load from $20
lw	$19,436($sp)
#nop # DEBUG: 'lw	$18,432($sp)' does not load from $19
lw	$18,432($sp)
#nop # DEBUG: 'lw	$17,428($sp)' does not load from $18
lw	$17,428($sp)
#nop # DEBUG: 'lw	$16,424($sp)' does not load from $17
lw	$16,424($sp)
#nop # DEBUG: 'addu	$sp,$sp,464' does not load from $16
addu	$sp,$sp,464
j	$31
nop  # DEBUG: branch/jump
.end	run_spawn
.section .rodata
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
.set	noreorder
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
addu	$20,$0,$zero
jal	load
addu	$18,$20,$zero

beq	$2,$0,$L105
lui	$4,%hi($LC16) # high

j	$L123
addiu	$4,$4,%lo($LC16) # low

$L122:
lui	$4,%hi($LC17) # high
addiu	$4,$4,%lo($LC17) # low
$L123:
jal	say
nop  # DEBUG: branch/jump
j	$L121
ori	$2,$zero,2

$L105:
addu	$19,$sp,16
addu	$4,$sp,64
$L124:
jal	next_word
addu	$5,$sp,16

beq	$2,$0,$L107
ori	$16,$zero,1

sll	$5,$16,2
$L125:
addu	$4,$sp,64
jal	next_word
addu	$5,$19,$5

beq	$2,$0,$L122
addu	$16,$16,1

slt	$2,$16,11
bne	$2,$0,$L125
sll	$5,$16,2

lw	$2,20($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,72($sp)' does not use $at
sw	$2,72($sp)
lw	$2,24($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,76($sp)' does not use $at
sw	$2,76($sp)
lw	$2,28($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,80($sp)' does not use $at
sw	$2,80($sp)
lw	$2,32($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,88($sp)' does not use $at
sw	$2,88($sp)
lw	$2,36($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,92($sp)' does not use $at
sw	$2,92($sp)
lw	$2,40($sp)
nop # DEBUG: Reuse of '$2'. 'sw	$2,96($sp)' does not use $at
sw	$2,96($sp)
addu	$7,$sp,72
addu	$4,$7,$zero
lw	$6,44($sp)
#nop # DEBUG: 'jal	func_8001ACA4' does not load from $6
jal	func_8001ACA4
addu	$5,$sp,88

addu	$20,$20,1
addu	$16,$0,$zero
sll	$2,$16,2
$L126:
addu	$17,$2,$19
lw	$3,56($17)
#nop # DEBUG: 'lw	$2,32($17)' does not load from $3
lw	$2,32($17)
nop # DEBUG: Reuse of '$2'. 'beq	$3,$2,$L117' does not use $at
beq	$3,$2,$L117
nop  # DEBUG: branch/jump
addu	$18,$18,1
lui	$4,%hi($LC18) # high
jal	say
addiu	$4,$4,%lo($LC18) # low

lw	$4,16($sp)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC19) # high
jal	say
addiu	$4,$4,%lo($LC19) # low

jal	num
addu	$4,$16,$zero

lui	$4,%hi($LC11) # high
jal	say
addiu	$4,$4,%lo($LC11) # low

lw	$4,56($17)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC12) # high
jal	say
addiu	$4,$4,%lo($LC12) # low

lw	$4,32($17)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC13) # high
jal	say
addiu	$4,$4,%lo($LC13) # low

$L117:
addu	$16,$16,1
slt	$2,$16,3
bne	$2,$0,$L126
sll	$2,$16,2

j	$L124
addu	$4,$sp,64

$L107:
lui	$4,%hi($LC20) # high
addiu	$4,$4,%lo($LC20) # low
addu	$5,$20,$zero
jal	report
addu	$6,$18,$zero

sltu	$2,$0,$18
$L121:
lw	$31,124($sp)
#nop # DEBUG: 'lw	$20,120($sp)' does not load from $31
lw	$20,120($sp)
#nop # DEBUG: 'lw	$19,116($sp)' does not load from $20
lw	$19,116($sp)
#nop # DEBUG: 'lw	$18,112($sp)' does not load from $19
lw	$18,112($sp)
#nop # DEBUG: 'lw	$17,108($sp)' does not load from $18
lw	$17,108($sp)
#nop # DEBUG: 'lw	$16,104($sp)' does not load from $17
lw	$16,104($sp)
#nop # DEBUG: 'addu	$sp,$sp,128' does not load from $16
addu	$sp,$sp,128
j	$31
nop  # DEBUG: branch/jump
.end	run_move
.section .rodata
.align	2
$LC21:
.ascii	"shot: usage <spawn-vectors> <move-vectors>\n\000"
.text
.align	2
.globl	main
.ent	main
.set	noreorder
main:
.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, extra= 0
.mask	0x80030000,-8
.fmask	0x00000000,0
subu	$sp,$sp,32
sw	$31,24($sp)
sw	$17,20($sp)
sw	$16,16($sp)
addu	$16,$4,$zero
jal	__main
addu	$17,$5,$zero

ori	$2,$zero,3
bne	$16,$2,$L128
lui	$4,%hi($LC21) # high

lw	$4,4($17)
#nop # DEBUG: 'jal	run_spawn' does not load from $4
jal	run_spawn
nop  # DEBUG: branch/jump
lw	$4,8($17)
#nop # DEBUG: 'jal	run_move' does not load from $4
jal	run_move
addu	$16,$2,$zero

j	$L129
or	$2,$16,$2

$L128:
jal	say
addiu	$4,$4,%lo($LC21) # low

ori	$2,$zero,2
$L129:
lw	$31,24($sp)
#nop # DEBUG: 'lw	$17,20($sp)' does not load from $31
lw	$17,20($sp)
#nop # DEBUG: 'lw	$16,16($sp)' does not load from $17
lw	$16,16($sp)
#nop # DEBUG: 'addu	$sp,$sp,32' does not load from $16
addu	$sp,$sp,32
j	$31
nop  # DEBUG: branch/jump
.end	main



.section .bss
	.globl recorded
	.align	2
recorded:
	.space 96
	.globl recorded_flag
	.align	2
recorded_flag:
	.space 4
	.globl data
	.align	2
data:
	.space 32768
