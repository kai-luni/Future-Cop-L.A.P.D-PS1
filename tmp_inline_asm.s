
.file	1 "tmp_inline.c"


gcc2_compiled.:
__gnu_compiled_c:
.text
.align	2
.globl	func_80082EF0
.ent	func_80082EF0
.set	noreorder
func_80082EF0:
.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, extra= 0
.mask	0x00000000,0
.fmask	0x00000000,0
addu	$3,$4,$zero
lw	$8,D_8009F9F0
#nop # DEBUG: 'lw	$2,D_8009FA28' does not load from $8
lw	$2,D_8009FA28
nop # DEBUG: Reuse of '$2'. 'sltu	$2,$8,$2' does not use $at
sltu	$2,$8,$2
beq	$2,$0,$L5
addu	$6,$3,$6

addu	$2,$8,52
sw	$2,D_8009F9F0
$L5:
li	$2,5			# 0x00000005
sb	$2,3($8)
li	$2,40			# 0x00000028
sb	$2,7($8)
lw	$2,20($sp)
nop # DEBUG: Reuse of '$2'. 'sb	$2,4($8)' does not use $at
sb	$2,4($8)
lw	$2,24($sp)
nop # DEBUG: Reuse of '$2'. 'sb	$2,5($8)' does not use $at
sb	$2,5($8)
lw	$2,28($sp)
#nop # DEBUG: 'addu	$7,$7,$5' does not load from $2
addu	$7,$7,$5
sh	$3,8($8)
sh	$3,16($8)
lw	$3,0($8)
#nop # DEBUG: 'li	$4,16711680			# 0x00ff0000' does not load from $3
li	$4,16711680			# 0x00ff0000
sh	$5,10($8)
sh	$6,12($8)
sh	$5,14($8)
sh	$7,18($8)
sh	$6,20($8)
sh	$7,22($8)
sb	$2,6($8)
li	$2,-16777216			# 0xff000000
and	$3,$3,$2
lw	$2,16($sp)
#nop # DEBUG: 'ori	$4,$4,0xffff' does not load from $2
ori	$4,$4,0xffff
and	$2,$2,$4
or	$3,$3,$2
j	$31
sw	$3,0($8)

.end	func_80082EF0
.set D_8009F9F0, 0x8009F9F0
.set D_8009FA28, 0x8009FA28
