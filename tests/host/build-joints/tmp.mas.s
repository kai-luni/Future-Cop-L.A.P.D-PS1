
.file	1 "/home/kai-luni/git/future-cop-lapd-ps1/tests/host/joints_probe.c"


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
$L6:
addu	$6,$0,$zero
sll	$2,$7,1
addu	$8,$2,$7
sll	$2,$6,1
$L16:
addu	$2,$2,$6
addu	$2,$2,$7
sll	$2,$2,2
addu	$2,$2,$4
lw	$3,0($2)
#nop # DEBUG: 'addu	$2,$8,$6' does not load from $3
addu	$2,$8,$6
sll	$2,$2,1
bne	$7,$9,$L11
addu	$5,$2,$10

subu	$2,$0,$3
j	$L15
sra	$2,$2,4

$L11:
sra	$2,$3,4
$L15:
sh	$2,0($5)
addu	$6,$6,1
slt	$2,$6,3
bne	$2,$0,$L16
sll	$2,$6,1

addu	$7,$7,1
slt	$2,$7,3
bne	$2,$0,$L6
nop  # DEBUG: branch/jump
j	$31
nop  # DEBUG: branch/jump
.end	func_80010820
.align	2
.globl	func_8001006C
.ent	func_8001006C
.set	noreorder
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
addu	$22,$4,$zero
addu	$21,$5,$zero
addu	$20,$6,$zero
blez	$20,$L19
addu	$12,$0,$zero

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
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,12' does not use $at
sll	$2,$2,12
sra	$24,$2,16
lw	$2,4($3)
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,12' does not use $at
sll	$2,$2,12
sra	$15,$2,16
lw	$2,8($3)
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,12' does not use $at
sll	$2,$2,12
sra	$14,$2,16
addu	$11,$0,$zero
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
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,16' does not use $at
sll	$2,$2,16
sra	$4,$2,16
sra	$5,$2,31
addu	$2,$24,$zero
addu	$3,$17,$zero
multu	$4,$2
mfhi	$9
mflo	$8
nop
nop
mult	$4,$3
# #nop  # DEBUG: skipped
# #nop  # DEBUG: skipped
# mult	$4,$3  # DEBUG: skipped
mflo	$7
addu	$9,$9,$7
nop  # DEBUG: mflo/mfhi with mult/div/rem and 1 instruction
mult	$2,$5
# #nop  # DEBUG: skipped
# #nop  # DEBUG: skipped
# addu	$9,$9,$7  # DEBUG: skipped
# mult	$2,$5  # DEBUG: skipped
mflo	$2
addu	$9,$9,$2
addu	$2,$18,$10
lhu	$2,2($2)
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,16' does not use $at
sll	$2,$2,16
sra	$6,$2,16
sra	$7,$2,31
addu	$4,$15,$zero
addu	$5,$16,$zero
multu	$6,$4
mfhi	$3
mflo	$2
nop
nop
mult	$6,$5
# #nop  # DEBUG: skipped
# #nop  # DEBUG: skipped
# mult	$6,$5  # DEBUG: skipped
mflo	$23
sw	$23,0($sp)
addu	$3,$3,$23
mult	$4,$7
mflo	$7
addu	$3,$3,$7
addu	$8,$8,$2
sltu	$4,$8,$2
addu	$9,$9,$3
addu	$9,$9,$4
addu	$10,$10,$19
lhu	$2,0($10)
nop # DEBUG: Reuse of '$2'. 'sll	$2,$2,16' does not use $at
sll	$2,$2,16
sra	$6,$2,16
sra	$7,$2,31
addu	$4,$14,$zero
addu	$5,$25,$zero
multu	$6,$4
mfhi	$3
mflo	$2
nop
nop
mult	$6,$5
# #nop  # DEBUG: skipped
# #nop  # DEBUG: skipped
# mult	$6,$5  # DEBUG: skipped
mflo	$10
addu	$3,$3,$10
nop  # DEBUG: mflo/mfhi with mult/div/rem and 1 instruction
mult	$4,$7
# #nop  # DEBUG: skipped
# #nop  # DEBUG: skipped
# addu	$3,$3,$10  # DEBUG: skipped
# mult	$4,$7  # DEBUG: skipped
mflo	$7
addu	$3,$3,$7
addu	$8,$8,$2
sltu	$4,$8,$2
addu	$9,$9,$3
addu	$9,$9,$4
srl	$8,$8,12
sll	$2,$9,20
or	$8,$8,$2
sra	$9,$9,12
ori	$2,$zero,32767
slt	$2,$2,$8
beq	$2,$0,$L33
slt	$2,$8,-32768

