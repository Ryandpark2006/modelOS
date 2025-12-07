	.file	"test_fs.c"
	.text
.Ltext0:
	.file 0 "/u/rpark/cs439/Prog8/p8test.dir/sbin" "test_fs.c"
	.p2align 4
	.globl	print
	.type	print, @function
print:
.LVL0:
.LFB0:
	.file 1 "test_fs.c"
	.loc 1 4 29 view -0
	.cfi_startproc
	.loc 1 5 5 view .LVU1
	.loc 1 6 5 view .LVU2
	.loc 1 4 29 is_stmt 0 view .LVU3
	pushl	%ebx
	.cfi_def_cfa_offset 8
	.cfi_offset 3, -8
	subl	$8, %esp
	.cfi_def_cfa_offset 16
	.loc 1 4 29 view .LVU4
	movl	16(%esp), %ebx
.LVL1:
	.loc 1 7 5 is_stmt 1 view .LVU5
	.loc 1 7 12 discriminator 1 view .LVU6
	.loc 1 7 12 is_stmt 0 discriminator 1 view .LVU7
	cmpb	$0, (%ebx)
	je	.L3
	subl	$12, %esp
	.cfi_def_cfa_offset 28
	leal	1(%ebx), %eax
.LVL2:
	.loc 1 7 12 discriminator 1 view .LVU8
	pushl	%eax
	.cfi_def_cfa_offset 32
	call	strlen
.LVL3:
	.loc 1 7 12 discriminator 1 view .LVU9
	addl	$16, %esp
	.cfi_def_cfa_offset 16
	addl	$1, %eax
.LVL4:
.L2:
	.loc 1 8 5 is_stmt 1 view .LVU10
	subl	$4, %esp
	.cfi_def_cfa_offset 20
	pushl	%eax
	.cfi_def_cfa_offset 24
	pushl	%ebx
	.cfi_def_cfa_offset 28
	pushl	$1
	.cfi_def_cfa_offset 32
	call	write
.LVL5:
	.loc 1 9 1 is_stmt 0 view .LVU11
	addl	$24, %esp
	.cfi_def_cfa_offset 8
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 4
	ret
.LVL6:
	.p2align 4,,10
	.p2align 3
.L3:
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -8
	.loc 1 7 12 discriminator 1 view .LVU12
	xorl	%eax, %eax
	jmp	.L2
	.cfi_endproc
.LFE0:
	.size	print, .-print
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"-"
	.text
	.p2align 4
	.globl	print_num
	.type	print_num, @function
print_num:
.LVL7:
.LFB1:
	.loc 1 11 23 is_stmt 1 view -0
	.cfi_startproc
	.loc 1 12 5 view .LVU14
	.loc 1 11 23 is_stmt 0 view .LVU15
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
	.loc 1 11 23 view .LVU16
	movl	32(%esp), %ebx
	.loc 1 12 8 view .LVU17
	testl	%ebx, %ebx
	js	.L7
.LVL8:
.L11:
	.loc 1 16 5 is_stmt 1 view .LVU18
	leal	15(%esp), %esi
	.loc 1 16 8 is_stmt 0 view .LVU19
	cmpl	$9, %ebx
	jg	.L16
.L8:
	.loc 1 19 5 is_stmt 1 view .LVU20
	.loc 1 19 23 is_stmt 0 view .LVU21
	movl	$-858993459, %eax
	.loc 1 20 5 view .LVU22
	subl	$4, %esp
	.cfi_def_cfa_offset 36
	.loc 1 19 23 view .LVU23
	mull	%ebx
	shrl	$3, %edx
	leal	(%edx,%edx,4), %eax
	addl	%eax, %eax
	subl	%eax, %ebx
.LVL9:
	.loc 1 19 18 view .LVU24
	addl	$48, %ebx
	movb	%bl, 19(%esp)
	.loc 1 20 5 is_stmt 1 view .LVU25
	pushl	$1
	.cfi_def_cfa_offset 40
	pushl	%esi
	.cfi_def_cfa_offset 44
	pushl	$1
	.cfi_def_cfa_offset 48
	call	write
.LVL10:
	.loc 1 21 1 is_stmt 0 view .LVU26
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
.LVL11:
	.p2align 4,,10
	.p2align 3
.L16:
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -16
	.cfi_offset 6, -12
	.cfi_offset 7, -8
	.loc 1 17 9 is_stmt 1 view .LVU27
	movl	$-858993459, %ecx
	movl	%ebx, %eax
	mull	%ecx
	movl	%edx, %edi
	shrl	$3, %edi
.LVL12:
.LBB12:
.LBI12:
	.loc 1 11 6 view .LVU28
.LBB13:
	.loc 1 12 5 view .LVU29
	.loc 1 16 5 view .LVU30
	.loc 1 16 8 is_stmt 0 view .LVU31
	cmpl	$99, %ebx
	jg	.L17
.L9:
	.loc 1 19 5 is_stmt 1 view .LVU32
	.loc 1 19 23 is_stmt 0 view .LVU33
	movl	$-858993459, %eax
	.loc 1 20 5 view .LVU34
	subl	$4, %esp
	.cfi_def_cfa_offset 36
	.loc 1 19 23 view .LVU35
	mull	%edi
	shrl	$3, %edx
	leal	(%edx,%edx,4), %eax
	addl	%eax, %eax
	subl	%eax, %edi
.LVL13:
	.loc 1 19 18 view .LVU36
	leal	48(%edi), %eax
	movb	%al, 19(%esp)
	.loc 1 20 5 is_stmt 1 view .LVU37
	pushl	$1
	.cfi_def_cfa_offset 40
	pushl	%esi
	.cfi_def_cfa_offset 44
	pushl	$1
	.cfi_def_cfa_offset 48
	call	write
.LVL14:
	.loc 1 21 1 is_stmt 0 view .LVU38
	addl	$16, %esp
	.cfi_def_cfa_offset 32
	jmp	.L8
.LVL15:
	.p2align 4,,10
	.p2align 3
.L7:
	.loc 1 21 1 view .LVU39
.LBE13:
.LBE12:
.LBB17:
.LBB18:
	.loc 1 7 18 is_stmt 1 discriminator 2 view .LVU40
	.loc 1 7 12 discriminator 1 view .LVU41
	.loc 1 8 5 view .LVU42
	subl	$4, %esp
	.cfi_def_cfa_offset 36
.LBE18:
.LBE17:
	.loc 1 14 11 is_stmt 0 view .LVU43
	negl	%ebx
.LBB20:
.LBB19:
	.loc 1 8 5 view .LVU44
	pushl	$1
	.cfi_def_cfa_offset 40
	pushl	$.LC0
	.cfi_def_cfa_offset 44
	pushl	$1
	.cfi_def_cfa_offset 48
	call	write
