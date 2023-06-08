	.file	"5.c"
	.text
	.section	.rodata
.LC0:
	.string	"shm_open"
.LC1:
	.string	"fstat"
.LC2:
	.string	"mmap"
	.text
	.globl	get_mem_block
	.type	get_mem_block, @function
get_mem_block:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$192, %rsp
	movq	%rdi, -184(%rbp)
	movq	%rsi, -192(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movq	-184(%rbp), %rax
	movl	$420, %edx
	movl	$2, %esi
	movq	%rax, %rdi
	call	shm_open@PLT
	movl	%eax, -172(%rbp)
	cmpl	$-1, -172(%rbp)
	jne	.L2
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
.L2:
	leaq	-160(%rbp), %rdx
	movl	-172(%rbp), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	fstat@PLT
	cmpl	$-1, %eax
	jne	.L3
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
.L3:
	movq	-112(%rbp), %rax
	movl	%eax, %edx
	movq	-192(%rbp), %rax
	movl	%edx, (%rax)
	movq	-192(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %esi
	movl	-172(%rbp), %eax
	movl	$0, %r9d
	movl	%eax, %r8d
	movl	$1, %ecx
	movl	$3, %edx
	movl	$0, %edi
	call	mmap@PLT
	movq	%rax, -168(%rbp)
	cmpq	$-1, -168(%rbp)
	jne	.L4
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
.L4:
	movl	-172(%rbp), %eax
	movl	%eax, %edi
	call	close@PLT
	movq	-168(%rbp), %rax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L6
	call	__stack_chk_fail@PLT
.L6:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	get_mem_block, .-get_mem_block
	.section	.rodata
.LC3:
	.string	"args"
.LC4:
	.string	"sem_wait"
.LC6:
	.string	"%f"
.LC7:
	.string	"munmap"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	%edi, -36(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	cmpl	$2, -36(%rbp)
	je	.L8
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
.L8:
	movl	$0, -28(%rbp)
	movq	-48(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	leaq	-28(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	get_mem_block
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	sem_wait@PLT
	cmpl	$-1, %eax
	jne	.L9
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
.L9:
	pxor	%xmm0, %xmm0
	movss	%xmm0, -24(%rbp)
	movl	$0, -20(%rbp)
	jmp	.L10
.L11:
	movq	-16(%rbp), %rax
	movl	-20(%rbp), %edx
	movslq	%edx, %rdx
	addq	$8, %rdx
	movss	(%rax,%rdx,4), %xmm0
	movss	-24(%rbp), %xmm1
	addss	%xmm1, %xmm0
	movss	%xmm0, -24(%rbp)
	addl	$1, -20(%rbp)
.L10:
	movq	-16(%rbp), %rax
	movl	4128(%rax), %edx
	movl	-20(%rbp), %eax
	cmpl	%eax, %edx
	ja	.L11
	movq	-16(%rbp), %rax
	movl	4128(%rax), %eax
	movl	%eax, %eax
	testq	%rax, %rax
	js	.L12
	pxor	%xmm0, %xmm0
	cvtsi2ssq	%rax, %xmm0
	jmp	.L13
.L12:
	movq	%rax, %rdx
	shrq	%rdx
	andl	$1, %eax
	orq	%rax, %rdx
	pxor	%xmm0, %xmm0
	cvtsi2ssq	%rdx, %xmm0
	addss	%xmm0, %xmm0
.L13:
	movss	-24(%rbp), %xmm1
	divss	%xmm0, %xmm1
	movss	%xmm1, -24(%rbp)
	pxor	%xmm2, %xmm2
	cvtss2sd	-24(%rbp), %xmm2
	movq	%xmm2, %rax
	movq	%rax, %xmm0
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	call	printf@PLT
	movl	-28(%rbp), %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	munmap@PLT
	cmpl	$-1, %eax
	jne	.L14
	leaq	.LC7(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
.L14:
	movl	$0, %edi
	call	exit@PLT
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
