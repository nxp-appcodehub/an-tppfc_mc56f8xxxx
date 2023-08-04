/*
* Copyright 2020 NXP
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "peripherals.h"
#include "PFC_statemachine.h"

/*!
 * @brief Initializes the pin functions.
 */
void Pin_init(void)
{
	CLOCKGATE_ENABLE(GPIO_A);
	CLOCKGATE_ENABLE(GPIO_B);
	CLOCKGATE_ENABLE(GPIO_C);
	CLOCKGATE_ENABLE(GPIO_E);
	CLOCKGATE_ENABLE(GPIO_F);
	
	/*************************LED pins configuration************************/
	/* enable GPIOC1 for USER_LED_2, HVP D1 */
	GPIOC->PER &= ~MASK_BIT1;  // Set to GPIO mode
	GPIOC->DDR |= MASK_BIT1;   // Set to output mode
	GPIOC->DR &= ~MASK_BIT1;   // Output 0
	/* enable GPIOC0 for User_LED_1, D1 */
	GPIOC->PER &= ~MASK_BIT0;  // Set to GPIO mode
	GPIOC->DDR |= MASK_BIT0;   // Set to output mode
	GPIOC->DR &= ~MASK_BIT0;   // Output 0
	/* enable GPIOF0 for User_LED_2, D2 */
	GPIOF->PER &= ~MASK_BIT0;  // Set to GPIO mode
	GPIOF->DDR |= MASK_BIT0;   // Set to output mode
	GPIOF->DR &= ~MASK_BIT0;   // Output 0
	
	/*************************ADC pins configuration*************************/
	/* enable GPIOA0/A1 for VDC and Vac_L sampling */
	GPIOA->PER |= MASK_BIT0|MASK_BIT1;  // set to peripheral mode	
	/* enable GPIOB7 for Vac_N sampling, GPIOB0/2 for ISNS sampling and protection */
	GPIOB->PER |= MASK_BIT0|MASK_BIT2|MASK_BIT7;  // set to peripheral mode
	
	/************************PWM pins configuration**************************/
	/* enable GPIOE1 for PWM_L, GPIOE0 for PWM_H */
	GPIOE->PER |= MASK_BIT0|MASK_BIT1;  // set to peripheral mode
	SIM->GPSEL = (SIM->GPSEL & ~SIM_GPSEL_E0_MASK)|SIM_GPSEL_E0(0); // Set as PWM_0B
	SIM->GPSEL = (SIM->GPSEL & ~SIM_GPSEL_E1_MASK)|SIM_GPSEL_E1(0); // Set as PWM_0A
	/* enable GPIOE3 for PWM_SR_L, GPIOE2 for PWM_SR_H */
	GPIOE->PER &= ~(MASK_BIT2|MASK_BIT3);  // set to GPIO mode
	GPIOE->DDR |= MASK_BIT2|MASK_BIT3; // set to output mode
	DISABLE_ALLMOSFET();
	
	/*************************polarity pin configuration***************/
	/* enable GPIOC4 for polarity input */
	GPIOC->PER &= ~MASK_BIT4;  // Set to GPIO mode
	GPIOC->DDR &= ~MASK_BIT4;   // Set to input mode
	
	/*************************relay pin configuration******************/
	/*enable GPIOF1 for relay control*/
	GPIOF->PER &= ~MASK_BIT1;  // Set to GPIO mode
	GPIOF->DDR |= MASK_BIT1;   // Set to output mode
	OPEN_RELAY(); 
	
	/*************************SCI configuration*******************************/
	/*SCI1 pins, HVP cp2102*/
	GPIOC->PER |= GPIO_PER_PE(MASK_BIT11|MASK_BIT12); // Set GPIOC11&12 to peripheral mode
	SIM->GPSCH = (SIM->GPSCH & ~SIM_GPSCH_C11_MASK)|SIM_GPSCH_C11(2); // Set GPIOC11 as TXD1
	SIM->GPSCH = (SIM->GPSCH & ~SIM_GPSCH_C12_MASK)|SIM_GPSCH_C12(2); // Set GPIOC12 as RXD1
	/*SCI0 pins, communicate with other device*/
#if SCI0_ENABLE
	GPIOC->PER |= GPIO_PER_PE(MASK_BIT2|MASK_BIT3); // Set to peripheral mode
	SIM->GPSCL = (SIM->GPSCL & ~SIM_GPSCL_C2_MASK)|SIM_GPSCL_C2(0); // GPIOC2 Set as TXD0
	SIM->GPSCL = (SIM->GPSCL & ~SIM_GPSCL_C3_MASK)|SIM_GPSCL_C3(2); // GPIOC3 Set as RXD0
#endif
	
	/*************************for test**************************************/
	/* power board TP10, GPIOE6 */
	GPIOE->PER &= ~MASK_BIT6;  // Set to GPIO mode
	GPIOE->DDR |= MASK_BIT6;   // Set to output mode
	TP10_DOWN;   // Output 0
	/* power board TP11, GPIOE7 */
	GPIOE->PER &= ~MASK_BIT7;  // Set to GPIO mode
	GPIOE->DDR |= MASK_BIT7;   // Set to output mode
	TP11_DOWN;   // Output 0
}