.LVL16:
	.loc 1 8 5 view .LVU45
.LBE19:
.LBE20:
	.loc 1 14 9 is_stmt 1 view .LVU46
	.loc 1 14 11 is_stmt 0 view .LVU47
	addl	$16, %esp
	.cfi_def_cfa_offset 32
	jmp	.L11
.LVL17:
	.p2align 4,,10
	.p2align 3
.L17:
.LBB21:
.LBB16:
	.loc 1 17 9 is_stmt 1 view .LVU48
	movl	%edi, %eax
	mull	%ecx
	movl	%edx, %esi
	shrl	$3, %esi
.LVL18:
.LBB14:
.LBI14:
	.loc 1 11 6 view .LVU49
.LBB15:
	.loc 1 12 5 view .LVU50
	.loc 1 16 5 view .LVU51
	.loc 1 16 8 is_stmt 0 view .LVU52
	cmpl	$999, %ebx
	jg	.L18
.L10:
	.loc 1 19 5 is_stmt 1 view .LVU53
	.loc 1 19 23 is_stmt 0 view .LVU54
	movl	$-858993459, %eax
	.loc 1 20 5 view .LVU55
	subl	$4, %esp
	.cfi_def_cfa_offset 36
	.loc 1 19 23 view .LVU56
	mull	%esi
	shrl	$3, %edx
	leal	(%edx,%edx,4), %eax
	addl	%eax, %eax
	subl	%eax, %esi
.LVL19:
	.loc 1 19 18 view .LVU57
	leal	48(%esi), %eax
	movb	%al, 19(%esp)
	.loc 1 20 5 is_stmt 1 view .LVU58
	pushl	$1
	.cfi_def_cfa_offset 40
	leal	23(%esp), %esi
	pushl	%esi
	.cfi_def_cfa_offset 44
	pushl	$1
	.cfi_def_cfa_offset 48
	call	write
.LVL20:
	.loc 1 21 1 is_stmt 0 view .LVU59
	addl	$16, %esp
	.cfi_def_cfa_offset 32
	jmp	.L9
.LVL21:
	.p2align 4,,10
	.p2align 3
.L18:
	.loc 1 17 9 is_stmt 1 view .LVU60
	movl	%esi, %eax
	subl	$12, %esp
	.cfi_def_cfa_offset 44
	mull	%ecx
	shrl	$3, %edx
	pushl	%edx
	.cfi_def_cfa_offset 48
	call	print_num
.LVL22:
	addl	$16, %esp
	.cfi_def_cfa_offset 32
	jmp	.L10
.LBE15:
.LBE14:
.LBE16:
.LBE21:
	.cfi_endproc
.LFE1:
	.size	print_num, .-print_num
	.section	.rodata.str1.1
.LC1:
	.string	" PASS\n"
.LC2:
	.string	" FAIL\n"
.LC3:
	.string	" PASS (correctly failed)\n"
.LC4:
	.string	" FAIL (should have failed)\n"
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align 4
.LC5:
	.string	"*** Filesystem Write Operations Test ***\n"
	.section	.rodata.str1.1
.LC6:
	.string	"\n1. Testing mkdir:\n"
.LC7:
	.string	"/testdir"
.LC8:
	.string	"   mkdir(\"/testdir\", 0755) = "
	.section	.rodata.str1.4
	.align 4
.LC9:
	.string	"\n2. Testing stat on created dir:\n"
	.section	.rodata.str1.1
.LC10:
	.string	"   stat(\"/testdir\") = "
.LC11:
	.string	" PASS (inode="
.LC12:
	.string	", mode="
.LC13:
	.string	")\n"
.LC14:
	.string	"\n3. Testing mkdir duplicate:\n"
	.section	.rodata.str1.4
	.align 4
.LC15:
	.string	"   mkdir(\"/testdir\", 0755) again = "
	.section	.rodata.str1.1
.LC16:
	.string	"\n4. Testing rename:\n"
.LC17:
	.string	"/newdir"
	.section	.rodata.str1.4
	.align 4
.LC18:
	.string	"   rename(\"/testdir\", \"/newdir\") = "
	.align 4
.LC19:
	.string	"\n5. Testing stat on renamed dir:\n"
	.section	.rodata.str1.1
.LC20:
	.string	"   stat(\"/newdir\") = "
	.section	.rodata.str1.4
	.align 4
.LC21:
	.string	"\n6. Testing stat on old name:\n"
	.section	.rodata.str1.1
.LC22:
	.string	"\n7. Testing rmdir:\n"
.LC23:
	.string	"   rmdir(\"/newdir\") = "
	.section	.rodata.str1.4
	.align 4
.LC24:
	.string	"\n8. Testing stat on deleted dir:\n"
	.align 4
.LC25:
	.string	"\n9. Testing mkdir another dir:\n"
	.section	.rodata.str1.1
.LC26:
	.string	"/testdir2"
	.section	.rodata.str1.4
	.align 4
.LC27:
	.string	"   mkdir(\"/testdir2\", 0755) = "
	.align 4
.LC28:
	.string	"\n10. Testing unlink on dir (should fail):\n"
	.section	.rodata.str1.1
.LC29:
	.string	"   unlink(\"/testdir2\") = "
	.section	.rodata.str1.4
	.align 4
.LC30:
	.string	"\n*** Filesystem Test Complete! ***\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB2:
	.loc 1 23 12 view -0
	.cfi_startproc
	.loc 1 24 5 view .LVU62
	.loc 1 23 12 is_stmt 0 view .LVU63
	leal	4(%esp), %ecx
	.cfi_def_cfa 1, 0
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	movl	%esp, %ebp
	.cfi_escape 0x10,0x5,0x2,0x75,0
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	pushl	%ecx
	.cfi_escape 0xf,0x3,0x75,0x70,0x6
	.cfi_escape 0x10,0x7,0x2,0x75,0x7c
	.cfi_escape 0x10,0x6,0x2,0x75,0x78
	.cfi_escape 0x10,0x3,0x2,0x75,0x74
	subl	$68, %esp
	.loc 1 24 5 view .LVU64
	pushl	$.LC5
	call	print
.LVL23:
	.loc 1 27 5 is_stmt 1 view .LVU65
	movl	$.LC6, (%esp)
	call	print
.LVL24:
	.loc 1 28 5 view .LVU66
	.loc 1 28 15 is_stmt 0 view .LVU67
	popl	%ebx
	popl	%esi
	pushl	$493
	pushl	$.LC7
	.loc 1 36 11 view .LVU68
	leal	-68(%ebp), %esi
	.loc 1 28 15 view .LVU69
	call	mkdir
.LVL25:
	.loc 1 29 5 view .LVU70
	movl	$.LC8, (%esp)
	.loc 1 28 15 view .LVU71
	movl	%eax, %ebx
