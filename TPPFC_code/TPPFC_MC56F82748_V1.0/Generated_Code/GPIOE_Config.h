/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : GPIOE_Config.h
**     Project     : TPPFC_MC56F82748_V1.0
**     Processor   : MC56F82748VLH
**     Component   : Init_GPIO
**     Version     : Component 01.000, Driver 01.00, CPU db: 3.50.001
**     Compiler    : CodeWarrior DSP C Compiler
**     Date/Time   : 2022-01-24, 16:51, # CodeGen: 0
**     Abstract    :
**          This file implements the GPIO (GPIOE) module initialization
**          according to the Peripheral Initialization settings, and
**          defines interrupt service routines prototypes.
**     Settings    :
**          Component name                                 : GPIOE
**          Device                                         : GPIOE
**          Settings                                       : 
**            Pin 0                                        : Do not initialize
**            Pin 1                                        : Do not initialize
**            Pin 2                                        : Do not initialize
**            Pin 3                                        : Do not initialize
**            Pin 4                                        : Do not initialize
**            Pin 5                                        : Do not initialize
**            Pin 6                                        : Do not initialize
**            Pin 7                                        : Initialize
**              Mode                                       : GPIO
**              Direction                                  : Output
**              Output value                               : 0
**              Pull enable                                : Enabled
**              Pull select                                : Pull Up
**              Open drain                                 : Push-Pull
**              Drive strength                             : High
**              Slew rate                                  : Fast
**              Interrupt                                  : Disabled
**              Interrupt polarity                         : Rising edge
**            Pin 8                                        : Do not initialize
**            Pin 9                                        : Do not initialize
**            Pin 10                                       : Do not initialize
**            Pin 11                                       : Do not initialize
**            Pin 12                                       : Do not initialize
**            Pin 13                                       : Do not initialize
**            Pin 14                                       : Do not initialize
**            Pin 15                                       : Do not initialize
**          Pin selection/routing                          : 
**            Pin 0                                        : Disabled
**            Pin 1                                        : Disabled
**            Pin 2                                        : Disabled
**            Pin 3                                        : Disabled
**            Pin 4                                        : Disabled
**            Pin 5                                        : Disabled
**            Pin 6                                        : Disabled
**            Pin 7                                        : Enabled
**              Pin                                        : GPIOE7/PWM_3A/XB_IN5
**              Pin signal                                 : 
**            Pin 8                                        : Disabled
**            Pin 9                                        : Disabled
**            Pin 10                                       : Disabled
**            Pin 11                                       : Disabled
**            Pin 12                                       : Disabled
**            Pin 13                                       : Disabled
**            Pin 14                                       : Disabled
**            Pin 15                                       : Disabled
**          Interrupts                                     : 
**            Port interrupt                               : 
**              Interrupt                                  : INT_GPIOE
**              Interrupt priority                         : disabled
**              ISR name                                   : 
**          Initialization                                 : 
**            Enable peripheral clock                      : yes
**            Call Init method                             : yes
**            Utilize after reset values                   : default
**     Contents    :
**         Init - void GPIOE_Init(void);
**
**     Copyright : 1997 - 2014 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file GPIOE_Config.h                                                  
** @version 01.00
** @brief
**          This file implements the GPIO (GPIOE) module initialization
**          according to the Peripheral Initialization settings, and
**          defines interrupt service routines prototypes.
*/         
/*!
**  @addtogroup GPIOE_Config_module GPIOE_Config module documentation
**  @{
*/         

#ifndef GPIOE_Config_H_
#define GPIOE_Config_H_

/* MODULE GPIOE. */

/* GPIOE_PUS: PUS|=0x80 */
#define GPIOE_PUS_VALUE      0x80
#define GPIOE_PUS_MASK       0x80
/* GPIOE_PUR: PU|=0x80 */
#define GPIOE_PUR_VALUE      0x80
#define GPIOE_PUR_MASK       0x80
/* GPIOE_DRIVE: DRIVE|=0x80 */
#define GPIOE_DRIVE_VALUE    0x80
#define GPIOE_DRIVE_MASK     0x80
/* GPIOE_SRE: SRE&=~0x80 */
#define GPIOE_SRE_VALUE      0x00
#define GPIOE_SRE_MASK       0x80
/* GPIOE_PPMODE: PPMODE|=0x80 */
#define GPIOE_PPMODE_VALUE   0x80
#define GPIOE_PPMODE_MASK    0x80
/* GPIOE_DR: D&=~0x80 */
#define GPIOE_DR_VALUE       0x00
#define GPIOE_DR_MASK        0x80
/* GPIOE_DDR: DD|=0x80 */
#define GPIOE_DDR_VALUE      0x80
#define GPIOE_DDR_MASK       0x80
/* GPIOE_IPOLR: IPOL&=~0x80 */
#define GPIOE_IPOLR_VALUE    0x00
#define GPIOE_IPOLR_MASK     0x80
/* GPIOE_PER: PE&=~0x80 */
#define GPIOE_PER_VALUE      0x00
#define GPIOE_PER_MASK       0x80
/* GPIOE_IESR: IES|=0x80 */
#define GPIOE_IESR_VALUE     0x80
#define GPIOE_IESR_MASK      0x80
/* GPIOE_IENR: IEN&=~0x80 */
#define GPIOE_IENR_VALUE     0x00
#define GPIOE_IENR_MASK      0x80

#define GPIOE_AUTOINIT

/* END GPIOE. */
#endif /* #ifndef __GPIOE_Config_H_ */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
