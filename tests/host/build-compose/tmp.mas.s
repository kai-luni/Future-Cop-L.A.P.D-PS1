
.file	1 "/home/kai-luni/git/future-cop-lapd-ps1/tests/host/compose_harness.c"


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
.ent	clamp
.set	noreorder
clamp:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
bgtz	$5,$L8
ori	$2,$zero,32767

bne	$5,$0,$L9
slt	$2,$5,-1

ori	$2,$zero,32767
sltu	$2,$2,$4
bne	$2,$0,$L7
ori	$2,$zero,32767

slt	$2,$5,-1
$L9:
bne	$2,$0,$L8
addiu	$2,$zero,-32768 & 0xFFFF

addiu	$2,$zero,-1
bne	$5,$2,$L8
addu	$2,$4,$zero

li	$at,-32768
sltu	$2,$4,$at
bne	$2,$0,$L7
addiu	$2,$zero,-32768 & 0xFFFF

addu	$2,$4,$zero
$L7:
$L8:
j	$31
nop  # DEBUG: branch/jump
.end	clamp
.align	2
.globl	func_80010820
.ent	func_80010820
.set	noreorder
func_80010820:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
addu	$7,$0,$zero
lui	$2,%hi(r) # high
addiu	$10,$2,%lo(r) # low
ori	$9,$zero,1
$L14:
addu	$6,$0,$zero
sll	$2,$7,1
addu	$8,$2,$7
sll	$2,$6,1
$L24:
addu	$2,$2,$6
addu	$2,$2,$7
sll	$2,$2,2
addu	$2,$2,$4
lw	$3,0($2)
#nop # DEBUG: 'addu	$2,$8,$6' does not load from $3
addu	$2,$8,$6
sll	$2,$2,1
bne	$7,$9,$L19
addu	$5,$2,$10

subu	$2,$0,$3
j	$L23
sra	$2,$2,4

$L19:
sra	$2,$3,4
$L23:
sh	$2,0($5)
addu	$6,$6,1
slt	$2,$6,3
bne	$2,$0,$L24
sll	$2,$6,1

addu	$7,$7,1
slt	$2,$7,3
bne	$2,$0,$L14
nop  # DEBUG: branch/jump
j	$31
nop  # DEBUG: branch/jump
.end	func_80010820
.align	2
.globl	func_8001006C
.ent	func_8001006C
.set	noreorder
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
addu	$fp,$5,$zero
addu	$23,$6,$zero
blez	$23,$L27
addu	$17,$0,$zero