.LVL26:
	.loc 1 29 5 is_stmt 1 view .LVU72
	call	print
.LVL27:
	.loc 1 30 5 view .LVU73
	movl	%ebx, (%esp)
	call	print_num
.LVL28:
	.loc 1 31 5 view .LVU74
	.loc 1 31 5 is_stmt 0 discriminator 1 view .LVU75
	testl	%ebx, %ebx
	movl	$.LC1, %edx
	movl	$.LC2, %eax
	cmove	%edx, %eax
	.loc 1 31 5 discriminator 4 view .LVU76
	movl	%eax, (%esp)
	call	print
.LVL29:
	.loc 1 34 5 is_stmt 1 view .LVU77
	movl	$.LC9, (%esp)
	call	print
.LVL30:
	.loc 1 35 5 view .LVU78
	.loc 1 36 5 view .LVU79
	.loc 1 36 11 is_stmt 0 view .LVU80
	popl	%edi
	popl	%eax
	pushl	%esi
	pushl	$.LC7
	call	stat
.LVL31:
	.loc 1 37 5 view .LVU81
	movl	$.LC10, (%esp)
	.loc 1 36 11 view .LVU82
	movl	%eax, %ebx
.LVL32:
	.loc 1 37 5 is_stmt 1 view .LVU83
	call	print
.LVL33:
	.loc 1 38 5 view .LVU84
	movl	%ebx, (%esp)
	call	print_num
.LVL34:
	.loc 1 39 5 view .LVU85
	.loc 1 39 8 is_stmt 0 view .LVU86
	addl	$16, %esp
	testl	%ebx, %ebx
	jne	.L21
	.loc 1 40 9 is_stmt 1 view .LVU87
	subl	$12, %esp
	pushl	$.LC11
	call	print
.LVL35:
	.loc 1 41 9 view .LVU88
	popl	%ecx
	pushl	-64(%ebp)
	call	print_num
.LVL36:
	.loc 1 42 9 view .LVU89
	movl	$.LC12, (%esp)
	call	print
.LVL37:
	.loc 1 43 9 view .LVU90
	movzwl	-60(%ebp), %eax
	movl	%eax, (%esp)
	call	print_num
.LVL38:
	.loc 1 44 9 view .LVU91
	movl	$.LC13, (%esp)
	call	print
.LVL39:
	addl	$16, %esp
.L22:
	.loc 1 50 5 view .LVU92
	subl	$12, %esp
	.loc 1 54 5 is_stmt 0 discriminator 1 view .LVU93
	movl	$.LC4, %edi
	.loc 1 50 5 view .LVU94
	pushl	$.LC14
	call	print
.LVL40:
	.loc 1 51 5 is_stmt 1 view .LVU95
	.loc 1 51 11 is_stmt 0 view .LVU96
	popl	%eax
	popl	%edx
	pushl	$493
	pushl	$.LC7
	call	mkdir
.LVL41:
	.loc 1 52 5 view .LVU97
	movl	$.LC15, (%esp)
	.loc 1 51 11 view .LVU98
	movl	%eax, %ebx
.LVL42:
	.loc 1 52 5 is_stmt 1 view .LVU99
	call	print
.LVL43:
	.loc 1 53 5 view .LVU100
	movl	%ebx, (%esp)
	call	print_num
.LVL44:
	.loc 1 54 5 view .LVU101
	.loc 1 54 5 is_stmt 0 discriminator 1 view .LVU102
	cmpl	$-1, %ebx
	movl	$.LC3, %eax
	cmovne	%edi, %eax
	.loc 1 54 5 discriminator 4 view .LVU103
	movl	%eax, (%esp)
	call	print
.LVL45:
	.loc 1 57 5 is_stmt 1 view .LVU104
	movl	$.LC16, (%esp)
	call	print
.LVL46:
	.loc 1 58 5 view .LVU105
	.loc 1 58 11 is_stmt 0 view .LVU106
	popl	%ecx
	popl	%ebx
.LVL47:
	.loc 1 58 11 view .LVU107
	pushl	$.LC17
	pushl	$.LC7
	.loc 1 61 5 discriminator 1 view .LVU108
	movl	$.LC1, %ebx
	.loc 1 58 11 view .LVU109
	call	rename
.LVL48:
	.loc 1 59 5 view .LVU110
	movl	$.LC18, (%esp)
	.loc 1 58 11 view .LVU111
	movl	%eax, %edi
.LVL49:
	.loc 1 59 5 is_stmt 1 view .LVU112
	call	print
.LVL50:
	.loc 1 60 5 view .LVU113
	movl	%edi, (%esp)
	call	print_num
.LVL51:
	.loc 1 61 5 view .LVU114
	.loc 1 61 5 is_stmt 0 discriminator 1 view .LVU115
	testl	%edi, %edi
	movl	$.LC2, %eax
	cmove	%ebx, %eax
	.loc 1 61 5 discriminator 4 view .LVU116
	movl	%eax, (%esp)
	call	print
.LVL52:
	.loc 1 64 5 is_stmt 1 view .LVU117
	movl	$.LC19, (%esp)
	call	print
.LVL53:
	.loc 1 65 5 view .LVU118
	.loc 1 65 11 is_stmt 0 view .LVU119
	popl	%edi
.LVL54:
	.loc 1 65 11 view .LVU120
	popl	%eax
	pushl	%esi
	pushl	$.LC17
	call	stat
.LVL55:
	.loc 1 66 5 view .LVU121
	movl	$.LC20, (%esp)
	.loc 1 65 11 view .LVU122
	movl	%eax, %edi
.LVL56:
	.loc 1 66 5 is_stmt 1 view .LVU123
	call	print
.LVL57:
	.loc 1 67 5 view .LVU124
	movl	%edi, (%esp)
	call	print_num
.LVL58:
	.loc 1 68 5 view .LVU125
	.loc 1 68 5 is_stmt 0 discriminator 1 view .LVU126
	testl	%edi, %edi
	movl	$.LC2, %eax
	cmove	%ebx, %eax
	.loc 1 68 5 discriminator 4 view .LVU127
	movl	%eax, (%esp)
	call	print
.LVL59:
	.loc 1 71 5 is_stmt 1 view .LVU128
	movl	$.LC21, (%esp)
	call	print
.LVL60:
	.loc 1 72 5 view .LVU129
	.loc 1 72 11 is_stmt 0 view .LVU130
	popl	%eax
	popl	%edx
	pushl	%esi
	pushl	$.LC7
	call	stat
.LVL61:
	.loc 1 73 5 view .LVU131
	movl	$.LC10, (%esp)
	.loc 1 72 11 view .LVU132
	movl	%eax, %edi
.LVL62:
	.loc 1 73 5 is_stmt 1 view .LVU133
	call	print
