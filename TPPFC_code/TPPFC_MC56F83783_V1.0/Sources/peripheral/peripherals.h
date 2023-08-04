/*
* Copyright 2021 NXP
* SPDX-License-Identifier: BSD-3-Clause
*/

/*
 * peripherals.h
 *
 *  Created on: Jan 21, 2019
 *      Author: nxa22573
 */

#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

#include <stdint.h>
#include "derivative.h"
#include "mlib.h"

/******************************************************************************
* Macros 
******************************************************************************/
#define XBAR_OUT               1

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

#define RELAY_STATE()    GPIOF->DR & 0x80
#define CLOSE_RELAY()    GPIOF->DR |= GPIO_DR_D(MASK_BIT7)
#define OPEN_RELAY()     GPIOF->DR &= ~GPIO_DR_D(MASK_BIT7)

#define PWMA_RUN()               PWMA->MCTRL |= PWM_MCTRL_RUN(7) /*Enable PWM generator clock*/
#define PWMA_STOP()              PWMA->MCTRL &= ~PWM_MCTRL_RUN(7) /*Disable PWM generator clock*/
#define ENABLE_DUTYLOAD          PWMA->MCTRL |= PWM_MCTRL_LDOK(5)
#define INVERTOUTPUTPOLARITY     PWMA->SM0OCTRL |= (PWM_SM0OCTRL_POLA_MASK | PWM_SM0OCTRL_POLB_MASK)/*Inverse polarity of PWM output in input negative cycle*/ 
#define RESETOUTPUTPOLARITY      PWMA->SM0OCTRL &= ~(PWM_SM0OCTRL_POLA_MASK | PWM_SM0OCTRL_POLB_MASK)/*Not invert polarity of PWM output in input positive cycle*/ 
#define PWMFAULTSTATE_0          PWMA->SM0OCTRL &= ~(PWM_SM0OCTRL_PWMAFS_MASK|PWM_SM0OCTRL_PWMBFS_MASK)
#define PWMFAULTSTATE_1          PWMA->SM0OCTRL = ((PWMA->SM0OCTRL & ~(PWM_SM0OCTRL_PWMAFS_MASK|PWM_SM0OCTRL_PWMBFS_MASK)) | 0x14)  

/************* operation for low speed sync MOS *************/
#define LOWERMOSFET_STATE       (GPIOF->DR & 0x10)
#define HIGHERMOSFET_STATE      (GPIOF->DR & 0x20)
#define SETLOWERMOSFET_HIGH     GPIOF->DR |= GPIO_DR_D(MASK_BIT4) 
#define SETLOWERMOSFET_LOW      GPIOF->DR &= ~GPIO_DR_D(MASK_BIT4)
#define SETHIGHERMOSFET_HIGH    GPIOF->DR |= GPIO_DR_D(MASK_BIT5)
#define SETHIGHERMOSFET_LOW     GPIOF->DR &= ~GPIO_DR_D(MASK_BIT5)
#define DISABLE_ALLMOSFET()     GPIOF->DR &= ~GPIO_DR_D(MASK_BIT4|MASK_BIT5)
/************* operation for high speed GaN *************/
#if XBAR_OUT
#define DISABLE_UPGAN()          EVTG->EVTG_INST[1].EVTG_AOI0_BFT01 &= ~EVTG_EVTG_AOI0_BFT01_PT0_DC_MASK/*Disable up-side GaN MOSFET.*/
#define ENABLE_UPGAN()           EVTG->EVTG_INST[1].EVTG_AOI0_BFT01 |= EVTG_EVTG_AOI0_BFT01_PT0_DC_MASK/*Enable up-side GaN MOSFET.*/  
#define ENABLE_DOWNGAN()         EVTG->EVTG_INST[0].EVTG_AOI0_BFT01 |= EVTG_EVTG_AOI0_BFT01_PT0_DC_MASK /*Enable low-side GaN MOSFET.*/
#define DISABLE_DOWNGAN()        EVTG->EVTG_INST[0].EVTG_AOI0_BFT01 &= ~EVTG_EVTG_AOI0_BFT01_PT0_DC_MASK /*Disable low-side GaN MOSFET.*/
#define DISABLE_ALL_GAN()        {EVTG->EVTG_INST[0].EVTG_AOI0_BFT01 &= ~EVTG_EVTG_AOI0_BFT01_PT0_DC_MASK;\
                                  EVTG->EVTG_INST[1].EVTG_AOI0_BFT01 &= ~EVTG_EVTG_AOI0_BFT01_PT0_DC_MASK;}
