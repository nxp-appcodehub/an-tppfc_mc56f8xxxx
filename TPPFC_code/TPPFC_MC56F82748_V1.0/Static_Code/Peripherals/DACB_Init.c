/** ###################################################################
 **     Filename    : DACB_Init.c
 **     Processor   : MC56F82748VLH
 **     Abstract    :
 **          This file implements the DACB module initialization
 **          according to the Peripheral Initialization settings, and
 **          defines interrupt service routines prototypes.
 **
 **     Contents    :
 **         Int   - void DACB_Init(void);
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
 * @file DACB_Init.c
 * @brief This file implements the DACB module initialization according to the
 *        Peripheral Initialization settings, and defines interrupt service
 *        routines prototypes.
 */


/* MODULE DACB_Init. */

#include "DACB_Init.h"
#include "MC56F82748.h"
#include "Init_Config.h"

#ifdef __cplusplus
extern "C" {
#endif

void DACB_Init(void) {

  /* Register 'DACB_CTRL1' initialization */
  #ifdef DACB_CTRL1_VALUE
  DACB_CTRL1 = DACB_CTRL1_VALUE;
  #endif

  /* Register 'DACB_DATAREG_FMT0' initialization */
  #ifdef DACB_DATAREG_FMT0_VALUE
  DACB_DATAREG_FMT0 = DACB_DATAREG_FMT0_VALUE;
  #endif

  /* Register 'DACB_DATAREG_FMT1' initialization */
  #ifdef DACB_DATAREG_FMT1_VALUE
  DACB_DATAREG_FMT1 = DACB_DATAREG_FMT1_VALUE;
  #endif

  /* Register 'DACB_MINVAL_FMT0' initialization */
  #ifdef DACB_MINVAL_FMT0_VALUE
  DACB_MINVAL_FMT0 = DACB_MINVAL_FMT0_VALUE;
  #endif

  /* Register 'DACB_MINVAL_FMT1' initialization */
  #ifdef DACB_MINVAL_FMT1_VALUE
  DACB_MINVAL_FMT1 = DACB_MINVAL_FMT1_VALUE;
  #endif

  /* Register 'DACB_MAXVAL_FMT0' initialization */
  #ifdef DACB_MAXVAL_FMT0_VALUE
  DACB_MAXVAL_FMT0 = DACB_MAXVAL_FMT0_VALUE;
  #endif

  /* Register 'DACB_MAXVAL_FMT1' initialization */
  #ifdef DACB_MAXVAL_FMT1_VALUE
  DACB_MAXVAL_FMT1 = DACB_MAXVAL_FMT1_VALUE;
  #endif

  /* Register 'DACB_STEPVAL_FMT0' initialization */
  #ifdef DACB_STEPVAL_FMT0_VALUE
  DACB_STEPVAL_FMT0 = DACB_STEPVAL_FMT0_VALUE;
  #endif

  /* Register 'DACB_STEPVAL_FMT1' initialization */
  #ifdef DACB_STEPVAL_FMT1_VALUE
  DACB_STEPVAL_FMT1 = DACB_STEPVAL_FMT1_VALUE;
  #endif

  /* Register 'DACB_CTRL0' initialization */
  #ifdef DACB_CTRL0_VALUE
  DACB_CTRL0 = DACB_CTRL0_VALUE;
  #endif
}

#ifdef __cplusplus
}
#endif

/* END DACB_Init. */

/** ###################################################################
 **
 **     This file is a part of Processor Expert static initialization
 **     library for Freescale microcontrollers.
 **
 ** ################################################################### */
