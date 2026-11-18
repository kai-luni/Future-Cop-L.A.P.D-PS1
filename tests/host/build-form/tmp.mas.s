
.file	1 "/home/kai-luni/git/future-cop-lapd-ps1/tests/host/form_harness.c"


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
.globl	func_800449F8
.ent	func_800449F8
.set	noreorder
func_800449F8:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
j	$31
nop  # DEBUG: branch/jump
.end	func_800449F8
.align	2
.globl	func_800448D8
.ent	func_800448D8
.set	noreorder
func_800448D8:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
lui	$2,%hi(clips) # high
j	$31
addiu	$2,$2,%lo(clips) # low

.end	func_800448D8
.align	2
.globl	func_80044AE8
.ent	func_80044AE8
.set	noreorder
func_80044AE8:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
j	$31
nop  # DEBUG: branch/jump
.end	func_80044AE8
.align	2
.globl	func_80019668
.ent	func_80019668
.set	noreorder
func_80019668:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
j	$31
addu	$2,$0,$zero

.end	func_80019668
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
beq	$6,$0,$L8
addu	$3,$6,-1

addiu	$6,$zero,-1
$L9:
sb	$5,0($7)
addu	$3,$3,-1
bne	$3,$6,$L9
addu	$7,$7,1

$L8:
j	$31
nop  # DEBUG: branch/jump
.end	memset
.align	2
.globl	memcpy
.ent	memcpy
.set	noreorder
memcpy:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
addu	$7,$4,$zero
beq	$6,$0,$L13
addu	$3,$6,-1

addiu	$6,$zero,-1
$L14:
lbu	$2,0($5)
nop # DEBUG: Reuse of '$2'. 'sb	$2,0($7)' does not use $at
sb	$2,0($7)
addu	$5,$5,1
addu	$3,$3,-1
bne	$3,$6,$L14
addu	$7,$7,1

$L13:
j	$31
addu	$2,$4,$zero

.end	memcpy
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
beq	$4,$0,$L18
andi	$2,$3,0x00ff

j	$L21
addu	$2,$2,-48

$L18:
addu	$2,$3,-97
andi	$2,$2,0x00ff
sltu	$2,$2,6
beq	$2,$0,$L19
andi	$2,$3,0x00ff

j	$L21
addu	$2,$2,-87

$L19:
addu	$2,$3,-65
andi	$2,$2,0x00ff
sltu	$2,$2,6
bne	$2,$0,$L20
andi	$2,$3,0x00ff

j	$L21
addiu	$2,$zero,-1

$L20:
addu	$2,$2,-55
$L21:
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
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L24' does not use $at
beq	$2,$0,$L24
addu	$7,$0,$zero

addu	$7,$7,1
$L27:
addu	$2,$6,$7
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L27' does not use $at
bne	$2,$0,$L27
addu	$7,$7,1

addu	$7,$7,-1
$L24:
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
beq	$8,$0,$L29
addu	$7,$0,$zero

subu	$5,$0,$5
$L29:
bne	$5,$0,$L30
addu	$3,$sp,$7

ori	$2,$zero,48
sb	$2,16($3)
addu	$7,$7,1
$L30:
blez	$5,$L32
nop  # DEBUG: branch/jump
addu	$6,$sp,16
lui	$9,(1717960704 >> 16) & 0xFFFF
ori	$9,$9,0x6667
$L33:
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
bgtz	$5,$L33
addu	$7,$7,1

$L32:
beq	$8,$0,$L35
addu	$3,$sp,$7

ori	$2,$zero,45
sb	$2,16($3)
addu	$7,$7,1
$L35:
sra	$2,$7,31
addu	$3,$2,$zero
subu	$2,$7,$2
sra	$2,$2,1
blez	$2,$L37
addu	$5,$0,$zero

addu	$8,$sp,16
subu	$2,$7,$3
sra	$9,$2,1
$L38:
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
bne	$2,$0,$L38
sb	$6,0($3)

$L37:
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
.ent	setClip
.set	noreorder
setClip:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
sb	$0,0($4)
ori	$2,$zero,3
sb	$2,1($4)
sb	$0,2($4)
sb	$0,3($4)
sh	$5,4($4)
sh	$6,6($4)
sb	$0,8($4)
sb	$0,9($4)
sh	$0,10($4)
ori	$2,$zero,10000
j	$31
sw	$2,12($4)

