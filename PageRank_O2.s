	.file	"PageRank.c"
	.section .rdata,"dr"
LC0:
	.ascii "r\0"
LC1:
	.ascii "Erreur ouverture fichier\0"
LC2:
	.ascii "%d %d %d\0"
LC4:
	.ascii "%d %d\0"
	.text
	.p2align 4,,15
	.globl	_lecturefichier_matrixm
	.def	_lecturefichier_matrixm;	.scl	2;	.type	32;	.endef
_lecturefichier_matrixm:
LFB61:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	pushl	%edi
	.cfi_def_cfa_offset 12
	.cfi_offset 7, -12
	pushl	%esi
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushl	%ebx
	.cfi_def_cfa_offset 20
	.cfi_offset 3, -20
	subl	$172, %esp
	.cfi_def_cfa_offset 192
	movl	192(%esp), %eax
	movl	$LC0, 4(%esp)
	movl	%eax, (%esp)
	call	_fopen
	testl	%eax, %eax
	je	L23
	leal	60(%esp), %ebx
	movl	%eax, %edi
L2:
	movl	%edi, 8(%esp)
	movl	$100, 4(%esp)
	movl	%ebx, (%esp)
	call	_fgets
	testl	%eax, %eax
	je	L4
	cmpb	$37, 60(%esp)
	je	L2
	movl	204(%esp), %eax
	movl	$LC2, 4(%esp)
	movl	%ebx, (%esp)
	movl	%eax, 16(%esp)
	movl	200(%esp), %eax
	movl	%eax, 12(%esp)
	movl	196(%esp), %eax
	movl	%eax, 8(%esp)
	call	_sscanf
L4:
	movl	196(%esp), %eax
	movl	(%eax), %eax
	movl	%eax, 44(%esp)
	sall	$2, %eax
	movl	%eax, (%esp)
	call	_malloc
	movl	%eax, %ebx
	movl	196(%esp), %eax
	fldz
	movl	(%eax), %ecx
	movl	%ebx, %eax
	testl	%ecx, %ecx
	leal	(%ebx,%ecx,4), %edx
	jle	L30
	.p2align 4,,10
L17:
	fsts	(%eax)
	addl	$4, %eax
	cmpl	%eax, %edx
	jne	L17
	fstp	%st(0)
	jmp	L10
L30:
	fstp	%st(0)
	.p2align 4,,10
L10:
	movl	204(%esp), %eax
	xorl	%ebp, %ebp
	movl	(%eax), %eax
	leal	(%eax,%eax,2), %eax
	sall	$2, %eax
	movl	%eax, (%esp)
	call	_malloc
	movl	%eax, 44(%esp)
	movl	204(%esp), %eax
	movl	44(%esp), %esi
	movl	(%eax), %eax
	testl	%eax, %eax
	jle	L29
	.p2align 4,,10
L11:
	leal	56(%esp), %eax
	movl	$LC4, 4(%esp)
	movl	%edi, (%esp)
	movl	$0, 52(%esp)
	movl	$0, 56(%esp)
	addl	$1, %ebp
	movl	%eax, 12(%esp)
	leal	52(%esp), %eax
	addl	$12, %esi
	movl	%eax, 8(%esp)
	call	_fscanf
	movl	52(%esp), %eax
	fld1
	movl	56(%esp), %ecx
	movl	%eax, -12(%esi)
	leal	-4(%ebx,%eax,4), %eax
	movl	%ecx, -8(%esi)
	fadds	(%eax)
	fstps	(%eax)
	movl	204(%esp), %eax
	movl	(%eax), %eax
	cmpl	%ebp, %eax
	jg	L11
	movl	196(%esp), %edi
	movl	(%edi), %ecx
	testl	%ecx, %ecx
	jle	L12
L15:
	fld1
	xorl	%edx, %edx
	.p2align 4,,10
L13:
	flds	(%ebx,%edx,4)
	fdivr	%st(1), %st
	fstps	(%ebx,%edx,4)
	addl	$1, %edx
	cmpl	%ecx, %edx
	jl	L13
	fstp	%st(0)