.LVL63:
	.loc 1 74 5 view .LVU134
	movl	%edi, (%esp)
	call	print_num
.LVL64:
	.loc 1 75 5 view .LVU135
	.loc 1 75 5 is_stmt 0 discriminator 1 view .LVU136
	cmpl	$-1, %edi
	movl	$.LC3, %eax
	movl	$.LC4, %edi
.LVL65:
	.loc 1 75 5 discriminator 1 view .LVU137
	cmovne	%edi, %eax
	.loc 1 75 5 discriminator 4 view .LVU138
	movl	%eax, (%esp)
	call	print
.LVL66:
	.loc 1 78 5 is_stmt 1 view .LVU139
	movl	$.LC22, (%esp)
	call	print
.LVL67:
	.loc 1 79 5 view .LVU140
	.loc 1 79 11 is_stmt 0 view .LVU141
	movl	$.LC17, (%esp)
	call	rmdir
.LVL68:
	.loc 1 80 5 view .LVU142
	movl	$.LC23, (%esp)
	.loc 1 79 11 view .LVU143
	movl	%eax, %edi
.LVL69:
	.loc 1 80 5 is_stmt 1 view .LVU144
	call	print
.LVL70:
	.loc 1 81 5 view .LVU145
	movl	%edi, (%esp)
	call	print_num
.LVL71:
	.loc 1 82 5 view .LVU146
	.loc 1 82 5 is_stmt 0 discriminator 1 view .LVU147
	testl	%edi, %edi
	movl	$.LC2, %eax
	cmove	%ebx, %eax
	.loc 1 82 5 discriminator 4 view .LVU148
	movl	%eax, (%esp)
	call	print
.LVL72:
	.loc 1 85 5 is_stmt 1 view .LVU149
	movl	$.LC24, (%esp)
	call	print
.LVL73:
	.loc 1 86 5 view .LVU150
	.loc 1 86 11 is_stmt 0 view .LVU151
	popl	%ecx
	popl	%edi
.LVL74:
	.loc 1 86 11 view .LVU152
	pushl	%esi
	pushl	$.LC17
	.loc 1 89 5 discriminator 1 view .LVU153
	movl	$.LC4, %edi
	.loc 1 86 11 view .LVU154
	call	stat
.LVL75:
	.loc 1 87 5 view .LVU155
	movl	$.LC20, (%esp)
	.loc 1 86 11 view .LVU156
	movl	%eax, %esi
.LVL76:
	.loc 1 87 5 is_stmt 1 view .LVU157
	call	print
.LVL77:
	.loc 1 88 5 view .LVU158
	movl	%esi, (%esp)
	call	print_num
.LVL78:
	.loc 1 89 5 view .LVU159
	.loc 1 89 5 is_stmt 0 discriminator 1 view .LVU160
	cmpl	$-1, %esi
	movl	$.LC3, %eax
	cmovne	%edi, %eax
	.loc 1 89 5 discriminator 4 view .LVU161
	movl	%eax, (%esp)
	call	print
.LVL79:
	.loc 1 92 5 is_stmt 1 view .LVU162
	movl	$.LC25, (%esp)
	call	print
.LVL80:
	.loc 1 93 5 view .LVU163
	.loc 1 93 11 is_stmt 0 view .LVU164
	popl	%eax
	popl	%edx
	pushl	$493
	pushl	$.LC26
	call	mkdir
.LVL81:
	.loc 1 94 5 view .LVU165
	movl	$.LC27, (%esp)
	.loc 1 93 11 view .LVU166
	movl	%eax, %esi
.LVL82:
	.loc 1 94 5 is_stmt 1 view .LVU167
	call	print
.LVL83:
	.loc 1 95 5 view .LVU168
	movl	%esi, (%esp)
	call	print_num
.LVL84:
	.loc 1 96 5 view .LVU169
	.loc 1 96 5 is_stmt 0 discriminator 1 view .LVU170
	testl	%esi, %esi
	movl	$.LC2, %eax
	cmove	%ebx, %eax
	.loc 1 96 5 discriminator 4 view .LVU171
	movl	%eax, (%esp)
	call	print
.LVL85:
	.loc 1 99 5 is_stmt 1 view .LVU172
	movl	$.LC28, (%esp)
	call	print
.LVL86:
	.loc 1 100 5 view .LVU173
	.loc 1 100 11 is_stmt 0 view .LVU174
	movl	$.LC26, (%esp)
	call	unlink
.LVL87:
	.loc 1 101 5 view .LVU175
	movl	$.LC29, (%esp)
	.loc 1 100 11 view .LVU176
	movl	%eax, %ebx
.LVL88:
	.loc 1 101 5 is_stmt 1 view .LVU177
	call	print
.LVL89:
	.loc 1 102 5 view .LVU178
	movl	%ebx, (%esp)
	call	print_num
.LVL90:
	.loc 1 103 5 view .LVU179
	.loc 1 103 5 is_stmt 0 discriminator 1 view .LVU180
	cmpl	$-1, %ebx
	movl	$.LC3, %eax
	cmovne	%edi, %eax
	.loc 1 103 5 discriminator 4 view .LVU181
	movl	%eax, (%esp)
	call	print
.LVL91:
	.loc 1 105 5 is_stmt 1 view .LVU182
	movl	$.LC30, (%esp)
	call	print
.LVL92:
	.loc 1 106 5 view .LVU183
	call	shutdown
.LVL93:
	.loc 1 107 5 view .LVU184
	.loc 1 108 1 is_stmt 0 view .LVU185
	addl	$16, %esp
	leal	-16(%ebp), %esp
	xorl	%eax, %eax
	popl	%ecx
	.cfi_remember_state
	.cfi_restore 1
	.cfi_def_cfa 1, 0
	popl	%ebx
	.cfi_restore 3
.LVL94:
	.loc 1 108 1 view .LVU186
	popl	%esi
	.cfi_restore 6
	popl	%edi
	.cfi_restore 7
	popl	%ebp
	.cfi_restore 5
	leal	-4(%ecx), %esp
	.cfi_def_cfa 4, 4
	ret
.LVL95:
.L21:
	.cfi_restore_state
	.loc 1 46 9 is_stmt 1 view .LVU187
	subl	$12, %esp
	pushl	$.LC2
	call	print
