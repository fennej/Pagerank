	.file	"PageRank.c"
	.globl	_EPSILON
	.section .rdata,"dr"
	.align 4
_EPSILON:
	.long	953267991
LC0:
	.ascii "r\0"
LC1:
	.ascii "Erreur ouverture fichier\0"
LC2:
	.ascii "%d %d %d\0"
LC4:
	.ascii "%d %d\0"
	.text
	.globl	_lecturefichier_matrixm
	.def	_lecturefichier_matrixm;	.scl	2;	.type	32;	.endef
_lecturefichier_matrixm:
LFB19:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$184, %esp
	movl	$LC0, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_fopen
	movl	%eax, -28(%ebp)
	cmpl	$0, -28(%ebp)
	jne	L4
	movl	$LC1, (%esp)
	call	_puts
	movl	$0, %eax
	jmp	L16
L7:
	movzbl	-136(%ebp), %eax
	cmpb	$37, %al
	jne	L5
	jmp	L4
L5:
	movl	20(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	16(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC2, 4(%esp)
	leal	-136(%ebp), %eax
	movl	%eax, (%esp)
	call	_sscanf
	jmp	L6
L4:
	movl	-28(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$100, 4(%esp)
	leal	-136(%ebp), %eax
	movl	%eax, (%esp)
	call	_fgets
	testl	%eax, %eax
	jne	L7
L6:
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	sall	$2, %eax
	movl	%eax, (%esp)
	call	_malloc
	movl	%eax, -32(%ebp)
	movl	$0, -12(%ebp)
	jmp	L8
L9:
	movl	-12(%ebp), %eax
	leal	0(,%eax,4), %edx
	movl	-32(%ebp), %eax
	addl	%edx, %eax
	fldz
	fstps	(%eax)
	addl	$1, -12(%ebp)
L8:
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	cmpl	-12(%ebp), %eax
	jg	L9
	movl	20(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$2, %eax
	movl	%eax, (%esp)
	call	_malloc
	movl	%eax, -36(%ebp)
	movl	$0, -16(%ebp)
	jmp	L10
L11:
	movl	$0, -140(%ebp)
	movl	$0, -144(%ebp)
	leal	-144(%ebp), %eax
	movl	%eax, 12(%esp)
	leal	-140(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC4, 4(%esp)
	movl	-28(%ebp), %eax
	movl	%eax, (%esp)
	call	_fscanf
	movl	-16(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$2, %eax
	movl	%eax, %edx
	movl	-36(%ebp), %eax
	addl	%eax, %edx
	movl	-140(%ebp), %eax
	movl	%eax, (%edx)
	movl	-16(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$2, %eax
	movl	%eax, %edx
	movl	-36(%ebp), %eax
	addl	%eax, %edx
	movl	-144(%ebp), %eax
	movl	%eax, 4(%edx)
	movl	-140(%ebp), %eax
	addl	$1073741823, %eax
	leal	0(,%eax,4), %edx
	movl	-32(%ebp), %eax
	addl	%edx, %eax
	flds	(%eax)
	fld1
	faddp	%st, %st(1)
	fstps	(%eax)
	addl	$1, -16(%ebp)
L10:
	movl	20(%ebp), %eax
	movl	(%eax), %eax
	cmpl	-16(%ebp), %eax
	jg	L11
	movl	$0, -20(%ebp)
	jmp	L12
L13:
	movl	-20(%ebp), %eax
	leal	0(,%eax,4), %edx
	movl	-32(%ebp), %eax
	addl	%edx, %eax
	movl	-20(%ebp), %edx
	leal	0(,%edx,4), %ecx
	movl	-32(%ebp), %edx
	addl	%ecx, %edx
	flds	(%edx)
	fld1
	fdivp	%st, %st(1)
	fstps	(%eax)
	addl	$1, -20(%ebp)
L12:
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	cmpl	-20(%ebp), %eax
	jg	L13
	movl	$0, -24(%ebp)
	jmp	L14
L15:
	movl	-24(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$2, %eax
	movl	%eax, %edx
	movl	-36(%ebp), %eax
	leal	(%edx,%eax), %ecx
	movl	-24(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$2, %eax
	movl	%eax, %edx
	movl	-36(%ebp), %eax
	addl	%edx, %eax
	movl	(%eax), %eax
	addl	$1073741823, %eax
	leal	0(,%eax,4), %edx
	movl	-32(%ebp), %eax
	addl	%edx, %eax
	flds	(%eax)
	fstps	8(%ecx)
	addl	$1, -24(%ebp)
L14:
	movl	20(%ebp), %eax
	movl	(%eax), %eax
	cmpl	-24(%ebp), %eax
	jg	L15
	movl	-36(%ebp), %eax
L16:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE19:
	.section .rdata,"dr"
LC7:
	.ascii "Sommet: %d, Probabilite: %f\12\0"
	.text
	.globl	_afficherliste
	.def	_afficherliste;	.scl	2;	.type	32;	.endef
_afficherliste:
LFB20:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	jmp	L18
L19:
	movl	8(%ebp), %eax
	flds	4(%eax)
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	fstpl	8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC7, (%esp)
	call	_printf
	movl	8(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, 8(%ebp)
L18:
	cmpl	$0, 8(%ebp)
	jne	L19
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE20:
	.section .rdata,"dr"
LC8:
	.ascii "pred du sommet %d:\12\0"
	.text
	.globl	_affichertab
	.def	_affichertab;	.scl	2;	.type	32;	.endef
_affichertab:
LFB21:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	movl	$0, -12(%ebp)
	jmp	L21
L22:
	movl	-12(%ebp), %eax
	addl	$1, %eax
	movl	%eax, 4(%esp)
	movl	$LC8, (%esp)
	call	_printf
	movl	-12(%ebp), %eax
	leal	0(,%eax,4), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movl	(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	_afficherliste
	addl	$1, -12(%ebp)
L21:
	movl	-12(%ebp), %eax
	cmpl	12(%ebp), %eax
	jl	L22
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE21:
	.section .rdata,"dr"
LC9:
	.ascii "Sommet %d: %f\12\0"
	.text
	.globl	_afficher_vecteur
	.def	_afficher_vecteur;	.scl	2;	.type	32;	.endef
_afficher_vecteur:
LFB22:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	movl	$0, -12(%ebp)
	jmp	L24
L25:
	movl	-12(%ebp), %eax
	leal	0(,%eax,4), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	flds	(%eax)
	movl	-12(%ebp), %eax
	addl	$1, %eax
	fstpl	8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC9, (%esp)
	call	_printf
	addl	$1, -12(%ebp)
L24:
	movl	-12(%ebp), %eax
	cmpl	12(%ebp), %eax
	jl	L25
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE22:
	.globl	_multvectmmat
	.def	_multvectmmat;	.scl	2;	.type	32;	.endef
_multvectmmat:
LFB23:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$56, %esp
	movl	20(%ebp), %eax
	sall	$2, %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_memset
	movl	$0, -12(%ebp)
	jmp	L27
L28:
	fldz
	fstps	-16(%ebp)
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$2, %eax
	movl	%eax, %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movl	(%eax), %eax
	movl	%eax, -20(%ebp)
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$2, %eax
	movl	%eax, %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movl	4(%eax), %eax
	movl	%eax, -24(%ebp)
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$2, %eax
	movl	%eax, %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	flds	8(%eax)
	fstps	-28(%ebp)
	movl	-24(%ebp), %eax
	addl	$1073741823, %eax
	leal	0(,%eax,4), %edx
	movl	12(%ebp), %eax
	addl	%edx, %eax
	movl	-24(%ebp), %edx
	addl	$1073741823, %edx
	leal	0(,%edx,4), %ecx
	movl	12(%ebp), %edx
	addl	%ecx, %edx
	flds	(%edx)
	movl	-20(%ebp), %edx
	addl	$1073741823, %edx
	leal	0(,%edx,4), %ecx
	movl	16(%ebp), %edx
	addl	%ecx, %edx
	flds	(%edx)
	fmuls	-28(%ebp)
	faddp	%st, %st(1)
	fstps	(%eax)
	addl	$1, -12(%ebp)
L27:
	movl	-12(%ebp), %eax
	cmpl	24(%ebp), %eax
	jl	L28
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE23:
	.globl	_norme_a1
	.def	_norme_a1;	.scl	2;	.type	32;	.endef
_norme_a1:
LFB24:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	fldz
	fstps	-4(%ebp)
	movl	$0, -8(%ebp)
	jmp	L30
L31:
	movl	-8(%ebp), %eax
	leal	0(,%eax,4), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	flds	(%eax)
	movl	-8(%ebp), %eax
	leal	0(,%eax,4), %edx
	movl	12(%ebp), %eax
	addl	%edx, %eax
	flds	(%eax)
	fsubrp	%st, %st(1)
	fabs
	flds	-4(%ebp)
	faddp	%st, %st(1)
	fstps	-4(%ebp)
	addl	$1, -8(%ebp)
L30:
	movl	-8(%ebp), %eax
	cmpl	16(%ebp), %eax
	jl	L31
	flds	-4(%ebp)
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE24:
	.section .rdata,"dr"
	.align 4
LC11:
	.ascii "Temps d'execution de la multiplication matrice-vecteur: %f secondes\12\0"
	.align 4
LC12:
	.ascii "Somme des elements du vecteur: %f\12\0"
	.text
	.globl	_recherchep
	.def	_recherchep;	.scl	2;	.type	32;	.endef
_recherchep:
LFB25:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$88, %esp
	movl	16(%ebp), %eax
	sall	$2, %eax
	movl	%eax, (%esp)
	call	_malloc
	movl	%eax, -12(%ebp)
	movl	16(%ebp), %eax
	sall	$2, %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_memset
	movl	20(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	16(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_multvectmmat
	jmp	L34
L37:
	movl	12(%ebp), %eax
	movl	%eax, -24(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 12(%ebp)
	movl	-24(%ebp), %eax
	movl	%eax, -12(%ebp)
	call	_clock
	movl	%eax, -28(%ebp)
	movl	20(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	16(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_multvectmmat
	call	_clock
	movl	%eax, -32(%ebp)
	movl	-32(%ebp), %eax
	subl	-28(%ebp), %eax
	movl	%eax, -44(%ebp)
	fildl	-44(%ebp)
	fldl	LC10
	fdivrp	%st, %st(1)
	fstpl	4(%esp)
	movl	$LC11, (%esp)
	call	_printf
	fldz
	fstps	-16(%ebp)
	movl	$0, -20(%ebp)
	jmp	L35
L36:
	movl	-20(%ebp), %eax
	leal	0(,%eax,4), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	flds	(%eax)
	flds	-16(%ebp)
	faddp	%st, %st(1)
	fstps	-16(%ebp)
	addl	$1, -20(%ebp)
L35:
	movl	-20(%ebp), %eax
	cmpl	16(%ebp), %eax
	jl	L36
	flds	-16(%ebp)
	fstpl	4(%esp)
	movl	$LC12, (%esp)
	call	_printf
L34:
	movl	16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_norme_a1
	flds	LC13
	fxch	%st(1)
	fucompp
	fnstsw	%ax
	sahf
	ja	L37
	movl	-12(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE25:
	.section .rdata,"dr"
	.align 4
LC14:
	.ascii "Pred: %d, Suc: %d, Probabilite: %f\12\0"
	.text
	.globl	_afficher_contigue
	.def	_afficher_contigue;	.scl	2;	.type	32;	.endef
_afficher_contigue:
LFB26:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$56, %esp
	movl	$0, -12(%ebp)
	jmp	L40
L41:
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$2, %eax
	movl	%eax, %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	flds	8(%eax)
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$2, %eax
	movl	%eax, %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movl	4(%eax), %ecx
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$2, %eax
	movl	%eax, %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movl	(%eax), %eax
	fstpl	12(%esp)
	movl	%ecx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC14, (%esp)
	call	_printf
	addl	$1, -12(%ebp)
L40:
	movl	-12(%ebp), %eax
	cmpl	12(%ebp), %eax
	jl	L41
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE26:
	.def	___main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
LC15:
	.ascii "wiki.mtx\0"
LC16:
	.ascii "Vecteur initial:\0"
LC17:
	.ascii "PageRank:\0"
	.text
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
LFB27:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	andl	$-16, %esp
	subl	$64, %esp
	call	___main
	movl	$0, 48(%esp)
	movl	$0, 32(%esp)
	movl	$0, 28(%esp)
	movl	$0, 44(%esp)
	movl	$0, 24(%esp)
	leal	24(%esp), %eax
	movl	%eax, 12(%esp)
	leal	32(%esp), %eax
	movl	%eax, 8(%esp)
	leal	28(%esp), %eax
	movl	%eax, 4(%esp)
	movl	$LC15, (%esp)
	call	_lecturefichier_matrixm
	movl	%eax, 40(%esp)
	movl	28(%esp), %eax
	movl	%eax, 48(%esp)
	movl	48(%esp), %eax
	sall	$2, %eax
	movl	%eax, (%esp)
	call	_malloc
	movl	%eax, 36(%esp)
	movl	36(%esp), %eax
	fld1
	fstps	(%eax)
	movl	$1, 60(%esp)
	jmp	L43
L44:
	movl	60(%esp), %eax
	leal	0(,%eax,4), %edx
	movl	36(%esp), %eax
	addl	%edx, %eax
	fldz
	fstps	(%eax)
	addl	$1, 60(%esp)
L43:
	movl	60(%esp), %eax
	cmpl	48(%esp), %eax
	jl	L44
	movl	$LC16, (%esp)
	call	_puts
	movl	24(%esp), %eax
	movl	%eax, 12(%esp)
	movl	48(%esp), %eax
	movl	%eax, 8(%esp)
	movl	36(%esp), %eax
	movl	%eax, 4(%esp)
	movl	40(%esp), %eax
	movl	%eax, (%esp)
	call	_recherchep
	movl	%eax, 36(%esp)
	movl	$LC17, (%esp)
	call	_puts
	fldz
	fstps	56(%esp)
	movl	$0, 52(%esp)
	jmp	L45
L46:
	movl	52(%esp), %eax
	leal	0(,%eax,4), %edx
	movl	36(%esp), %eax
	addl	%edx, %eax
	flds	(%eax)
	flds	56(%esp)
	faddp	%st, %st(1)
	fstps	56(%esp)
	addl	$1, 52(%esp)
L45:
	movl	52(%esp), %eax
	cmpl	48(%esp), %eax
	jl	L46
	flds	56(%esp)
	fstpl	4(%esp)
	movl	$LC12, (%esp)
	call	_printf
	movl	36(%esp), %eax
	movl	%eax, (%esp)
	call	_free
	movl	40(%esp), %eax
	movl	%eax, (%esp)
	call	_free
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE27:
	.section .rdata,"dr"
	.align 8
LC10:
	.long	0
	.long	1083129856
	.align 4
LC13:
	.long	953267991
	.ident	"GCC: (MinGW.org GCC-6.3.0-1) 6.3.0"
	.def	_fopen;	.scl	2;	.type	32;	.endef
	.def	_puts;	.scl	2;	.type	32;	.endef
	.def	_sscanf;	.scl	2;	.type	32;	.endef
	.def	_fgets;	.scl	2;	.type	32;	.endef
	.def	_malloc;	.scl	2;	.type	32;	.endef
	.def	_fscanf;	.scl	2;	.type	32;	.endef
	.def	_printf;	.scl	2;	.type	32;	.endef
	.def	_memset;	.scl	2;	.type	32;	.endef
	.def	_clock;	.scl	2;	.type	32;	.endef
	.def	_free;	.scl	2;	.type	32;	.endef