ori	$8,$zero,32767
slt	$2,$8,-32768
$L33:
beq	$2,$0,$L34
addu	$2,$13,$11

addiu	$8,$zero,-32768 & 0xFFFF
$L34:
sll	$2,$2,2
addu	$3,$2,$21
ori	$2,$zero,1
bne	$11,$2,$L28
subu	$2,$0,$8

j	$L32
sll	$2,$2,4

$L28:
sll	$2,$8,4
$L32:
sw	$2,0($3)
addu	$11,$11,1
slt	$2,$11,3
bne	$2,$0,$L35
sll	$10,$11,1

addu	$12,$12,1
slt	$2,$12,$20
bne	$2,$0,$L36
sll	$3,$12,1

$L19:
lw	$23,36($sp)
#nop # DEBUG: 'lw	$22,32($sp)' does not load from $23
lw	$22,32($sp)
#nop # DEBUG: 'lw	$21,28($sp)' does not load from $22
lw	$21,28($sp)
#nop # DEBUG: 'lw	$20,24($sp)' does not load from $21
lw	$20,24($sp)
#nop # DEBUG: 'lw	$19,20($sp)' does not load from $20
lw	$19,20($sp)
#nop # DEBUG: 'lw	$18,16($sp)' does not load from $19
lw	$18,16($sp)
#nop # DEBUG: 'lw	$17,12($sp)' does not load from $18
lw	$17,12($sp)
#nop # DEBUG: 'lw	$16,8($sp)' does not load from $17
lw	$16,8($sp)
#nop # DEBUG: 'addu	$sp,$sp,40' does not load from $16
addu	$sp,$sp,40
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
.ent	hexval
.set	noreorder
hexval:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
addu	$3,$4,$zero
addu	$4,$4,-48
andi	$4,$4,0x00ff
sltu	$4,$4,10
beq	$4,$0,$L39
andi	$2,$3,0x00ff

j	$L42
addu	$2,$2,-48

$L39:
addu	$2,$3,-97
andi	$2,$2,0x00ff
sltu	$2,$2,6
beq	$2,$0,$L40
andi	$2,$3,0x00ff

j	$L42
addu	$2,$2,-87

$L40:
addu	$2,$3,-65
andi	$2,$2,0x00ff
sltu	$2,$2,6
bne	$2,$0,$L41
andi	$2,$3,0x00ff

j	$L42
addiu	$2,$zero,-1

$L41:
addu	$2,$2,-55
$L42:
j	$31
nop  # DEBUG: branch/jump
.end	hexval
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
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L45' does not use $at
beq	$2,$0,$L45
addu	$7,$0,$zero

addu	$7,$7,1
$L48:
addu	$2,$6,$7
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L48' does not use $at
bne	$2,$0,$L48
addu	$7,$7,1

addu	$7,$7,-1
$L45:
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
.ascii	"0\000"
.text
.align	2
.ent	say_dec
.set	noreorder
say_dec:
.frame	$sp,40,$31		# vars= 16, regs= 1/0, args= 16, extra= 0
.mask	0x80000000,-8
.fmask	0x00000000,0
subu	$sp,$sp,40
sw	$31,32($sp)
addu	$5,$4,$zero
ori	$6,$zero,11
bgtz	$5,$L56
sb	$0,27($sp)

lui	$4,%hi($LC0) # high
j	$L57
addiu	$4,$4,%lo($LC0) # low

