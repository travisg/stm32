.text
.syntax unified

.section .text.boot
.global _vectab
_vectab:
	.word _initial_stack_end
	.word _main

.section .bss.initial_stack
.align 3
_initial_stack:
	.skip 0x1000
_initial_stack_end:
