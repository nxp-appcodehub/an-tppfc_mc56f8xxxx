/** ###################################################################
 **     Filename    : EWM_Init.c
 **     Processor   : MC56F82748VLH
 **     Abstract    :
 **          This file implements the EWM module initialization
 **          according to the Peripheral Initialization settings, and
 **          defines interrupt service routines prototypes.
 **
 **     Contents    :
 **         Int   - void EWM_Init(void);
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
 * @file EWM_Init.c
 * @brief This file implements the EWM module initialization according to the
 *        Peripheral Initialization settings, and defines interrupt service
 *        routines prototypes.
 */


/* MODULE EWM_Init. */

#include "EWM_Init.h"
#include "MC56F82748.h"
#include "Init_Config.h"

#ifdef __cplusplus
extern "C" {
#endif

void EWM_Init(void) {

  /* Register 'EWM_CLKCTRL' initialization */
  #ifdef EWM_CLKCTRL_VALUE
  EWM_CLKCTRL = EWM_CLKCTRL_VALUE;
  #endif

  /* Register 'EWM_CLKPRESCALER' initialization */
  #ifdef EWM_CLKPRESCALER_VALUE
  EWM_CLKPRESCALER = EWM_CLKPRESCALER_VALUE;
  #endif

  /* Register 'EWM_CMPL' initialization */
  #ifdef EWM_CMPL_VALUE
  EWM_CMPL = EWM_CMPL_VALUE;
  #endif

  /* Register 'EWM_CMPH' initialization */
  #ifdef EWM_CMPH_VALUE
  EWM_CMPH = EWM_CMPH_VALUE;
  #endif

  /* Register 'EWM_CTRL' initialization */
  #ifdef EWM_CTRL_VALUE
  EWM_CTRL = EWM_CTRL_VALUE;
  #endif
}

#ifdef __cplusplus
}
#endif

/* END EWM_Init. */

/** ###################################################################
 **
 **     This file is a part of Processor Expert static initialization
 **     library for Freescale microcontrollers.
 **
 ** ################################################################### */