lui	$2,%hi(r) # high
addiu	$21,$2,%lo(r) # low
lui	$2,%hi(r+4) # high
addiu	$22,$2,%lo(r+4) # low
sll	$3,$17,1
$L40:
addu	$3,$3,$17
sll	$3,$3,2
lw	$11,56($sp)
nop # DEBUG: Reuse of '$11'. 'addu	$3,$3,$11' does not use $at
addu	$3,$3,$11
lw	$2,0($3)
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,12' does not use $at
sll	$2,$2,12
sra	$20,$2,16
lw	$2,4($3)
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,12' does not use $at
sll	$2,$2,12
sra	$19,$2,16
lw	$2,8($3)
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,12' does not use $at
sll	$2,$2,12
sra	$18,$2,16
addu	$16,$0,$zero
sll	$10,$16,1
$L39:
addu	$10,$10,$16
sll	$10,$10,1
addu	$2,$10,$21
lhu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,16' does not use $at
sll	$2,$2,16
sra	$6,$2,16
sra	$7,$2,31
addu	$2,$20,$zero
sra	$3,$20,31
multu	$6,$2
mfhi	$5
mflo	$4
nop
nop
mult	$6,$3
# #nop  # DEBUG: skipped
# #nop  # DEBUG: skipped
# mult	$6,$3  # DEBUG: skipped
mflo	$8
addu	$5,$5,$8
nop  # DEBUG: mflo/mfhi with mult/div/rem and 1 instruction
mult	$2,$7
# #nop  # DEBUG: skipped
# #nop  # DEBUG: skipped
# addu	$5,$5,$8  # DEBUG: skipped
# mult	$2,$7  # DEBUG: skipped
mflo	$3
addu	$5,$5,$3
addu	$2,$21,$10
lhu	$2,2($2)
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,16' does not use $at
sll	$2,$2,16
sra	$8,$2,16
sra	$9,$2,31
addu	$6,$19,$zero
sra	$7,$19,31
multu	$8,$6
mfhi	$3
mflo	$2
nop
nop
mult	$8,$7
# #nop  # DEBUG: skipped
# #nop  # DEBUG: skipped
# mult	$8,$7  # DEBUG: skipped
mflo	$12
addu	$3,$3,$12
nop  # DEBUG: mflo/mfhi with mult/div/rem and 1 instruction
mult	$6,$9
# #nop  # DEBUG: skipped
# #nop  # DEBUG: skipped
# addu	$3,$3,$12  # DEBUG: skipped
# mult	$6,$9  # DEBUG: skipped
mflo	$7
addu	$3,$3,$7
addu	$4,$4,$2
sltu	$6,$4,$2
addu	$5,$5,$3
addu	$5,$5,$6
addu	$10,$10,$22
lhu	$2,0($10)
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,16' does not use $at
sll	$2,$2,16
sra	$8,$2,16
sra	$9,$2,31
addu	$6,$18,$zero
sra	$7,$18,31
multu	$8,$6
mfhi	$3
mflo	$2
nop
nop
mult	$8,$7
# #nop  # DEBUG: skipped
# #nop  # DEBUG: skipped
# mult	$8,$7  # DEBUG: skipped
mflo	$10
addu	$3,$3,$10
nop  # DEBUG: mflo/mfhi with mult/div/rem and 1 instruction
mult	$6,$9
# #nop  # DEBUG: skipped
# #nop  # DEBUG: skipped
# addu	$3,$3,$10  # DEBUG: skipped
# mult	$6,$9  # DEBUG: skipped
mflo	$7
addu	$3,$3,$7
addu	$4,$4,$2
sltu	$6,$4,$2
addu	$5,$5,$3
addu	$5,$5,$6
srl	$4,$4,12
sll	$2,$5,20
or	$4,$4,$2
jal	clamp
sra	$5,$5,12

addu	$4,$2,$zero
sll	$2,$17,1
addu	$2,$2,$17
addu	$2,$2,$16
sll	$2,$2,2
addu	$3,$2,$fp
ori	$2,$zero,1
bne	$16,$2,$L34
subu	$2,$0,$4

j	$L38
sll	$2,$2,4

$L34:
sll	$2,$4,4
$L38:
sw	$2,0($3)
addu	$16,$16,1
slt	$2,$16,3
bne	$2,$0,$L39
sll	$10,$16,1

addu	$17,$17,1
slt	$2,$17,$23
bne	$2,$0,$L40
sll	$3,$17,1

$L27:
lw	$31,52($sp)
#nop # DEBUG: 'lw	$fp,48($sp)' does not load from $31
lw	$fp,48($sp)
#nop # DEBUG: 'lw	$23,44($sp)' does not load from $fp
lw	$23,44($sp)
#nop # DEBUG: 'lw	$22,40($sp)' does not load from $23
lw	$22,40($sp)
#nop # DEBUG: 'lw	$21,36($sp)' does not load from $22
lw	$21,36($sp)
#nop # DEBUG: 'lw	$20,32($sp)' does not load from $21
lw	$20,32($sp)
#nop # DEBUG: 'lw	$19,28($sp)' does not load from $20
lw	$19,28($sp)
#nop # DEBUG: 'lw	$18,24($sp)' does not load from $19
lw	$18,24($sp)
#nop # DEBUG: 'lw	$17,20($sp)' does not load from $18
lw	$17,20($sp)
#nop # DEBUG: 'lw	$16,16($sp)' does not load from $17
lw	$16,16($sp)
#nop # DEBUG: 'addu	$sp,$sp,56' does not load from $16
addu	$sp,$sp,56
j	$31
nop  # DEBUG: branch/jump
.end	func_8001006C
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
beq	$4,$0,$L43
andi	$2,$3,0x00ff

j	$L46
addu	$2,$2,-48

$L43:
addu	$2,$3,-97
andi	$2,$2,0x00ff
sltu	$2,$2,6
beq	$2,$0,$L44
andi	$2,$3,0x00ff

