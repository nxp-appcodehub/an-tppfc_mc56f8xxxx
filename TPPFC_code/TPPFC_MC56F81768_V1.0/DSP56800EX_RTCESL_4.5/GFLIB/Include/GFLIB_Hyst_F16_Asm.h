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
* @brief  Hysteresis function 
* 
*******************************************************************************/
#ifndef _GFLIB_HYST_F16_ASM_H_
#define _GFLIB_HYST_F16_ASM_H_

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
#define GFLIB_Hyst_F16_Asmi(f16Val, psParam) GFLIB_Hyst_F16_FAsmi(f16Val, psParam)

/******************************************************************************
* Types
******************************************************************************/
typedef struct
{
	frac16_t f16HystOn;
    frac16_t f16HystOff;
    frac16_t f16OutValOn;
    frac16_t f16OutValOff;  
    frac16_t f16OutState;
}GFLIB_HYST_T_F16;

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
* @brief  Hysteresis function
*
* @param  ptr			GFLIB_HYST_T_F16 *psParam
*						  - f16HystOn: Upper threshold
*						  - f16HystOff: Lower threshold
*						  - f16OutValOn: Output when input is above f16HystOn
*						  - f16OutValOff: Output when input is below f16HystOff
*						  - f16OutState: Output of the function
*						  
* @param  in    		frac16_t f16Val
*                         - Argument in [-1;1] in frac16_t
*
* @return N/A
*		
* @remarks 	The function represents a hysteresis (or relay) function. The
* 			function switches output between the two predefined values. When
*			the input is higher than upper threshold f16HystOn, the output
*			is high; when the input is below another (lower) threshold
*			f16HystOff, the output is low; when the input is between the two,
*			the output retains its value.
*
*			SATURATION INDEPENDENT!
*
****************************************************************************/
extern inline frac16_t GFLIB_Hyst_F16_FAsmi(register frac16_t f16Val, register GFLIB_HYST_T_F16 *psParam)
{
	register frac16_t f16Temp, f16Out;
	
	asm(.optimize_iasm on);
	
	asm(move.w #0,A);					/* A = 0 */
	asm(move.w #1,B);					/* B = 0x10000 */
	asm(move.w X:(psParam+1),f16Out);	/* f16Out = f16HystOff */
	asm(cmp f16Val,f16Out);				/* Compares f16Val with f16HystOff */
	asm(tlt B,A);                       /* If f16Val > f16HystOff, then A = 0x10000 */	
	
	asm(move.w #0,B);					/* B = 0 */
	asm(move.w X:(psParam+4),f16Out);	/* f16Out = f16OutState */
	asm(move.w X:(psParam+2),f16Temp);	/* f16Temp = f16OutValOn */
	asm(cmp f16Temp,f16Out);			/* Compares f16OutState with f16OutValOn */
	asm(teq A,B);                       /* If f16OutState = f16OutValOn, then B = A */		
	
	asm(move.w X:(psParam+3),A);		/* A = f16OutValOff */
	asm(tst B);							/* Compares B with 0 */
	asm(tne f16Temp,A);					/* If B != 0, then A = f16OutValOn */
	
	asm(move.w X:(psParam),f16Out);		/* f16Out = f16HystOn */
	asm(cmp f16Val,f16Out);				/* Compares f16Val with f16HystOn */
	asm(tlt f16Temp,A);					/* If f16Val > f16HystOn, then A = f16OutValOn */
	
	asm(move.w A,X:(psParam+4));		/* Save output */
	asm(move.w A,f16Out);				/* Save output */

	asm(.optimize_iasm off);
	
	return (f16Out);
}

#if defined(__cplusplus) 
} 
#endif 

#endif /* _GFLIB_HYST_F16_ASM_H_ */