L12:
	testl	%eax, %eax
	jle	L1
	movl	44(%esp), %edi
	leal	(%eax,%eax,2), %eax
	leal	(%edi,%eax,4), %ecx
	movl	%edi, %edx
	.p2align 4,,10
L14:
	movl	(%edx), %eax
	addl	$12, %edx
	flds	-4(%ebx,%eax,4)
	fstps	-4(%edx)
	cmpl	%ecx, %edx
	jne	L14
L1:
	movl	44(%esp), %eax
	addl	$172, %esp
	.cfi_remember_state
	.cfi_def_cfa_offset 20
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 16
	popl	%esi
	.cfi_restore 6
	.cfi_def_cfa_offset 12
	popl	%edi
	.cfi_restore 7
	.cfi_def_cfa_offset 8
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa_offset 4
	ret
L29:
	.cfi_restore_state
	movl	196(%esp), %edi
	movl	(%edi), %ecx
	testl	%ecx, %ecx
	jg	L15
	jmp	L1
L23:
	movl	$LC1, (%esp)
	call	_puts
	movl	$0, 44(%esp)
	jmp	L1
	.cfi_endproc
LFE61:
	.section .rdata,"dr"
LC7:
	.ascii "Sommet: %d, Probabilite: %f\12\0"
	.text
	.p2align 4,,15
	.globl	_afficherliste
	.def	_afficherliste;	.scl	2;	.type	32;	.endef
_afficherliste:
LFB62:
	.cfi_startproc
	pushl	%ebx
	.cfi_def_cfa_offset 8
	.cfi_offset 3, -8
	subl	$24, %esp
	.cfi_def_cfa_offset 32
	movl	32(%esp), %ebx
	testl	%ebx, %ebx
	je	L31
	.p2align 4,,10
L35:
	flds	4(%ebx)
	fstpl	8(%esp)
	movl	(%ebx), %eax
	movl	$LC7, (%esp)
	movl	%eax, 4(%esp)
	call	_printf
	movl	8(%ebx), %ebx
	testl	%ebx, %ebx
	jne	L35
L31:
	addl	$24, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
LFE62:
	.section .rdata,"dr"
LC8:
	.ascii "pred du sommet %d:\12\0"
	.text
	.p2align 4,,15
	.globl	_affichertab
	.def	_affichertab;	.scl	2;	.type	32;	.endef
_affichertab:
LFB63:
	.cfi_startproc
	pushl	%edi
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	pushl	%esi
	.cfi_def_cfa_offset 12
	.cfi_offset 6, -12
	pushl	%ebx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	subl	$16, %esp
	.cfi_def_cfa_offset 32
	movl	36(%esp), %esi
	movl	32(%esp), %edi
	testl	%esi, %esi
	jle	L38
	xorl	%ebx, %ebx
	.p2align 4,,10
L42:
	addl	$1, %ebx
	movl	$LC8, (%esp)
	movl	%ebx, 4(%esp)
	call	_printf
	movl	-4(%edi,%ebx,4), %eax
	movl	%eax, (%esp)
	call	_afficherliste
	cmpl	%ebx, %esi
	jne	L42
L38:
	addl	$16, %esp
	.cfi_def_cfa_offset 16
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 12
	popl	%esi
	.cfi_restore 6
	.cfi_def_cfa_offset 8
	popl	%edi
	.cfi_restore 7
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
LFE63:
	.section .rdata,"dr"
LC9:
	.ascii "Sommet %d: %f\12\0"
	.text
	.p2align 4,,15
	.globl	_afficher_vecteur
	.def	_afficher_vecteur;	.scl	2;	.type	32;	.endef
_afficher_vecteur:
LFB64:
	.cfi_startproc
	pushl	%edi
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	pushl	%esi
	.cfi_def_cfa_offset 12
	.cfi_offset 6, -12
	pushl	%ebx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	subl	$16, %esp
	.cfi_def_cfa_offset 32
	movl	36(%esp), %esi
	movl	32(%esp), %edi
	testl	%esi, %esi
	jle	L45
	xorl	%ebx, %ebx
	.p2align 4,,10