$L56:
addu	$7,$sp,16
lui	$8,(1717960704 >> 16) & 0xFFFF
ori	$8,$8,0x6667
$L55:
blez	$6,$L52
mult	$5,$8

addu	$6,$6,-1
addu	$4,$7,$6
mfhi	$9
sra	$3,$9,2
sra	$2,$5,31
subu	$3,$3,$2
sll	$2,$3,2
addu	$2,$2,$3
sll	$2,$2,1
subu	$2,$5,$2
addu	$2,$2,48
addu	$5,$3,$zero
bgtz	$5,$L55
sb	$2,0($4)

$L52:
addu	$4,$sp,16
addu	$4,$4,$6
$L57:
jal	say
nop  # DEBUG: branch/jump
lw	$31,32($sp)
#nop # DEBUG: 'addu	$sp,$sp,40' does not load from $31
addu	$sp,$sp,40
j	$31
nop  # DEBUG: branch/jump
.end	say_dec
.align	2
.ent	hexword
.set	noreorder
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
addu	$17,$4,$zero
addu	$19,$5,$zero
lw	$2,0($17)
nop # DEBUG: Reuse of '$2'. 'lbu	$3,0($2)' does not use $at
lbu	$3,0($2)
#nop # DEBUG: 'j	$L70' does not load from $3
j	$L70
addu	$18,$0,$zero

$L61:
lw	$2,0($17)
nop # DEBUG: Reuse of '$2'. 'addu	$3,$2,1' does not use $at
addu	$3,$2,1
sw	$3,0($17)
lbu	$3,1($2)
#nop # DEBUG: 'li	$2,32			# 0x00000020' does not load from $3
$L70:
ori	$2,$zero,32
beq	$3,$2,$L61
ori	$2,$zero,9

beq	$3,$2,$L61
ori	$2,$zero,13

beq	$3,$2,$L61
addu	$16,$0,$zero

$L66:
lw	$2,0($17)
nop # DEBUG: Reuse of '$2'. 'lbu	$4,0($2)' does not use $at
lbu	$4,0($2)
#nop # DEBUG: 'jal	hexval' does not load from $4
jal	hexval
nop  # DEBUG: branch/jump
addu	$3,$2,$zero
bltz	$3,$L69
addiu	$2,$zero,-1

sll	$2,$18,4
or	$18,$2,$3
lw	$2,0($17)
nop # DEBUG: Reuse of '$2'. 'addu	$2,$2,1' does not use $at
addu	$2,$2,1
sw	$2,0($17)
addu	$16,$16,1
slt	$2,$16,8
bne	$2,$0,$L66
nop  # DEBUG: branch/jump
sw	$18,0($19)
addu	$2,$0,$zero
$L69:
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
.end	hexword



.section .rodata
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
.set	noreorder
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
addu	$16,$4,$zero
jal	__main
addu	$17,$5,$zero

ori	$2,$zero,2
beq	$16,$2,$L72
lui	$4,%hi($LC1) # high

j	$L167
addiu	$4,$4,%lo($LC1) # low

$L72:
ori	$4,$zero,4005
lw	$5,4($17)
#nop # DEBUG: 'move	$6,$0' does not load from $5
addu	$6,$0,$zero
jal	sys3
addu	$7,$6,$zero

addu	$16,$2,$zero
bgez	$16,$L73
addu	$5,$16,$zero

lui	$4,%hi($LC2) # high
j	$L167
addiu	$4,$4,%lo($LC2) # low

$L73:
ori	$4,$zero,4003
lui	$6,%hi(data.18) # high
addiu	$6,$6,%lo(data.18) # low
jal	sys3
ori	$7,$zero,65535

addu	$17,$2,$zero
ori	$4,$zero,4006
addu	$5,$16,$zero
addu	$6,$0,$zero
jal	sys3
addu	$7,$6,$zero

bgez	$17,$L74
addu	$23,$0,$zero

lui	$4,%hi($LC3) # high
j	$L167
addiu	$4,$4,%lo($LC3) # low

