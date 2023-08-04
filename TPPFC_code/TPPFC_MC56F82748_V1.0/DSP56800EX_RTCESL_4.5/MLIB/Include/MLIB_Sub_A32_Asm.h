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
* @brief  Subtraction functions with 32-bit accumulator output in assembler
* 
*******************************************************************************/
#ifndef _MLIB_SUB_A32_ASM_H_
#define _MLIB_SUB_A32_ASM_H_

#if defined(__cplusplus) 
extern "C" { 
#endif 
/******************************************************************************
* Includes
******************************************************************************/
#include "mlib_types.h"

/******************************************************************************
* Constants
******************************************************************************/

/******************************************************************************
* Macros 
******************************************************************************/
#define MLIB_Sub_A32ss_Asmi(f16Min, f16Sub) MLIB_Sub_A32ss_FAsmi(f16Min, f16Sub)
#define MLIB_Sub_A32as_Asmi(a32Accum, f16Sub) MLIB_Sub_A32as_FAsmi(a32Accum, f16Sub)

/******************************************************************************
* Types
******************************************************************************/

/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Global functions
******************************************************************************/

/******************************************************************************
* Inline functions
******************************************************************************/

/***************************************************************************//*!
*
* @brief  Subtraction of two 16-bit fractional arguments resulting a 32-bit accumulator
*
* @param  ptr			
* 
* @param  in    		frac16_t f16Min
*                         - Argument in [-1;1] in frac16_t
*						frac16_t f16Sub
*                         - Argument in [-1;1] in frac16_t
*
* @return This function returns
*     - acc32_t value [-65536.0;-65536.0 - (2^-15)]
*		
* @remarks 	This function returns the subtraction of two inputs. The result is stored
* 			in the lower word of the result with sing extension to the 32 bits.
* 			This function does not saturate the the output
*
*			SATURATION INDEPENDENT!
*
****************************************************************************/
extern inline acc32_t MLIB_Sub_A32ss_FAsmi(register frac16_t f16Min, register frac16_t f16Sub)
{
	register acc32_t a32Acc, a32Temp;
	
	asm(.optimize_iasm on);
		
	asm(asr16 f16Min,a32Acc);				/* a32Acc = f16Min >> 16 */
		
	asm(asr16 f16Sub,a32Temp);				/* a32Temp = f16Sub >> 16 */
		
	asm(sub a32Temp,a32Acc);				/* a32Acc = a32Acc - f16Sub */
		
	asm(.optimize_iasm off);
		
	return a32Acc;
}

/***************************************************************************//*!
*
* @brief  Subtraction of a 16-bit fractional value from a 32-bit accumulator resulting
* 		  a 32-bit accumulator
*
* @param  ptr			
* 
* @param  in    		acc32_t a32Accum
*                         - Argument in [-65536.0;65536.0 - (2^-15)] in acc32_t
*						frac16_t f16Sub
*                         - Argument in [-1;1] in frac16_t
*
* @return This function returns
*     - acc32_t value [-65536.0;65536.0 - (2^-15)]
*		
* @remarks 	This function returns the subtraction of a 32-bit accumulator from a
* 			16-bit fractional input. The f16Sub value is subtracted from the lower
* 			word of a32Acc.
* 			This function does not saturate the the output
*
*			SATURATION INDEPENDENT!
*
****************************************************************************/
extern inline acc32_t MLIB_Sub_A32as_FAsmi(register acc32_t a32Accum, register frac16_t f16Sub)
{
	register acc32_t a32Acc, a32Temp;
	
	asm(.optimize_iasm on);
		
	asm(asr16 f16Sub,a32Temp);				/* a32Temp = f16Sub >> 16 */
		
	asm(sub a32Temp,a32Accum);				/* a32Accum = a32Accum - f16Sub */
		
	asm(.optimize_iasm off);
		
	return a32Accum;
}

#if defined(__cplusplus) 
} 
#endif 

#endif /* _MLIB_SUB_A32_ASM_H_ */
