h2:
	.fmask	0x00000000,0
	lui	$2,%hi(D_800A374D) # high
	lb	$4,%lo(D_800A374D)($2)
	subu	$sp,$sp,24
	sw	$31,16($sp)
	xori	$4,$4,0x0003
	jal	func_8001EF34
	sltu	$4,$4,1

	lw	$31,16($sp)
	#nop
	j	$31
	addu	$sp,$sp,24