/*!
 * @brief Initializes internal signal connection through XBAR.
 */
void XBAR_init(void)
{	
	/**************** ADC trigger ********************/
	XBAR_INIT(in_PWMA0_OUT_TRIG0,out_ADCA_TRIG); // pwma0 trigger0 as adc trigger
	
	/**************** PWM fault **********************/
	XBAR_INIT(in_CMPB_OUT,out_PWMA_FAULT0);	// CMPB_OUT as pwma fault0
	XBAR_INIT(in_CMPC_OUT,out_PWMA_FAULT1);	// CMPC_OUT as pwma fault1
	XBAR_INIT(in_CMPA_OUT,out_PWMA_FAULT2);	// CMPA_OUT as pwma fault2
}

/*!
 * @brief Initializes PWMA SM0/1.
 * SM0 is configured to generate pwm driver for HB, ADC trigger and fast current loop.
 * SM1 is configured to generate slow voltage loop.
 */
void eFlexPWMA_init(void)
{
	CLOCKGATE_ENABLE(PWMA_SM0);
	CLOCKGATE_ENABLE(PWMA_SM1);
	
	/*********** configure sm0 to generate PWM dirvers, ADC trigger and current control interrupt ***********/
	PWM->SM[0].CTRL |= PWM_CTRL_FULL_MASK; // full cycle reload
	PWM->SM[0].CTRL &= ~PWM_CTRL_COMPMODE_MASK; // Edge is generated on counter "equal to" value register
	PWM->SM[0].CTRL &= ~PWM_CTRL_LDMOD_MASK; // buffered registers take effect at PWM reload signal when LDOK is set
	PWM->SM[0].CTRL2 = (PWM->SM[0].CTRL2 & ~PWM_CTRL2_CLK_SEL_MASK)|PWM_CTRL2_CLK_SEL(0); // use IPBus clock
	PWM->SM[0].CTRL2 = (PWM->SM[0].CTRL2 & ~PWM_CTRL2_INIT_SEL_MASK)|PWM_CTRL2_INIT_SEL(0); // use local sync for SM0
	PWM->SM[0].CTRL2 &= ~PWM_CTRL2_RELOAD_SEL_MASK; // use local reload signal
	PWM->SM[0].CTRL2 &= ~~PWM_CTRL2_INDEP_MASK; // complementary mode 
	
	/* trigger configuration */
	PWM->SM[0].TCTRL &= ~PWM_TCTRL_PWAOT0_MASK; // pwm out_trig is routed to the PWM_TRIG0 port
	PWM->SM[0].TCTRL = (PWM->SM[0].TCTRL & ~PWM_TCTRL_OUT_TRIG_EN_MASK)| PWM_TCTRL_OUT_TRIG_EN(MASK_BIT4); // val4 trigger is used for ADC trigger
			
	PWM->SM[0].INIT = -625; // 80KHz @100MHz PWM clock
	PWM->SM[0].VAL1 = 624;
#if SPLL_EN
	PWM->SM[0].VAL0 = -470;
#else
	PWM->SM[0].VAL0 = 10;
#endif	
	PWM->SM[0].VAL2 = -20;
	PWM->SM[0].VAL3 = 19;
	PWM->SM[0].VAL4 = 10;
	PWM->SM[0].DTCNT0 = 0; // hardware dead time is used, no software deadtime
	PWM->SM[0].DTCNT1 = 0;
	PWM->SM[0].FRCTRL |= PWM_FRCTRL_FRAC_PU_MASK;  //power up fractional delay circuit
	PWM->SM[0].FRCTRL |= PWM_FRCTRL_FRAC23_EN_MASK;  //fractional cycle placement enable
	PWM->SM[0].FRACVAL2 = 0;
	PWM->SM[0].FRACVAL3 = 0;
	
	/* sm0 PWM output configuration */
	PWM->MCTRL &= ~PWM_MCTRL_IPOL(1); // PWM23 is used to generate complementary PWM pair
	PWM->DTSRCSEL &= ~PWM_DTSRCSEL_SM0SEL23_MASK; // generated pwm23 is used
	PWMFAULTSTATE_0;// PWMA&B force to logic0 prior to polarity control in positive cycle
	RESETOUTPUTPOLARITY;  //PWM output not invert in positive cycle
	PWM->SM[0].DISMAP[0] = PWM_DISMAP_DIS0A(7) | PWM_DISMAP_DIS0B(7);
	PWM->SM[0].DISMAP[1] = 0;
	DISABLE_ALL_GAN();
	
	/* enable sm0 val0 compare interrupt for control interrupt */
	PWM->SM[0].STS |= PWM_STS_CMPF(1); // clear sm0 val0 compare flag
	PWM->SM[0].INTEN |= PWM_INTEN_CMPIE(1); // enable sm0 val0 compare interrupt
	INTC->IPR9 = (INTC->IPR9 & ~INTC_IPR9_PWMA_CMP0_MASK) | INTC_IPR9_PWMA_CMP0(2);  // set PWMA_CMP0 irq priority level 1
	
	/*********** configure sm1 to generate slow voltage loop***********/
	PWM->SM[1].CTRL |= PWM_CTRL_FULL_MASK; // full cycle reload
	PWM->SM[1].CTRL &= ~PWM_CTRL_COMPMODE_MASK; // Edge is generated on counter "equal to" value register
	PWM->SM[1].CTRL &= ~PWM_CTRL_LDMOD_MASK; // buffered registers take effect at PWM reload signal when LDOK is set
	PWM->SM[1].CTRL2 = (PWM->SM[1].CTRL2 & ~PWM_CTRL2_CLK_SEL_MASK)|PWM_CTRL2_CLK_SEL(2); // SM0's clock is used
	PWM->SM[1].CTRL2 = (PWM->SM[1].CTRL2 & ~PWM_CTRL2_INIT_SEL_MASK)|PWM_CTRL2_INIT_SEL(0); // use local sync
	PWM->SM[1].CTRL2 &= ~PWM_CTRL2_RELOAD_SEL_MASK; // reload source: master reload
	
	PWM->SM[1].INIT  = -5000; // 10KHz @100MHz PWM clock
	PWM->SM[1].VAL1  = 4999; 
	PWM->SM[1].VAL0 = 0;	
	
	/* enable sm1 val0 compare interrupt for slow voltage loop control */
	PWM->SM[1].STS |= PWM_STS_CMPF(1); // clear sm1 val0 compare flag
	PWM->SM[1].INTEN |= PWM_INTEN_CMPIE(1); // enable sm1 val0 compare interrupt
	INTC->IPR9 = (INTC->IPR9 & ~INTC_IPR9_PWMA_CMP1_MASK) | INTC_IPR9_PWMA_CMP1(1);  // set PWMA_CMP1 irq priority level 0
	
	/*********** fault configuration **************/
	PWM->FAULT[0].FCTRL = (PWM->FAULT[0].FCTRL & ~PWM_FCTRL_FLVL_MASK)|PWM_FCTRL_FLVL(7); // A logic 1 on fault0/1/2 indicates a fault condition
	PWM->FAULT[0].FCTRL = (PWM->FAULT[0].FCTRL & ~PWM_FCTRL_FAUTO_MASK)|PWM_FCTRL_FAUTO(0); // Manual clearing for fault0/1/2
	PWM->FAULT[0].FCTRL = (PWM->FAULT[0].FCTRL & ~PWM_FCTRL_FSAFE_MASK)|PWM_FCTRL_FSAFE(7); // Safe mode for fault0/1/2
	
	PWM->FAULT[0].FSTS = (PWM->FAULT[0].FSTS & ~PWM_FSTS_FFULL_MASK)|PWM_FSTS_FFULL(7); // Full cycle recovery for fault0/1/2
	PWM->FAULT[0].FCTRL2 = (PWM->FAULT[0].FCTRL2 & ~PWM_FCTRL2_NOCOMB_MASK)|PWM_FCTRL2_NOCOMB(0); // enable combinational path for fault
	PWM->FAULT[0].FFILT = PWM_FFILT_FILT_CNT(2)|PWM_FFILT_FILT_PER(1);
			
	PWM->MCTRL |= PWM_MCTRL_CLDOK(3); // clear LDOK of sm0-sm1
	PWM->MCTRL |= PWM_MCTRL_LDOK(3); // set LDOK of sm0-sm1
	
	/* enable PWM fault interrupt */
	/*PWM->FAULT[0].FSTS |= PWM_FSTS_FFLAG_MASK; // Clear fault flags of fault0~3
	PWM->FAULT[0].FCTRL |= PWM_FCTRL_FIE(4);  // only over voltage interrupt is enabled at the beginning
	INTC->IPR8 = (INTC->IPR8 & ~INTC_IPR8_PWMA_FAULT_MASK) | INTC_IPR8_PWMA_FAULT(3);*/  // set PWMA_FAULT irq priority level 2
}