$L74:
lui	$4,%hi(data.18) # high
addiu	$3,$4,%lo(data.18) # low
addu	$2,$17,$3
sb	$0,0($2)
sw	$3,736($sp)
sw	$0,744($sp)
lbu	$2,%lo(data.18)($4) # DEBUG: leaving for assembler to expand
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L76' does not use $at
beq	$2,$0,$L76
addu	$18,$23,$zero

ori	$21,$zero,32
ori	$20,$zero,9
ori	$19,$zero,13
ori	$22,$zero,10
addu	$fp,$sp,224
$L77:
lw	$2,736($sp)
nop # DEBUG: Reuse of '$2'. 'lbu	$2,0($2)' does not use $at
lbu	$2,0($2)
#nop # DEBUG: 'j	$L168' does not load from $2
j	$L168
nop  # DEBUG: branch/jump
$L80:
lw	$3,736($sp)
nop # DEBUG: Reuse of '$3'. 'addu	$2,$3,1' does not use $at
addu	$2,$3,1
sw	$2,736($sp)
lbu	$2,1($3)
$L168:
nop # DEBUG: Reuse of '$2'. 'beq	$2,$21,$L80' does not use $at
# $L168:  # DEBUG: skipped
beq	$2,$21,$L80
nop  # DEBUG: branch/jump
beq	$2,$20,$L80
nop  # DEBUG: branch/jump
beq	$2,$19,$L80
nop  # DEBUG: branch/jump
beq	$2,$22,$L80
nop  # DEBUG: branch/jump
lw	$2,736($sp)
nop # DEBUG: Reuse of '$2'. 'lbu	$3,0($2)' does not use $at
lbu	$3,0($2)
nop # DEBUG: Reuse of '$3'. 'beq	$3,$0,$L76' does not use $at
beq	$3,$0,$L76
ori	$2,$zero,35

bne	$3,$2,$L83
ori	$2,$zero,84

$L88:
lw	$3,736($sp)
nop # DEBUG: Reuse of '$3'. 'lbu	$2,0($3)' does not use $at
lbu	$2,0($3)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$22,$L75' does not use $at
beq	$2,$22,$L75
addu	$2,$3,1

sw	$2,736($sp)
lbu	$2,1($3)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L76' does not use $at
beq	$2,$0,$L76
nop  # DEBUG: branch/jump
j	$L88
nop  # DEBUG: branch/jump
$L83:
lw	$4,736($sp)
nop # DEBUG: Reuse of '$4'. 'lbu	$3,0($4)' does not use $at
lbu	$3,0($4)
nop # DEBUG: Reuse of '$3'. 'bne	$3,$2,$L104' does not use $at
bne	$3,$2,$L104
addu	$16,$0,$zero

lbu	$3,1($4)
#nop # DEBUG: 'li	$2,65			# 0x00000041' does not load from $3
ori	$2,$zero,65
bne	$3,$2,$L104
ori	$2,$zero,66

lbu	$3,2($4)
nop # DEBUG: Reuse of '$3'. 'bne	$3,$2,$L104' does not use $at
bne	$3,$2,$L104
ori	$2,$zero,76

lbu	$3,3($4)
nop # DEBUG: Reuse of '$3'. 'bne	$3,$2,$L104' does not use $at
bne	$3,$2,$L104
ori	$2,$zero,69

lbu	$3,4($4)
nop # DEBUG: Reuse of '$3'. 'bne	$3,$2,$L89' does not use $at
bne	$3,$2,$L89
addu	$2,$4,5

