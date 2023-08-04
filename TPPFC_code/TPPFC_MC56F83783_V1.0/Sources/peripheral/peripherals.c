/*
* Copyright 2021 NXP
* SPDX-License-Identifier: BSD-3-Clause
*/

/*
 * peripherals.c
 *
 *  Created on: Jan 21, 2019
 *      Author: nxa22573
 */

#include "peripherals.h"
#include "PFC_statemachine.h"
#include "PFC_Ctrl.h"


void Pin_init(void)
{
	/* pin clock enable */
	SIM->PCE0 |= (SIM_PCE0_GPIOA_MASK | SIM_PCE0_GPIOB_MASK | SIM_PCE0_GPIOC_MASK |
				 SIM_PCE0_GPIOE_MASK | SIM_PCE0_GPIOF_MASK);
	
	/* enable GPIOC1 for USER_LED_2, HVP D1 */
	GPIOC->PER &= ~MASK_BIT1;  // Set to GPIO mode
	GPIOC->DDR |= MASK_BIT1;   // Set to output mode
	GPIOC->DR &= ~MASK_BIT1;   // Output 0
	/* enable GPIOC0 for User_LED_1, D1 */
	GPIOC->PER &= ~MASK_BIT0;  // Set to GPIO mode
	GPIOC->DDR |= MASK_BIT0;   // Set to output mode
	GPIOC->DR &= ~MASK_BIT0;   // Output 0
	/* enable GPIOF6 for User_LED_2, D2 */
	GPIOF->PER &= ~MASK_BIT6;  // Set to GPIO mode
	GPIOF->DDR |= MASK_BIT6;   // Set to output mode
	GPIOF->DR &= ~MASK_BIT6;   // Output 0
	
	/* enable GPIOC4 for polarity input */
	GPIOC->PER &= ~MASK_BIT7;  // Set to GPIO mode
	GPIOC->DDR &= ~MASK_BIT7;   // Set to input mode
	/* enable GPIOF7 for relay control */
	GPIOF->PER &= ~MASK_BIT7;  // Set to GPIO mode
	GPIOF->DDR |= MASK_BIT7;   // Set to output mode
	OPEN_RELAY();  
	
	/* enable GPIOA0/A1 for Vac, GPIOA4 for VDC sampling */
	GPIOA->PER |= MASK_BIT0|MASK_BIT1|MASK_BIT4;  // set to peripheral mode	
	/* enable GPIOB0/B2 for ISNS sampling and protection  */
	GPIOB->PER |= MASK_BIT0|MASK_BIT2;  // set to peripheral mode

#if XBAR_OUT
	/* enable GPIOF2 for XBOUT6(PWM_L), GPIOF3 for XBOUT7(PWM_H) */
	GPIOF->PER |= MASK_BIT2|MASK_BIT3;  // set to peripheral mode
	SIM->GPSFL = (SIM->GPSFL & ~SIM_GPSFL_F2_MASK)|SIM_GPSFL_F2(1); // Set as XBOUT6
	SIM->GPSFL = (SIM->GPSFL & ~SIM_GPSFL_F3_MASK)|SIM_GPSFL_F3(1); // Set as XBOUT7
#else	
	/* enable GPIOE1 for PWM_L, GPIOE0 for PWM_H */
	GPIOE->PER |= MASK_BIT0|MASK_BIT1;  // set to peripheral mode
	SIM->GPSEL &= ~(SIM_GPSEL_E0_MASK | SIM_GPSEL_E1_MASK);  //select GPIOE1 as PWMA_0A, GPIOE0 as PWMA_0B
#endif 
	/* enable GPIOF4 for PWM_SR_L, GPIOF5 for PWM_SR_H */
	GPIOF->PER &= ~(MASK_BIT4|MASK_BIT5);  //set to GPIO mode
	GPIOF->DDR |= (MASK_BIT4|MASK_BIT5);  // set to output mode
	DISABLE_ALLMOSFET();
	
	/* Configure SCI1 pins for freeMaster communication */
	GPIOC->PER |= GPIO_PER_PE(MASK_BIT11|MASK_BIT12); // Set to peripheral mode
	SIM->GPSCH = (SIM->GPSCH & ~SIM_GPSCH_C11_MASK)|SIM_GPSCH_C11(2); // GPIOC11 Set as TXD1
	SIM->GPSCH = (SIM->GPSCH & ~SIM_GPSCH_C12_MASK)|SIM_GPSCH_C12(2); // GPIOC12 Set as RXD1
#if COMMUNICATION_EN
	/* Configure SCI0 pins for power board communication */
	GPIOC->PER |= GPIO_PER_PE(MASK_BIT2|MASK_BIT3); // Set to peripheral mode
	SIM->GPSCL = (SIM->GPSCL & ~SIM_GPSCL_C2_MASK)|SIM_GPSCL_C2(0); // GPIOC2 Set as RXD0
	SIM->GPSCL = (SIM->GPSCL & ~SIM_GPSCL_C3_MASK)|SIM_GPSCL_C3(2); // GPIOC3 Set as TXD0
#endif	
	
	/*************** for test ****************/
	/* TP9, GPIOC13 */
	GPIOC->PER &= ~MASK_BIT13;  // Set to GPIO mode
	GPIOC->DDR |= MASK_BIT13;   // Set to output mode
	TP9_DOWN();   // Output 0
	/* TP11, GPIOE7 */
	GPIOE->PER &= ~MASK_BIT7;  // Set to GPIO mode
	GPIOE->DDR |= MASK_BIT7;   // Set to output mode
	TP11_DOWN();   // Output 0
}