.LVL96:
	addl	$16, %esp
	jmp	.L22
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.text
.Letext0:
	.file 2 "stdint.h"
	.file 3 "sys.h"
	.file 4 "<built-in>"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x5bd
	.value	0x5
	.byte	0x1
	.byte	0x4
	.long	.Ldebug_abbrev0
	.uleb128 0x10
	.long	.LASF36
	.byte	0xc
	.long	.LASF0
	.long	.LASF1
	.long	.LLRL10
	.long	0
	.long	.Ldebug_line0
	.uleb128 0x4
	.byte	0x1
	.byte	0x8
	.long	.LASF2
	.uleb128 0x4
	.byte	0x1
	.byte	0x6
	.long	.LASF3
	.uleb128 0x11
	.long	0x2d
	.uleb128 0x6
	.long	.LASF6
	.byte	0x2
	.byte	0x7
	.byte	0x18
	.long	0x45
	.uleb128 0x4
	.byte	0x2
	.byte	0x7
	.long	.LASF4
	.uleb128 0x4
	.byte	0x2
	.byte	0x5
	.long	.LASF5
	.uleb128 0x6
	.long	.LASF7
	.byte	0x2
	.byte	0xa
	.byte	0x17
	.long	0x5f
	.uleb128 0x4
	.byte	0x4
	.byte	0x7
	.long	.LASF8
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.long	.LASF9
	.uleb128 0x6
	.long	.LASF10
	.byte	0x2
	.byte	0x13
	.byte	0x16
	.long	0x79
	.uleb128 0x4
	.byte	0x4
	.byte	0x7
	.long	.LASF11
	.uleb128 0x6
	.long	.LASF12
	.byte	0x2
	.byte	0x14
	.byte	0xd
	.long	0x8c
	.uleb128 0x12
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x4
	.byte	0x8
	.byte	0x7
	.long	.LASF13
	.uleb128 0x6
	.long	.LASF14
	.byte	0x3
	.byte	0x57
	.byte	0x16
	.long	0x79
	.uleb128 0x13
	.long	.LASF31
	.byte	0x2c
	.byte	0x3
	.byte	0x59
	.byte	0x8
	.long	0x143
	.uleb128 0x2
	.long	.LASF15
	.byte	0x5a
	.long	0x53
	.byte	0
	.uleb128 0x2
	.long	.LASF16
	.byte	0x5b
	.long	0x53
	.byte	0x4
	.uleb128 0x2
	.long	.LASF17
	.byte	0x5c
	.long	0x39
	.byte	0x8
	.uleb128 0x2
	.long	.LASF18
	.byte	0x5d
	.long	0x39
	.byte	0xa
	.uleb128 0x2
	.long	.LASF19
	.byte	0x5e
	.long	0x39
	.byte	0xc
	.uleb128 0x2
	.long	.LASF20
	.byte	0x5f
	.long	0x39
	.byte	0xe
	.uleb128 0x2
	.long	.LASF21
	.byte	0x60
	.long	0x53
	.byte	0x10
	.uleb128 0x2
	.long	.LASF22
	.byte	0x61
	.long	0x53
	.byte	0x14
	.uleb128 0x2
	.long	.LASF23
	.byte	0x62
	.long	0x53
	.byte	0x18
	.uleb128 0x2
	.long	.LASF24
	.byte	0x63
	.long	0x53
	.byte	0x1c
	.uleb128 0x2
	.long	.LASF25
	.byte	0x64
	.long	0x53
	.byte	0x20
	.uleb128 0x2
	.long	.LASF26
	.byte	0x65
	.long	0x53
	.byte	0x24
	.uleb128 0x2
	.long	.LASF27
	.byte	0x66
	.long	0x53
	.byte	0x28
	.byte	0
	.uleb128 0x14
	.long	.LASF37
	.byte	0x3
	.byte	0x38
	.byte	0xc
	.long	0x8c
	.uleb128 0x5
	.long	.LASF28
	.byte	0x83
	.byte	0xc
	.long	0x8c
	.long	0x164
	.uleb128 0x3
	.long	0x164
	.byte	0
	.uleb128 0xb
	.long	0x34
	.uleb128 0x5
	.long	.LASF29
	.byte	0x82
	.byte	0xc
	.long	0x8c
	.long	0x17e
	.uleb128 0x3
	.long	0x164
	.byte	0
	.uleb128 0x5
	.long	.LASF30
	.byte	0x84
	.byte	0xc
	.long	0x8c
	.long	0x198
	.uleb128 0x3
	.long	0x164
	.uleb128 0x3
	.long	0x164
	.byte	0
	.uleb128 0x5
	.long	.LASF31
	.byte	0x7c
	.byte	0xc
	.long	0x8c
	.long	0x1b2
	.uleb128 0x3
	.long	0x164
	.uleb128 0x3
	.long	0x1b2
	.byte	0
	.uleb128 0xb
	.long	0xa6
	.uleb128 0x5
	.long	.LASF32
	.byte	0x81
	.byte	0xc
	.long	0x8c
	.long	0x1d1
	.uleb128 0x3
	.long	0x164
	.uleb128 0x3
	.long	0x9a
	.byte	0
	.uleb128 0x5
	.long	.LASF33
	.byte	0x1d
	.byte	0x10
	.long	0x80
	.long	0x1f0
	.uleb128 0x3
	.long	0x8c
	.uleb128 0x3
	.long	0x1f0
	.uleb128 0x3
	.long	0x6d
	.byte	0
	.uleb128 0x15
	.byte	0x4
	.uleb128 0x16
	.long	.LASF38
	.byte	0x1
	.byte	0x17
	.byte	0x5
	.long	0x8c
	.long	.LFB2
	.long	.LFE2-.LFB2
	.uleb128 0x1
	.byte	0x9c
	.long	0x43a
	.uleb128 0x17
	.string	"ret"
	.byte	0x1
	.byte	0x1c
	.byte	0x9
	.long	0x8c
	.long	.LLST9
	.long	.LVUS9
	.uleb128 0x18
	.string	"st"
	.byte	0x1
	.byte	0x23
	.byte	0x11
	.long	0xa6
	.uleb128 0x3
	.byte	0x75
	.sleb128 -68
	.uleb128 0x1
	.long	.LVL23
	.long	0x457
	.uleb128 0x1
	.long	.LVL24
	.long	0x457
	.uleb128 0x1
	.long	.LVL25
	.long	0x1b7
	.uleb128 0x1
	.long	.LVL27
	.long	0x457
	.uleb128 0x1
	.long	.LVL28
	.long	0x43a
	.uleb128 0x1
	.long	.LVL29
	.long	0x457
	.uleb128 0x1
	.long	.LVL30
	.long	0x457
	.uleb128 0x1
	.long	.LVL31
	.long	0x198
	.uleb128 0x1
	.long	.LVL33
	.long	0x457
	.uleb128 0x1
	.long	.LVL34
	.long	0x43a
	.uleb128 0x1
	.long	.LVL35
	.long	0x457
	.uleb128 0x1
	.long	.LVL36
	.long	0x43a
	.uleb128 0x1
	.long	.LVL37
	.long	0x457
	.uleb128 0x1
	.long	.LVL38
	.long	0x43a
	.uleb128 0x1
	.long	.LVL39
	.long	0x457
	.uleb128 0x1
	.long	.LVL40
	.long	0x457
	.uleb128 0x1
	.long	.LVL41
	.long	0x1b7
	.uleb128 0x1
	.long	.LVL43
	.long	0x457
	.uleb128 0x1
	.long	.LVL44
	.long	0x43a
	.uleb128 0x1
	.long	.LVL45
	.long	0x457
	.uleb128 0x1
	.long	.LVL46
	.long	0x457
	.uleb128 0x1
	.long	.LVL48
	.long	0x17e
	.uleb128 0x1
	.long	.LVL50
	.long	0x457
	.uleb128 0x1
	.long	.LVL51
	.long	0x43a
	.uleb128 0x1
	.long	.LVL52
	.long	0x457
	.uleb128 0x1
	.long	.LVL53
	.long	0x457
	.uleb128 0x1
	.long	.LVL55
	.long	0x198
	.uleb128 0x1
	.long	.LVL57
	.long	0x457
	.uleb128 0x1
	.long	.LVL58
	.long	0x43a
	.uleb128 0x1
	.long	.LVL59
	.long	0x457
	.uleb128 0x1
	.long	.LVL60
	.long	0x457
	.uleb128 0x1
	.long	.LVL61
	.long	0x198
	.uleb128 0x1
	.long	.LVL63
	.long	0x457
	.uleb128 0x1
	.long	.LVL64
	.long	0x43a
	.uleb128 0x1
	.long	.LVL66
	.long	0x457
	.uleb128 0x1
	.long	.LVL67
	.long	0x457
	.uleb128 0x1
	.long	.LVL68
	.long	0x169
	.uleb128 0x1
	.long	.LVL70
	.long	0x457
	.uleb128 0x1
	.long	.LVL71
	.long	0x43a
	.uleb128 0x1
	.long	.LVL72
	.long	0x457
	.uleb128 0x1
	.long	.LVL73
	.long	0x457
	.uleb128 0x1
	.long	.LVL75
	.long	0x198
	.uleb128 0x1
	.long	.LVL77
	.long	0x457
	.uleb128 0x1
	.long	.LVL78
	.long	0x43a
	.uleb128 0x1
	.long	.LVL79
	.long	0x457
	.uleb128 0x1
	.long	.LVL80
	.long	0x457
	.uleb128 0x1
	.long	.LVL81
	.long	0x1b7
	.uleb128 0x1
	.long	.LVL83
	.long	0x457
	.uleb128 0x1
	.long	.LVL84
	.long	0x43a
	.uleb128 0x1
	.long	.LVL85
	.long	0x457
	.uleb128 0x1
	.long	.LVL86
	.long	0x457
	.uleb128 0x1
	.long	.LVL87
	.long	0x14f
	.uleb128 0x1
	.long	.LVL89
	.long	0x457
	.uleb128 0x1
	.long	.LVL90
	.long	0x43a
	.uleb128 0x1
	.long	.LVL91
	.long	0x457
	.uleb128 0x1
	.long	.LVL92
	.long	0x457
	.uleb128 0x1
	.long	.LVL93
	.long	0x143
	.uleb128 0x1
	.long	.LVL96
	.long	0x457
	.byte	0
	.uleb128 0xc
	.long	.LASF34
	.byte	0xb
	.long	0x457
	.uleb128 0xd
	.string	"n"
	.byte	0xb
	.byte	0x14
	.long	0x8c
	.uleb128 0x8
	.string	"c"
	.byte	0x13
	.byte	0xa
	.long	0x2d
	.byte	0
	.uleb128 0xc
	.long	.LASF35
	.byte	0x4
	.long	0x481
	.uleb128 0xd
	.string	"msg"
	.byte	0x4
	.byte	0x18
	.long	0x164
	.uleb128 0x8
	.string	"len"
	.byte	0x5
	.byte	0x9
	.long	0x8c
	.uleb128 0x8
	.string	"p"
	.byte	0x6
	.byte	0x11
	.long	0x164
	.byte	0
	.uleb128 0xe
	.long	0x457
	.long	.LFB0
	.long	.LFE0-.LFB0
	.uleb128 0x1
	.byte	0x9c
	.long	0x4c9
	.uleb128 0x19
	.long	0x461
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x7
	.long	0x46c
	.long	.LLST0
	.long	.LVUS0
	.uleb128 0x7
	.long	0x477
	.long	.LLST1
	.long	.LVUS1
	.uleb128 0x1
	.long	.LVL3
	.long	0x5b5
	.uleb128 0x1
	.long	.LVL5
	.long	0x1d1
	.byte	0
	.uleb128 0xe
	.long	0x43a
	.long	.LFB1
	.long	.LFE1-.LFB1
	.uleb128 0x1
	.byte	0x9c
	.long	0x5b5
	.uleb128 0x9
	.long	0x444
	.long	.LLST2
	.long	.LVUS2
	.uleb128 0xa
	.long	0x44d
	.uleb128 0x2
	.byte	0x91
	.sleb128 -17
	.uleb128 0x1a
	.long	0x43a
	.long	.LBI12
	.byte	.LVU28
	.long	.LLRL3
	.byte	0x1
	.byte	0x11
	.byte	0x9
	.long	0x56c
	.uleb128 0x9
	.long	0x444
	.long	.LLST4
	.long	.LVUS4
	.uleb128 0xf
	.long	.LLRL3
	.uleb128 0xa
	.long	0x44d
	.uleb128 0x2
	.byte	0x91
	.sleb128 -17
	.uleb128 0x1b
	.long	0x43a
	.long	.LBI14
	.byte	.LVU49
	.long	.LBB14
	.long	.LBE14-.LBB14
	.byte	0x1
	.byte	0x11
	.byte	0x9
	.long	0x561
	.uleb128 0x9
	.long	0x444
	.long	.LLST5
	.long	.LVUS5
	.uleb128 0xa
	.long	0x44d
	.uleb128 0x2
	.byte	0x91
	.sleb128 -17
	.uleb128 0x1
	.long	.LVL20
	.long	0x1d1
	.uleb128 0x1
	.long	.LVL22
	.long	0x43a
	.byte	0
	.uleb128 0x1
	.long	.LVL14
	.long	0x1d1
	.byte	0
	.byte	0
	.uleb128 0x1c
	.long	0x457
	.long	.LLRL6
	.byte	0x1
	.byte	0xd
	.byte	0x9
	.long	0x5ab
	.uleb128 0x1d
	.long	0x461
	.uleb128 0xf
	.long	.LLRL6
	.uleb128 0x7
	.long	0x46c
	.long	.LLST7
	.long	.LVUS7
	.uleb128 0x7
	.long	0x477
	.long	.LLST8
	.long	.LVUS8
	.uleb128 0x1
	.long	.LVL16
	.long	0x1d1
	.byte	0
	.byte	0
	.uleb128 0x1
	.long	.LVL10
	.long	0x1d1
	.byte	0
	.uleb128 0x1e
	.long	.LASF39
	.long	.LASF40
	.byte	0x4
	.byte	0
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x48
	.byte	0
	.uleb128 0x7d
	.uleb128 0x1
	.uleb128 0x7f
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 3
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 14
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 3
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.uleb128 0x2137
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.uleb128 0x2137
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0x21
	.sleb128 4
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 6
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x20
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7a
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x55
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x1f
	.uleb128 0x1b
	.uleb128 0x1f
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7a
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.uleb128 0x2137
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x2138
	.uleb128 0xb
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.uleb128 0x57
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x2138
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.uleb128 0x57
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.uleb128 0x57
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loclists,"",@progbits
	.long	.Ldebug_loc3-.Ldebug_loc2