j	$L46
addu	$2,$2,-87

$L44:
addu	$2,$3,-65
andi	$2,$2,0x00ff
sltu	$2,$2,6
bne	$2,$0,$L45
andi	$2,$3,0x00ff

j	$L46
addiu	$2,$zero,-1

$L45:
addu	$2,$2,-55
$L46:
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
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L49' does not use $at
beq	$2,$0,$L49
addu	$7,$0,$zero

addu	$7,$7,1
$L52:
addu	$2,$6,$7
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L52' does not use $at
bne	$2,$0,$L52
addu	$7,$7,1

addu	$7,$7,-1
$L49:
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
.set	noreorder
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
addu	$16,$4,$zero
jal	__main
addu	$17,$5,$zero

ori	$2,$zero,2
beq	$16,$2,$L54
lui	$4,%hi($LC0) # high

j	$L136
addiu	$4,$4,%lo($LC0) # low

$L54:
ori	$4,$zero,4005
lw	$5,4($17)
#nop # DEBUG: 'move	$6,$0' does not load from $5
addu	$6,$0,$zero
jal	sys3
addu	$7,$6,$zero

addu	$16,$2,$zero
bgez	$16,$L55
addu	$5,$16,$zero

lui	$4,%hi($LC1) # high
j	$L136
addiu	$4,$4,%lo($LC1) # low

$L55:
ori	$4,$zero,4003
lui	$6,%hi(data.16) # high
addiu	$6,$6,%lo(data.16) # low
jal	sys3
ori	$7,$zero,32767

addu	$17,$2,$zero
ori	$4,$zero,4006
addu	$5,$16,$zero
addu	$6,$0,$zero
jal	sys3
addu	$7,$6,$zero

bgez	$17,$L56
lui	$2,%hi(data.16) # high

lui	$4,%hi($LC2) # high
j	$L136
addiu	$4,$4,%lo($LC2) # low

$L56:
addiu	$2,$2,%lo(data.16) # low
addu	$3,$17,$2
sb	$0,0($3)
addu	$18,$2,$zero
sw	$0,296($sp)
sw	$0,300($sp)
addu	$23,$0,$zero
addu	$17,$23,$zero
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
bne	$2,$0,$L143
sll	$2,$17,1

lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L63' does not use $at
beq	$2,$0,$L63
nop  # DEBUG: branch/jump
ori	$22,$zero,32
ori	$21,$zero,9
ori	$fp,$zero,13
j	$L147
addu	$20,$sp,16

$L137:
lbu	$2,0($18)
$L147:
nop # DEBUG: Reuse of '$2'. 'beq	$2,$22,$L137' does not use $at
# $L147:  # DEBUG: skipped
beq	$2,$22,$L137
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$21,$L137
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$fp,$L137
addu	$18,$18,1

addu	$18,$18,-1
ori	$8,$zero,10
beq	$2,$8,$L137
addu	$18,$18,1

addu	$18,$18,-1
lbu	$3,0($18)
nop # DEBUG: Reuse of '$3'. 'beq	$3,$0,$L63' does not use $at
beq	$3,$0,$L63
ori	$2,$zero,35

bne	$3,$2,$L70
ori	$2,$zero,84

lbu	$2,0($18)
#nop # DEBUG: 'li	$8,10			# 0x0000000a' does not load from $2
ori	$8,$zero,10
$L142:
beq	$2,$8,$L62
nop  # DEBUG: branch/jump
addu	$18,$18,1
lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L63' does not use $at
beq	$2,$0,$L63
nop  # DEBUG: branch/jump
j	$L142
nop  # DEBUG: branch/jump
$L70:
bne	$3,$2,$L140
addu	$17,$0,$zero

lbu	$3,1($18)
#nop # DEBUG: 'li	$2,65			# 0x00000041' does not load from $3
ori	$2,$zero,65
bne	$3,$2,$L140
ori	$2,$zero,66

lbu	$3,2($18)
nop # DEBUG: Reuse of '$3'. 'bne	$3,$2,$L140' does not use $at
bne	$3,$2,$L140
ori	$2,$zero,76

lbu	$3,3($18)
nop # DEBUG: Reuse of '$3'. 'bne	$3,$2,$L140' does not use $at
bne	$3,$2,$L140
ori	$2,$zero,69

