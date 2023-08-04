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
* @brief  Arcus tangent function
* 
*******************************************************************************/
#ifndef _GFLIB_ATAN_F32_ASM_H_
#define _GFLIB_ATAN_F32_ASM_H_

#if defined(__cplusplus) 
extern "C" { 
#endif 
/******************************************************************************
* Includes
******************************************************************************/
#include "mlib.h"
#include "gflib_types.h"

/******************************************************************************
* Constants
******************************************************************************/

/******************************************************************************
* Macros 
******************************************************************************/

/******************************************************************************
* Types
******************************************************************************/
typedef struct
{
	frac32_t f32A[3];
} GFLIB_ATAN_COEF_T_F32;

typedef struct
{
	GFLIB_ATAN_COEF_T_F32 sAtanSector[8];
} GFLIB_ATAN_T_F32;

/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Global functions
******************************************************************************/

extern asm frac16_t GFLIB_Atan_F16_FAsm(frac16_t f16Angle, const GFLIB_ATAN_T_F32 *psParam);

/******************************************************************************
* Inline functions
******************************************************************************/
#if defined(__cplusplus) 
} 
#endif 

#endif /* _GFLIB_ATAN_F32_ASM_H_ */