/*!
 * @brief Initializes ADC module .
 */
void ADC_init(void)
{
	CLOCKGATE_ENABLE(CYC_ADC);
	
	ADC->CTRL2 &= ~ADC_CTRL2_DIV0_MASK;
	ADC->CTRL2 |= ADC_CTRL2_DIV0(4); // ADC CLK = sys_clk/5 = 10MHz
	ADC->CTRL1 = (ADC->CTRL1 & ~ADC_CTRL1_SMODE_MASK) | ADC_CTRL1_SMODE(5); // Triggered parallel
	ADC->CTRL2 |= ADC_CTRL2_SIMULT_MASK; // Simultaneous mode
	/* sample list configuration */
	ADC->SDIS = 0xFCFC; // four channel to be scanned
	ADC->CLIST1 = (ADC->CLIST1 & ~ADC_CLIST1_SAMPLE0_MASK)|ADC_CLIST1_SAMPLE0(0x0); // ANA0->Sample0, vout 
	ADC->CLIST1 = (ADC->CLIST1 & ~ADC_CLIST1_SAMPLE1_MASK)|ADC_CLIST1_SAMPLE1(0x1); // ANA1->Sample1, vac_L
	ADC->CLIST3 = (ADC->CLIST3 & ~ADC_CLIST3_SAMPLE8_MASK)|ADC_CLIST3_SAMPLE8(0x8); // ANB0->Sample8, Isns
	ADC->CLIST3 = (ADC->CLIST3 & ~ADC_CLIST3_SAMPLE9_MASK)|ADC_CLIST3_SAMPLE9(0xF); // ANB7->Sample9, vac_N
	ADC->OFFST[8] = 0x4000; // init isns offset as 1.65V
	
	ADC->CTRL1 |= ADC_CTRL1_SYNC0_MASK;  // ADC0 can be triggered by SYNC0 input pulse
	ADC->CTRL1 &= ~ADC_CTRL1_STOP0_MASK; // Normal operation for ADC0
	
	ADC->PWR &= ~(ADC_PWR_PD0_MASK|ADC_PWR_PD1_MASK); // Power up ADC0&1
}

