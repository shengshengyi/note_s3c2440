
.text
.global _start

_start:

	/* 关闭看门狗 */
	ldr r0,=0x53000000
	ldr r1,=0
	str r1,[r0]

	/* 为了保险可重新设置一下LOCKTIME(0x4C000000) = 0xFFFFFFFF */
	ldr r0,=0x4C000000
	ldr r1,=0xFFFFFFFF
	str r1,[r0]

	/* (1)设置MPLLCON,MDIV = 92,PDIV = 1,SDIV = 1, */
	ldr r0,=0x4C000004
	ldr r1,=(92<<12) | (1<<4) | (1<<0)
	str r1,[r0]
		
	/* (2)设置CLKDIVN：通过[0] [2:1]来设置PCLK = HCLK/2,HCLK = FCLK/4的值 */
	ldr r0,=0x4C000014
	ldr r1,=0x5
	str r1,[r0]

	/* (3)设置工作在异步模式：直接复制芯片手册 */
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#0xc0000000
	mcr p15,0,r0,c1,c0,0
	

	/* 设置栈 sp*/
	//ldr sp, =4096+0x40000000 //nor片内地址为片内0x4000 0000 + 4k ram 
	//ldr sp, =4096 		  //nand片内地址为片内0 + 4k ram 
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

	

	/* 用r0传递c程序参数 */
	//ldr r0,=100000
	//bl delay

	/* 调用led.c */
	bl main

halt:
	b halt
	