lbu	$3,4($18)
nop # DEBUG: Reuse of '$3'. 'bne	$3,$2,$L140' does not use $at
bne	$3,$2,$L140
nop  # DEBUG: branch/jump
addu	$18,$18,5
$L139:
lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L78' does not use $at
beq	$2,$0,$L78
ori	$8,$zero,10

beq	$2,$8,$L78
addu	$17,$0,$zero

j	$L138
nop  # DEBUG: branch/jump
$L144:
lbu	$2,0($18)
$L138:
nop # DEBUG: Reuse of '$2'. 'beq	$2,$22,$L144' does not use $at
# $L138:  # DEBUG: skipped
beq	$2,$22,$L144
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$21,$L144
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$fp,$L144
addu	$18,$18,1

addu	$18,$18,-1
lbu	$2,0($18)
#nop # DEBUG: 'li	$8,10			# 0x0000000a' does not load from $2
ori	$8,$zero,10
beq	$2,$8,$L78
nop  # DEBUG: branch/jump
beq	$2,$0,$L78
addu	$16,$0,$zero

$L90:
lbu	$4,0($18)
#nop # DEBUG: 'jal	hex' does not load from $4
jal	hex
addu	$18,$18,1

addu	$3,$2,$zero
bltz	$3,$L132
sll	$2,$17,4

or	$17,$2,$3
addu	$16,$16,1
slt	$2,$16,8
bne	$2,$0,$L90
slt	$2,$23,64

beq	$2,$0,$L139
sll	$2,$23,1

addu	$2,$2,$20
sh	$17,128($2)
j	$L139
addu	$23,$23,1

$L78:
bne	$23,$0,$L62
nop  # DEBUG: branch/jump
lui	$4,%hi($LC4) # high
j	$L136
addiu	$4,$4,%lo($LC4) # low

$L135:
lw	$8,300($sp)
nop # DEBUG: Reuse of '$8'. 'addu	$8,$8,1' does not use $at
addu	$8,$8,1
j	$L122
sw	$8,300($sp)

$L140:
lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$22,$L140' does not use $at
beq	$2,$22,$L140
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$21,$L140
addu	$18,$18,1

addu	$18,$18,-1
sll	$2,$17,2
addu	$2,$2,$20
sw	$0,0($2)
addu	$16,$0,$zero
sll	$2,$17,2
addu	$19,$2,$20
$L107:
lbu	$4,0($18)
#nop # DEBUG: 'jal	hex' does not load from $4
jal	hex
addu	$18,$18,1

addu	$3,$2,$zero
bltz	$3,$L133
addu	$16,$16,1

lw	$2,0($19)
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,4' does not use $at
sll	$2,$2,4
or	$2,$2,$3
sw	$2,0($19)
slt	$2,$16,8
bne	$2,$0,$L107
nop  # DEBUG: branch/jump
addu	$17,$17,1
slt	$2,$17,32
bne	$2,$0,$L140
nop  # DEBUG: branch/jump
$L141:
lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$22,$L141' does not use $at
beq	$2,$22,$L141
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$21,$L141
addu	$18,$18,1

addu	$18,$18,-1
beq	$2,$fp,$L141
addu	$18,$18,1

addu	$18,$18,-1
lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L115' does not use $at
beq	$2,$0,$L115
ori	$8,$zero,10

bne	$2,$8,$L134
lui	$4,%hi($LC6) # high

$L115:
addu	$17,$0,$zero
lui	$2,%hi(D_800A0300) # high
addiu	$4,$2,%lo(D_800A0300) # low
sll	$2,$17,2
$L145:
addu	$3,$2,$4
addu	$2,$2,$20
lw	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'sw	$2,0($3)' does not use $at
sw	$2,0($3)
addu	$17,$17,1
slt	$2,$17,12
bne	$2,$0,$L145
sll	$2,$17,2

