/** ###################################################################
 **     Filename    : AOI_Init.c
 **     Processor   : MC56F82748VLH
 **     Abstract    :
 **          This file implements the AOI module initialization
 **          according to the Peripheral Initialization settings, and
 **          defines interrupt service routines prototypes.
 **
 **     Contents    :
 **         Int   - void AOI_Init(void);
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
 * @file AOI_Init.c
 * @brief This file implements the AOI module initialization according to the
 *        Peripheral Initialization settings, and defines interrupt service
 *        routines prototypes.
 */


/* MODULE AOI_Init. */

#include "AOI_Init.h"
#include "MC56F82748.h"
#include "Init_Config.h"

#ifdef __cplusplus
extern "C" {
#endif

void AOI_Init(void) {

  /* Register 'AOI_BFCRT010' initialization */
  #ifdef AOI_BFCRT010_VALUE
  AOI_BFCRT010 = AOI_BFCRT010_VALUE;
  #endif

  /* Register 'AOI_BFCRT230' initialization */
  #ifdef AOI_BFCRT230_VALUE
  AOI_BFCRT230 = AOI_BFCRT230_VALUE;
  #endif

  /* Register 'AOI_BFCRT011' initialization */
  #ifdef AOI_BFCRT011_VALUE
  AOI_BFCRT011 = AOI_BFCRT011_VALUE;
  #endif

  /* Register 'AOI_BFCRT231' initialization */
  #ifdef AOI_BFCRT231_VALUE
  AOI_BFCRT231 = AOI_BFCRT231_VALUE;
  #endif

  /* Register 'AOI_BFCRT012' initialization */
  #ifdef AOI_BFCRT012_VALUE
  AOI_BFCRT012 = AOI_BFCRT012_VALUE;
  #endif

  /* Register 'AOI_BFCRT232' initialization */
  #ifdef AOI_BFCRT232_VALUE
  AOI_BFCRT232 = AOI_BFCRT232_VALUE;
  #endif

  /* Register 'AOI_BFCRT013' initialization */
  #ifdef AOI_BFCRT013_VALUE
  AOI_BFCRT013 = AOI_BFCRT013_VALUE;
  #endif

  /* Register 'AOI_BFCRT233' initialization */
  #ifdef AOI_BFCRT233_VALUE
  AOI_BFCRT233 = AOI_BFCRT233_VALUE;
  #endif
}

#ifdef __cplusplus
}
#endif

/* END AOI_Init. */

/** ###################################################################
 **
 **     This file is a part of Processor Expert static initialization
 **     library for Freescale microcontrollers.
 **
 ** ################################################################### */
