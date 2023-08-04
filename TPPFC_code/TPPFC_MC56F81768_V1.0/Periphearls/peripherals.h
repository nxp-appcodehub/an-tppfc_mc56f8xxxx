/*
* Copyright 2020 NXP
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef PERIPHEARLS_H_
#define PERIPHEARLS_H_

#include "derivative.h"
#include "sim.h"
#include "intc.h"
#include "xbar.h"
/******************************************
 * Definitions
 ******************************************/
#define MASK_BIT15 0x8000
#define MASK_BIT14 0x4000
#define MASK_BIT13 0x2000
#define MASK_BIT12 0x1000
#define MASK_BIT11 0x0800
#define MASK_BIT10 0x0400
#define MASK_BIT9 0x0200
#define MASK_BIT8 0x0100
#define MASK_BIT7 0x0080
#define MASK_BIT6 0x0040
#define MASK_BIT5 0x0020
#define MASK_BIT4 0x0010
#define MASK_BIT3 0x0008
#define MASK_BIT2 0x0004
#define MASK_BIT1 0x0002
#define MASK_BIT0 0x0001

#define CLOSE_RELAY()      GPIOF->DR |= GPIO_DR_D(MASK_BIT1)
#define OPEN_RELAY()       GPIOF->DR &= ~GPIO_DR_D(MASK_BIT1)
#define RELAY_STATE()      GPIOF->DR & GPIO_DR_D(MASK_BIT1)

#define PWMA_RUN()               PWM->MCTRL |= PWM_MCTRL_RUN(3) /*Enable PWM generator clock*/
#define PWMA_STOP()              PWM->MCTRL &= ~PWM_MCTRL_RUN(3) /*Disable PWM generator clock*/
#define ENABLE_DUTYLOAD          PWM->MCTRL |= PWM_MCTRL_LDOK(1)
#define INVERTOUTPUTPOLARITY     PWM->SM[0].OCTRL |= (PWM_OCTRL_POLA_MASK | PWM_OCTRL_POLB_MASK)/*Inverse polarity of PWM output in input negative cycle*/ 
#define RESETOUTPUTPOLARITY      PWM->SM[0].OCTRL &= ~(PWM_OCTRL_POLA_MASK | PWM_OCTRL_POLB_MASK)/*Not invert polarity of PWM output in input positive cycle*/ 
#define PWMFAULTSTATE_0          PWM->SM[0].OCTRL &= ~(PWM_OCTRL_PWMAFS_MASK|PWM_OCTRL_PWMBFS_MASK)
#define PWMFAULTSTATE_1          PWM->SM[0].OCTRL = ((PWM->SM[0].OCTRL & ~(PWM_OCTRL_PWMAFS_MASK|PWM_OCTRL_PWMBFS_MASK)) | 0x14)  

/************* operation for low speed sync MOS *************/
#define LOWERMOSFET_STATE       (GPIOE->DR & 0x8)
#define HIGHERMOSFET_STATE      (GPIOE->DR & 0x4)
#define SETLOWERMOSFET_HIGH     GPIOE->DR |= GPIO_DR_D(MASK_BIT3) 
#define SETLOWERMOSFET_LOW      GPIOE->DR &= ~GPIO_DR_D(MASK_BIT3)
#define SETHIGHERMOSFET_HIGH    GPIOE->DR |= GPIO_DR_D(MASK_BIT2)
#define SETHIGHERMOSFET_LOW     GPIOE->DR &= ~GPIO_DR_D(MASK_BIT2)
#define DISABLE_ALLMOSFET()     GPIOE->DR &= ~GPIO_DR_D(MASK_BIT2|MASK_BIT3)
/************* operation for high speed GaN *************/
#if XBAR_OUT
#define DISABLE_UPGAN()          EVTG->EVTG_INST[1].EVTG_AOI0_BFT01 &= ~EVTG_EVTG_AOI0_BFT01_PT0_DC_MASK/*Disable up-side GaN MOSFET.*/
#define ENABLE_UPGAN()           EVTG->EVTG_INST[1].EVTG_AOI0_BFT01 |= EVTG_EVTG_AOI0_BFT01_PT0_DC_MASK/*Enable up-side GaN MOSFET.*/  
#define ENABLE_DOWNGAN()         EVTG->EVTG_INST[0].EVTG_AOI0_BFT01 |= EVTG_EVTG_AOI0_BFT01_PT0_DC_MASK /*Enable low-side GaN MOSFET.*/
#define DISABLE_DOWNGAN()        EVTG->EVTG_INST[0].EVTG_AOI0_BFT01 &= ~EVTG_EVTG_AOI0_BFT01_PT0_DC_MASK /*Disable low-side GaN MOSFET.*/
#define DISABLE_ALL_GAN()        {EVTG->EVTG_INST[0].EVTG_AOI0_BFT01 &= ~EVTG_EVTG_AOI0_BFT01_PT0_DC_MASK;\
                                  EVTG->EVTG_INST[1].EVTG_AOI0_BFT01 &= ~EVTG_EVTG_AOI0_BFT01_PT0_DC_MASK;}
