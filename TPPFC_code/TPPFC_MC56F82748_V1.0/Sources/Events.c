/*
* Copyright 2021 NXP
* SPDX-License-Identifier: BSD-3-Clause
*/

/* ###################################################################
**     Filename    : Events.c
**     Project     : TPPFC_82748_V0
**     Processor   : MC56F82748VLH
**     Component   : Events
**     Version     : Driver 01.03
**     Compiler    : CodeWarrior DSP C Compiler
**     Date/Time   : 2016-12-16, 19:44, # CodeGen: 30
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.03
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Init_Config.h"
#include "Cpu.h"
#include "Events.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "PFC_statemachine.h"
#include "common_func.h"
/******************************************************************************
* Local functions
******************************************************************************/
void Zerocross_reconfig();

/***************************************************************************//*!
*
* @brief   zero-cross event handling  
*
* @param   none
*
* @return  none
*
******************************************************************************/
#pragma section CODES_IN_RAM begin
#pragma interrupt called
void Zerocross_reconfig()
{
	gsPFC_Drive.sFlag.Pol = gsPFC_Drive.sFlag.PolTemp;
	if(gsPFC_Drive.sFlag.Pol)
	{
		DISABLE_DOWNGAN(); /* Disable master switch of the first switching period to restrain current peak */
		SETHIGHERMOSFET_LOW; /* Disable slave MOS */
	}
	else
	{
		DISABLE_UPGAN();
		SETLOWERMOSFET_LOW;
	}
	
	if(gsPFC_Drive.sFlag.FisrtZerocrossingDetect) 
	{
		/* zero-crossing soft-start is disabled at startup to avoid current pulse, because the voltage starting point is unknown  */
		gsPFC_Drive.sFlag.Ensoftzero = 1; 
		/* discard the first zero-crossing to avoid wrong rms value */
		gsPFC_Drive.sVacTest.u16HalfCycleCntforMetering = 0;
		gsPFC_Drive.sIacTest.f32IacSquareSum = 0;
		gsPFC_Drive.sVacTest.f32VacSquareSum = 0;
		gsPFC_Drive.f32InstPowerSum = 0;
		gsPFC_Drive.sFlag.FisrtZerocrossingDetect = 0;
	}
	else
	{	
		if(gsPFC_Ctrl.eState == RUN)
		{
			/* used to re-enable vac jump correction in the first zero-crossing after ac drop recovery*/					        
			gsPFC_Drive.sFlag.Startup = 1;
		}
		
		/* enable low&high sync switches and power metering */
		gsPFC_Drive.sFlag.EnLowFreSync = 1;
		gsPFC_Drive.sFlag.EnHighFreSync = 1;
		gsPFC_Drive.sFlag.EnMetering = 1;
		gsPFC_Drive.sVacTest.u16PeriodCnt = gsPFC_Drive.sVacTest.u16PointCnt; /* track the real-time frequency of input voltage */
	}
	
	/* get current sampling offset when current is zero */
	gsPFC_Drive.sIinCtrl.f16IsnsOffset = gsPFC_Drive.sIinCtrl.f16out;
	gsPFC_Drive.sVacTest.u16PointCnt = 0;
	gsPFC_Drive.sVacTest.u16PolChangeConfirmCnt = 0;
	gsPFC_Drive.sFlag.Zerocross = 1;
	
	/* set dutycycle = 0, make both master and synchronous GAN stay low in the first period after zero cross */
	DUTY_UPDATE(0); 
	ENABLE_DUTYLOAD; // enable pwm value reload
}
#pragma interrupt off
#pragma section CODES_IN_RAM end

#pragma inline_max_size( 3000 )