.end	setClip
.align	2
.ent	setTrack
.set	noreorder
setTrack:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
sw	$5,0($4)
sw	$5,4($4)
sw	$0,8($4)
sh	$0,12($4)
sh	$0,14($4)
ori	$2,$zero,1
sb	$2,16($4)
sb	$0,17($4)
sb	$2,18($4)
j	$31
sb	$0,19($4)

.end	setTrack

.section .rodata
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
.set	noreorder
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
addu	$16,$4,$zero
jal	__main
addu	$17,$5,$zero

sw	$0,48($sp)
sw	$0,52($sp)
addu	$4,$sp,16
addu	$5,$0,$zero
jal	memset
ori	$6,$zero,4

ori	$2,$zero,29
sh	$2,24($sp)
sh	$2,26($sp)
ori	$2,$zero,2
beq	$16,$2,$L43
lui	$4,%hi($LC0) # high

j	$L110
addiu	$4,$4,%lo($LC0) # low

$L43:
ori	$4,$zero,4005
lw	$5,4($17)
#nop # DEBUG: 'move	$6,$0' does not load from $5
addu	$6,$0,$zero
jal	sys3
addu	$7,$6,$zero

addu	$16,$2,$zero
bgez	$16,$L44
addu	$5,$16,$zero

lui	$4,%hi($LC1) # high
j	$L110
addiu	$4,$4,%lo($LC1) # low

$L44:
ori	$4,$zero,4003
lui	$6,%hi(data.28) # high
addiu	$6,$6,%lo(data.28) # low
jal	sys3
ori	$7,$zero,32767

addu	$17,$2,$zero
ori	$4,$zero,4006
addu	$5,$16,$zero
addu	$6,$0,$zero
jal	sys3
addu	$7,$6,$zero

bgez	$17,$L45
lui	$4,%hi($LC2) # high

j	$L110
addiu	$4,$4,%lo($LC2) # low

$L45:
lui	$4,%hi(data.28) # high
addiu	$3,$4,%lo(data.28) # low
addu	$2,$17,$3
sb	$0,0($2)
lbu	$2,%lo(data.28)($4) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L47' does not use $at
beq	$2,$0,$L47
addu	$18,$3,$zero

ori	$22,$zero,32
ori	$21,$zero,9
ori	$20,$zero,13
ori	$23,$zero,10
lui	$2,%hi(tracks) # high
addiu	$fp,$2,%lo(tracks) # low
$L111:
lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$22,$L111' does not use $at
beq	$2,$22,$L111
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$21,$L111
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$20,$L111
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$23,$L111
addu	$18,$18,1

addu	$18,$18,-1
lbu	$3,0($18)
nop # DEBUG: Reuse of '$3'. 'beq	$3,$0,$L47' does not use $at
beq	$3,$0,$L47
ori	$2,$zero,35

bne	$3,$2,$L54
ori	$2,$zero,84

lbu	$2,0($18)
$L114:
nop # DEBUG: Reuse of '$2'. 'beq	$2,$23,$L46' does not use $at
# $L114:  # DEBUG: skipped
beq	$2,$23,$L46
nop  # DEBUG: branch/jump
addu	$18,$18,1
lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L47' does not use $at
beq	$2,$0,$L47
nop  # DEBUG: branch/jump
j	$L114
nop  # DEBUG: branch/jump
$L54:
bne	$3,$2,$L115
addu	$19,$0,$zero

lbu	$3,1($18)
#nop # DEBUG: 'li	$2,65			# 0x00000041' does not load from $3
ori	$2,$zero,65
bne	$3,$2,$L113
addu	$17,$0,$zero

lbu	$3,2($18)
#nop # DEBUG: 'li	$2,66			# 0x00000042' does not load from $3
ori	$2,$zero,66
bne	$3,$2,$L113
ori	$2,$zero,76

lbu	$3,3($18)
nop # DEBUG: Reuse of '$3'. 'bne	$3,$2,$L113' does not use $at
bne	$3,$2,$L113
ori	$2,$zero,69

lbu	$3,4($18)
nop # DEBUG: Reuse of '$3'. 'bne	$3,$2,$L113' does not use $at
bne	$3,$2,$L113
nop  # DEBUG: branch/jump
addu	$18,$18,5
$L112:
lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$22,$L112' does not use $at
beq	$2,$22,$L112
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$21,$L112
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$20,$L112
addu	$18,$18,1