sw	$2,736($sp)
lbu	$2,5($4)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L91' does not use $at
beq	$2,$0,$L91
nop  # DEBUG: branch/jump
$L102:
lw	$2,736($sp)
nop # DEBUG: Reuse of '$2'. 'lbu	$2,0($2)' does not use $at
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$22,$L91' does not use $at
beq	$2,$22,$L91
nop  # DEBUG: branch/jump
j	$L169
nop  # DEBUG: branch/jump
$L96:
lw	$3,736($sp)
nop # DEBUG: Reuse of '$3'. 'addu	$2,$3,1' does not use $at
addu	$2,$3,1
sw	$2,736($sp)
lbu	$2,1($3)
$L169:
nop # DEBUG: Reuse of '$2'. 'beq	$2,$21,$L96' does not use $at
# $L169:  # DEBUG: skipped
beq	$2,$21,$L96
nop  # DEBUG: branch/jump
beq	$2,$20,$L96
nop  # DEBUG: branch/jump
beq	$2,$19,$L96
nop  # DEBUG: branch/jump
lw	$2,736($sp)
nop # DEBUG: Reuse of '$2'. 'lbu	$2,0($2)' does not use $at
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$22,$L91' does not use $at
beq	$2,$22,$L91
nop  # DEBUG: branch/jump
beq	$2,$0,$L91
addu	$4,$sp,736

jal	hexword
addu	$5,$sp,740

bne	$2,$0,$L157
slt	$2,$18,64

beq	$2,$0,$L90
sll	$2,$18,1

lui	$8,%hi(table.19) # high
addiu	$8,$8,%lo(table.19) # low
addu	$2,$2,$8
lhu	$3,740($sp)
nop # DEBUG: Reuse of '$3'. 'sh	$3,0($2)' does not use $at
sh	$3,0($2)
addu	$18,$18,1
$L90:
lw	$2,736($sp)
nop # DEBUG: Reuse of '$2'. 'lbu	$2,0($2)' does not use $at
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L102' does not use $at
bne	$2,$0,$L102
nop  # DEBUG: branch/jump
$L91:
bne	$18,$0,$L75
nop  # DEBUG: branch/jump
lui	$4,%hi($LC5) # high
j	$L167
addiu	$4,$4,%lo($LC5) # low

$L165:
j	$L142
addu	$23,$23,1

$L166:
j	$L148
addu	$23,$23,1

$L89:
$L104:
lw	$2,736($sp)
nop # DEBUG: Reuse of '$2'. 'lbu	$2,0($2)' does not use $at
lbu	$2,0($2)
#nop # DEBUG: 'j	$L170' does not load from $2
j	$L170
nop  # DEBUG: branch/jump
$L109:
lw	$3,736($sp)
nop # DEBUG: Reuse of '$3'. 'addu	$2,$3,1' does not use $at
addu	$2,$3,1
sw	$2,736($sp)
lbu	$2,1($3)
$L170:
nop # DEBUG: Reuse of '$2'. 'beq	$2,$21,$L109' does not use $at
# $L170:  # DEBUG: skipped
beq	$2,$21,$L109
nop  # DEBUG: branch/jump
beq	$2,$20,$L109
nop  # DEBUG: branch/jump
beq	$2,$19,$L109
nop  # DEBUG: branch/jump
lw	$2,736($sp)
nop # DEBUG: Reuse of '$2'. 'lbu	$2,0($2)' does not use $at
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'beq	$2,$0,$L105' does not use $at
beq	$2,$0,$L105
nop  # DEBUG: branch/jump
beq	$2,$22,$L105
slt	$2,$16,128

beq	$2,$0,$L158
sll	$5,$16,2

addu	$4,$sp,736
jal	hexword
addu	$5,$fp,$5

bne	$2,$0,$L159
addu	$16,$16,1

j	$L104
nop  # DEBUG: branch/jump
$L105:
beq	$16,$0,$L75
nop  # DEBUG: branch/jump
beq	$18,$0,$L160
slt	$2,$16,15

bne	$2,$0,$L161
lui	$4,%hi($LC9) # high

lw	$17,280($sp)
nop # DEBUG: Reuse of '$17'. 'addu	$2,$17,-1' does not use $at
addu	$2,$17,-1
sltu	$2,$2,5
beq	$2,$0,$L162
sll	$2,$17,3

addu	$2,$2,39
bne	$16,$2,$L163
slt	$2,$17,5