.Ldebug_loc2:
	.value	0x5
	.byte	0x4
	.byte	0
	.long	0
.Ldebug_loc0:
.LVUS9:
	.uleb128 .LVU72
	.uleb128 .LVU73
	.uleb128 .LVU73
	.uleb128 .LVU83
	.uleb128 .LVU83
	.uleb128 .LVU84
	.uleb128 .LVU84
	.uleb128 .LVU99
	.uleb128 .LVU99
	.uleb128 .LVU100
	.uleb128 .LVU100
	.uleb128 .LVU107
	.uleb128 .LVU112
	.uleb128 .LVU113
	.uleb128 .LVU113
	.uleb128 .LVU120
	.uleb128 .LVU123
	.uleb128 .LVU124
	.uleb128 .LVU124
	.uleb128 .LVU133
	.uleb128 .LVU133
	.uleb128 .LVU134
	.uleb128 .LVU134
	.uleb128 .LVU137
	.uleb128 .LVU144
	.uleb128 .LVU145
	.uleb128 .LVU145
	.uleb128 .LVU152
	.uleb128 .LVU157
	.uleb128 .LVU158
	.uleb128 .LVU158
	.uleb128 .LVU167
	.uleb128 .LVU167
	.uleb128 .LVU168
	.uleb128 .LVU168
	.uleb128 .LVU177
	.uleb128 .LVU177
	.uleb128 .LVU178
	.uleb128 .LVU178
	.uleb128 .LVU186
	.uleb128 .LVU187
	.uleb128 0