addu	$18,$18,-1
lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$23,$L117' does not use $at
beq	$2,$23,$L117
lui	$4,%hi($LC3) # high

bne	$2,$0,$L69
addu	$16,$0,$zero

lui	$4,%hi($LC3) # high
$L117:
j	$L110
addiu	$4,$4,%lo($LC3) # low

$L69:
$L74:
lbu	$4,0($18)
#nop # DEBUG: 'jal	hex' does not load from $4
jal	hex
addu	$18,$18,1

addu	$3,$2,$zero
bltz	$3,$L107
sll	$2,$17,4

or	$17,$2,$3
addu	$16,$16,1
slt	$2,$16,8
bne	$2,$0,$L74
nop  # DEBUG: branch/jump
bne	$19,$0,$L77
ori	$2,$zero,1

j	$L63
sh	$17,16($sp)

$L77:
bne	$19,$2,$L79
ori	$2,$zero,2

j	$L63
sh	$17,24($sp)

$L79:
bne	$19,$2,$L81
nop  # DEBUG: branch/jump
j	$L63
sh	$17,18($sp)

$L81:
sh	$17,26($sp)
$L63:
addu	$19,$19,1
slt	$2,$19,4
bne	$2,$0,$L112
addu	$17,$0,$zero

lui	$4,%hi(clips) # high
lh	$5,16($sp)
#nop # DEBUG: 'lh	$6,24($sp)' does not load from $5
lh	$6,24($sp)
#nop # DEBUG: 'jal	setClip' does not load from $6
jal	setClip
addiu	$4,$4,%lo(clips) # low

lui	$4,%hi(clips+16) # high
lh	$5,18($sp)
#nop # DEBUG: 'lh	$6,26($sp)' does not load from $5
lh	$6,26($sp)
#nop # DEBUG: 'jal	setClip' does not load from $6
jal	setClip
addiu	$4,$4,%lo(clips+16) # low

j	$L46
nop  # DEBUG: branch/jump
$L115:
addu	$17,$0,$zero
$L113:
lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$22,$L113' does not use $at
beq	$2,$22,$L113
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$21,$L113
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$20,$L113
addu	$18,$18,1

addu	$18,$18,-1
lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$23,$L85' does not use $at
beq	$2,$23,$L85
nop  # DEBUG: branch/jump
beq	$2,$0,$L116
slt	$2,$19,4

addu	$16,$0,$zero
$L97:
lbu	$4,0($18)
#nop # DEBUG: 'jal	hex' does not load from $4
jal	hex
addu	$18,$18,1

addu	$3,$2,$zero
bltz	$3,$L108
sll	$2,$17,4

or	$17,$2,$3
addu	$16,$16,1
slt	$2,$16,8
bne	$2,$0,$L97
sll	$2,$19,2

addu	$2,$sp,$2
sw	$17,32($2)
addu	$19,$19,1
slt	$2,$19,4
bne	$2,$0,$L115
nop  # DEBUG: branch/jump
$L85:
slt	$2,$19,4
$L116:
bne	$2,$0,$L109
lui	$5,%hi(clips) # high

lw	$8,52($sp)
nop # DEBUG: Reuse of '$8'. 'addu	$8,$8,1' does not use $at
addu	$8,$8,1
sw	$8,52($sp)
lui	$8,%hi(tracks) # high
addiu	$4,$8,%lo(tracks) # low
jal	setTrack
addiu	$5,$5,%lo(clips) # low

lui	$16,%hi(tracks+20) # high
addiu	$4,$16,%lo(tracks+20) # low
lui	$5,%hi(clips+16) # high
jal	setTrack
addiu	$5,$5,%lo(clips+16) # low

addu	$4,$0,$zero
lui	$8,%hi(tracks) # high
addiu	$5,$8,%lo(tracks) # low
lw	$6,36($sp)
#nop # DEBUG: 'jal	func_80044B34' does not load from $6
jal	func_80044B34
addu	$7,$4,$zero

addu	$4,$0,$zero
addiu	$5,$16,%lo(tracks+20) # low
lw	$6,36($sp)
#nop # DEBUG: 'jal	func_80044B34' does not load from $6
jal	func_80044B34
addu	$7,$4,$zero