beq	$2,$0,$L120
sll	$2,$17,5

addu	$2,$sp,$2
lbu	$2,256($2)
nop # DEBUG: Reuse of '$2'. 'andi	$2,$2,0x0080' does not use $at
andi	$2,$2,0x0080
beq	$2,$0,$L164
lui	$4,%hi($LC12) # high

$L120:
addu	$4,$0,$zero
addu	$6,$sp,16
addu	$5,$sp,232
sll	$2,$4,2
$L171:
addu	$3,$2,$6
addu	$2,$5,$2
lw	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'sw	$2,36($3)' does not use $at
sw	$2,36($3)
addu	$4,$4,1
slt	$2,$4,12
bne	$2,$0,$L171
sll	$2,$4,2

blez	$17,$L127
addu	$4,$0,$zero

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
nop # DEBUG: Reuse of '$3'. 'sb	$3,0($2)' does not use $at
sb	$3,0($2)
addu	$3,$0,$zero
sll	$2,$4,2
addu	$2,$2,$4
sll	$2,$2,2
addu	$6,$2,$7
addu	$2,$6,$3
$L172:
sb	$0,1($2)
addu	$3,$3,1
slt	$2,$3,6
bne	$2,$0,$L172
addu	$2,$6,$3

sll	$2,$4,2
addu	$2,$2,$4
sll	$2,$2,2
addu	$2,$2,$7
lbu	$3,4($5)
nop # DEBUG: Reuse of '$3'. 'sb	$3,7($2)' does not use $at
sb	$3,7($2)
lhu	$3,8($5)
nop # DEBUG: Reuse of '$3'. 'sh	$3,8($2)' does not use $at
sh	$3,8($2)
lhu	$3,12($5)
nop # DEBUG: Reuse of '$3'. 'sh	$3,10($2)' does not use $at
sh	$3,10($2)
lhu	$3,16($5)
nop # DEBUG: Reuse of '$3'. 'sh	$3,12($2)' does not use $at
sh	$3,12($2)
lhu	$3,20($5)
nop # DEBUG: Reuse of '$3'. 'sh	$3,14($2)' does not use $at
sh	$3,14($2)
lhu	$3,24($5)
nop # DEBUG: Reuse of '$3'. 'sh	$3,16($2)' does not use $at
sh	$3,16($2)
lhu	$3,28($5)
nop # DEBUG: Reuse of '$3'. 'sh	$3,18($2)' does not use $at
sh	$3,18($2)
addu	$4,$4,1
slt	$2,$4,$17
bne	$2,$0,$L173
sll	$2,$4,5

$L127:
lui	$2,%hi(rec.20) # high
addiu	$2,$2,%lo(rec.20) # low
sw	$2,36($sp)
la	$8,table.19
sw	$8,40($sp)
addu	$2,$sp,16
sw	$2,48($sp)
lhu	$2,228($sp)
nop # DEBUG: Reuse of '$2'. 'sh	$2,120($sp)' does not use $at
sh	$2,120($sp)
addu	$4,$0,$zero
addu	$5,$sp,16
lui	$3,(305397760 >> 16) & 0xFFFF
ori	$3,$3,0x5678
sll	$2,$4,2
$L174:
addu	$2,$2,$5
sw	$3,112($2)
sw	$3,160($2)
addu	$4,$4,1
slt	$2,$4,12
bne	$2,$0,$L174
sll	$2,$4,2

addu	$4,$sp,48
addu	$5,$sp,128
jal	func_80052960
addu	$6,$sp,176

addu	$4,$0,$zero
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
#nop # DEBUG: 'lw	$2,0($2)' does not load from $3
lw	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'bne	$3,$2,$L165' does not use $at
bne	$3,$2,$L165
addu	$4,$4,1

slt	$2,$4,12
bne	$2,$0,$L175
sll	$3,$4,2

$L142:
addu	$4,$0,$zero
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
#nop # DEBUG: 'lw	$2,0($2)' does not load from $3
lw	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'bne	$3,$2,$L166' does not use $at
bne	$3,$2,$L166
addu	$4,$4,1

