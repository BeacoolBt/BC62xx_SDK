	.syntax	unified
	.arch	armv6-m
	
	 .extern main

	.section .stack
	.align	3

	.equ	Stack_Size, 0x1000

	.globl	__StackTop
	.globl	__StackLimit
__StackLimit:
	.space	Stack_Size
	.size	__StackLimit, . - __StackLimit
__StackTop:
	.size	__StackTop, . - __StackTop

	.section .heap
	.align	3

	.equ	Heap_Size, 0

	.globl	__HeapBase
	.globl	__HeapLimit
__HeapBase:
	.if	Heap_Size
	.space	Heap_Size
	.endif
	.size	__HeapBase, . - __HeapBase
__HeapLimit:
	.size	__HeapLimit, . - __HeapLimit

	.section .isr_vector
	.align 2
	.globl	__isr_vector
__isr_vector:
		.long	0x10014000
		.long	Reset_Handler	
		.long	NMI_Handler
		.long	HardFault_Handler
		.long 0
		.long	0
		.long	0
		.long	0
		.long	0
		.long	0
		.long	0
		.long	0
		.long	0
		.long	0
		.long	0
		.long 	SysTick_Handler
		.long	usb_handler
		.long	iicd_handler
		.long	qspi_handler
		.long	spid_handler
		.long	uart_handler
		.long	uartb_handler
		.long	adcd_handler
		.long	i2s_handler
		.long	bt_handler
		.long 	gpio0_handler
		.long 	gpio1_handler
		.long 	gpi02_handler
		.long 	gpio3_handler
		.long 	gpio4_handler
		.long 	gpio5_handler
		.long 	gpio6_handler
		.long 	gpio7_handler
		.long 	gpio8_handler
		.long 	gpio9_handler
		.long 	gpio10_handler
		.long 	gpio11_handler
		.long 	gpio12_handler
		
	.size	__isr_vector, . - __isr_vector

	.text

	
	
	.thumb
	.thumb_func
	.align	1
	.globl	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:
	ldr	r1, =__etext
	ldr	r2, =__data_start__
	ldr	r3, =__data_end__

	subs	r3, r2
	ble	.L_loop1_done

.L_loop1:
	subs	r3, #4
	ldr	r0, [r1,r3]
	str	r0, [r2,r3]
	bgt	.L_loop1

.L_loop1_done:



/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__

	movs	r0, 0

	subs	r2, r1
	ble	.L_loop3_done

.L_loop3:
	subs	r2, #4
	str	r0, [r1, r2]
	bgt	.L_loop3
.L_loop3_done:

	ldr	r0,=0x12345
	ldr	r3,=0x1111
	bl	main

	.pool
	.size	Reset_Handler, . - Reset_Handler

	.align	1
	.thumb_func
	.weak	Default_Handler
	.type	Default_Handler, %function
Default_Handler:
	b	.
	.size	Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
	.macro	def_irq_handler	handler_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm

	def_irq_handler	NMI_Handler
	def_irq_handler	HardFault_Handler
	def_irq_handler	SVC_Handler
	def_irq_handler	PendSV_Handler
	def_irq_handler	SysTick_Handler
	def_irq_handler	DEF_IRQHandler
	def_irq_handler	usb_handler
	def_irq_handler 	iicd_handler
	def_irq_handler 	qspi_handler
	def_irq_handler 	spid_handler
	def_irq_handler 	uart_handler
	def_irq_handler 	uartb_handler 
	def_irq_handler 	adcd_handler
	def_irq_handler 	i2s_handler
	def_irq_handler 	bt_handler
	def_irq_handler 	gpio0_handler
	def_irq_handler 	gpio1_handler
	def_irq_handler 	gpi02_handler
	def_irq_handler 	gpio3_handler
	def_irq_handler 	gpio4_handler
	def_irq_handler 	gpio5_handler
	def_irq_handler 	gpio6_handler
	def_irq_handler 	gpio7_handler
	def_irq_handler 	gpio8_handler
	def_irq_handler 	gpio9_handler
	def_irq_handler 	gpio10_handler
	def_irq_handler 	gpio11_handler
	def_irq_handler 	gpio12_handler
		
	