lh	$3,12($fp)
#nop # DEBUG: 'lh	$2,40($sp)' does not load from $3
lh	$2,40($sp)
nop # DEBUG: Reuse of '$2'. 'bne	$3,$2,$L103' does not use $at
bne	$3,$2,$L103
nop  # DEBUG: branch/jump
lh	$3,32($fp)
#nop # DEBUG: 'lh	$2,44($sp)' does not load from $3
lh	$2,44($sp)
nop # DEBUG: Reuse of '$2'. 'beq	$3,$2,$L46' does not use $at
beq	$3,$2,$L46
nop  # DEBUG: branch/jump
$L103:
lw	$8,48($sp)
nop # DEBUG: Reuse of '$8'. 'addu	$8,$8,1' does not use $at
addu	$8,$8,1
sw	$8,48($sp)
lui	$4,%hi($LC7) # high
jal	say
addiu	$4,$4,%lo($LC7) # low

lw	$4,32($sp)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
lui	$16,%hi($LC10) # high

lui	$4,%hi($LC8) # high
jal	say
addiu	$4,$4,%lo($LC8) # low

lw	$4,36($sp)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC9) # high
jal	say
addiu	$4,$4,%lo($LC9) # low

lh	$4,12($fp)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
jal	say
addiu	$4,$16,%lo($LC10) # low

lh	$4,40($sp)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC11) # high
jal	say
addiu	$4,$4,%lo($LC11) # low

lh	$4,32($fp)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
jal	say
addiu	$4,$16,%lo($LC10) # low

lh	$4,44($sp)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC12) # high
jal	say
addiu	$4,$4,%lo($LC12) # low

$L46:
lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L111' does not use $at
bne	$2,$0,$L111
nop  # DEBUG: branch/jump
$L47:
lw	$8,48($sp)
nop # DEBUG: Reuse of '$8'. 'beq	$8,$0,$L105' does not use $at
beq	$8,$0,$L105
lui	$4,%hi($LC13) # high

lui	$4,%hi($LC15) # high
jal	say
addiu	$4,$4,%lo($LC15) # low

lw	$4,48($sp)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC16) # high
jal	say
addiu	$4,$4,%lo($LC16) # low

lw	$4,52($sp)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC12) # high
jal	say
addiu	$4,$4,%lo($LC12) # low

j	$L106
ori	$2,$zero,1

$L107:
lui	$4,%hi($LC4) # high
j	$L110
addiu	$4,$4,%lo($LC4) # low

$L108:
lui	$4,%hi($LC5) # high
j	$L110
addiu	$4,$4,%lo($LC5) # low

$L109:
lui	$4,%hi($LC6) # high
addiu	$4,$4,%lo($LC6) # low
$L110:
jal	say
nop  # DEBUG: branch/jump
j	$L106
ori	$2,$zero,2

$L105:
jal	say
addiu	$4,$4,%lo($LC13) # low

lw	$4,52($sp)
#nop # DEBUG: 'jal	num' does not load from $4
jal	num
nop  # DEBUG: branch/jump
lui	$4,%hi($LC14) # high
jal	say
addiu	$4,$4,%lo($LC14) # low

addu	$2,$0,$zero
$L106:
lw	$31,92($sp)
#nop # DEBUG: 'lw	$fp,88($sp)' does not load from $31
lw	$fp,88($sp)
#nop # DEBUG: 'lw	$23,84($sp)' does not load from $fp
lw	$23,84($sp)
#nop # DEBUG: 'lw	$22,80($sp)' does not load from $23
lw	$22,80($sp)
#nop # DEBUG: 'lw	$21,76($sp)' does not load from $22
lw	$21,76($sp)
#nop # DEBUG: 'lw	$20,72($sp)' does not load from $21
lw	$20,72($sp)
#nop # DEBUG: 'lw	$19,68($sp)' does not load from $20
lw	$19,68($sp)
#nop # DEBUG: 'lw	$18,64($sp)' does not load from $19
lw	$18,64($sp)
#nop # DEBUG: 'lw	$17,60($sp)' does not load from $18
lw	$17,60($sp)
#nop # DEBUG: 'lw	$16,56($sp)' does not load from $17
lw	$16,56($sp)
#nop # DEBUG: 'addu	$sp,$sp,96' does not load from $16
addu	$sp,$sp,96
j	$31
nop  # DEBUG: branch/jump
.end	main


.section .bss
	.globl data.28
	.align	2
data.28:
	.space 32768
	.globl clips
	.align	2
clips:
	.space 32
	.globl tracks
	.align	2
tracks:
	.space 40
