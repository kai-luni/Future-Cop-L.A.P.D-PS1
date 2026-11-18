
.file	1 "build/us/fcq/work/func_80082EF0-20261103-072920/exp/v_goto1.c"


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
addu	$8,$4,$zero
addu	$9,$8,$6
lw	$3,D_8009F9F0
#nop # DEBUG: 'lw	$2,D_8009FA28' does not load from $3
lw	$2,D_8009FA28
#nop # DEBUG: 'addu	$7,$7,$5' does not load from $2
addu	$7,$7,$5
sltu	$2,$3,$2
beq	$2,$0,$L4
addu	$6,$3,$zero

addu	$2,$6,52
sw	$2,D_8009F9F0
$L4:
li	$2,5			# 0x00000005
sb	$2,3($6)
li	$2,40			# 0x00000028
sb	$2,7($6)
lw	$2,20($sp)
#nop # DEBUG: 'lw	$3,0($6)' does not load from $2
lw	$3,0($6)
#nop # DEBUG: 'sb	$2,4($6)' does not load from $3
sb	$2,4($6)
lw	$2,24($sp)
nop # DEBUG: Reuse of '$2'. 'sb	$2,5($6)' does not use $at
sb	$2,5($6)
lw	$2,28($sp)
#nop # DEBUG: 'li	$4,16711680			# 0x00ff0000' does not load from $2
li	$4,16711680			# 0x00ff0000
sh	$8,8($6)
sh	$5,10($6)
sh	$9,12($6)
sh	$5,14($6)
sh	$8,16($6)
sh	$7,18($6)
sh	$9,20($6)
sh	$7,22($6)
sb	$2,6($6)
li	$2,-16777216			# 0xff000000
and	$3,$3,$2
lw	$2,16($sp)
#nop # DEBUG: 'ori	$4,$4,0xffff' does not load from $2
ori	$4,$4,0xffff
and	$2,$2,$4
or	$3,$3,$2
j	$31
sw	$3,0($6)

.end	func_80082EF0
.set D_8009F9F0, 0x8009F9F0
.set D_8009FA28, 0x8009FA28
