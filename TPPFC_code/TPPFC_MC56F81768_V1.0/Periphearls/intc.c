/*
* Copyright 2020 NXP
* SPDX-License-Identifier: BSD-3-Clause
*/

/*
 * 	How to enable a normal interrupt:
 * 	1. Use the MACRO in Cpu.h to permit all the interrupt priorities by invoking "__EI(0)". 
 * 	   This is an enable/disable switch in CPU;
 * 	2. There is INTC module to arbitrate among all the interrupt requests. Permit all interrupts in INTC module.
 * 	   This is another switch in INTC. Actually, INTC permits all interrupts by default after reset.
 * 	   "intc.h" also provides a MACRO to enable it:  INTC_INT_ENABLE()
 * 	3. Set proper interrupt in INTC priority level for the peripheral or CPU interrupt vector which is about to use. 
 * 	   "intc.h" provides MACROs to set interrupt priority, such as INTERRUPT_CONFIG(ADC0_INTRUPT, PERIPH_INTRUPT_LEVEL_0)
 * 	4. Enable the interrupt at the corresponding peripheral register. 
 * 	5. Write interrupt function in a C source file.
 * 	
 * 		E.g.
 * 		
 * 		#pragma interrupt saveall
 * 		void adc0Isr(void)
 * 		{
 * 			// Clear flag
 * 			...
 * 		}
 * 		#pragma interrupt off
 * 		
 * 	6. Open "ISR.h", declare the interrupt function first, and then use the function name to replace the corresponding "Cpu_Interrupt" in the ISR redefinition table.
 * 	
 * 		E.g.
 * 		
 * 		void adc0Isr(void);
 * 		
 * 		#define  ivINT_ADC12_CC0        adc0Isr         // Interrupt no. 30
 * 
 * */

/*
 * 	How to enable a fast interrupt:
 * 	1. Use the MACRO in Cpu.h to permit all the interrupt priorities by invoking "__EI(0)". 
 * 	   This is an enable/disable switch in CPU;
 * 	2. There is INTC module to arbitrate among all the interrupt requests. Permit all interrupts in INTC module.
 * 	   This is another switch in INTC. Actually, INTC permits all interrupts by default after reset.
 * 	   "intc.h" also provides a MACRO to enable it:  INTC_INT_ENABLE()
 * 	3. Set interrupt priority to level 2 for the peripheral or CPU interrupt vector which is about to use. 
 * 	   "intc.h" provides MACROs to set interrupt priority, such as INTERRUPT_CONFIG(ADC0_INTRUPT, PERIPH_INTRUPT_LEVEL_2)
 * 	4. Enable the interrupt at the corresponding peripheral register. 
 * 	5. Write interrupt function in a C source file.
 * 	
 * 		E.g.
 * 		
 * 		#pragma interrupt fast
 * 		void adc0Isr(void)
 * 		{
 * 			// Clear flag
 * 			asm();
 * 			...
 * 		}
 * 		#pragma interrupt off
 * 		
 * 	6. There are totally two fast interrupts available. Choose fast interrupt 0 or fast interrupt 1 by setting interrupt vector number to INTC_FIM0 or INTC_FIM1 register.
 * 	   Use the MACRO in "intc.h": CONFIG_FAST_INTERRUPT_0(vector_num)
 * 		E.g.
 * 		
 * 		CONFIG_FAST_INTERRUPT_0(ADC0_NUM);
 * 		
 * 	7. Put the ISR function name to INTC_FIVAL0/FIVAH0 or INTC_FIVAL1/FIVAH1 depending on fast interrupt 0 or 1 is used.
 * 	   Use the MACRO in "intc.h": SET_ADDR_TO_FAST_INTERRUPT_0(addr)
 * 	   
 * 	   E.g.
 * 	   SET_ADDR_TO_FAST_INTERRUPT_0(adc0Isr)
 * 	   
 * 
 * */

/*
 * 	Enable/Disable peripheral/CPU interrupt in INTC
 * 
 *  Use the MACROS in intc.h to enable/disable interrupt:INTERRUPT_CONFIG(interrupt_source,level)
 *  interrupt_source - it can be found in "CPU interrupt sources" and "Peripheral interrupt sources".
 *  level - for CPU interrupt, use CPU_INTRUPT_LEVEL_x (x=1,2,3) or INT_DISABLE;
 *          for peripheral interrupt, use PERIPH_INTRUPT_LEVEL_x (x=0,1,2) or INT_DISABLE;
 *   
 *  E.g.
 *  	INTERRUPT_CONFIG(ADC0_INTRUPT, PERIPH_INTRUPT_LEVEL_0); 	
 *  	
 *  This is to set interrupt level 0 for ADC0 interrupt.	
 *  
 * */