.LLST9:
	.byte	0x6
	.long	.LVL26
	.byte	0x4
	.uleb128 .LVL26-.LVL26
	.uleb128 .LVL27-1-.LVL26
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL27-1-.LVL26
	.uleb128 .LVL32-.LVL26
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL32-.LVL26
	.uleb128 .LVL33-1-.LVL26
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL33-1-.LVL26
	.uleb128 .LVL42-.LVL26
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL42-.LVL26
	.uleb128 .LVL43-1-.LVL26
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL43-1-.LVL26
	.uleb128 .LVL47-.LVL26
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL49-.LVL26
	.uleb128 .LVL50-1-.LVL26
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL50-1-.LVL26
	.uleb128 .LVL54-.LVL26
	.uleb128 0x1
	.byte	0x57
	.byte	0x4
	.uleb128 .LVL56-.LVL26
	.uleb128 .LVL57-1-.LVL26
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL57-1-.LVL26
	.uleb128 .LVL62-.LVL26
	.uleb128 0x1
	.byte	0x57
	.byte	0x4
	.uleb128 .LVL62-.LVL26
	.uleb128 .LVL63-1-.LVL26
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL63-1-.LVL26
	.uleb128 .LVL65-.LVL26
	.uleb128 0x1
	.byte	0x57
	.byte	0x4
	.uleb128 .LVL69-.LVL26
	.uleb128 .LVL70-1-.LVL26
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL70-1-.LVL26
	.uleb128 .LVL74-.LVL26
	.uleb128 0x1
	.byte	0x57
	.byte	0x4
	.uleb128 .LVL76-.LVL26
	.uleb128 .LVL77-1-.LVL26
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL77-1-.LVL26
	.uleb128 .LVL82-.LVL26
	.uleb128 0x1
	.byte	0x56
	.byte	0x4
	.uleb128 .LVL82-.LVL26
	.uleb128 .LVL83-1-.LVL26
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL83-1-.LVL26
	.uleb128 .LVL88-.LVL26
	.uleb128 0x1
	.byte	0x56
	.byte	0x4
	.uleb128 .LVL88-.LVL26
	.uleb128 .LVL89-1-.LVL26
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL89-1-.LVL26
	.uleb128 .LVL94-.LVL26
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL95-.LVL26
	.uleb128 .LFE2-.LVL26
	.uleb128 0x1
	.byte	0x53
	.byte	0
.LVUS0:
	.uleb128 .LVU2
	.uleb128 .LVU10
	.uleb128 .LVU12
	.uleb128 0
.LLST0:
	.byte	0x6
	.long	.LVL0
	.byte	0x4
	.uleb128 .LVL0-.LVL0
	.uleb128 .LVL4-.LVL0
	.uleb128 0x2
	.byte	0x30
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL6-.LVL0
	.uleb128 .LFE0-.LVL0
	.uleb128 0x2
	.byte	0x30
	.byte	0x9f
	.byte	0
.LVUS1:
	.uleb128 .LVU5
	.uleb128 .LVU7
	.uleb128 .LVU7
	.uleb128 .LVU8
	.uleb128 .LVU8
	.uleb128 .LVU9
	.uleb128 .LVU9
	.uleb128 .LVU10
	.uleb128 .LVU12
	.uleb128 0
.LLST1:
	.byte	0x6
	.long	.LVL1
	.byte	0x4
	.uleb128 .LVL1-.LVL1
	.uleb128 .LVL1-.LVL1
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL1-.LVL1
	.uleb128 .LVL2-.LVL1
	.uleb128 0x3
	.byte	0x73
	.sleb128 1
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL2-.LVL1
	.uleb128 .LVL3-1-.LVL1
	.uleb128 0x1
	.byte	0x50
	.byte	0x4
	.uleb128 .LVL3-1-.LVL1
	.uleb128 .LVL4-.LVL1
	.uleb128 0x3
	.byte	0x73
	.sleb128 1
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL6-.LVL1
	.uleb128 .LFE0-.LVL1
	.uleb128 0x3
	.byte	0x73
	.sleb128 1
	.byte	0x9f
	.byte	0
.LVUS2:
	.uleb128 0
	.uleb128 .LVU18
	.uleb128 .LVU18
	.uleb128 .LVU24
	.uleb128 .LVU27
	.uleb128 .LVU39
	.uleb128 .LVU39
	.uleb128 .LVU47
	.uleb128 .LVU47
	.uleb128 0
