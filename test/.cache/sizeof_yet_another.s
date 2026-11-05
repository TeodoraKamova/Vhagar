	.text
	.intel_syntax noprefix
	.file	"sizeof_yet_another.c"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	push	rax
	.cfi_def_cfa_offset 16
	mov	edi, offset ".L%d"
	mov	esi, 4
	xor	eax, eax
	call	printf@PLT
	mov	edi, offset ".L%s"
	mov	esi, offset .L__unnamed_1
	xor	eax, eax
	call	printf@PLT
	mov	edi, offset ".L%d"
	mov	esi, 1
	xor	eax, eax
	call	printf@PLT
	mov	edi, offset ".L%s"
	mov	esi, offset .L__unnamed_2
	xor	eax, eax
	call	printf@PLT
	mov	edi, offset ".L%d"
	mov	esi, 8
	xor	eax, eax
	call	printf@PLT
	mov	edi, offset ".L%s"
	mov	esi, offset .L__unnamed_3
	xor	eax, eax
	call	printf@PLT
	mov	edi, offset ".L%d"
	mov	esi, 8
	xor	eax, eax
	call	printf@PLT
	mov	edi, offset ".L%s"
	mov	esi, offset .L__unnamed_4
	xor	eax, eax
	call	printf@PLT
	mov	edi, offset ".L%d"
	mov	esi, 12
	xor	eax, eax
	call	printf@PLT
	mov	edi, offset ".L%s"
	mov	esi, offset .L__unnamed_5
	xor	eax, eax
	call	printf@PLT
	mov	edi, offset ".L%d"
	mov	esi, 8
	xor	eax, eax
	call	printf@PLT
	mov	edi, offset ".L%s"
	mov	esi, offset .L__unnamed_6
	xor	eax, eax
	call	printf@PLT
	xor	eax, eax
	pop	rcx
	.cfi_def_cfa_offset 8
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	.L__unnamed_1,@object           # @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	" "
	.size	.L__unnamed_1, 2

	.type	.L__unnamed_2,@object           # @1
.L__unnamed_2:
	.asciz	" "
	.size	.L__unnamed_2, 2

	.type	.L__unnamed_3,@object           # @2
.L__unnamed_3:
	.asciz	" "
	.size	.L__unnamed_3, 2

	.type	.L__unnamed_4,@object           # @3
.L__unnamed_4:
	.asciz	" "
	.size	.L__unnamed_4, 2

	.type	.L__unnamed_5,@object           # @4
.L__unnamed_5:
	.asciz	" "
	.size	.L__unnamed_5, 2

	.type	.L__unnamed_6,@object           # @5
.L__unnamed_6:
	.asciz	"\n"
	.size	.L__unnamed_6, 2

	.type	".L%d",@object                  # @"%d"
".L%d":
	.asciz	"%d"
	.size	".L%d", 3

	.type	".L%s",@object                  # @"%s"
".L%s":
	.asciz	"%s"
	.size	".L%s", 3

	.section	".note.GNU-stack","",@progbits