#pragma section CODES_IN_RAM begin
#pragma interrupt saveall
void Pwm0_ISR(void) //80KHz
{		
	TP9_UP();
	gsPFC_Drive.sVacTest.u16PointCnt++;
	
/*========================= adjust PI parameters according to input voltage =========================*/
	if(gsPFC_Drive.sVacTest.f16VacRec < gsPFC_Drive.sVacTest.f16VacLowTh) // different PI when vin is low for zero-crossing soft start
	{
		gsPFC_Drive.sIinCtrl.sPIpAWParams.a32PGain = ISNS_KP_GAIN_LVIN;
		gsPFC_Drive.sIinCtrl.sPIpAWParams.a32IGain = ISNS_KI_GAIN_LVIN;
	}
	else
	{
		gsPFC_Drive.sIinCtrl.sPIpAWParams.a32PGain = gsPFC_Drive.sIinCtrl.a32HVinkp;
		gsPFC_Drive.sIinCtrl.sPIpAWParams.a32IGain = gsPFC_Drive.sIinCtrl.a32HVinki;
	}

/*====== use previous sampled ac input for phase lock to ensure sufficient loop calculation time after sampling =======*/
#if SPLL_EN 
	if(gsPFC_Drive.sVacTest.u16VrmsDoneCnt != 0 && gsPFC_Drive.sFlag.Pol == 1)
	{
		gsPFC_Drive.sFlag.SPLLStart=1;
	}
	if(gsPFC_Drive.sFlag.SPLLStart)
	{
		SPLL_1PH_SOGI_Run(&spll_obj, (frac32_t)gsPFC_Drive.sVacTest.f16Vac<<16);
	}
#endif

/*================= begin to read the polarity after about a quarter of period =====================*/
	if((gsPFC_Drive.sVacTest.u16PointCnt >= MIN_QUARTER_CYCLE_POINT_COUNTER)||(gsPFC_Drive.sFlag.FisrtZerocrossingDetect)) 
	{
#if BOARD_TEST
		if(gsPFC_Drive.sFlag.Pol)   gsPFC_Drive.sFlag.PolTemp  = 0;
		else gsPFC_Drive.sFlag.PolTemp = 1;		
		Zerocross_reconfig();
#else
		POLARITY(gsPFC_Drive.sFlag.PolTemp);
		if(gsPFC_Drive.sFlag.PolTemp != gsPFC_Drive.sFlag.Pol) 
		{		
		    gsPFC_Drive.sVacTest.u16PolChangeConfirmCnt++;
		    /* if detected polarity is different from previous polarity for several consecutive times, a zero-cross event happened. */
			if(gsPFC_Drive.sVacTest.u16PolChangeConfirmCnt >= VAC_POLARITY_CONFIRM_NUM)   Zerocross_reconfig();	
		}
		else  gsPFC_Drive.sVacTest.u16PolChangeConfirmCnt = 0;
#endif
	}
	
/*========================= Read input voltage and current sampling result =========================*/
	while(!(ADC_RDY & 0x1)) {;}
	gsPFC_Drive.sVacTest.f16Vac = ADC_RSLT0;
	gsPFC_Drive.sVacTest.f16Vac = GDFLIB_FilterIIR1_F16(gsPFC_Drive.sVacTest.f16Vac,&gsPFC_Drive.sVacTest.sFilterParam);
	gsPFC_Drive.sVacTest.f16Vac = MLIB_Sub_F16(gsPFC_Drive.sVacTest.f16Vac, FRAC16(0.5));
	gsPFC_Drive.sVacTest.f16VacRec = MLIB_Abs_F16(gsPFC_Drive.sVacTest.f16Vac);	

	#if ISNS_HALL
	gsPFC_Drive.sIinCtrl.f16out = ADC_RSLT8; 
	gsPFC_Drive.sIinCtrl.f16IsnsRec = gsPFC_Drive.sFlag.Pol? gsPFC_Drive.sIinCtrl.f16out:(-gsPFC_Drive.sIinCtrl.f16out);
	#endif
	
/*================================== Current controller =================================*/	
	/* skip controller at zero cross point, PWM value have been updated in zero-crossing function */
	if((gsPFC_Drive.sFlag.PWM_enable) && (gsPFC_Drive.sFlag.PolLast == gsPFC_Drive.sFlag.Pol))
	{
		if(gsPFC_Drive.sVacTest.u16PointCnt < ZERO_CROSS_SOFT_START_PWM_CYCLES)
	   	{
	   		gsPFC_Drive.sIinCtrl.f16outError = 0; /* no regulation at zero-crossing soft-start interval */
	   	}
	   	else 
	   	{   			  			  			
         #if SPLL_EN
	   		/* Iref = (vout_pi*sqrt(2)*sin)/vrms */ 
	   		gsPFC_Drive.sIinCtrl.f16outRef = MLIB_Abs_F16(MLIB_MulSat_F16as(gsPFC_Drive.a32IrefReq, spll_obj.f16PLLoutsine)); 
	     #else
	   		/* Iref = (vout_pi*vac_rec)/vrms^2 */
	   		gsPFC_Drive.sIinCtrl.f16outRef = MLIB_MulSat_F16as(gsPFC_Drive.a32IrefReq, gsPFC_Drive.sVacTest.f16VacRec); 
	   	#endif
	   	    /* a32IrefReq is limited with last input voltage RMS, adding extra limit to prevent over-current during voltage jump */
	   		gsPFC_Drive.sIinCtrl.f16outRef = GFLIB_Limit_F16(gsPFC_Drive.sIinCtrl.f16outRef, 0, gsPFC_Drive.sIinCtrl.f16RefHlim);
	      	gsPFC_Drive.sIinCtrl.f16outError = MLIB_Sub_F16(gsPFC_Drive.sIinCtrl.f16outRef, gsPFC_Drive.sIinCtrl.f16IsnsRec);
	   	    gsPFC_Drive.sIinCtrl.f16outError = GFLIB_Limit_F16(gsPFC_Drive.sIinCtrl.f16outError,-ISNS_LOOP_ERROR_LIMIT,ISNS_LOOP_ERROR_LIMIT);
	   	}
	    gsPFC_Drive.sIinCtrl.f16PIResult = GFLIB_CtrlPIpAW_F16(gsPFC_Drive.sIinCtrl.f16outError, &gsPFC_Drive.sIinCtrl.StopIntegFlag, &gsPFC_Drive.sIinCtrl.sPIpAWParams);
	    gsPFC_Drive.f16Duty = gsPFC_Drive.sIinCtrl.f16PIResult >> 5; /* to cooperate with nano-edge placement, the lowest 5 bits are used as the decimal parts of duty */
	  		
	   	//gsPFC_Drive.f16Duty = GFLIB_Limit_F16(gsPFC_Drive.f16Duty, DUTY_LLIMIT, DUTY_HLIMIT);
	   	PFC_PWM_UPDATE(gsPFC_Drive.sVacTest.u16PointCnt,gsPFC_Drive.f16Duty);
	}
	gsPFC_Drive.sFlag.PolLast = gsPFC_Drive.sFlag.Pol;
	TP9_DOWN();
	
	/* Clear flag */
	PWMA_SM0STS |= PWM_STS_CMPF(1);
}
#pragma interrupt off
#pragma section CODES_IN_RAM end