void XBAR_init(void)
{
	/* PWM fault */ 
	XBARA->SEL14 = (XBARA->SEL14 & ~XBARA_SEL14_SEL29_MASK) | XBARA_SEL14_SEL29(0xD); // CMPB_OUT as pwma fault0
	XBARA->SEL15 = 0xF0E; // CMPC_OUT and CMPD_out as pwma fault1 and fault2
	SIM->IPS0 |= (SIM_IPS0_PWMAF0_MASK|SIM_IPS0_PWMAF1_MASK|SIM_IPS0_PWMAF2_MASK); // pwm fault0/1/2 comes form xbar
	
	/* pwm xbar out */
#if XBAR_OUT
	SIM->PWM_SEL &= ~(SIM_PWM_SEL_XBAR_IN20_MASK | SIM_PWM_SEL_XBAR_IN21_MASK); // Select PWMA0 trigger as xbar_in20/21
	XBARA->SEL23 = (XBARA->SEL23 & ~XBARA_SEL23_SEL47_MASK) | XBARA_SEL23_SEL47(0x14); // PWMA0_TRIG0 as EVTG0_A
	XBARA->SEL24 = 0xD0E; // CMPB and CMPC as EVTG0_B and C
	EVTG->EVTG_INST[0].EVTG_AOI0_BFT01 |= EVTG_EVTG_AOI0_BFT01_PT0_AC(1)|EVTG_EVTG_AOI0_BFT01_PT0_BC(2)|EVTG_EVTG_AOI0_BFT01_PT0_CC(2); 
	EVTG->EVTG_INST[0].EVTG_CTRL &= ~EVTG_EVTG_CTRL_SYNC_CTRL_MASK; // disable input sync to avoid trigger loss
	EVTG->EVTG_INST[0].EVTG_CTRL &= ~EVTG_EVTG_CTRL_MODE_SEL_MASK; // EVTG0_OUTA work in bypass mode
	EVTG->EVTG_INST[0].EVTG_AOI0_FILT = 0; // disable filter to avoid trigger loss
	
	XBARA->SEL25 = (XBARA->SEL25 & ~XBARA_SEL25_SEL51_MASK) | XBARA_SEL25_SEL51(0x15); // PWMA0_TRIG1 as EVTG1_A
	XBARA->SEL26 = 0xD0E; // CMPB and CMPC as EVTG1_B and C
	EVTG->EVTG_INST[1].EVTG_AOI0_BFT01 |= EVTG_EVTG_AOI0_BFT01_PT0_AC(1)|EVTG_EVTG_AOI0_BFT01_PT0_BC(2)|EVTG_EVTG_AOI0_BFT01_PT0_CC(2);
	EVTG->EVTG_INST[1].EVTG_CTRL &= ~EVTG_EVTG_CTRL_SYNC_CTRL_MASK; // disable input sync to avoid trigger loss
	EVTG->EVTG_INST[1].EVTG_CTRL &= ~EVTG_EVTG_CTRL_MODE_SEL_MASK; // EVTG1_OUTA work in bypass mode
	EVTG->EVTG_INST[1].EVTG_AOI0_FILT = 0; // disable filter to avoid trigger loss
	
	XBARA->SEL3 = 0x2928; // EVTG0 and EVTG1 output routine to XBOUT6 and XBOUT7
#endif
    
    /* ADC trigger */
#if XBAR_OUT
    SIM->PWM_SEL &= ~SIM_PWM_SEL_XBAR_IN24_MASK; // select pwma2 trigger0 as xbar_in24
    XBARA->SEL6 = (XBARA->SEL6 & ~XBARA_SEL6_SEL12_MASK) | XBARA_SEL6_SEL12(0x18); // pwma2 trigger0 as adc trigger
#else
    SIM->PWM_SEL &= ~SIM_PWM_SEL_XBAR_IN20_MASK; // select pwma0 trigger0 as xbar_in20
    XBARA->SEL6 = (XBARA->SEL6 & ~XBARA_SEL6_SEL12_MASK) | XBARA_SEL6_SEL12(0x14); // pwma0 trigger0 as adc trigger
#endif	
}