#define UPGAN_STATE              (EVTG->EVTG_INST[1].EVTG_AOI0_BFT01 & 0x300)
#define DOWNGAN_STATE            (EVTG->EVTG_INST[0].EVTG_AOI0_BFT01 & 0x300)

#else
#define DISABLE_UPGAN()          PWMA->OUTEN &= ~PWM_OUTEN_PWMB_EN(1)
#define ENABLE_UPGAN()           PWMA->OUTEN |= PWM_OUTEN_PWMB_EN(1)
#define ENABLE_DOWNGAN()         PWMA->OUTEN |= PWM_OUTEN_PWMA_EN(1)
#define DISABLE_DOWNGAN()        PWMA->OUTEN &= ~PWM_OUTEN_PWMA_EN(1)
#define DISABLE_ALL_GAN()        PWMA->OUTEN &= ~(PWM_OUTEN_PWMA_EN(1)|PWM_OUTEN_PWMB_EN(1))
#endif

#define DUTY_UPDATE(Duty)        {PWMA->SM0VAL3 = Duty; PWMA->SM0VAL2 = -Duty;}//Update PWM VAL2&3 registers

#define CLR_PWM1_ISR_FLAG           PWMA->SM1STS |= PWM_SM1STS_CMPF(1)
#define CLR_PWM0_ISR_FLAG           PWMA->SM0STS |= PWM_SM0STS_CMPF(1)

#define ENABLE_HARDWARE_CURRENT_PROTECTION()   PWMA->SM0DISMAP0 |= (PWM_SM0DISMAP0_DIS0A(3) | PWM_SM0DISMAP0_DIS0B(3))//Enable Hardware current protection
#define DISABLE_HARDWARE_CURRENT_PROTECTION()  PWMA->SM0DISMAP0 &= ~(PWM_SM0DISMAP0_DIS0A(3)| PWM_SM0DISMAP0_DIS0B(3)) //disable Hardware current protection
#define HW_FAULT()                     PWMA->FSTS0 & PWM_FSTS0_FFLAG(7)
#define IIN_HW_FAULT()                 PWMA->FSTS0 & PWM_FSTS0_FFLAG(3)
#define VOUT_HW_FAULT()                PWMA->FSTS0 & PWM_FSTS0_FFLAG(4)
#define HW_FAULT_FLAG_CLEAR()  	       PWMA->FSTS0 |= PWM_FSTS0_FFLAG(7)

/************************ polarity detection ************************/ 
#define POLARITY(pol)  if(GPIOC->DR & 0x10) pol = 1; else pol = 0;

/***************** Test point settings *****************/
#define TP9_UP()      GPIOC->DR |= 0x2000
#define TP9_DOWN()    GPIOC->DR &= ~0x2000
#define TP11_UP()     GPIOE->DR |= 0x80
#define TP11_DOWN()   GPIOE->DR &= ~0x80
/******************************************************************************
* Functions
******************************************************************************/
extern void Pin_init(void);
extern void XBAR_init(void);
extern void eFlexPWMA_init(void);
extern void sci_init(void);
extern void ADC_init(void);
extern void CMP_init(void);
extern void Peripheral_init(void);

#endif /* PERIPHERALS_H_ */