.LLST2:
	.byte	0x6
	.long	.LVL7
	.byte	0x4
	.uleb128 .LVL7-.LVL7
	.uleb128 .LVL8-.LVL7
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.byte	0x4
	.uleb128 .LVL8-.LVL7
	.uleb128 .LVL9-.LVL7
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL11-.LVL7
	.uleb128 .LVL15-.LVL7
	.uleb128 0x1
	.byte	0x53
	.byte	0x4
	.uleb128 .LVL15-.LVL7
	.uleb128 .LVL16-.LVL7
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.byte	0x4
	.uleb128 .LVL16-.LVL7
	.uleb128 .LFE1-.LVL7
	.uleb128 0x1
	.byte	0x53
	.byte	0
.LVUS4:
	.uleb128 .LVU28
	.uleb128 .LVU36
	.uleb128 .LVU48
	.uleb128 0
.LLST4:
	.byte	0x6
	.long	.LVL12
	.byte	0x4
	.uleb128 .LVL12-.LVL12
	.uleb128 .LVL13-.LVL12
	.uleb128 0x1
	.byte	0x57
	.byte	0x4
	.uleb128 .LVL17-.LVL12
	.uleb128 .LFE1-.LVL12
	.uleb128 0x1
	.byte	0x57
	.byte	0
.LVUS5:
	.uleb128 .LVU49
	.uleb128 .LVU57
	.uleb128 .LVU60
	.uleb128 0
.LLST5:
	.byte	0x6
	.long	.LVL18
	.byte	0x4
	.uleb128 .LVL18-.LVL18
	.uleb128 .LVL19-.LVL18
	.uleb128 0x1
	.byte	0x56
	.byte	0x4
	.uleb128 .LVL21-.LVL18
	.uleb128 .LFE1-.LVL18
	.uleb128 0x1
	.byte	0x56
	.byte	0
.LVUS7:
	.uleb128 .LVU39
	.uleb128 .LVU41
	.uleb128 .LVU41
	.uleb128 .LVU45
.LLST7:
	.byte	0x6
	.long	.LVL15
	.byte	0x4
	.uleb128 .LVL15-.LVL15
	.uleb128 .LVL15-.LVL15
	.uleb128 0x2
	.byte	0x30
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL15-.LVL15
	.uleb128 .LVL16-.LVL15
	.uleb128 0x2
	.byte	0x31
	.byte	0x9f
	.byte	0
.LVUS8:
	.uleb128 .LVU39
	.uleb128 .LVU42
	.uleb128 .LVU42
	.uleb128 .LVU45
.LLST8:
	.byte	0x6
	.long	.LVL15
	.byte	0x4
	.uleb128 .LVL15-.LVL15
	.uleb128 .LVL15-.LVL15
	.uleb128 0x6
	.byte	0x3
	.long	.LC0+1
	.byte	0x9f
	.byte	0x4
	.uleb128 .LVL15-.LVL15
	.uleb128 .LVL16-.LVL15
	.uleb128 0x6
	.byte	0x3
	.long	.LC0+2
	.byte	0x9f
	.byte	0
.Ldebug_loc3:
	.section	.debug_aranges,"",@progbits
	.long	0x24
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x4
	.byte	0
	.value	0
	.value	0
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	.LFB2
	.long	.LFE2-.LFB2
	.long	0
	.long	0
	.section	.debug_rnglists,"",@progbits
.Ldebug_ranges0:
	.long	.Ldebug_ranges3-.Ldebug_ranges2
.Ldebug_ranges2:
	.value	0x5
	.byte	0x4
	.byte	0
	.long	0
.LLRL3:
	.byte	0x5
	.long	.LBB12
	.byte	0x4
	.uleb128 .LBB12-.LBB12
	.uleb128 .LBE12-.LBB12
	.byte	0x4
	.uleb128 .LBB21-.LBB12
	.uleb128 .LBE21-.LBB12
	.byte	0
.LLRL6:
	.byte	0x5
	.long	.LBB17
	.byte	0x4
	.uleb128 .LBB17-.LBB17
	.uleb128 .LBE17-.LBB17
	.byte	0x4
	.uleb128 .LBB20-.LBB17
	.uleb128 .LBE20-.LBB17
	.byte	0
.LLRL10:
	.byte	0x7
	.long	.Ltext0
	.uleb128 .Letext0-.Ltext0
	.byte	0x7
	.long	.LFB2
	.uleb128 .LFE2-.LFB2
	.byte	0
.Ldebug_ranges3:
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF26:
	.string	"st_mtime"
.LASF10:
	.string	"size_t"
.LASF31:
	.string	"stat"
.LASF20:
	.string	"st_gid"
.LASF25:
	.string	"st_atime"
.LASF28:
	.string	"unlink"
.LASF18:
	.string	"st_nlink"
.LASF30:
	.string	"rename"
.LASF32:
	.string	"mkdir"
.LASF22:
	.string	"st_size"
.LASF34:
	.string	"print_num"
.LASF12:
	.string	"ssize_t"
.LASF16:
	.string	"st_ino"
.LASF23:
	.string	"st_blksize"
.LASF39:
	.string	"strlen"
.LASF21:
	.string	"st_rdev"
.LASF13:
	.string	"long long unsigned int"
.LASF2:
	.string	"unsigned char"
.LASF24:
	.string	"st_blocks"
.LASF8:
	.string	"long unsigned int"
.LASF17:
	.string	"st_mode"
.LASF4:
	.string	"short unsigned int"
.LASF33:
	.string	"write"
.LASF40:
	.string	"__builtin_strlen"
.LASF38:
	.string	"main"
.LASF29:
	.string	"rmdir"
.LASF27:
	.string	"st_ctime"
.LASF11:
	.string	"unsigned int"
.LASF36:
	.string	"GNU C99 14.1.0 -m32 -mtune=generic -march=x86-64 -g -O2 -std=c99 -fno-builtin-strlen -fcf-protection=none"
.LASF3:
	.string	"char"
.LASF37:
	.string	"shutdown"
.LASF19:
	.string	"st_uid"
.LASF35:
	.string	"print"
.LASF5:
	.string	"short int"
.LASF15:
	.string	"st_dev"
.LASF6:
	.string	"uint16_t"
.LASF7:
	.string	"uint32_t"
.LASF9:
	.string	"long int"
.LASF14:
	.string	"mode_t"
	.section	.debug_line_str,"MS",@progbits,1
.LASF1:
	.string	"/u/rpark/cs439/Prog8/p8test.dir/sbin"
.LASF0:
	.string	"test_fs.c"
	.ident	"GCC: (GNU) 14.1.0"
	.section	.note.GNU-stack,"",@progbits