L49:
	flds	(%edi,%ebx,4)
	addl	$1, %ebx
	movl	$LC9, (%esp)
	movl	%ebx, 4(%esp)
	fstpl	8(%esp)
	call	_printf
	cmpl	%ebx, %esi
	jne	L49
L45:
	addl	$16, %esp
	.cfi_def_cfa_offset 16
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 12
	popl	%esi
	.cfi_restore 6
	.cfi_def_cfa_offset 8
	popl	%edi
	.cfi_restore 7
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
LFE64:
	.p2align 4,,15
	.globl	_multvectmmat
	.def	_multvectmmat;	.scl	2;	.type	32;	.endef
_multvectmmat:
LFB65:
	.cfi_startproc
	pushl	%edi
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	pushl	%esi
	.cfi_def_cfa_offset 12
	.cfi_offset 6, -12
	pushl	%ebx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	subl	$16, %esp
	.cfi_def_cfa_offset 32
	movl	44(%esp), %eax
	movl	36(%esp), %ebx
	movl	48(%esp), %edi
	movl	$0, 4(%esp)
	movl	40(%esp), %esi
	sall	$2, %eax
	movl	%ebx, (%esp)
	movl	%eax, 8(%esp)
	call	_memset
	testl	%edi, %edi
	jle	L52
	movl	32(%esp), %eax
	leal	(%edi,%edi,2), %edx
	leal	(%eax,%edx,4), %edi
	.p2align 4,,10
L54:
	movl	(%eax), %ecx
	movl	4(%eax), %edx
	addl	$12, %eax
	flds	-4(%esi,%ecx,4)
	leal	-4(%ebx,%edx,4), %edx
	fmuls	-4(%eax)
	cmpl	%eax, %edi
	fadds	(%edx)
	fstps	(%edx)
	jne	L54
L52:
	addl	$16, %esp
	.cfi_def_cfa_offset 16
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 12
	popl	%esi
	.cfi_restore 6
	.cfi_def_cfa_offset 8
	popl	%edi
	.cfi_restore 7
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
LFE65:
	.p2align 4,,15
	.globl	_norme_a1
	.def	_norme_a1;	.scl	2;	.type	32;	.endef
_norme_a1:
LFB66:
	.cfi_startproc
	movl	12(%esp), %ecx
	testl	%ecx, %ecx
	jle	L61
	movl	4(%esp), %eax
	movl	8(%esp), %edx
	fldz
	leal	(%eax,%ecx,4), %ecx
	.p2align 4,,10
L60:
	flds	(%eax)
	addl	$4, %eax
	addl	$4, %edx
	fsubs	-4(%edx)
	cmpl	%eax, %ecx
	fabs
	faddp	%st, %st(1)
	jne	L60
	rep ret
L61:
	fldz
	ret
	.cfi_endproc
LFE66:
	.section .rdata,"dr"
	.align 4
LC10:
	.ascii "Somme des elements du vecteur: %f\12\0"
	.align 4
LC12:
	.ascii "Temps d'execution de la multiplication matrice-vecteur: %f secondes\12\0"
	.text
	.p2align 4,,15
	.globl	_recherchep
	.def	_recherchep;	.scl	2;	.type	32;	.endef
_recherchep:
LFB67:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	pushl	%edi
	.cfi_def_cfa_offset 12
	.cfi_offset 7, -12
	pushl	%esi
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushl	%ebx
	.cfi_def_cfa_offset 20
	.cfi_offset 3, -20
	subl	$60, %esp
	.cfi_def_cfa_offset 80
	movl	88(%esp), %edi
	movl	$1, 4(%esp)
	movl	84(%esp), %ebx
	leal	0(,%edi,4), %eax
	movl	%eax, (%esp)
	movl	%eax, 36(%esp)
	call	_calloc
	movl	%eax, %esi
	movl	92(%esp), %eax
	movl	%edi, 12(%esp)
	movl	%ebx, 8(%esp)
	movl	%esi, 4(%esp)
	movl	%eax, 16(%esp)
	movl	80(%esp), %eax
	movl	%eax, (%esp)
	call	_multvectmmat
	testl	%edi, %edi
	jle	L63
	.p2align 4,,10