void eFlexPWMA_init(void)
{	
	/* enable PWMA sm0&1 clock */
	SIM->PCE3 |= SIM_PCE3_PWMACH0_MASK|SIM_PCE3_PWMACH1_MASK;
	
	/*********** configure sm0 to generate PWM driver and fast current loop interrupt ***********/
	PWMA->SM0CTRL |= PWM_SM0CTRL_FULL_MASK; // full cycle reload
	PWMA->SM0CTRL &= ~PWM_SM0CTRL_COMPMODE_MASK; // Edge is generated on counter "equal to" value register
	PWMA->SM0CTRL &= ~PWM_SM0CTRL_LDMOD_MASK; // buffered registers take effect at PWM reload signal when LDOK is set
	PWMA->SM0CTRL2 = (PWMA->SM0CTRL2 & ~PWM_SM0CTRL2_CLK_SEL_MASK)|PWM_SM0CTRL2_CLK_SEL(0); // use IPBus clock
	PWMA->SM0CTRL2 &= ~PWM_SM0CTRL2_INDEP_MASK; // complementary mode 
	PWMA->SM0CTRL2 = (PWMA->SM0CTRL2 & ~PWM_SM0CTRL2_INIT_SEL_MASK)|PWM_SM0CTRL2_INIT_SEL(0); // use local sync for SM0
	PWMA->SM0CTRL2 &= ~PWM_SM0CTRL2_RELOAD_SEL_MASK; // use local reload signal
	
	PWMA->SM0INIT  = -625; // 80KHz @100MHz PWM clock
	PWMA->SM0VAL1  = 624; 
#if SPLL_EN
	PWMA->SM0VAL0 = -470;
#else
	PWMA->SM0VAL0 = 10;
#endif	
	PWMA->SM0VAL2  = -20;
	PWMA->SM0VAL3  = 19;
	PWMA->SM0DTCNT0 = 0; // hardware dead time is used, no software deadtime
	PWMA->SM0DTCNT1 = 0;
	PWMA->SM0FRCTRL |= PWM_SM0FRCTRL_FRAC_PU_MASK;  //power up fractional delay circuit
	PWMA->SM0FRCTRL |= PWM_SM0FRCTRL_FRAC23_EN_MASK;  //fractional cycle placement enable
	PWMA->SM0FRACVAL2 = 0;
	PWMA->SM0FRACVAL3 = 0;
	
	/* PWM trigger configuration */
#if XBAR_OUT
	PWMA->SM0TCTRL |= (PWM_SM0TCTRL_PWAOT0_MASK | PWM_SM0TCTRL_PWBOT1_MASK); // pwm output is routed to the PWM_TRIGx port
	PWMA->OUTEN |= (PWM_OUTEN_PWMA_EN(1)|PWM_OUTEN_PWMB_EN(1));
#else
	PWMA->SM0TCTRL &= ~PWM_SM0TCTRL_PWAOT0_MASK; // pwm out_trig is routed to the PWM_TRIG0 port
	PWMA->SM0TCTRL = (PWMA->SM0TCTRL & ~PWM_SM0TCTRL_OUT_TRIG_EN_MASK)| PWM_SM0TCTRL_OUT_TRIG_EN(MASK_BIT4); // val4 trigger is used for ADC trigger
#endif	
	
	/* sm0 PWM output configuration */
	PWMA->MCTRL &= ~PWM_MCTRL_IPOL(1); // PWM23 is used to generate complementary PWM pair
	PWMA->DTSRCSEL &= ~PWM_DTSRCSEL_SM0SEL23_MASK; // generated pwm23 is used
	PWMFAULTSTATE_0;// PWMA&B force to logic0 prior to polarity control in positive cycle
	RESETOUTPUTPOLARITY;  //PWM output not invert in positive cycle
	PWMA->SM0DISMAP0 = PWM_SM0DISMAP0_DIS0A(7) | PWM_SM0DISMAP0_DIS0B(7);
	PWMA->SM0DISMAP1 = 0;
	DISABLE_ALL_GAN();
	
	/************* configure sm1 to generate slow voltage loop interrupt ***************/
	PWMA->SM1CTRL |= PWM_SM1CTRL_FULL_MASK; // full cycle reload
	PWMA->SM1CTRL &= ~PWM_SM1CTRL_LDMOD_MASK; // buffered registers take effect at PWM reload signal when LDOK is set
	PWMA->SM1CTRL2 = (PWMA->SM1CTRL2 & ~PWM_SM1CTRL2_CLK_SEL_MASK)|PWM_SM1CTRL2_CLK_SEL(2); // SM0's clock is used
	PWMA->SM1CTRL2 = (PWMA->SM1CTRL2 & ~PWM_SM1CTRL2_INIT_SEL_MASK)|PWM_SM1CTRL2_INIT_SEL(0); // use local sync
	PWMA->SM1CTRL2 &= ~PWM_SM1CTRL2_RELOAD_SEL_MASK; // use local reload
	
	PWMA->SM1INIT  = -5000; // 10KHz @100MHz PWM clock
	PWMA->SM1VAL1  = 4999; 
	PWMA->SM1VAL0 = 0;	

#if XBAR_OUT
	/*************** configure sm2 to generate ADC trigger **********************/
	SIM->PCE3 |= SIM_PCE3_PWMACH2_MASK; // enable PWMA sm2 clock 
	PWMA->SM2CTRL |= PWM_SM0CTRL_FULL_MASK; // full cycle reload
	PWMA->SM2CTRL &= ~PWM_SM0CTRL_LDMOD_MASK; // buffered registers take effect at PWM reload signal when LDOK is set
	PWMA->SM2CTRL2 = (PWMA->SM2CTRL2 & ~PWM_SM2CTRL2_CLK_SEL_MASK)|PWM_SM2CTRL2_CLK_SEL(2); // SM0's clock is used
	PWMA->SM2CTRL2 = (PWMA->SM2CTRL2 & ~PWM_SM0CTRL2_INIT_SEL_MASK)|PWM_SM0CTRL2_INIT_SEL(2); // use master sync from SM0
	PWMA->SM2CTRL2 |= PWM_SM2CTRL2_RELOAD_SEL_MASK; // use master reload from SM0
	PWMA->SM2INIT  = -625; // 80KHz @100MHz PWM clock
	PWMA->SM2VAL1  = 624;
	PWMA->SM2VAL0 = 0;
	PWMA->SM2TCTRL &= ~PWM_SM2TCTRL_PWAOT0_MASK; // route trigger0 to PWM_MUX_TRIG0 port
	PWMA->SM2TCTRL |= PWM_SM2TCTRL_OUT_TRIG_EN(1); // val0 trigger enable	
#endif
	
	/*********** fault configuration **************/
	PWMA->FCTRL0 = (PWMA->FCTRL0 & ~PWM_FCTRL0_FLVL_MASK)|PWM_FCTRL0_FLVL(7); // A logic 1 on fault0/1/2 indicates a fault condition
	PWMA->FCTRL0 = (PWMA->FCTRL0 & ~PWM_FCTRL0_FAUTO_MASK)|PWM_FCTRL0_FAUTO(0); // Manual clearing for fault0/1/2
	PWMA->FCTRL0 = (PWMA->FCTRL0 & ~PWM_FCTRL0_FSAFE_MASK)|PWM_FCTRL0_FSAFE(7); // Safe mode for fault0/1/2
	
	PWMA->FSTS0 = (PWMA->FSTS0 & ~PWM_FSTS0_FFULL_MASK)|PWM_FSTS0_FFULL(7); // Full cycle recovery for fault0/1/2
	PWMA->FCTRL20 = (PWMA->FCTRL20 & ~PWM_FCTRL20_NOCOMB_MASK)|PWM_FCTRL20_NOCOMB(0); // enable combinational path for fault
	PWMA->FFILT0 = PWM_FFILT0_FILT_CNT(1)|PWM_FFILT0_FILT_PER(1);
	
	PWMA->FSTS0 |= PWM_FSTS0_FFLAG_MASK; // Clear fault flags of fault0~3
	
	PWMA->MCTRL |= PWM_MCTRL_CLDOK(7); // clear LDOK of sm0-sm2
	PWMA->MCTRL |= PWM_MCTRL_LDOK(7); // set LDOK of sm0-sm2
	
	/* enable sm0 val0 compare interrupt for fast current loop control */
	PWMA->SM0STS |= PWM_SM0STS_CMPF(1); // clear sm0 val0 compare flag
	PWMA->SM0INTEN |= PWM_SM0INTEN_CMPIE(1); // enable sm0 val0 compare interrupt
	INTC->IPR9 = (INTC->IPR9 & ~INTC_IPR9_PWMA_CMP0_MASK) | INTC_IPR9_PWMA_CMP0(2);  // set PWMA_CMP0 irq priority level 1
	/* enable sm1 val0 compare interrupt for slow voltage loop control */
	PWMA->SM1STS |= PWM_SM1STS_CMPF(1); // clear sm1 val0 compare flag
	PWMA->SM1INTEN |= PWM_SM1INTEN_CMPIE(1); // enable sm1 val0 compare interrupt
	INTC->IPR9 = (INTC->IPR9 & ~INTC_IPR9_PWMA_CMP1_MASK) | INTC_IPR9_PWMA_CMP1(1);  // set PWMA_CMP1 irq priority level 0
	/* enable PWM fault interrupt */
	//PWMA->FCTRL0 |= PWM_FCTRL0_FIE(4);  // only over voltage interrupt is enabled at the beginning
	//INTC->IPR8 = (INTC->IPR8 & ~INTC_IPR8_PWMA_FAULT_MASK) | INTC_IPR8_PWMA_FAULT(3);  // set PWMA_FAULT irq priority level 2
}