/*!
 * @brief Initializes compartors.
 * CMPA for output over-voltage protection: Postive input: ANA0, negative input: DACA
 * CMPB for inductor positive over-current protection: Postive input: ANB0, negative input: DACB
 * CMPC for inductor negative over-current protection: Postive input: ANB2, negative input: DACC
 */
void CMP_init(void)
{
	/*CMPA configuration for DCBus over-voltage protection */
	CLOCKGATE_ENABLE(CMP_A);
	CMPA->CR1 |= CMP_CR1_PMODE_MASK; // high speed
	CMPA->DACCR = CMP_DACCR_VOSEL(224); 
	CMPA->MUXCR |= CMP_MUXCR_DACEN_MASK; // enable DAC 
	CMPA->MUXCR = (CMPA->MUXCR & ~CMP_MUXCR_PSEL_MASK)| CMP_MUXCR_PSEL(3); // plus input: IN3, ANA0
	CMPA->MUXCR = (CMPA->MUXCR & ~CMP_MUXCR_MSEL_MASK)| CMP_MUXCR_MSEL(7); // minus input: IN7, DACA
			
	CMPA->CR0 = (CMPA->CR0 & ~CMP_CR0_FILTER_CNT_MASK)| CMP_CR0_FILTER_CNT(7); // filter count, 7 consecutive samples must agree
	CMPA->FPR = (CMPA->FPR & ~CMP_FPR_FILT_PER_MASK)| CMP_FPR_FILT_PER(3); // filter sample period = 3
	CMPA->CR1 |= CMP_CR1_EN_MASK; // enable comparator module
		
	/*CMPB configuration for inductor positive over-current protection */
	CLOCKGATE_ENABLE(CMP_B);
	CMPB->CR1 |= CMP_CR1_PMODE_MASK; // high speed		
	CMPB->DACCR = CMP_DACCR_VOSEL(200); 
	CMPB->MUXCR |= CMP_MUXCR_DACEN_MASK; // enable DAC 
	CMPB->MUXCR = (CMPB->MUXCR & ~CMP_MUXCR_PSEL_MASK)| CMP_MUXCR_PSEL(3); // plus input: IN3, ANB0
	CMPB->MUXCR = (CMPB->MUXCR & ~CMP_MUXCR_MSEL_MASK)| CMP_MUXCR_MSEL(7); // minus input: IN7, DACB
			
	CMPB->CR0 = (CMPB->CR0 & ~CMP_CR0_FILTER_CNT_MASK)| CMP_CR0_FILTER_CNT(7); // filter count, 7 consecutive samples must agree
	CMPB->FPR = (CMPB->FPR & ~CMP_FPR_FILT_PER_MASK)| CMP_FPR_FILT_PER(3); // filter sample period = 3
	CMPB->CR1 |= CMP_CR1_EN_MASK; // enable comparator module
		
	/*CMPC configuration for inductor negative over-current protectio */
	CLOCKGATE_ENABLE(CMP_C);
	CMPC->CR1 |= CMP_CR1_PMODE_MASK; // high speed			
	CMPC->DACCR = CMP_DACCR_VOSEL(52); 
	CMPC->MUXCR |= CMP_MUXCR_DACEN_MASK; // enable DAC 
	CMPC->MUXCR = (CMPC->MUXCR & ~CMP_MUXCR_PSEL_MASK)| CMP_MUXCR_PSEL(7); // plus input: IN7, DACC
	CMPC->MUXCR = (CMPC->MUXCR & ~CMP_MUXCR_MSEL_MASK)| CMP_MUXCR_MSEL(3); // minus input: IN3, ANB2
				
	CMPC->CR0 = (CMPC->CR0 & ~CMP_CR0_FILTER_CNT_MASK)| CMP_CR0_FILTER_CNT(7); // filter count, 7 consecutive samples must agree
	CMPC->FPR = (CMPC->FPR & ~CMP_FPR_FILT_PER_MASK)| CMP_FPR_FILT_PER(3); // filter sample period = 3
	CMPC->CR1 |= CMP_CR1_EN_MASK; // enable comparator module
}