L75:
	movl	36(%esp), %ecx
	fldz
	movl	%esi, %eax
	movl	%ebx, %edx
	leal	(%esi,%ecx), %ebp
	movl	%ebx, %ecx
	.p2align 4,,10
L67:
	flds	(%eax)
	addl	$4, %eax
	addl	$4, %ecx
	fsubs	-4(%ecx)
	cmpl	%ebp, %eax
	fabs
	faddp	%st, %st(1)
	jne	L67
	flds	LC13
	fxch	%st(1)
	fucompp
	fnstsw	%ax
	sahf
	jbe	L63
	movl	%edx, 44(%esp)
	call	_clock
	movl	%eax, %ebp
	movl	92(%esp), %eax
	movl	%edi, 12(%esp)
	movl	%esi, 8(%esp)
	movl	%ebx, 4(%esp)
	movl	%eax, 16(%esp)
	movl	80(%esp), %eax
	movl	%eax, (%esp)
	call	_multvectmmat
	call	_clock
	subl	%ebp, %eax
	movl	$LC12, (%esp)
	movl	%eax, 40(%esp)
	fildl	40(%esp)
	fdivs	LC11
	fstpl	4(%esp)
	call	_printf
	movl	36(%esp), %eax
	movl	44(%esp), %edx
	fldz
	addl	%ebx, %eax
	.p2align 4,,10
L65:
	fadds	(%edx)
	addl	$4, %edx
	cmpl	%edx, %eax
	jne	L65
	fstpl	4(%esp)
	movl	$LC10, (%esp)
	call	_printf
	movl	%ebx, %eax
	testl	%edi, %edi
	movl	%esi, %ebx
	movl	%eax, %esi
	jg	L75
L63:
	addl	$60, %esp
	.cfi_def_cfa_offset 20
	movl	%esi, %eax
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 16
	popl	%esi
	.cfi_restore 6
	.cfi_def_cfa_offset 12
	popl	%edi
	.cfi_restore 7
	.cfi_def_cfa_offset 8
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
LFE67:
	.section .rdata,"dr"
	.align 4
LC14:
	.ascii "Pred: %d, Suc: %d, Probabilite: %f\12\0"
	.text
	.p2align 4,,15
	.globl	_afficher_contigue
	.def	_afficher_contigue;	.scl	2;	.type	32;	.endef
_afficher_contigue:
LFB68:
	.cfi_startproc
	pushl	%edi
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	pushl	%esi
	.cfi_def_cfa_offset 12
	.cfi_offset 6, -12
	pushl	%ebx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	subl	$32, %esp
	.cfi_def_cfa_offset 48
	movl	52(%esp), %edi
	movl	48(%esp), %ebx
	testl	%edi, %edi
	jle	L76
	xorl	%esi, %esi
	.p2align 4,,10
L80:
	flds	8(%ebx)
	addl	$1, %esi
	addl	$12, %ebx
	fstpl	12(%esp)
	movl	-8(%ebx), %eax
	movl	%eax, 8(%esp)
	movl	-12(%ebx), %eax
	movl	$LC14, (%esp)
	movl	%eax, 4(%esp)
	call	_printf
	cmpl	%esi, %edi
	jne	L80
L76:
	addl	$32, %esp
	.cfi_def_cfa_offset 16
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 12
	popl	%esi
	.cfi_restore 6
	.cfi_def_cfa_offset 8
	popl	%edi
	.cfi_restore 7
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
LFE68:
	.def	___main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
LC15:
	.ascii "wiki.mtx\0"
