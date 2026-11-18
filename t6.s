	.file	1 "t6.c"
gcc2_compiled.:
__gnu_compiled_c:
	.text
	.align	2
	.globl	h1
	.ent	h1
h1:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, extra= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	lui	$2,%hi(D_800A374D) # high
	lbu	$4,%lo(D_800A374D)($2)
	subu	$sp,$sp,24
	sw	$31,16($sp)
	xori	$4,$4,0x0003
	.set	noreorder
	.set	nomacro
	jal	func_8001EF34
	sltu	$4,$4,1
	.set	macro
	.set	reorder

	lw	$31,16($sp)
	#nop
	.set	noreorder
	.set	nomacro
	j	$31
	addu	$sp,$sp,24
	.set	macro
	.set	reorder

	.end	h1
	.align	2
	.globl	h2
	.ent	h2
h2:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, extra= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	lui	$2,%hi(D_800A374D) # high
	lb	$4,%lo(D_800A374D)($2)
	subu	$sp,$sp,24
	sw	$31,16($sp)
	xori	$4,$4,0x0003
	.set	noreorder
	.set	nomacro
	jal	func_8001EF34
	sltu	$4,$4,1
	.set	macro
	.set	reorder

	lw	$31,16($sp)
	#nop
	.set	noreorder
	.set	nomacro
	j	$31
	addu	$sp,$sp,24
	.set	macro
	.set	reorder

	.end	h2
