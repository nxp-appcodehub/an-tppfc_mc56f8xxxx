/*
* Copyright 2021 NXP
* SPDX-License-Identifier: BSD-3-Clause
*/

/*
 * common_func.h
 *
 *  Created on: Jan 26, 2021
 *      Author: nxa16823
 */

#ifndef COMMON_FUNC_H_
#define COMMON_FUNC_H_

#include "PFC_statemachine.h"
#include "PFC_Ctrl.h"

static inline void PFC_PWM_UPDATE(uint16_t counter,frac16_t Dutycycle)
{
	frac16_t f16duty_temp;
	
	f16duty_temp = Dutycycle;
	if((counter < ZERO_CROSS_SOFT_START_PWM_CYCLES) && gsPFC_Drive.sFlag.Ensoftzero)
	{
		f16duty_temp = (counter << 5); /* duty cycle for zero cross soft start, max = 9*32= 288 < 624 */
	}
	
	if(gsPFC_Drive.sFlag.Pol)
	{
		
		/* change high frequency arm switches operation after zero crossing */
		if(counter <= 3)
		{
			DISABLE_UPGAN(); /* Disable Slave GaN */
		}
		RESETOUTPUTPOLARITY; /* cooperate with the exchanged master switch, PWM outputs don't invert */
		PWMFAULTSTATE_0; /* cooperate with the output invert, change fault state */
		ENABLE_DOWNGAN(); /* Enable Master GaN, ensure the tube can be turned on in all cases  */
		
		/* close all sync tubes before zero crossing */
		if((gsPFC_Drive.sVacTest.u16PointCnt > MIN_QUARTER_CYCLE_POINT_COUNTER) && (gsPFC_Drive.sVacTest.f16VacRec < gsPFC_Drive.sVacTest.f16VacLowTh))
		{
			SETLOWERMOSFET_LOW;
			DISABLE_UPGAN();
		}
		
		/* When reverse current occurs, disable all sync tubes */
		if((gsPFC_Drive.sIinCtrl.f16out < -TURNOFF_SYNC_REVERSE_CURR))
		{
			SETLOWERMOSFET_LOW;
			DISABLE_UPGAN();
			/* go to fault state when reverse current exceeds limit */
			if(gsPFC_Drive.sIinCtrl.f16out < -TURNOFF_CONVERTER_REVERSE_CURR)
			{
				FAULT_SET(gsPFC_Drive.f16FaultIDPending, CURRENTDISTORTION);
				gf16SwRevDistoritonCur = gsPFC_Drive.sIinCtrl.f16out;
			}
		}
	}
	else
	{
		if(counter <= 3)
		{
			DISABLE_DOWNGAN();
			INVERTOUTPUTPOLARITY;
			PWMFAULTSTATE_1;
			ENABLE_UPGAN();
		}
		if((gsPFC_Drive.sVacTest.u16PointCnt > MIN_QUARTER_CYCLE_POINT_COUNTER) && (gsPFC_Drive.sVacTest.f16VacRec < gsPFC_Drive.sVacTest.f16VacLowTh))
		{
			SETHIGHERMOSFET_LOW;
			DISABLE_DOWNGAN();
		}
		//When input voltage decrease to a threshold or current distortion happens, disable synchronous rectification
		if(gsPFC_Drive.sIinCtrl.f16out > TURNOFF_SYNC_REVERSE_CURR)
		{
			SETHIGHERMOSFET_LOW;
			DISABLE_DOWNGAN();
			if(gsPFC_Drive.sIinCtrl.f16out > TURNOFF_CONVERTER_REVERSE_CURR)
			{
				FAULT_SET(gsPFC_Drive.f16FaultIDPending, CURRENTDISTORTION);
				gf16SwRevDistoritonCur = gsPFC_Drive.sIinCtrl.f16out;
			}
		}
	}

/*========================= adjust sampling point ========================= */	
	if(f16duty_temp > 125)//when duty is big enough, for example over 20%, trigger point is set around rising middle to avoid disturbance by hardware switch
	{
		PWMA_SM2VAL0 =20;
	}
	else if(f16duty_temp < 93)//when duty is small enough, for example below 15%, trigger point is set around falling middle to avoid disturbance by hardware switch
	{		
		PWMA_SM2VAL0 = -293;
	}
	
	PWMA_SM0FRACVAL3 = ((uint16_t)gsPFC_Drive.sIinCtrl.f16PIResult & 0x1F) << 11;
#if BOARD_TEST
	f16duty_temp = DUTY_OPEN_LOOP;
#endif	
    DUTY_UPDATE(f16duty_temp);//gf16Duty_temp is the duty value to down GaN MOS
    ENABLE_DUTYLOAD; /* load new values to PWM registers, MCTRL[LDOK] is automatically cleared after the new values are loaded */ 						
}