/*!
 * @brief Initializes SCI0 for communication with other device.
 */
void sci0_init(void)
{
	CLOCKGATE_ENABLE(SCI_0);
	QSCI0->RATE = (((uint16_t)(27)<<3)|1); // 115200
	QSCI0->CTRL2 |= QSCI_CTRL2_FIFO_EN_MASK; // enable 4-word-deep FIFOs
	QSCI0->CTRL3 = 0;
	QSCI0->CTRL1 |= (QSCI_CTRL1_TE_MASK|QSCI_CTRL1_RE_MASK);//enable TX and RX
}

/*!
 * @brief Initializes SCI1 for FreeMASTER communiction.
 */
void sci1_init(void)
{
	CLOCKGATE_ENABLE(SCI_1);
	QSCI1->RATE = (((uint16_t)(27)<<3)|1); // 115200
	QSCI1->CTRL3 = 0;
	QSCI1->CTRL1 |= (QSCI_CTRL1_TE_MASK|QSCI_CTRL1_RE_MASK);//enable TX and RX
}

/*!
 * @brief Initializes all used module.
 */
void Peripherals_init(void)
{
	Pin_init();
#if SCI0_ENABLE
	sci0_init();
#endif
	sci1_init();
	eFlexPWMA_init();
	ADC_init();
	CMP_init();
	XBAR_init();
}