LC16:
	.ascii "Vecteur initial:\0"
LC17:
	.ascii "PageRank:\0"
	.section	.text.startup,"x"
	.p2align 4,,15
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
LFB69:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	andl	$-16, %esp
	subl	$48, %esp
	.cfi_offset 7, -12
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	call	___main
	leal	44(%esp), %eax
	movl	$LC15, (%esp)
	movl	$0, 36(%esp)
	movl	$0, 40(%esp)
	movl	$0, 44(%esp)
	movl	%eax, 12(%esp)
	leal	36(%esp), %eax
	movl	%eax, 8(%esp)
	leal	40(%esp), %eax
	movl	%eax, 4(%esp)
	call	_lecturefichier_matrixm
	movl	40(%esp), %esi
	movl	%eax, 28(%esp)
	leal	0(,%esi,4), %ebx
	movl	%ebx, (%esp)
	call	_malloc
	cmpl	$1, %esi
	movl	%eax, %edi
	movl	$0x3f800000, (%eax)
	leal	(%eax,%ebx), %edx
	jle	L98
	fldz
	addl	$4, %eax
	.p2align 4,,10
L90:
	fsts	(%eax)
	addl	$4, %eax
	cmpl	%edx, %eax
	jne	L90
	fstp	%st(0)
	movl	$LC16, (%esp)
	call	_puts
	movl	44(%esp), %eax
	movl	%edi, 4(%esp)
	movl	%esi, 8(%esp)
	movl	%eax, 12(%esp)
	movl	28(%esp), %eax
	movl	%eax, (%esp)
	call	_recherchep
	movl	$LC17, (%esp)
	movl	%eax, %edi
	call	_puts
L85:
	fldz
	movl	%edi, %eax
	addl	%edi, %ebx
	.p2align 4,,10
L89:
	fadds	(%eax)
	addl	$4, %eax
	cmpl	%eax, %ebx
	jne	L89
L86:
	fstpl	4(%esp)
	movl	$LC10, (%esp)
	call	_printf
	movl	%edi, (%esp)
	call	_free
	movl	28(%esp), %eax
	movl	%eax, (%esp)
	call	_free
	leal	-12(%ebp), %esp
	xorl	%eax, %eax
	popl	%ebx
	.cfi_remember_state
	.cfi_restore 3
	popl	%esi
	.cfi_restore 6
	popl	%edi
	.cfi_restore 7
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
L98:
	.cfi_restore_state
	movl	$LC16, (%esp)
	call	_puts
	movl	44(%esp), %eax
	movl	%edi, 4(%esp)
	movl	%esi, 8(%esp)
	movl	%eax, 12(%esp)
	movl	28(%esp), %eax
	movl	%eax, (%esp)
	call	_recherchep
	movl	$LC17, (%esp)
	movl	%eax, %edi
	call	_puts
	testl	%esi, %esi
	jg	L85
	fldz
	jmp	L86
	.cfi_endproc
LFE69:
	.globl	_EPSILON
	.section .rdata,"dr"
	.align 4
_EPSILON:
	.long	953267991
	.align 4
LC11:
	.long	1148846080
	.align 4
LC13:
	.long	953267991
	.ident	"GCC: (MinGW.org GCC-6.3.0-1) 6.3.0"
	.def	_fopen;	.scl	2;	.type	32;	.endef
	.def	_fgets;	.scl	2;	.type	32;	.endef
	.def	_sscanf;	.scl	2;	.type	32;	.endef
	.def	_malloc;	.scl	2;	.type	32;	.endef
	.def	_fscanf;	.scl	2;	.type	32;	.endef
	.def	_puts;	.scl	2;	.type	32;	.endef
	.def	_printf;	.scl	2;	.type	32;	.endef
	.def	_memset;	.scl	2;	.type	32;	.endef
	.def	_calloc;	.scl	2;	.type	32;	.endef
	.def	_clock;	.scl	2;	.type	32;	.endef
	.def	_free;	.scl	2;	.type	32;	.endef