static inline long Div_int_ll(long register l_numerator, long register l_denominator)
{
                long register w32ClbDenom;
                short register w16LeadBits; 
                // A is l_numerator, B is l_denominator
                
                asm(.optimize_iasm on);
                asm( tfr l_numerator, A);
                asm( tfr l_denominator, B);
                
                asm( tfr B, Y);                // w32ClbDenom is for normalized denominator
                asm( clb Y, w16LeadBits);      // count leading bits of denominator
                asm( asll.l w16LeadBits, Y);   // normalize the denominator
                asm( sub Y, A);                // l_numerator - w32ClbDenom
                asm( bftstl #$8, A2); // set Carry if positive
                // divide loop
                asm( rep w16LeadBits);
                asm( div Y, A);
                asm( rol.l A);      // now low "w16LeadBits+1" bits of A contains the quotient
                // mask
                asm( eor.l Y, Y); // clear w32ClbDenom
                asm( add.l #2, Y);
                asm( asll.l w16LeadBits,Y);
                asm( dec.l Y); // w32ClbDenom contains the mask
                asm( and.l Y, A); // apply the mask
                asm( tfr A, l_numerator);
                asm(.optimize_iasm off);
                return l_numerator;
}

static inline void Get_ISNS_ADC_offset(void)
{
	/* The preset offset is 1.65V, if the detected current is less than the OFFSET_ERROR_LIMIT when current is zero,
	 * update the offset. Otherwise, the sampling is considered wrong and the offset is not updated.*/
	if((MLIB_Abs_F16(gsPFC_Drive.sIinCtrl.f16IsnsOffset)) < OFFSET_ERROR_LIMIT)
	{ADC_OFFST8 += gsPFC_Drive.sIinCtrl.f16IsnsOffset;}
}

static inline void Vac_jump_correction(PFC_STRUC_T *ptr)
{
	frac16_t f16Amplitude1, f16Amplitude2, f16Error_Amplitude;
	
	/* input voltage phase calculation */
	gsPFC_Drive.sVacTest.f16VacPhase = MLIB_DivSat_F16(ptr->sVacTest.u16PointCnt, ptr->sVacTest.u16PeriodCnt);
	
	/* the voltage value corresponding to this angle in previous period */
	f16Amplitude1 = MLIB_Mul_F16(GFLIB_Sin_F16(gsPFC_Drive.sVacTest.f16VacPhase), ptr->sVacTest.f16VacRms);
	f16Amplitude2 = MLIB_Mul_F16as(ACC32(1.41421356237), f16Amplitude1);
	f16Error_Amplitude = MLIB_Sub_F16(ptr->sVacTest.f16VacRec, f16Amplitude2);
	
	/* enable voltage loop correction only when input voltage change is greater than VAC_CHANGTHRESHOLD*/
	if((MLIB_Abs_F16(f16Error_Amplitude) > VAC_CHANGTHRESHOLD) && (ptr->sFlag.EnVolLoopCor == 1))
	{
		/* decrease voltage PI integrator (current reference) when input voltage increase and output exceeds reference */
		if((f16Error_Amplitude > 0) && (ptr->sVoutCtrl.f16outFilt > ptr->sVoutCtrl.sVoutRamp.f16Target))
		{
			ptr->sVoutCtrl.sPIpAWParams.f32IAccK_1 -= 409*(ptr->sVoutCtrl.f16outFilt - ptr->sVoutCtrl.sVoutRamp.f16Target)*F16K1;
		}
		/* increase voltage PI integrator (current reference) when input voltage decrease and output is below reference */
		else if((f16Error_Amplitude < 0) && (ptr->sVoutCtrl.f16outFilt < ptr->sVoutCtrl.sVoutRamp.f16Target))
		{
			ptr->sVoutCtrl.sPIpAWParams.f32IAccK_1 += 409*(ptr->sVoutCtrl.sVoutRamp.f16Target - ptr->sVoutCtrl.f16outFilt)*F16K2;
		}
		
		/* voltage loop correction will be disabled after a defined maximum duration  */
		ptr->sVacTest.u16VacJumpCorCnt++;
		if(ptr->sVacTest.u16VacJumpCorCnt >= VAC_JUMP_COR_EN_DURATION)
		{
			ptr->sFlag.EnVolLoopCor = 0;
			ptr->sVacTest.u16VacJumpCorCnt = 0;
			ptr->sVacTest.u16VacJumpCorDisCnt = 0;
		}
	}
	
	/* voltage loop correction will not be enabled again a defined time pass away */
	if(ptr->sVacTest.u16VacJumpCorDisCnt < VAC_JUMP_COR_DIS_DURATION)
	{
		ptr->sVacTest.u16VacJumpCorDisCnt++;	
	}
	else
	{
		ptr->sFlag.EnVolLoopCor = 1;
	}
}

static inline void Check_AC_drop(PFC_STRUC_T *ptr)
{
	if(!ptr->sFlag.AcDrop)
	{
	    if(ptr->sVacTest.f16VacRec < ptr->sVacTest.f16VacLackTh)
	    {
	    	ptr->sVacTest.u16VacFailCnt++;
	    	
	    	/* if the input voltage sampling value is less than the power-down threshold for 10 
	    	 * consecutive times, the input voltage is considered power down. */ 
	        if(ptr->sVacTest.u16VacFailCnt == VAC_FAIL_CONFIRM_NUM)
	        {
	        	/* Disable all switches, but the converter stays in RUN mode to ensure fast recovery. */
	        	ptr->sFlag.PWM_enable = 0;
	            DISABLE_ALL_GAN();
	            DISABLE_ALLMOSFET();
	    	    ptr->sVacTest.u16VacRecoveryOkCnt = 0;
	    	    
	    	    /* disable polarity detection and clear polChangeConfirmCnt status
	    	     * u16PointCnt is cleared every slow loop when ac drop, so no zero crossing will be detected before ac recovery */
	    	    ptr->sFlag.FisrtZerocrossingDetect = 0;
	    	    ptr->sVacTest.u16PointCnt = 0;
	    	    ptr->sVacTest.u16PolChangeConfirmCnt = 0;
	    	    ptr->sFlag.EnMetering = 0;
	    	    
	    	    /* disable vac jump correction when ac drop and the new recovery voltage RMS is unknown */
	    	    ptr->sFlag.Startup = 0;
	    	    ptr->sFlag.EnVolLoopCor = 1;
	    	    ptr->sVacTest.u16VacJumpCorDisCnt = 0;
	    	    
	    	    ptr->sFlag.AcDrop = 1;
		    }
	    }
	    else ptr->sVacTest.u16VacFailCnt = 0;
	}
	else 
	{
		/* open relay if Vout is less than predefined value for energy saving */
		if(ptr->sVoutCtrl.f16outFilt <= FRAC16(VDC_OPEN_RELAY_WHEN_ACDROP/VDC_SCALE))
		{
			OPEN_RELAY();
			ptr->sFlag.RelayFlag = 0;
		}
		
		if(ptr->sVacTest.f16VacRec >= ptr->sVacTest.f16VacRecoveryTh)
		{
			ptr->sVacTest.u16VacRecoveryOkCnt++;
			/* if the input voltage sampling value is larger than the ac recovery threshold for several 
			 * consecutive times, the input voltage is considered power up again. */ 
			if(ptr->sVacTest.u16VacRecoveryOkCnt == VAC_RECOVERY_CONFIRM_NUM)
			{
				/* get current polarity for below polarity confirmation */
				POLARITY(gsPFC_Drive.sFlag.PolTemp);
				
				ptr->sVacTest.u16VacFailCnt = 0;
				ptr->sFlag.AcDrop = 0;
				CLOSE_RELAY();
				ptr->sFlag.RelayFlag = 1;
			}
		}
		else  
		{
			ptr->sVacTest.u16VacRecoveryOkCnt = 0;
			ptr->sVacTest.u16PointCnt = 0; /* ensure no polarity detection (zero-crossing) when ac drop */
		}
	}
	
	/* after the input voltage is restored, start PFC converter again in the positive half cycle */
	if(ptr->sVacTest.u16VacRecoveryOkCnt == VAC_RECOVERY_CONFIRM_NUM)
	{
		
		/* confirm current polarity before enabling zero-crossing detection, now the polarity detection in fast is not working. */
		if(!ptr->sFlag.FisrtZerocrossingDetect)
		{
		    gsPFC_Drive.sFlag.PolLast = gsPFC_Drive.sFlag.PolTemp;
		    POLARITY(gsPFC_Drive.sFlag.PolTemp);
		    
		    /* accept the detected polarity as current polarity when several consecutive polarity detection results are the same. */
		    if(gsPFC_Drive.sFlag.PolTemp == gsPFC_Drive.sFlag.PolLast) 
		    {		
			    gsPFC_Drive.sVacTest.u16PolChangeConfirmCnt++;
			    if(gsPFC_Drive.sVacTest.u16PolChangeConfirmCnt >= VAC_POLARITY_CONFIRM_NUM)   
			    {
			    	/* enable fast loop polarity (zero crossing) detection,
			    	 * because of uncertain recovery point, set pol_detect flag to discard the first zero-crossing. */
			    	gsPFC_Drive.sFlag.Pol = gsPFC_Drive.sFlag.PolTemp;
			    	gsPFC_Drive.sVacTest.u16PolChangeConfirmCnt = 0;
			    	ptr->sFlag.FisrtZerocrossingDetect = 1;				    
			    }
		    }
		    else  gsPFC_Drive.sVacTest.u16PolChangeConfirmCnt = 0;
		}
		else
		{
		    if(ptr->sFlag.Pol)
		    {
		    /* uncertain recovery point, disable zero-crossing soft-start to avoid current surge 
		     * cause by high input voltage value when it is restored */ 
		        if(ptr->sFlag.FisrtZerocrossingDetect)
		        {
			        ptr->sFlag.Ensoftzero = 0;
		        }
		        
		        /* reset PI integrator, PFC converter recovery from small duty cycle to avoid current surge */
		        ptr->sVoutCtrl.sPIpAWParams.f32IAccK_1 = 0;
		        ptr->sIinCtrl.sPIpAWParams.f32IAccK_1 = 0;
		        /* reset point_counter because the main switch is enabled when point_counter <3 */ 
		        ptr->sVacTest.u16PointCnt = 0;
		        ptr->sFlag.PWM_enable = 1;
		        ptr->sVacTest.u16VacRecoveryOkCnt = 0;
		    }
		}
	}	
}

#endif /* COMMON_FUNC_H_ */
