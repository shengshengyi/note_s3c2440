
.text
.global _start

_start:
	b reset  /* vector 0 : reset */
	ldr pc, und_addr /* vector 4 : 此句在nor中，执行会带回到sdram*/
					//b und_addr  /* 不用b相对跳转是担心如果程序超出4k会出错，跳到sdram则不用担心 */
	ldr pc, swi_addr /* vector 8 : 此句在nor中，执行会带回到sdram*/
	b halt			 /* vector 0x0c : prefetch aboot */
	b halt			 /* vector 0x10 : data abort */
	b halt			 /* vector 0x14 : reserved */
	ldr pc, irq_addr /* vector 0x18 : irq */
	b halt			 /* vector 0x1c : fiq */
	
	
	
und_addr:
	.word do_und

swi_addr:
	.word do_swi

irq_addr:
	.word do_irq
	
do_und:
	/* 执行到这里之前:
	 * 1. lr_und保存有被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_und保存有被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为11011, 进入到und模式
	 * 4. 跳到0x4的地方执行程序 
	 */

	/* sp_und未设置, 先设置它 */
	ldr sp, =0x34000000


	/* 在und异常处理函数中有可能会修改r0-r12, 所以先保存 */
	/* lr是异常处理完后的返回地址, 也要保存 */
	stmdb sp!, {r0-r12, lr}  
	
	/* 保存现场 */
	/* 处理und异常 */
	mrs r0, cpsr
	ldr r1, =und_string
	bl printException
	
	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^  /* ^会把spsr的值恢复到cpsr里 */
	
und_string:
	.string "undefined instruction exception"

.align 4

do_swi:
	/* 执行到这里之前:
	 * 1. lr_swi保存有被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_swi保存有被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为10011, 进入到svc模式
	 * 4. 跳到0x8的地方执行程序 
	 * 这是硬件执行的
	 */

	/* sp_swi未设置, 先设置它 */
	ldr sp, =0x33e00000

	/* 在und异常处理函数中有可能会修改r0-r12, 所以先保存 */
	/* lr是异常处理完后的返回地址, 也要保存 */
	stmdb sp!, {r0-r12, lr}  

	/* 这里把lr保存一下，放到r4里，防止被printException破坏
	 * 之前说过ATPCS规则
	 * 对于r4-r11在c函数里都会被保存，用到就保存起来执行完c函数再释放掉
	 * 把lr放在r4里，就不会被c函数破坏
	 */
	mov r4,lr

	/* 处理swi异常 */
	/* 得出swi0x123指令的值，lr保存有发生异常指令的下一条指令地址  ，所以把lr的地址减4  */
	/* 把r4赋值给r0后打印 */

	mrs r0, cpsr
	ldr r1, =swi_string
	bl printException
	
	sub r0,r4,#4
	bl printSWIVal
	
	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^  /* ^会把spsr的值恢复到cpsr里 */
	
swi_string:
	.string "swi  exception"

.align 4

do_irq:
	/* 执行到这里之前:
	 * 1. lr_irq保存有被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_irq保存有被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为10010, 进入到und模式
	 * 4. 跳到0x18的地方执行程序 
	 */

	/* sp_und未设置, 先设置它 */
	ldr sp, =0x33d00000


	/* 在und异常处理函数中有可能会修改r0-r12, 所以先保存 */
	/* lr是异常处理完后的返回地址, 也要保存 */
	/* 查表发现irq模式返回值是lr-4，und,swi都是lr,所以这里先把lr减了，处理完直接返回 */
	sub lr,lr,#4
	stmdb sp!, {r0-r12, lr}  
	

	/* 处理irq异常 */
	bl handle_irq_c
	
	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^  /* ^会把spsr的值恢复到cpsr里 */
		
reset:
	/* 关闭看门狗 */
	ldr r0, =0x53000000
	ldr r1, =0
	str r1, [r0]

	/* 设置MPLL, FCLK : HCLK : PCLK = 400m : 100m : 50m */
	/* LOCKTIME(0x4C000000) = 0xFFFFFFFF */
	ldr r0, =0x4C000000
	ldr r1, =0xFFFFFFFF
	str r1, [r0]

	/* CLKDIVN(0x4C000014) = 0X5, tFCLK:tHCLK:tPCLK = 1:4:8  */
	ldr r0, =0x4C000014
	ldr r1, =0x5
	str r1, [r0]

	/* 设置CPU工作于异步模式 */
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#0xc0000000   //R1_nF:OR:R1_iA
	mcr p15,0,r0,c1,c0,0

	/* 设置MPLLCON(0x4C000004) = (92<<12)|(1<<4)|(1<<0) 
	 *  m = MDIV+8 = 92+8=100
	 *  p = PDIV+2 = 1+2 = 3
	 *  s = SDIV = 1
	 *  FCLK = 2*m*Fin/(p*2^s) = 2*100*12/(3*2^1)=400M
	 */
	ldr r0, =0x4C000004
	ldr r1, =(92<<12)|(1<<4)|(1<<0)
	str r1, [r0]

	/* 一旦设置PLL, 就会锁定lock time直到PLL输出稳定
	 * 然后CPU工作于新的频率FCLK
	 */
	

	/* 设置内存: sp 栈 */
	/* 分辨是nor/nand启动
	 * 写0到0地址, 再读出来
	 * 如果得到0, 表示0地址上的内容被修改了, 它对应ram, 这就是nand启动
	 * 否则就是nor启动
	 */
	mov r1, #0
	ldr r0, [r1] /* 读出原来的值备份 */
	str r1, [r1] /* 0->[0] */ 
	ldr r2, [r1] /* r2=[0] */
	cmp r1, r2   /* r1==r2? 如果相等表示是NAND启动 */
	ldr sp, =0x40000000+4096 /* 先假设是nor启动 */
	moveq sp, #4096  /* nand启动 */
	streq r0, [r1]   /* 恢复原来的值 */

	bl sdram_init
	//bl sdram_init2	 /* 用到有初始值的数组, 不是位置无关码 */

	/* 重定位text, rodata, data段整个程序 */
	bl copy2sdram

	/* 清除BSS段 */
	bl clean_bss

	/* 复位之后，cpu处于svc模式，现在切换到usr模式 */
	mrs r0,cpsr		/* 读出cpsr */
	bic r0,r0,#0xf	/* 修改M0-M4为10000进入usr */
	bic r0,r0,#(1<<7)/* cpsr I位清零，开中断总开关 */
	msr cpsr,r0
	
	/* 设置sp_usr */
	ldr sp,=0x33f00000
	
	ldr pc, =sdram/*这句话在nor执行，但使程序跳到sdram执行发生异常*/
sdram:
	bl uart0_init

	/* 故意加入一条未定义指令 */
und_code:
	.word 0xdeadc0de  /* 未定义指令 */

	swi 0x123

	//bl main  /* 使用BL命令相对跳转, 程序仍然在NOR/sram执行 */
	ldr pc, =main  /* 绝对跳转, 跳到SDRAM */

halt:
	b halt