slt	$2,$4,12
bne	$2,$0,$L176
sll	$3,$4,2

$L148:
lw	$8,744($sp)
nop # DEBUG: Reuse of '$8'. 'addu	$8,$8,1' does not use $at
addu	$8,$8,1
sw	$8,744($sp)
$L75:
lw	$2,736($sp)
nop # DEBUG: Reuse of '$2'. 'lbu	$2,0($2)' does not use $at
lbu	$2,0($2)
nop # DEBUG: Reuse of '$2'. 'bne	$2,$0,$L77' does not use $at
bne	$2,$0,$L77
nop  # DEBUG: branch/jump
$L76:
lw	$8,744($sp)
nop # DEBUG: Reuse of '$8'. 'beq	$8,$0,$L155' does not use $at
beq	$8,$0,$L155
nop  # DEBUG: branch/jump
beq	$23,$0,$L154
lui	$4,%hi($LC14) # high

$L155:
lui	$4,%hi($LC13) # high
jal	say
addiu	$4,$4,%lo($LC13) # low

j	$L156
ori	$2,$zero,1

$L157:
lui	$4,%hi($LC4) # high
j	$L167
addiu	$4,$4,%lo($LC4) # low

$L158:
lui	$4,%hi($LC6) # high
j	$L167
addiu	$4,$4,%lo($LC6) # low

$L159:
lui	$4,%hi($LC7) # high
j	$L167
addiu	$4,$4,%lo($LC7) # low

$L160:
lui	$4,%hi($LC8) # high
j	$L167
addiu	$4,$4,%lo($LC8) # low

$L161:
j	$L167
addiu	$4,$4,%lo($LC9) # low

$L162:
lui	$4,%hi($LC10) # high
j	$L167
addiu	$4,$4,%lo($LC10) # low

$L163:
lui	$4,%hi($LC11) # high
j	$L167
addiu	$4,$4,%lo($LC11) # low

$L164:
addiu	$4,$4,%lo($LC12) # low
$L167:
jal	say
nop  # DEBUG: branch/jump
j	$L156
ori	$2,$zero,2

$L154:
jal	say
addiu	$4,$4,%lo($LC14) # low

lw	$4,744($sp)
#nop # DEBUG: 'jal	say_dec' does not load from $4
jal	say_dec
nop  # DEBUG: branch/jump
lui	$4,%hi($LC15) # high
jal	say
addiu	$4,$4,%lo($LC15) # low

addu	$2,$0,$zero
$L156:
lw	$31,788($sp)
#nop # DEBUG: 'lw	$fp,784($sp)' does not load from $31
lw	$fp,784($sp)
#nop # DEBUG: 'lw	$23,780($sp)' does not load from $fp
lw	$23,780($sp)
#nop # DEBUG: 'lw	$22,776($sp)' does not load from $23
lw	$22,776($sp)
#nop # DEBUG: 'lw	$21,772($sp)' does not load from $22
lw	$21,772($sp)
#nop # DEBUG: 'lw	$20,768($sp)' does not load from $21
lw	$20,768($sp)
#nop # DEBUG: 'lw	$19,764($sp)' does not load from $20
lw	$19,764($sp)
#nop # DEBUG: 'lw	$18,760($sp)' does not load from $19
lw	$18,760($sp)
#nop # DEBUG: 'lw	$17,756($sp)' does not load from $18
lw	$17,756($sp)
#nop # DEBUG: 'lw	$16,752($sp)' does not load from $17
lw	$16,752($sp)
#nop # DEBUG: 'addu	$sp,$sp,792' does not load from $16
addu	$sp,$sp,792
j	$31
nop  # DEBUG: branch/jump
.end	main

.section .bss
	.globl data.18
	.align	2
data.18:
	.space 65536
	.globl table.19
	.align	2
table.19:
	.space 128
	.globl rec.20
	.align	2
rec.20:
	.space 160
	.globl r
	.align	2
r:
	.space 18