@ int __aeabi_idiv(int num:r0, int denom:r1)
@
@ Divide r0 by r1 and return quotient in r0 (all signed).
@ Use __aeabi_uidivmod() but check signs before and change signs afterwards.
@

	.syntax unified				@should be here
	.thumb_func
	.global __aeabi_idiv
__aeabi_idiv:
	cmp	r0, #0
	bge	L_num_pos
	rsbs r0, #0
	cmp	r1, #0
	bge	L_neg_result
	rsbs r1, #0		@ den = -den
	b	__aeabi_uidivmod
L_num_pos:
	cmp	r1, #0
	bge	__aeabi_uidivmod
	rsbs r1, #0		@ den = -den
L_neg_result:
	push	{lr}
	bl	__aeabi_uidivmod
	rsbs r0, #0		@ quot = -quot
	pop	{pc}

@ {int quotient:r0, int remainder:r1}
@ __aeabi_idivmod(int numerator:r0, int denominator:r1)
@
@ Divide r0 by r1 and return the quotient in r0 and the remainder in r1
@
	.thumb_func
	.global __aeabi_idivmod
__aeabi_idivmod:
	cmp	r0, #0
	bge	L_num_pos_bis
	rsbs r0, #0		@ num = -num
	cmp	r1, #0
	bge	L_neg_both
	rsbs r1, #0		@ den = -den
	push	{lr}
	bl	__aeabi_uidivmod
	rsbs r1, #0		@ rem = -rem
	pop	{pc}
L_neg_both:
	push	{lr}
	bl	__aeabi_uidivmod
	rsbs r0, #0		@ quot = -quot
	rsbs r1, #0		@ rem = -rem
	pop	{pc}
L_num_pos_bis:
	cmp	r1, #0
	bge	__aeabi_uidivmod
	rsbs r1, #0		@ den = -den
	push	{lr}
	bl	__aeabi_uidivmod
	rsbs r0, #0		@ quot = -quot
	pop	{pc}

@ unsigned __aeabi_uidiv(unsigned num, unsigned denom)
@
@ Just an alias for __aeabi_uidivmod(), the remainder is ignored
@
	.thumb_func
	.global __aeabi_uidiv
__aeabi_uidiv:

@ {unsigned quotient:r0, unsigned remainder:r1}
@  __aeabi_uidivmod(unsigned numerator:r0, unsigned denominator:r1)
@
@ Divide r0 by r1 and return the quotient in r0 and the remainder in r1
@
	.thumb_func
	.global __aeabi_uidivmod
__aeabi_uidivmod:

	cmp	r1, #0
	bne	L_no_div0
	b	__aeabi_idiv0
L_no_div0:
	@ Shift left the denominator until it is greater than the numerator
	movs	r2, #1		@ counter
	movs	r3, #0		@ result
	cmp	r0, r1
	bls	L_sub_loop0
	adds	r1, #0		@ dont shift if denominator would overflow
	bmi	L_sub_loop0
L_denom_shift_loop:
	lsls	r2, #1
	lsls	r1, #1
	bmi	L_sub_loop0
	cmp	r0, r1
	bhi	L_denom_shift_loop
L_sub_loop0:
	cmp	r0, r1
	bcc	L_dont_sub0	@ if (num&gt;denom)
	subs	r0, r1		@ numerator -= denom
	orrs	r3, r2		@ result(r3) |= bitmask(r2)
L_dont_sub0:
	lsrs	r1, #1		@ denom(r1) &gt;&gt;= 1
	lsrs	r2, #1		@ bitmask(r2) &gt;&gt;= 1
	bne	L_sub_loop0
	mov	r1, r0		@ remainder(r1) = numerator(r0)
	mov	r0, r3		@ quotient(r0) = result(r3)
	bx	lr
__aeabi_idiv0:
	b	__aeabi_idiv0
	.end

	

	
