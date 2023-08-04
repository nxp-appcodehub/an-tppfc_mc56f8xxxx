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
* @brief  One-quadrant division functions with 32-bit accumulator output in assembler
* 
*******************************************************************************/
#ifndef _MLIB_DIV1Q_A32_ASM_H_
#define _MLIB_DIV1Q_A32_ASM_H_

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
/* Accumulator-output division */
#define MLIB_Div1Q_A32ss_Asmi(f16Num, f16Denom) MLIB_Div1Q_A32ss_FAsmi(f16Num, f16Denom)
#define MLIB_Div1Q_A32ls_Asmi(f32Num, f16Denom) MLIB_Div1Q_A32ls_FAsmi(f32Num, f16Denom)
#define MLIB_Div1Q_A32ll_Asm(f32Num, f32Denom) MLIB_Div1Q_A32ll_FAsm(f32Num, f32Denom)

/******************************************************************************
* Types
******************************************************************************/

/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Global functions
******************************************************************************/
extern acc32_t MLIB_Div1Q_A32ll_FAsm(frac32_t f32Num, frac32_t f32Denom);

/******************************************************************************
* Inline functions
******************************************************************************/

/***************************************************************************//*!
*
* @brief  16-bit numerator, 16-bit denominator inputs 32-bit accumulator output
* 		  1-quadrant division function
*
* @param  ptr			
* 
* @param  in    		frac16_t f16Num
*                         - Numerator in [0;1] in frac16_t
*						frac16_t f16Denom
*                         - Denominator in [0;1] in frac16_t
*                       
*
* @return This function returns
*     - acc32_t value [0;65536.0 - (2^-15)]
*		
* @remarks 	This function divides two non-negative fractional inputs:
* 			result = f16Num / f16Denom.
* 			The function normalises the inputs to get higher precision of
* 			division. The output is the accumulator type where the lower
* 			word is the fractional part.
* 			If the denominator is 0, the output is 0x7FFF FFFF.
*
*			SATURATION INDEPENDENT!
*
****************************************************************************/
extern inline acc32_t MLIB_Div1Q_A32ss_FAsmi(register frac16_t f16Num, register frac16_t f16Denom)
{
	register int16_t w16ClbNum, w16ClbDenom;
	register acc32_t a32Result;
	register frac32_t f32NumTemp;
		
	asm(.optimize_iasm on);

	asm(move.w f16Num,f32NumTemp);
		
	asm(clb f32NumTemp,w16ClbNum);		/* w16ClbNum = number of leading bits of f16Num */
		
	asm(sub.w #1,w16ClbNum);			/* w16ClbNum = w16ClbNum - 1 because we need to have f16Num 0.25 to 0.5 */
	
	asm(clb f16Denom,w16ClbDenom);		/* w16ClbDenom = number of leading bits of f16Denom */
		
	asm(asll.l w16ClbNum,f32NumTemp);	/* Normalisation of f16Num to the range 0.25 to 0.5 */	
		
	asm(asll.l w16ClbDenom,f16Denom);	/* Normalisation of f16Denom to 0.5 to 1.0 */
	
	asm(sub w16ClbNum,w16ClbDenom);		/* w16ClbDenom = shifts of f16Denom - shifts of w16ClbNum */
	
	asm(tst f32NumTemp);				/* Clears the C flag */
	asm(rep 8);							/* Repeat 8 times */
	asm(div f16Denom,f32NumTemp);		/* f16Num = f32NumTemp / f16Denom */
	asm(rep 8);							/* Repeat 8 times */
	asm(div f16Denom,f32NumTemp);		/* f16Num = f32NumTemp / f16Denom */
		
	asm(move.w f32NumTemp.0,f32NumTemp);/* f32NumTemp = f32NumTemp << 16 */	

	asm(asr16 f32NumTemp,a32Result);	/* a32Result = f32NumTemp >> 16 */ 
	asm(asll.l w16ClbDenom,a32Result);	/* a32Result = a32Result << w16ClbDenom (arithmetically) */
		
	asm(bfchg #0x8000,f32NumTemp.1);	/* Changes the MSB of the result */
	asm(sat f32NumTemp);
		
	asm(tst.w f16Denom);				/* Leading bits of denominator, comparison to 15 */
	asm(teq f32NumTemp,a32Result);		/* In case of 0, uses the maximum output */
		
	asm(.optimize_iasm off);
		
	return a32Result;
}

/***************************************************************************//*!
*
* @brief  32-bit numerator, 16-bit denominator inputs 32-bit accumulator output
* 		  1-quadrant division function
*
* @param  ptr			
* 
* @param  in    		frac32_t f32Num
*                         - Numerator in [0;1] in frac32_t
*						frac16_t f16Denom
*                         - Denominator in [0;1] in frac16_t
*                       
*
* @return This function returns
*     - acc32_t value [0;65536.0 - (2^-15)]
*		
* @remarks 	This function divides two non-negative fractional inputs:
* 			result = f32Num / f16Denom.
* 			The function normalises the inputs to get higher precision of
* 			division. The output is the accumulator type where the lower
* 			word is the fractional part.
* 			If the denominator is 0, the output is 0x7FFF FFFF.
*
*			SATURATION INDEPENDENT!
*
****************************************************************************/
extern inline acc32_t MLIB_Div1Q_A32ls_FAsmi(register frac32_t f32Num, register frac16_t f16Denom)
{
	register int16_t w16ClbNum, w16ClbDenom;
	register acc32_t a32Result;
	register frac32_t f32NumTemp;
		
	asm(.optimize_iasm on);

	asm(tfr f32Num,f32NumTemp);
		
	asm(clb f32NumTemp,w16ClbNum);		/* w16ClbNum = number of leading bits of f32Num */
		
	asm(sub.w #1,w16ClbNum);			/* w16ClbNum = w16ClbNum - 1 because we need to have f16Num 0.25 to 0.5 */
	
	asm(clb f16Denom,w16ClbDenom);		/* w16ClbDenom = number of leading bits of f16Denom */
		
	asm(asll.l w16ClbNum,f32NumTemp);	/* Normalisation of f16Num to the range 0.25 to 0.5 */	
		
	asm(asll.l w16ClbDenom,f16Denom);	/* Normalisation of f16Denom to 0.5 to 1.0 */
	
	asm(sub w16ClbNum,w16ClbDenom);		/* w16ClbDenom = shifts of f16Denom - shifts of w16ClbNum */
	
	asm(tst f32NumTemp);				/* Clears the C flag */
	asm(rep 8);							/* Repeat 8 times */
	asm(div f16Denom,f32NumTemp);		/* f32Num = f32NumTemp / f16Denom */
	asm(rep 8);							/* Repeat 8 times */
	asm(div f16Denom,f32NumTemp);		/* f32Num = f32NumTemp / f16Denom */
		
	asm(move.w f32NumTemp.0,f32NumTemp);/* f32NumTemp = f32NumTemp << 16 */	

	asm(asr16 f32NumTemp,a32Result);	/* a32Result = f32NumTemp >> 16 */ 
	asm(asll.l w16ClbDenom,a32Result);	/* a32Result = a32Result << w16ClbDenom (arithmetically) */
		
	asm(bfchg #0x8000,f32NumTemp.1);	/* Changes the MSB of the result */
	asm(sat f32NumTemp);
		
	asm(tst.w f16Denom);				/* Leading bits of denominator, comparison to 15 */
	asm(teq f32NumTemp,a32Result);		/* In case of 0, uses the maximum output */
		
	asm(.optimize_iasm off);
		
	return a32Result;
}

#if defined(__cplusplus) 
} 
#endif 

#endif /* _MLIB_DIV1Q_A32_ASM_H_ */
