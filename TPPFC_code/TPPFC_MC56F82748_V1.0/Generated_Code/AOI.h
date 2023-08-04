/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : AOI.h
**     Project     : TPPFC_MC56F82748_V1.0
**     Processor   : MC56F82748VLH
**     Component   : Init_AOI
**     Version     : Component 01.000, Driver 01.00, CPU db: 3.50.001
**     Compiler    : CodeWarrior DSP C Compiler
**     Date/Time   : 2022-01-24, 16:51, # CodeGen: 0
**     Abstract    :
**          This file implements the AOI (AOI) module initialization
**          according to the Peripheral Initialization settings, and
**          defines interrupt service routines prototypes.
**     Settings    :
**          Component name                                 : AOI
**          Device                                         : AOI
**          Settings                                       : 
**            Boolean Function 0 Configuration             : 
**              Product Term 0                             : 
**                A input                                  : Pass
**                B input                                  : Complement
**                C input                                  : Complement
**                D input                                  : Force logical zero
**              Product Term 1                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**              Product Term 2                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**              Product Term 3                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**            Boolean Function 1 Configuration             : 
**              Product Term 0                             : 
**                A input                                  : Pass
**                B input                                  : Complement
**                C input                                  : Complement
**                D input                                  : Force logical zero
**              Product Term 1                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**              Product Term 2                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**              Product Term 3                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**            Boolean Function 2 Configuration             : 
**              Product Term 0                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**              Product Term 1                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**              Product Term 2                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**              Product Term 3                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**            Boolean Function 3 Configuration             : 
**              Product Term 0                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**              Product Term 1                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**              Product Term 2                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**              Product Term 3                             : 
**                A input                                  : Force logical zero
**                B input                                  : Force logical zero
**                C input                                  : Force logical zero
**                D input                                  : Force logical zero
**          Pins                                           : 
**            Term 0 pins                                  : 
**              Boolean function A input                   : Enabled
**                Pin                                      : PWM0_TRG0
**                Pin signal                               : 
**              Boolean function B input                   : Enabled
**                Pin                                      : CMPB_OUT
**                Pin signal                               : 
**              Boolean function C input                   : Enabled
**                Pin                                      : CMPC_OUT
**                Pin signal                               : 
**              Boolean function D input                   : Disabled
**              Boolean term output                        : Disabled
**            Term 1 pins                                  : 
**              Boolean function A input                   : Enabled
**                Pin                                      : PWM0_TRG1
**                Pin signal                               : 
**              Boolean function B input                   : Enabled
**                Pin                                      : CMPB_OUT
**                Pin signal                               : 
**              Boolean function C input                   : Enabled
**                Pin                                      : CMPC_OUT
**                Pin signal                               : 
**              Boolean function D input                   : Disabled
**              Boolean term output                        : Disabled
**            Term 2 pins                                  : 
**              Boolean function A input                   : Disabled
**              Boolean function B input                   : Disabled
**              Boolean function C input                   : Disabled
**              Boolean function D input                   : Disabled
**              Boolean term output                        : Disabled
**            Term 3 pins                                  : 
**              Boolean function A input                   : Disabled
**              Boolean function B input                   : Disabled
**              Boolean function C input                   : Disabled
**              Boolean function D input                   : Disabled
**              Boolean term output                        : Disabled
**          Initialization                                 : 
**            Call Init method                             : yes
**            Utilize after reset values                   : default
**     Contents    :
**         Init - void AOI_Init(void);
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
** @file AOI.h
** @version 01.00
** @brief
**          This file implements the AOI (AOI) module initialization
**          according to the Peripheral Initialization settings, and
**          defines interrupt service routines prototypes.
*/         
/*!
**  @addtogroup AOI_module AOI module documentation
**  @{
*/         

#ifndef AOI_H_
#define AOI_H_

/* MODULE AOI. */

#include "AOI_Init.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialization method user name */
#define AOI_Init AOI_Init
/* PDD macros peripheral base address parameter */
#define AOI_DEVICE AOI_BASE_PTR

#ifdef __cplusplus
}
#endif

/* END AOI. */
#endif /* #ifndef __AOI_H_ */
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