void ADC_init(void)
{
	/* enable cyclic ADC clock */
	SIM->PCE2 |= SIM_PCE2_CYCADC_MASK;
	
	ADC->CTRL2 &= ~ADC_CTRL2_DIV0_MASK;
	ADC->CTRL2 |= ADC_CTRL2_DIV0(3); // ADC CLK = sys_clk/4 = 25MHz
	ADC->CTRL1 &= ~ADC_CTRL1_SMODE_MASK;
	ADC->CTRL1 |= ADC_CTRL1_SMODE(5); // Triggered parallel
	ADC->CTRL2 |= ADC_CTRL2_SIMULT_MASK; // Simultaneous mode
	/* sample list configuration */
	ADC->SDIS = 0xFCFC; // four channel to be scanned
	ADC->CTRL1 |= ADC_CTRL1_CHNCFG_L(1); // ANA0-ANA1 configured as differential pair
	ADC->CLIST1 = (ADC->CLIST1 & ~ADC_CLIST1_SAMPLE0_MASK)|ADC_CLIST1_SAMPLE0(0x0); // ANA0->Sample0, vac 
	ADC->CLIST1 = (ADC->CLIST1 & ~ADC_CLIST1_SAMPLE1_MASK)|ADC_CLIST1_SAMPLE1(0x4); // ANA4->Sample1, vout
	ADC->CLIST3 = (ADC->CLIST3 & ~ADC_CLIST3_SAMPLE8_MASK)|ADC_CLIST3_SAMPLE8(0x8); // ANB0->Sample8, Isns
	ADC->CLIST3 = (ADC->CLIST3 & ~ADC_CLIST3_SAMPLE9_MASK)|ADC_CLIST3_SAMPLE9(0x8); // ANB0->Sample9, Isns
	
	ADC->OFFST[8] = 0x4000; // init isns offset as 1.65V	
	
	ADC->CTRL1 |= ADC_CTRL1_SYNC0_MASK;  // ADC0 can be triggered by SYNC0 input pulse
	ADC->CTRL1 &= ~ADC_CTRL1_STOP0_MASK; // Normal operation for ADC0	
	
	ADC->PWR &= ~(ADC_PWR_PD0_MASK|ADC_PWR_PD1_MASK); // Power up ADC0&1
}

