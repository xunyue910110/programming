	.file	"setoutput.c"
	.section	.rodata
.LC1:
	.string	"/dev/pts/1"
.LC2:
	.string	"%s\n"
.LC3:
	.string	"fd1 = %d, fd = %d:%d\n"
.LC0:
	.string	"abc.txt"
	.text
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$48, %esp
	movl	.LC0, %eax
	movl	.LC0+4, %edx
	movl	%eax, 28(%esp)
	movl	%edx, 32(%esp)
	movl	$0, 36(%esp)
	movl	$1, (%esp)
	call	close
	movl	$1, 4(%esp)
	movl	$.LC1, (%esp)
	call	open
	movl	%eax, 40(%esp)
	movl	$0, 4(%esp)
	leal	28(%esp), %eax
	movl	%eax, (%esp)
	call	open
	movl	%eax, 44(%esp)
	cmpl	$-1, 44(%esp)
	jne	.L2
	call	__errno_location
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	strerror
	movl	$.LC2, %edx
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	printf
	movl	$0, %eax
	leave
	ret
.L2:
	movl	36(%esp), %edx
	addl	$1, 36(%esp)
	movl	$.LC3, %eax
	movl	%edx, 12(%esp)
	movl	44(%esp), %edx
	movl	%edx, 8(%esp)
	movl	40(%esp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	printf
	movl	$1, (%esp)
	call	sleep
	jmp	.L2
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.7 20120313 (Red Hat 4.4.7-11)"
	.section	.note.GNU-stack,"",@progbits