#else
#define DISABLE_UPGAN()          PWM->OUTEN &= ~PWM_OUTEN_PWMB_EN(1)
#define ENABLE_UPGAN()           PWM->OUTEN |= PWM_OUTEN_PWMB_EN(1)
#define ENABLE_DOWNGAN()         PWM->OUTEN |= PWM_OUTEN_PWMA_EN(1)
#define DISABLE_DOWNGAN()        PWM->OUTEN &= ~PWM_OUTEN_PWMA_EN(1)
#define DISABLE_ALL_GAN()        PWM->OUTEN &= ~(PWM_OUTEN_PWMA_EN(1)|PWM_OUTEN_PWMB_EN(1))
#define UPGAN_STATE              PWM->OUTEN & PWM_OUTEN_PWMB_EN(1)
#define DOWNGAN_STATE            PWM->OUTEN & PWM_OUTEN_PWMA_EN(1)
#endif

#define CLR_PWM1_ISR_FLAG           PWM->SM[1].STS |= PWM_STS_CMPF(1)
#define CLR_PWM0_ISR_FLAG           PWM->SM[0].STS |= PWM_STS_CMPF(1)

#define DUTY_UPDATE(Duty)        {PWM->SM[0].VAL3 = Duty; PWM->SM[0].VAL2 = -Duty;}//Update PWM VAL0-5 registers

#define ENABLE_HARDWARE_CURRENT_PROTECTION()   PWM->SM[0].DISMAP[0] |= (PWM_DISMAP_DIS0A(3) | PWM_DISMAP_DIS0B(3))//Enable Hardware current protection
#define DISABLE_HARDWARE_CURRENT_PROTECTION()  PWM->SM[0].DISMAP[0] &= ~(PWM_DISMAP_DIS0A(3)| PWM_DISMAP_DIS0B(3)) //disable Hardware current protection
#define DISABLE_CURFAULT_ISR       PWM->FAULT[0].FCTRL &= ~PWM_FCTRL_FIE(3)
#define ENABLE_CURFAULT_ISR        PWM->FAULT[0].FCTRL |= PWM_FCTRL_FIE(3)
#define HW_FAULT()                     PWM->FAULT[0].FSTS & PWM_FSTS_FFLAG(7)
#define IIN_HW_FAULT()                 PWM->FAULT[0].FSTS & PWM_FSTS_FFLAG(3)
#define VOUT_HW_FAULT()                PWM->FAULT[0].FSTS & PWM_FSTS_FFLAG(4)
#define HW_FAULT_FLAG_CLEAR()  	       PWM->FAULT[0].FSTS |= PWM_FSTS_FFLAG(7)

/***************** polarity test *****************/
#define POLARITY(pol)  if(GPIOC->DR & 0x10) pol = 1; else pol = 0;
/***************** Test point settings *****************/
#define TP10_UP     GPIOE->DR |= 0x40
#define TP10_DOWN   GPIOE->DR &= ~0x40
#define TP11_UP     GPIOE->DR |= 0x80
#define TP11_DOWN   GPIOE->DR &= ~0x80
/******************************************************************************
* Functions
******************************************************************************/
void Pin_init(void);
void XBAR_init(void);
void eFlexPWMA_init(void);
void sci0_init(void);
void sci1_init(void);
void ADC_init(void);
void CMP_init(void);
extern void Peripherals_init(void);

#endif /* PERIPHEARLS_H_ */
