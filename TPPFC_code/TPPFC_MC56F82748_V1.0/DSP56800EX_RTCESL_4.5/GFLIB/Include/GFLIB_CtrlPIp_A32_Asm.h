/*******************************************************************************
*
 * Copyright 2013 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
* 
*
****************************************************************************//*!
*
* @brief  The parallel form of Proportional-Integral (PI) regulator algorithms
* 		  implemented in assembler
* 
*******************************************************************************/
#ifndef _GFLIB_CTRLPIP_A32_ASM_H_
#define _GFLIB_CTRLPIP_A32_ASM_H_

#if defined(__cplusplus) 
extern "C" { 
#endif 
/******************************************************************************
* Includes
******************************************************************************/
#include "mlib.h"

/******************************************************************************
* Constants
******************************************************************************/

/******************************************************************************
* Macros 
******************************************************************************/
#define GFLIB_CtrlPIpAWInit_F16_Asmi(f16InitVal, psParam) GFLIB_CtrlPIpAWInit_F16_FAsmi(f16InitVal, psParam)

#define GFLIB_CtrlPIpAW_F16_Asm(f16InErr, pbStopIntegFlag, psParam) GFLIB_CtrlPIpAW_F16_FAsm(f16InErr, pbStopIntegFlag, psParam)

/******************************************************************************
* Types
******************************************************************************/
typedef struct
{
   acc32_t a32PGain;
   acc32_t a32IGain;
   frac32_t f32IAccK_1;
   frac16_t f16InErrK_1;
   frac16_t f16UpperLim;  
   frac16_t f16LowerLim;    
   bool_t  bLimFlag;
} GFLIB_CTRL_PI_P_AW_T_A32;



/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Global functions
******************************************************************************/
extern asm frac16_t GFLIB_CtrlPIpAW_F16_FAsm(frac16_t f16InErr, const bool_t *pbStopIntegFlag, GFLIB_CTRL_PI_P_AW_T_A32 *psParam);


/******************************************************************************
* Inline functions
******************************************************************************/

/***************************************************************************//*!
*
* @brief  The function initializes the integral part of the PI controller.
*
* @param  ptr			GFLIB_CTRL_PI_P_AW_T_A32 *psParam
* 						- pointer to the structure of the PI controller
*
*                         GFLIB_CTRL_PI_P_AW_T_A32 data structure:
*                           acc32_t a32PGain    
*                               - proportional gain; input parameter
*                                 in the following range
*                                 0 <= f16PGain < 65536.0  
*                           acc32 a32IGain
*                               - integral gain; input parameter
*                                 in the following range
*                                 0 <= f16IGain < 65536.0    
*                           frac32_t f32IAccK_1
*                               - state variable; fractional part (lower long)
*                                 of integral part at step k-1;
*                                 can be modified outside of the function;
*                                 input/output parameter
*                                 in the following range
*                                 -1.0 <= f32IAccK_1 < 1.0                               
*							frac16_t f16InErrK_1
*								- error at the step k-1;
*                                 input/output parameter
*                                 in the following range
*                                 -1.0 <= f16InErrK_1 < 1.0   
*                           frac16_t f16UpperLim
*                               - upper limit of the controller output;
*                                 input parameter
*                                 in the following range
*                                 -1.0 <= f16UpperLim < 1.0                                    
*                                 f16UpperLim > f16LowerLim
*                           frac16_t f16LowerLim
*                               - lower limit of the controller output;
*                                 input parameter
*                                 in the following range
*                                 -1.0 <= f16LowerLim < 1.0   
*                                 f16UpperLim > f16LowerLim
*                           bool_t bLimFlag
*                               - limitation flag; if set, the controller
*                                 output reached either f16UpperLim or f16LowerLim,
*                                 otherwise it is cleared
*                                 output parameter 
*
* @param  in    		frac16_t f16InitVal
* 							- Initial value
*
* @return N/A
*		
* @remarks The function sets the integral portion state variable f32IAccK_1 to
* 		   the value f16InitVal in such a way that if the inputs are zero
* 		   the result of the controller will be equal to f16InitVal.
* 		   At the same time the f16InErrK_1 value is zeroed.
*
****************************************************************************/
extern inline void GFLIB_CtrlPIpAWInit_F16_FAsmi(register frac16_t f16InitVal, register GFLIB_CTRL_PI_P_AW_T_A32 *psParam)
{
	register frac32_t f32Integral;
	
	asm(move.w f16InitVal,f32Integral);			/* f32Integral = f16InitVal << 16 */
	asm(move.l f32Integral,x:(psParam + 4));	/* Stores the integral portion */

	psParam->f16InErrK_1 = 0;					/* Resets the previous step error */
}


#if defined(__cplusplus) 
} 
#endif 

#endif /* _GFLIB_CTRLPIP_A32_ASM_H_ */