void CMP_init(void)
{
	/* enable both CMP and 8-bit DAC clock of CMPB/C/D */
	SIM->PCE2 |= (SIM_PCE2_CMPD_MASK | SIM_PCE2_CMPC_MASK | SIM_PCE2_CMPB_MASK);
	
	/*CMPB configuration for inductor positive over-current protection */
	CMPB->CR1 |= CMP_CR1_PMODE_MASK; // high speed
	
	CMPB->DACCR = CMP_DACCR_VOSEL(200); 
	CMPB->MUXCR |= CMP_MUXCR_DACEN_MASK; // enable DAC 
	CMPB->MUXCR = (CMPB->MUXCR & ~CMP_MUXCR_PSEL_MASK)| CMP_MUXCR_PSEL(3); // plus input: IN3, ANB0
	CMPB->MUXCR = (CMPB->MUXCR & ~CMP_MUXCR_MSEL_MASK)| CMP_MUXCR_MSEL(7); // minus input: IN7, DACB
	
	CMPB->CR0 = (CMPB->CR0 & ~CMP_CR0_FILTER_CNT_MASK)| CMP_CR0_FILTER_CNT(7); // filter count, 7 consecutive samples must agree
	CMPB->FPR = (CMPB->FPR & ~CMP_FPR_FILT_PER_MASK)| CMP_FPR_FILT_PER(3); // filter sample period = 3
	CMPB->CR1 |= CMP_CR1_EN_MASK; // enable comparator module
	
	/*CMPC configuration for inductor negative over-current protection */
	CMPC->CR1 |= CMP_CR1_PMODE_MASK; // high speed
		
	CMPC->DACCR = CMP_DACCR_VOSEL(52); 
	CMPC->MUXCR |= CMP_MUXCR_DACEN_MASK; // enable DAC 
	CMPC->MUXCR = (CMPC->MUXCR & ~CMP_MUXCR_PSEL_MASK)| CMP_MUXCR_PSEL(7); // plus input: IN7, DACC
	CMPC->MUXCR = (CMPC->MUXCR & ~CMP_MUXCR_MSEL_MASK)| CMP_MUXCR_MSEL(3); // minus input: IN3, ANB2
	
	CMPC->CR0 = (CMPC->CR0 & ~CMP_CR0_FILTER_CNT_MASK)| CMP_CR0_FILTER_CNT(7); // filter count, 7 consecutive samples must agree
	CMPC->FPR = (CMPC->FPR & ~CMP_FPR_FILT_PER_MASK)| CMP_FPR_FILT_PER(3); // filter sample period = 3
	CMPC->CR1 |= CMP_CR1_EN_MASK; // enable comparator module
	
	/*CMPD configuration for DCBus over-voltage protection */
	CMPD->CR1 |= CMP_CR1_PMODE_MASK; // high speed
		
	CMPD->DACCR = CMP_DACCR_VOSEL(224); 
	CMPD->MUXCR |= CMP_MUXCR_DACEN_MASK; // enable DAC 
	CMPD->MUXCR = (CMPD->MUXCR & ~CMP_MUXCR_PSEL_MASK)| CMP_MUXCR_PSEL(0); // plus input: IN0, ANA4
	CMPD->MUXCR = (CMPD->MUXCR & ~CMP_MUXCR_MSEL_MASK)| CMP_MUXCR_MSEL(7); // minus input: IN7, DACD
		
	CMPD->CR0 = (CMPD->CR0 & ~CMP_CR0_FILTER_CNT_MASK)| CMP_CR0_FILTER_CNT(7); // filter count, 7 consecutive samples must agree
	CMPD->FPR = (CMPD->FPR & ~CMP_FPR_FILT_PER_MASK)| CMP_FPR_FILT_PER(3); // filter sample period = 3
	CMPD->CR1 |= CMP_CR1_EN_MASK; // enable comparator module
}

void sci_init(void)
{
	/* sci1 clock enable, for freemaster communication */
	SIM->PCE1 |= SIM_PCE1_SCI1_MASK; // SCI default clock rate = 100MHz
	QSCI1->CTRL1 |= (QSCI_CTRL1_TE_MASK|QSCI_CTRL1_RE_MASK);
	QSCI1->RATE = (((uint16_t)(324)<<3)|4); // 19200 baudrate
	QSCI1->CTRL3 = 0;
	
#if COMMUNICATION_EN
	/* sci0 clock enable, for communication with DC-DC stage */
	SIM->PCE1 |= SIM_PCE1_SCI0_MASK;
	QSCI0->CTRL2 |= QSCI_CTRL2_FIFO_EN_MASK; // enable 4-word-deep FIFOs
	QSCI0->CTRL1 |= (QSCI_CTRL1_TE_MASK|QSCI_CTRL1_RE_MASK);
	QSCI0->RATE = (((uint16_t)(2604)<<3)|1); // 2400 baudrate
	QSCI0->CTRL3 = 0;
#endif
}

void Peripheral_init(void)
{
	Pin_init();	
	sci_init();
	CMP_init();
	eFlexPWMA_init();
	ADC_init();
	XBAR_init(); 
}