#pragma section CODES_IN_RAM begin
#pragma interrupt saveall
void Pwm1_ISR(void) //10KHz
{	
	frac32_t f32ActivePowertemp;
	bool_t  bSkipVolLoop = 0;
	
	TP11_UP();
	/*================================== time base counter =================================*/	
	gsPFC_Drive.sVacTest.u16HalfCycleCntforMetering++; /* counter for input voltage and current RMS calculation */
	gu16MsgIntervalCnt++; /* a counter to record the time interval of two messages, T=0.1ms */
	gu16TimeBaseCnt++; /* counter for time base */
		
#if !BOARD_TEST
	if(gsPFC_Ctrl.eState == RUN)
	{
		Check_AC_drop(&gsPFC_Drive);
	}
#endif
	
#if VACJUMP_CORRECTION_EN
	if(gsPFC_Drive.sFlag.Startup)
	{
		Vac_jump_correction(&gsPFC_Drive);
	}
#endif
	
	/*======================== read output voltage sampling result =================================*/	
	gsPFC_Drive.sVoutCtrl.f16out = ADC_RSLT1;
	gsPFC_Drive.sVoutCtrl.f16outFilt = GDFLIB_FilterIIR1_F16(gsPFC_Drive.sVoutCtrl.f16out,&gsPFC_Drive.sVoutCtrl.sFilterParam);

	/*======================================= metering ============================================*/
	/* input current rms calculate */
	gsPFC_Drive.sIacTest.f16IacRec = MLIB_Abs_F16(gsPFC_Drive.sIinCtrl.f16out);
	gsPFC_Drive.sIacTest.f32IacSquare = MLIB_Mul_F32ss(gsPFC_Drive.sIacTest.f16IacRec, gsPFC_Drive.sIacTest.f16IacRec) >> 6; /* prevent overflow */
	gsPFC_Drive.sIacTest.f32IacSquareSum += gsPFC_Drive.sIacTest.f32IacSquare;
	
	/* input voltage rms calculate */
	gsPFC_Drive.sVacTest.f32VacSquare = MLIB_Mul_F32ss(gsPFC_Drive.sVacTest.f16VacRec, gsPFC_Drive.sVacTest.f16VacRec)>> 7; /* prevent overflow */
	gsPFC_Drive.sVacTest.f32VacSquareSum += gsPFC_Drive.sVacTest.f32VacSquare;
	
	/* input power calculate */
	gsPFC_Drive.f32InstPower = MLIB_Mul_F32ss(gsPFC_Drive.sVacTest.f16VacRec, gsPFC_Drive.sIacTest.f16IacRec);
	gsPFC_Drive.f32InstPowerSum += gsPFC_Drive.f32InstPower;		
	
	if(gsPFC_Drive.sFlag.EnMetering) /* zero-cross event occur */
	{
		gsPFC_Drive.sVacTest.f32VrmsSquare = Div_int_ll(gsPFC_Drive.sVacTest.f32VacSquareSum, gsPFC_Drive.sVacTest.u16HalfCycleCntforMetering);
		gsPFC_Drive.sVacTest.f32VrmsSquare = gsPFC_Drive.sVacTest.f32VrmsSquare << 7;
		gsPFC_Drive.sVacTest.f16VacRms = GFLIB_Sqrt_F16l(gsPFC_Drive.sVacTest.f32VrmsSquare);
		gsPFC_Drive.sVacTest.f32VacSquareSum = 0;	
		
		/* update voltage controller output limit, that is, the current reference limit, according to last RMS */
		gsPFC_Drive.sVoutCtrl.sPIpAWParams.f16LowerLim = MLIB_Mul_F16(FRAC16(LOW_CURRENT*LOOP_SCALE), gsPFC_Drive.sVacTest.f16VacRms);
		gsPFC_Drive.sVoutCtrl.sPIpAWParams.f16UpperLim = MLIB_Mul_F16(FRAC16(HIGH_CURRENT*LOOP_SCALE), gsPFC_Drive.sVacTest.f16VacRms);
				
		/* update current controller parameters, according to last RMS */
		if(gsPFC_Drive.sVacTest.f16VacRms < VRMS_LOW_TH)
		{
			gsPFC_Drive.sVacTest.f16VacLowTh = FRAC16(VAC_LOWRMS_LOWTH/VAC_SCALE);
			gsPFC_Drive.sIinCtrl.a32HVinkp = ISNS_KP_GAIN_LVRMS;
			gsPFC_Drive.sIinCtrl.a32HVinki = ISNS_KI_GAIN_LVRMS;	  
		}
		else if(gsPFC_Drive.sVacTest.f16VacRms > VRMS_HIGH_TH)
		{
			gsPFC_Drive.sVacTest.f16VacLowTh = FRAC16(VAC_HIGHRMS_LOWTH/VAC_SCALE);
			gsPFC_Drive.sIinCtrl.a32HVinkp = ISNS_KP_GAIN_HVRMS;
			gsPFC_Drive.sIinCtrl.a32HVinki = ISNS_KI_GAIN_HVRMS;		  
		}
		
		gsPFC_Drive.sIacTest.f32IrmsSquare = Div_int_ll(gsPFC_Drive.sIacTest.f32IacSquareSum, gsPFC_Drive.sVacTest.u16HalfCycleCntforMetering);
		gsPFC_Drive.sIacTest.f32IrmsSquare = gsPFC_Drive.sIacTest.f32IrmsSquare << 6;
		gsPFC_Drive.sIacTest.f16IacRms = GFLIB_Sqrt_F16l(gsPFC_Drive.sIacTest.f32IrmsSquare);
		gsPFC_Drive.sIacTest.f32IacSquareSum = 0;
		
		f32ActivePowertemp = Div_int_ll(gsPFC_Drive.f32InstPowerSum, gsPFC_Drive.sVacTest.u16HalfCycleCntforMetering);
		gsPFC_Drive.f16ActivePower = f32ActivePowertemp >> 16;
		gsPFC_Drive.f32InstPowerSum = 0;
		gsPFC_Drive.f16ApparentPower = MLIB_Mul_F16(gsPFC_Drive.sIacTest.f16IacRms, gsPFC_Drive.sVacTest.f16VacRms);
		gsPFC_Drive.f16PowerFactor =  MLIB_DivSat_F16(gsPFC_Drive.f16ActivePower, gsPFC_Drive.f16ApparentPower);
		
		gsPFC_Drive.sFlag.EnMetering = 0;
		bSkipVolLoop = 1; //skip voltage controller in first cycle
		gsPFC_Drive.sVacTest.u16HalfCycleCntforMetering = 0;
		
		if(gsPFC_Drive.sVacTest.u16VrmsDoneCnt < INPUT_VRMS_DET_NUM)
		{
			gsPFC_Drive.sVacTest.u16VrmsDoneCnt++;
			if(gsPFC_Drive.sVacTest.u16VrmsDoneCnt == INPUT_VRMS_DET_NUM)
			{
				gsPFC_Drive.sVacTest.bSeveralVrmsDetectionFlag = 1;
			}
		}
		
        #if SPLL_EN
		/* update SPLL SOGI coefficient according to the real-time grid frequency */
        spll_obj.a32NominalFreq = MLIB_Div1Q_A32ll(80000>>1, gsPFC_Drive.sVacTest.u16PeriodCnt);
        /* PLL control frequency/2 is used because it's out of Q16.15 range */
        SPLL_1PH_SOGI_PDCoeffConfig(&spll_obj,spll_obj.a32NominalFreq,ACC32(SPLL_CTRL_FREQ/2),ACC32(SOGI_K_COEFF));
		#endif
	}
	
	/*====================================== run sub-state =======================================*/
	if(gsPFC_Ctrl.eState == RUN)
	{	
		mPFC_STATE_RUN_TABLE[gsPFC_Runsub]();
	}
	
	/*===================================== voltage controller ==================================*/
	if((gsPFC_Drive.sFlag.PWM_enable) && (!bSkipVolLoop))
	{
		if(gsPFC_Runsub == LIGHTLOAD) /* fixed current reference is used in light load mode */
		{
		    gsPFC_Drive.sVoutCtrl.f16UDcCtrlout = gsPFC_Drive.sVoutCtrl.sPIpAWParams.f16LowerLim;
		}
		else
		{
			gsPFC_Drive.sVoutCtrl.f16outError = MLIB_Sub_F16(gsPFC_Drive.sVoutCtrl.f16outRef, gsPFC_Drive.sVoutCtrl.f16outFilt); 
			gsPFC_Drive.sVoutCtrl.f16outError = GFLIB_Limit_F16(gsPFC_Drive.sVoutCtrl.f16outError, -VOL_LOOP_ERROR_LIMIT, VOL_LOOP_ERROR_LIMIT);
			gsPFC_Drive.sVoutCtrl.f16PIResult = GFLIB_CtrlPIpAW_F16(gsPFC_Drive.sVoutCtrl.f16outError, &gsPFC_Drive.sVoutCtrl.StopIntegFlag, &gsPFC_Drive.sVoutCtrl.sPIpAWParams);
			gsPFC_Drive.sVoutCtrl.f16UDcCtrlout = gsPFC_Drive.sVoutCtrl.f16PIResult;
		}
#if SPLL_EN == 0
		gsPFC_Drive.a32IrefReq = MLIB_Div1Q_A32ll((frac32_t)gsPFC_Drive.sVoutCtrl.f16UDcCtrlout << 16, gsPFC_Drive.sVacTest.f32VrmsSquare);
#else SPLL_EN
		gsPFC_Drive.a32IrefReq = MLIB_Div1Q_A32ss(gsPFC_Drive.sVoutCtrl.f16UDcCtrlout, gsPFC_Drive.sVacTest.f16VacRms);
		gsPFC_Drive.a32IrefReq = MLIB_Mul_A32(gsPFC_Drive.a32IrefReq, ACC32(1.4142));
#endif
	}
	else
	{
		bSkipVolLoop = 0;
	}	 
	
	FMSTR_Recorder();
	
	TP11_DOWN();
	
	/* Clear flag */
	PWMA_SM1STS |= PWM_STS_CMPF(1);	
}
#pragma interrupt off
#pragma section CODES_IN_RAM end

/* END Events */

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