sh	$0,276($sp)
sh	$0,274($sp)
sh	$0,272($sp)
sb	$0,278($sp)
lbu	$2,64($sp)
nop # DEBUG: Reuse of '$2'. 'sb	$2,279($sp)' does not use $at
sb	$2,279($sp)
lw	$7,68($sp)
#nop # DEBUG: 'lhu	$2,72($sp)' does not load from $7
lhu	$2,72($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,280($sp)' does not use $at
sh	$2,280($sp)
lhu	$2,76($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,282($sp)' does not use $at
sh	$2,282($sp)
lhu	$2,80($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,284($sp)' does not use $at
sh	$2,284($sp)
lhu	$2,84($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,286($sp)' does not use $at
sh	$2,286($sp)
lhu	$2,88($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,288($sp)' does not use $at
sh	$2,288($sp)
lhu	$2,92($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,290($sp)' does not use $at
sh	$2,290($sp)
ori	$4,$zero,1
addu	$5,$sp,272
jal	func_80013CB8
addu	$6,$sp,144

addu	$4,$0,$zero
lui	$2,%hi(D_800A0300) # high
addiu	$5,$2,%lo(D_800A0300) # low
sll	$2,$4,2
$L146:
addu	$3,$2,$5
addu	$2,$2,$20
lw	$3,48($3)
#nop # DEBUG: 'lw	$2,80($2)' does not load from $3
lw	$2,80($2)
nop # DEBUG: Reuse of '$2'. 'bne	$3,$2,$L135' does not use $at
bne	$3,$2,$L135
addu	$4,$4,1

slt	$2,$4,12
bne	$2,$0,$L146
sll	$2,$4,2

$L122:
lw	$8,296($sp)
nop # DEBUG: Reuse of '$8'. 'addu	$8,$8,1' does not use $at
addu	$8,$8,1
sw	$8,296($sp)
$L62:
lbu	$2,0($18)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L137' does not use $at
bne	$2,$0,$L137
nop  # DEBUG: branch/jump
$L63:
lw	$8,296($sp)
nop # DEBUG: Reuse of '$8'. 'beq	$8,$0,$L129' does not use $at
beq	$8,$0,$L129
nop  # DEBUG: branch/jump
lw	$8,300($sp)
nop # DEBUG: Reuse of '$8'. 'beq	$8,$0,$L128' does not use $at
beq	$8,$0,$L128
lui	$4,%hi($LC8) # high

$L129:
lui	$4,%hi($LC7) # high
jal	say
addiu	$4,$4,%lo($LC7) # low

j	$L130
ori	$2,$zero,1

$L132:
lui	$4,%hi($LC3) # high
j	$L136
addiu	$4,$4,%lo($LC3) # low

$L133:
lui	$4,%hi($LC5) # high
j	$L136
addiu	$4,$4,%lo($LC5) # low

$L134:
addiu	$4,$4,%lo($LC6) # low
$L136:
jal	say
nop  # DEBUG: branch/jump
j	$L130
ori	$2,$zero,2

$L128:
jal	say
addiu	$4,$4,%lo($LC8) # low

addu	$2,$0,$zero
$L130:
lw	$31,340($sp)
#nop # DEBUG: 'lw	$fp,336($sp)' does not load from $31
lw	$fp,336($sp)
#nop # DEBUG: 'lw	$23,332($sp)' does not load from $fp
lw	$23,332($sp)
#nop # DEBUG: 'lw	$22,328($sp)' does not load from $23
lw	$22,328($sp)
#nop # DEBUG: 'lw	$21,324($sp)' does not load from $22
lw	$21,324($sp)
#nop # DEBUG: 'lw	$20,320($sp)' does not load from $21
lw	$20,320($sp)
#nop # DEBUG: 'lw	$19,316($sp)' does not load from $20
lw	$19,316($sp)
#nop # DEBUG: 'lw	$18,312($sp)' does not load from $19
lw	$18,312($sp)
#nop # DEBUG: 'lw	$17,308($sp)' does not load from $18
lw	$17,308($sp)
#nop # DEBUG: 'lw	$16,304($sp)' does not load from $17
lw	$16,304($sp)
#nop # DEBUG: 'addu	$sp,$sp,344' does not load from $16
addu	$sp,$sp,344
j	$31
nop  # DEBUG: branch/jump
.end	main


.section .bss
	.globl data.16
	.align	2
data.16:
	.space 32768
	.globl D_800A0300
	.align	2
D_800A0300:
	.space 384
	.globl r
	.align	2
r:
	.space 18
