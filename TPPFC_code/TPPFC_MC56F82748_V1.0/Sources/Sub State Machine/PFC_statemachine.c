/******************************************************************************
* 
* Copyright 2021 NXP
* SPDX-License-Identifier: BSD-3-Clause                      
*
******************************************************************************* 
*
* @file      PFC_statemachine.c
*
* @author    
* 
* @version   1.0.0.0

* @date      Dec-02-2016
* 
* @brief     PFC statemachines code
*
*******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/
#include "PFC_statemachine.h"
#include "common_func.h"

/******************************************************************************
* Global variables
******************************************************************************/
uint16_t             gu16TimeBaseCnt; /* comparison basis for time interval measurement */
uint16_t             gu16LLCRun = 0, gu16LLCVprimOK = 0, gu16LLCFaultID = 0; /* LLC information received through communication */

PFC_STRUC_T          gsPFC_Drive;
PFC_RUN_SUBSTATE_T   gsPFC_Runsub;
SPLL_1PH_SOGI_T      spll_obj;
/******************************************************************************
*Local variables
******************************************************************************/
bool_t     bPFC_Run; /* PFC run/stop command */
bool_t     bPFCVarInit = 0; /* PFC variables init status flag */
bool_t     bVolStableAfterRelayAction = 0; /* output voltage stable after relay action flag */

uint16_t   u16VprimToLLC; /* output voltage value for LLC through communication */
uint8_t    ucLLC_VprimH, ucLLC_VprimL;

uint16_t   u16VolRampUpCnt, u16DelayAftRelayCloseCnt, u16SoftStartStepCnt, u16FaultReleaseCnt; /* time interval measure count */ 
uint16_t   u16PWMOffDurCnt; /* a counter to record how many half cycles of PWM turn off at light load */ 
frac16_t   gf16SwOverCur, gf16SwOverVol, gf16SwRevDistoritonCur, gf16FreUnderCnt, gf16FreOverCnt; /* for test */

/******************************************************************************
*Local functions
******************************************************************************/
static void FaultDetection(void);
static bool_t TimeDelay(uint16_t count, uint16_t Delaytime);

/*------------------------------------
 * User state machine functions
 * ----------------------------------*/
static void PFC_StateFault(void);
static void PFC_StateInit(void);
static void PFC_StateStop(void);
static void PFC_StateRun(void);
/*------------------------------------
 * User state-transition functions
 * ----------------------------------*/
static void PFC_TransFaultInit(void);
static void PFC_TransInitFault(void);
static void PFC_TransInitStop(void);
static void PFC_TransInitRun(void);
static void PFC_TransStopFault(void);
static void PFC_TransStopRun(void);
static void PFC_TransRunFault(void);
static void PFC_TransRunStop(void);

/* State machine functions field (in pmem) */
__pmem static const SM_APP_STATE_FCN_T msSTATE = {PFC_StateFault, PFC_StateInit, PFC_StateStop, PFC_StateRun};


/* State-transition functions field (in pmem) */
__pmem static const SM_APP_TRANS_FCN_T msTRANS = {PFC_TransFaultInit, PFC_TransInitFault, PFC_TransInitStop,PFC_TransInitRun, PFC_TransStopFault, PFC_TransStopRun, PFC_TransRunFault, PFC_TransRunStop};

/* State machine structure declaration and initialization */
SM_APP_CTRL_T gsPFC_Ctrl = 
{
	/* gsM1_Ctrl.psState, User state functions  */
	&msSTATE,
 	
 	/* gsM1_Ctrl.psTrans, User state-transition functions */
 	&msTRANS,
 
  	/* gsM1_Ctrl.uiCtrl, Default no control command */
  	SM_CTRL_NONE,
  	
  	/* gsM1_Ctrl.eState, Default state after reset */
  	INIT 	
};

/*------------------------------------
 * User run sub-state machine functions
 * ----------------------------------*/
static void PFC_StateRun_Softstart(void);
static void PFC_StateRun_Normal(void);
static void PFC_StateRun_Lightload(void);

/*------------------------------------
 * User run sub-state-transition functions
 * ----------------------------------*/
static void PFC_TransRun_Softstart_Normal(void);
static void PFC_TransRun_Normal_Lightload(void);
static void PFC_TransRun_Lightload_Normal(void);

/* Sub-state machine functions field (in pmem)*/ 
__pmem const PFCN_VOID_VOID mPFC_STATE_RUN_TABLE[3] = 
{
	PFC_StateRun_Softstart, PFC_StateRun_Normal, PFC_StateRun_Lightload	
};


/***************************************************************************//*!
*
* @brief   fault detection function
*
* @param   none
*
* @return  none
*
******************************************************************************/
static void FaultDetection(void)
{
	/* clear pending faults except CURRENTDISTORTION before detecting them again */
	FAULT_CLEAR(gsPFC_Drive.f16FaultIDPending, 0xFFFF&(~CURRENTDISTORTION)); 
	 
	/* fault: output voltage detection */
	if(gsPFC_Drive.sVoutCtrl.f16outFilt > gsPFC_Drive.sFaultThresholds.f16UDcMax)
	{
		FAULT_SET(gsPFC_Drive.f16FaultIDPending, VOUT_OVER);
		gf16SwOverVol = gsPFC_Drive.sVoutCtrl.f16outFilt;
	}
	else if(gsPFC_Drive.sVoutCtrl.f16outFilt < gsPFC_Drive.sFaultThresholds.f16UDcMin)
	{
		FAULT_SET(gsPFC_Drive.f16FaultIDPending, VOUT_LACK);
	}
	 
	/* current and input frequency detection only works when converter is working */
	if(gsPFC_Ctrl.eState == RUN)
	{
		/* fault: input current detection */
		if(MLIB_Abs_F16(gsPFC_Drive.sIinCtrl.f16out) > gsPFC_Drive.sFaultThresholds.f16IinMax)
		{
			FAULT_SET(gsPFC_Drive.f16FaultIDPending, IIN_OVER);
			gf16SwOverCur = gsPFC_Drive.sIinCtrl.f16out;
		}
	 
	     /* fault: input frequency detection
	      * note: input frequency is not updated when ac drop, so no fault will be generated. */
	    if(gsPFC_Drive.sVacTest.u16PeriodCnt > PFC_VIN_FREQUNDER_CNT) 
	    {
	    	FAULT_SET(gsPFC_Drive.f16FaultIDPending, FREQ_UNDER);
		    gf16FreUnderCnt = gsPFC_Drive.sVacTest.u16PointCnt;
	    }
	    else if(gsPFC_Drive.sVacTest.u16PeriodCnt < PFC_VIN_FREQOVER_CNT)
	    {
	    	FAULT_CLEAR(gsPFC_Drive.f16FaultIDPending, FREQ_OVER); 
	    	gf16FreOverCnt = gsPFC_Drive.sVacTest.u16PointCnt;
	    }
	}
	 
    /* fault: input voltage detection
     * note: input voltage rms is not updated when ac drop, so no fault will be generated */
	if(gsPFC_Drive.sVacTest.bSeveralVrmsDetectionFlag)
	{
		if(gsPFC_Drive.sVacTest.f16VacRms < gsPFC_Drive.sFaultThresholds.f16VacMin)
		{
			FAULT_SET(gsPFC_Drive.f16FaultIDPending, VAC_LACK);
		}
		else if(gsPFC_Drive.sVacTest.f16VacRms > gsPFC_Drive.sFaultThresholds.f16VacMax)
		{
			FAULT_SET(gsPFC_Drive.f16FaultIDPending, VAC_OVER);
		}
	}
	
	/* hardware fault flag is not cleared here, so the system won't restart after a HW fault */
	/* hardware input current over fault */
	if(IIN_HW_FAULT())
	{
		FAULT_SET(gsPFC_Drive.f16FaultIDPending, IIN_HW_OVER);
	}
	/* hardware output voltage over fault */
	if(VOUT_HW_FAULT())
	{
		FAULT_SET(gsPFC_Drive.f16FaultIDPending, VOUT_HW_OVER);
	}
	
	/* received LLC fault information, stop PFC too */
	if(gsPFC_Drive.f16FaultIDPending > 0 || gu16LLCFaultID > 0) 
	{		
		gsPFC_Ctrl.uiCtrl |= SM_CTRL_FAULT;
		u16FaultReleaseCnt = gu16TimeBaseCnt;
	}
	
	/* pass faultIDPending to FaultID for recording */
	gsPFC_Drive.f16FaultID |= gsPFC_Drive.f16FaultIDPending;
}
 /***************************************************************************//*!
 *
 * @brief   Time Delay function
 *
 * @param   UInt16, UInt16
 *
 * @return  none
 *
 ******************************************************************************/
 static bool_t TimeDelay(uint16_t Currentcount, uint16_t Delaytime)
 {
 	uint16_t delta_t;
 	
 	/* time base accumulates in the slow loop, period 0.1ms */
 	if(Currentcount > gu16TimeBaseCnt)
 	{
 		delta_t = gu16TimeBaseCnt + 65536 -Currentcount;
 	}
 	else
 	{
 		delta_t = gu16TimeBaseCnt - Currentcount;
 	}
 	if(delta_t >= Delaytime)   return 1;
 	else                       return 0;
 }
 /***************************************************************************//*!
*
* @brief   FAULT state
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_StateFault(void)
{
	gsPFC_Drive.sFlag.PWM_enable = 0;
	DISABLE_ALL_GAN();
	DISABLE_ALLMOSFET();
	
#if COMMUNICATION_EN
	/* if LLC is working when fault happens, stop LLC */
	if(gu16LLCRun == 1 && (QSCI0_STAT & QSCI_STAT_TDRE_MASK))
	{
		QSCI0_DATA = 0x01;
		QSCI0_DATA = 0x33; /* 0x55, start LLC; 0x33, stop LLC; else no effect */
		QSCI0_DATA = 0x32; /* check sum */
	}
#endif
	
	gu16LLCVprimOK = 0; /* new LLC input voltage confirmation is needed because VDC may drop in fault condition */
	gsPFC_Drive.sFlag.Startup = 0; /* disable VAC jump correction */
	
	FaultDetection(); /* detect fault */
	/* no fault detected for a certain interval, recovery from INIT */
	if(TimeDelay(u16FaultReleaseCnt,FAULT_RELEASE_DURATION))
	{
		gsPFC_Ctrl.uiCtrl = SM_CTRL_FAULT_CLEAR; 
	}
	
  	/* open relay when output voltage is too low, prevent high current when input voltage rises */
  	if(gsPFC_Drive.sVoutCtrl.f16outFilt < gsPFC_Drive.sVoutCtrl.f16VOpenRelay)
	{
		OPEN_RELAY();
		gsPFC_Drive.sFlag.RelayFlag = 0;
		bVolStableAfterRelayAction = 0;
	}
}

/***************************************************************************//*!
*
* @brief   INIT state
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_StateInit(void)
{
	if(!bPFCVarInit)
	{
	    PWMA_STOP(); /*Disable the clocks to the PWM generator */
	    DISABLE_ALL_GAN();
	    DISABLE_HARDWARE_CURRENT_PROTECTION();

	    HW_FAULT_FLAG_CLEAR();
	   // FAULT_ALL_CLEAR(gsPFC_Drive.f16FaultID);
	    FAULT_ALL_CLEAR(gsPFC_Drive.f16FaultIDPending);
	
	    bPFC_Run = 1; /* enale PFC work */
	
	    /*==================== flag initialization ====================*/
	    gsPFC_Drive.sFlag.Zerocross = 0;
	    gsPFC_Drive.sFlag.EnMetering = 0;
	    gsPFC_Drive.sFlag.EnHighFreSync = 0;
	    gsPFC_Drive.sFlag.EnLowFreSync = 0;
	    gsPFC_Drive.sFlag.Ensoftzero = 0;
	    gsPFC_Drive.sFlag.AcDrop = 0;
	    gsPFC_Drive.sFlag.FisrtZerocrossingDetect = 0;
	    gsPFC_Drive.sFlag.PWM_enable = 0;
	    gsPFC_Drive.sFlag.EnVolLoopCor = 0;
	    gsPFC_Drive.sFlag.Startup = 0;
	    if(RELAY_STATE() == 1)  gsPFC_Drive.sFlag.RelayFlag = 1;
	    else   gsPFC_Drive.sFlag.RelayFlag = 0;

	    gsPFC_Runsub = SOFTSTART; /* converter always start from softstart mode */
	
	    u16VolRampUpCnt = 0;
	    u16DelayAftRelayCloseCnt = 0;
	    bVolStableAfterRelayAction = 0;
	    gu16TimeBaseCnt = 0;
	    
        /*======================= fault thresholds ==============================*/
        gsPFC_Drive.sFaultThresholds.f16IinMax = FRAC16(ISNS_LIMIT/ISNS_SCALE);
        gsPFC_Drive.sFaultThresholds.f16UDcMax = FRAC16(VDC_HLIMIT/VDC_SCALE);
        gsPFC_Drive.sFaultThresholds.f16UDcMin = FRAC16(0.0);
        gsPFC_Drive.sFaultThresholds.f16VacMax = FRAC16(VAC_RMS_UPLIMIT/VAC_SCALE);
        #if BOARD_TEST
	    gsPFC_Drive.sFaultThresholds.f16VacMin= 0;
        #else
	    gsPFC_Drive.sFaultThresholds.f16VacMin = FRAC16(VAC_RMS_LOWLIMIT/VAC_SCALE);
        #endif
	
	/*==================== VAC detection parameters ==========================*/
	    gsPFC_Drive.sVacTest.u16VrmsDoneCnt = 0;
	    gsPFC_Drive.sVacTest.bSeveralVrmsDetectionFlag = 0;
	    gsPFC_Drive.sVacTest.u16HalfCycleCntforMetering = 0;
		gsPFC_Drive.sVacTest.u16VacJumpCorCnt = 0;
		gsPFC_Drive.sVacTest.u16VacJumpCorDisCnt = 0;
	
	    gsPFC_Drive.sVacTest.f16VacRec = 0;
	    gsPFC_Drive.sVacTest.f16VacRms = 0;
	    gsPFC_Drive.sVacTest.f16VacLackTh = VAC_FAIL_THRESHOLD; 
	    gsPFC_Drive.sVacTest.f16VacRecoveryTh = VAC_RECOVERY_THRESHOLD;
	    gsPFC_Drive.sVacTest.f32VacSquareSum = 0;
	    gsPFC_Drive.sVacTest.f16VacLowTh =FRAC16(VAC_HIGHRMS_LOWTH/VAC_SCALE);
	
	    gsPFC_Drive.sVacTest.sFilterParam.sFltCoeff.f32B0 = VAC_IIR_B0;
	    gsPFC_Drive.sVacTest.sFilterParam.sFltCoeff.f32B1 = VAC_IIR_B1;
	    gsPFC_Drive.sVacTest.sFilterParam.sFltCoeff.f32A1 = VAC_IIR_A1;
	    GDFLIB_FilterIIR1Init_F16(&gsPFC_Drive.sVacTest.sFilterParam);	
	
	    /*==================== VDC control parameters ==========================*/
	    /* soft start vout ramp parameters */
	    gsPFC_Drive.sVoutCtrl.sVoutRamp.sRamp.f16RampUp = FRAC16(RAMPUP_VOL_VAL/VDC_SCALE);
	    gsPFC_Drive.sVoutCtrl.sVoutRamp.f16Target = FRAC16(VDC_REF/VDC_SCALE);
	    u16SoftStartStepCnt = 0;	
	
	    /* VOUT control parameters */
	    gsPFC_Drive.sVoutCtrl.sFilterParam.sFltCoeff.f32B0 = VDC_IIR_B0;
        gsPFC_Drive.sVoutCtrl.sFilterParam.sFltCoeff.f32B1 = VDC_IIR_B1;
	    gsPFC_Drive.sVoutCtrl.sFilterParam.sFltCoeff.f32A1 = VDC_IIR_A1;
	    GDFLIB_FilterIIR1Init_F16(&gsPFC_Drive.sVoutCtrl.sFilterParam);
	
	    gsPFC_Drive.sVoutCtrl.sPIpAWParams.a32PGain = VDC_KP_GAIN;
        gsPFC_Drive.sVoutCtrl.sPIpAWParams.a32IGain = VDC_KI_GAIN;
	    gsPFC_Drive.sVoutCtrl.StopIntegFlag = 0;
	    gsPFC_Drive.sVoutCtrl.sPIpAWParams.f32IAccK_1 = 0;
	    gsPFC_Drive.sVoutCtrl.sPIpAWParams.f16InErrK_1 = 0;
	
	    u16PWMOffDurCnt = 0;
	    gsPFC_Drive.sVoutCtrl.f16VBurston = FRAC16(VDC_BURSTON/VDC_SCALE);
	    gsPFC_Drive.sVoutCtrl.f16VBurstoff = FRAC16(VDC_BURSTOFF/VDC_SCALE); 
	
	    gsPFC_Drive.sVoutCtrl.f16VOpenRelay = FRAC16(VDC_OPEN_RELAY/VDC_SCALE);
	
	    /*==================== current control parameters ==========================*/	
	    gsPFC_Drive.sIinCtrl.f16IsnsRec = 0;
	    gsPFC_Drive.sIacTest.f16IacRms = 0;
	    gsPFC_Drive.sIacTest.f32IacSquareSum = 0;
	    gsPFC_Drive.sVoutCtrl.f16UDcCtrlout = 0;
	    gsPFC_Drive.a32IrefReq = 0;
	    gsPFC_Drive.sIinCtrl.f16outRef = 0;
	    gsPFC_Drive.sIinCtrl.f16RefHlim = IREF_HLIMIT;
	
	    gsPFC_Drive.sIinCtrl.sPIpAWParams.a32PGain = ISNS_KP_GAIN_LVRMS;
        gsPFC_Drive.sIinCtrl.sPIpAWParams.a32IGain = ISNS_KI_GAIN_LVRMS;
        gsPFC_Drive.sIinCtrl.StopIntegFlag = 0;
        gsPFC_Drive.sIinCtrl.sPIpAWParams.f16LowerLim = ISNS_PI_LOW_LIMIT;
        gsPFC_Drive.sIinCtrl.sPIpAWParams.f16UpperLim = ISNS_PI_UP_LIMIT;
    
        gsPFC_Drive.sIinCtrl.f16TurnOnHighFreSyncCur = TURNON_HIGHFRE_SYNC_CURR;
        gsPFC_Drive.sIinCtrl.f16TurnOffHighFreSyncCur = TURNOFF_HIGHFRE_SYNC_CURR;
        gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt2 = 0;
        gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt1 = 0;
        
        #if SPLL_EN
        SPLL_1PH_SOGI_Reset(&spll_obj);
        SPLL_1PH_SOGI_LFConfig(&spll_obj,SPLL_SOGI_LF_B0,SPLL_SOGI_LF_B1);
        #endif 
	    /**************** duty params *******************/
	    gsPFC_Drive.f16Duty = 0;
	
	    gsPFC_Drive.sVacTest.u16PolChangeConfirmCnt = 0;
	    POLARITY(gsPFC_Drive.sFlag.PolTemp);
	    bPFCVarInit = 1;
	}

	FaultDetection();
#if BOARD_TEST
	gsPFC_Drive.sFlag.Pol = 1;
	PWMA_RUN(); // PWM module start 	   
    gsPFC_Ctrl.uiCtrl |= SM_CTRL_INIT_DONE; /* INIT_DONE command */
#else	
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
			gsPFC_Drive.sFlag.FisrtZerocrossingDetect = 1;			
		}
	}
	else  gsPFC_Drive.sVacTest.u16PolChangeConfirmCnt = 0;
	
	/* switch to stop mode when a trusted polarity is detected */
	if(gsPFC_Drive.sFlag.FisrtZerocrossingDetect) 
	{
	    PWMA_RUN(); // PWM module start 	
	    /* INIT_DONE command */
	    gsPFC_Ctrl.uiCtrl |= SM_CTRL_INIT_DONE;
	}
#endif
}

/***************************************************************************//*!
*
* @brief   STOP state
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_StateStop(void)
{
	/*================= relay control ========================*/
	if((gsPFC_Drive.sVoutCtrl.f16outFilt < gsPFC_Drive.sVoutCtrl.f16VOpenRelay))
	{
		OPEN_RELAY();
		gsPFC_Drive.sFlag.RelayFlag = 0;
		bVolStableAfterRelayAction = 0;
	}

	/*================= subsequent circuit (LLC) control ========================*/
#if COMMUNICATION_EN
	/* if LLC is working when PFC stops, stop LLC */
	if(gu16LLCRun == 1 && (QSCI0_STAT & QSCI_STAT_TDRE_MASK))
	{
		QSCI0_DATA = 0x01;
		QSCI0_DATA = 0x33; /* 0x55, start LLC; 0x33, stop LLC; else no effect */
		QSCI0_DATA = 0x01 ^ 0x33; /* checksum */
	}
#endif

#if BOARD_TEST
	if(gsPFC_Drive.sVacTest.bSeveralVrmsDetectionFlag)
	{
		gsPFC_Ctrl.uiCtrl |= SM_CTRL_START;
	}
#else
	
	/* start PFC startup process when trusted and acceptable vrms is detected */
	if(gsPFC_Drive.sVacTest.bSeveralVrmsDetectionFlag && gsPFC_Drive.sVacTest.f16VacRms > gsPFC_Drive.sFaultThresholds.f16VacMin)
	{		
		gsPFC_Drive.sVoutCtrl.f16VCloseRelay = MLIB_Mul_F16as(VDC_CLOSE_RELAY_TO_VRMS_COEFF, gsPFC_Drive.sVacTest.f16VacRms); //0.8*1.414*Vrms*1082/417.8=2.6*Vrms
		
		/* always close relay before PFC starts working to avoid damage of starting resistor */
		if(!gsPFC_Drive.sFlag.RelayFlag)
		{
			/* delay some time for diode rectifier output ramping up, then close the relay */
			if(gsPFC_Drive.sVoutCtrl.f16outFilt > gsPFC_Drive.sVoutCtrl.f16VCloseRelay)
			{
				if(TimeDelay(u16VolRampUpCnt, UDCBUS_RAMPUP_DURATION_AFTER_RELAY_ON) && gsPFC_Drive.sVacTest.u16PointCnt <= 20)
				{
					CLOSE_RELAY(); 
					gsPFC_Drive.sFlag.RelayFlag = 1;
					u16DelayAftRelayCloseCnt = gu16TimeBaseCnt;
				}
			}
			else  u16VolRampUpCnt = gu16TimeBaseCnt;
		}
		else if(bPFC_Run == 1) /* remote control of PFC, relay has been closed now */
		{
			/* delay some time after close relay for ouput voltage ramping up before starting the converter */
			if(gsPFC_Drive.sVoutCtrl.f16outFilt > gsPFC_Drive.sVoutCtrl.f16VCloseRelay)
			{
				if((bVolStableAfterRelayAction == 0) && TimeDelay(u16DelayAftRelayCloseCnt, UDCBUS_RAMPUP_DURATION_BEFORE_RELAY_ON)) 
				{
					gsPFC_Drive.sFlag.Zerocross = 0; /* clear this flag to capture next zero-crossing point */
					bVolStableAfterRelayAction = 1;
				}
				/* after the output is stable, converter starts near the zero crossing of the positive cycle */
				if(bVolStableAfterRelayAction == 1)
				{
					if((gsPFC_Drive.sFlag.Zerocross == 1) && gsPFC_Drive.sFlag.Pol) 
					{
						/* update current sampling offset when current is zero */
						Get_ISNS_ADC_offset(); 
						
						gsPFC_Ctrl.uiCtrl |= SM_CTRL_START;
						gsPFC_Drive.sFlag.Zerocross = 0;
					}
				}				
			}
		}
		else u16DelayAftRelayCloseCnt = gu16TimeBaseCnt;
	}
	else
	{
		u16VolRampUpCnt = gu16TimeBaseCnt;
		u16DelayAftRelayCloseCnt = gu16TimeBaseCnt;
    }
#endif
	
	FaultDetection();
}

/***************************************************************************//*!
*
* @brief   RUN state
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_StateRun(void)
{
	/*================= subsequent circuit (LLC) control ========================*/
#if COMMUNICATION_EN
	/* when PFC output reaches its reference and LLC does not accept it, 
	   keep transfer the output voltage to LLC until accept the accept ack message */
	if(gu16LLCVprimOK == 0 && gsPFC_Runsub != SOFTSTART)
	{
		if(QSCI0_STAT & QSCI_STAT_TDRE_MASK)
		{
			QSCI0_DATA = 0x02;
			u16VprimToLLC = MLIB_Mul_F16(VDC_SCALE, gsPFC_Drive.sVoutCtrl.f16outFilt);
			ucLLC_VprimH = u16VprimToLLC >> 8;
			ucLLC_VprimL = u16VprimToLLC;
			QSCI0_DATA = ucLLC_VprimH;
			QSCI0_DATA = ucLLC_VprimL;
			QSCI0_DATA = 0x02 ^ ucLLC_VprimH ^ ucLLC_VprimL; /* check sum */
		}
	}
	/* when PFC output satisfy LLC requirements, can control LLC on/off any time when PFC is runing */
	else if(gu16LLCVprimOK == 1)
	{
		if(QSCI0_STAT & QSCI_STAT_TDRE_MASK)
		{
			QSCI0_DATA = 0x01;
			QSCI0_DATA = 0x55; /* 0x55, start LLC; 0x33, stop LLC; else no effect */
			QSCI0_DATA = 0x01 ^ 0x55;
		}
	}
#endif

	
	if(gsPFC_Runsub  == NORMAL)
	{
	#if BOARD_TEST
		if(gsPFC_Drive.sFlag.Pol)
		{
			if(gsPFC_Drive.sFlag.EnHighFreSync)
			{
				gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt2++;
				if(gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt2 >= TURNON_HIGH_FRE_SYNC_CUR_HIGH_NUM)
				{
					gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt2 = 0;
					ENABLE_UPGAN(); // enable synchronous GAN when input current is large enough to avoid irrigation
					gsPFC_Drive.sFlag.EnHighFreSync = 0; // not to set high slave GaN MOS twice a period
				}
			}
			else   gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt2 = 0;
			// disable synchronous GAN when current low after a quarter of cycle
			if((gsPFC_Drive.sIinCtrl.f16out < gsPFC_Drive.sIinCtrl.f16TurnOffHighFreSyncCur) && (gsPFC_Drive.sVacTest.u16PointCnt > MIN_QUARTER_CYCLE_POINT_COUNTER))
			{
				DISABLE_UPGAN();
				SETLOWERMOSFET_LOW;
			}
			//enable corresponding slave mos after zero cross 	
			if(gsPFC_Drive.sFlag.EnLowFreSync && (gsPFC_Drive.sVacTest.u16PointCnt >= 10) && (HIGHERMOSFET_STATE == 0)) 
			{
				SETLOWERMOSFET_HIGH;
				gsPFC_Drive.sFlag.EnLowFreSync = 0;//in order not to set high synchronous rectifier MOS twice a period
			}
		}
		else
		{
			if( gsPFC_Drive.sFlag.EnHighFreSync)
			{
				gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt1++;
				if(gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt1 >= TURNON_HIGH_FRE_SYNC_CUR_HIGH_NUM)
				{
					gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt1 = 0;
					ENABLE_DOWNGAN();
					gsPFC_Drive.sFlag.EnHighFreSync = 0;//in order not to set high slave GaN MOS twice a period
				}
			}
			else gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt1 = 0;
			if((gsPFC_Drive.sIinCtrl.f16out > -gsPFC_Drive.sIinCtrl.f16TurnOffHighFreSyncCur) && (gsPFC_Drive.sVacTest.u16PointCnt > MIN_QUARTER_CYCLE_POINT_COUNTER))
			{
				DISABLE_DOWNGAN();
				SETHIGHERMOSFET_LOW;
			}
			if(gsPFC_Drive.sFlag.EnLowFreSync && (gsPFC_Drive.sVacTest.u16PointCnt >= 10)&& (LOWERMOSFET_STATE == 0))
			{
				SETHIGHERMOSFET_HIGH;
				gsPFC_Drive.sFlag.EnLowFreSync = 0;//in order not to set high synchronous rectifier MOS twice a period
			}
		}	
	#else
		/*================= high&low frequency sync tube control ========================*/
		if(gsPFC_Drive.sFlag.Pol)
		{
			/* high frequency sync tube on control */
			if((gsPFC_Drive.sIinCtrl.f16out > gsPFC_Drive.sIinCtrl.f16TurnOnHighFreSyncCur) && gsPFC_Drive.sFlag.EnHighFreSync)
			{
				gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt2++;
				if(gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt2 >= TURNON_HIGH_FRE_SYNC_CUR_HIGH_NUM)
				{
					gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt2 = 0;
					ENABLE_UPGAN(); /* enable synchronous GAN when input current is large enough to avoid irrigation */
					gsPFC_Drive.sFlag.EnHighFreSync = 0; /* not to set high slave GaN MOS twice a period */
				}
			}
			else   gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt2 = 0;
			
			/* high frequency sync tube off control */
			if((gsPFC_Drive.sIinCtrl.f16out < gsPFC_Drive.sIinCtrl.f16TurnOffHighFreSyncCur) && (UPGAN_STATE != 0) && (gsPFC_Drive.sVacTest.u16PointCnt > MIN_QUARTER_CYCLE_POINT_COUNTER))
			{
				DISABLE_UPGAN();
				SETLOWERMOSFET_LOW;
			}
			
			/* low frequency sync tube on control */ 	
			if(gsPFC_Drive.sFlag.EnLowFreSync && (gsPFC_Drive.sVacTest.u16PointCnt >= 10) && (HIGHERMOSFET_STATE == 0)) 
			{
				SETLOWERMOSFET_HIGH;
				gsPFC_Drive.sFlag.EnLowFreSync = 0;
			}
		}
		else
		{
			/* high frequency sync tube on control */
			if((gsPFC_Drive.sIinCtrl.f16out < -gsPFC_Drive.sIinCtrl.f16TurnOnHighFreSyncCur)&& gsPFC_Drive.sFlag.EnHighFreSync)
			{
				gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt1++;
				if(gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt1 >= TURNON_HIGH_FRE_SYNC_CUR_HIGH_NUM)
				{
					gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt1 = 0;
					ENABLE_DOWNGAN();
					gsPFC_Drive.sFlag.EnHighFreSync = 0; /* in order not to set high slave GaN MOS twice a period */
				}
			}
			else gsPFC_Drive.sIinCtrl.u16HigherthanOnsynccurCnt1 = 0;
			
			/* high frequency sync tube off control */
			if((gsPFC_Drive.sIinCtrl.f16out > -gsPFC_Drive.sIinCtrl.f16TurnOffHighFreSyncCur) && (DOWNGAN_STATE != 0) && (gsPFC_Drive.sVacTest.u16PointCnt > MIN_QUARTER_CYCLE_POINT_COUNTER))
			{
				DISABLE_DOWNGAN();
				SETHIGHERMOSFET_LOW;
			}
			
			/* low frequency sync tube on control */
			if(gsPFC_Drive.sFlag.EnLowFreSync && (gsPFC_Drive.sVacTest.u16PointCnt >= 10) && (LOWERMOSFET_STATE == 0))
			{
				SETHIGHERMOSFET_HIGH;
				gsPFC_Drive.sFlag.EnLowFreSync = 0;
			}
		}
		
		/*================= normal to light-load mode switch control ========================*/
		/* Enter light-load mode (burst off) when output voltage is larger than low overshoot command and voltage controller
		  outputs lower limit or enter burst mode when DC bus voltage is larger than high overshoot command */
		if(((gsPFC_Drive.sVoutCtrl.f16PIResult == gsPFC_Drive.sVoutCtrl.sPIpAWParams.f16LowerLim) && \
		     (gsPFC_Drive.sVoutCtrl.f16outFilt > FRAC16(VDC_NORMAL_OVERSHOOT_LOW/VDC_SCALE))) \
			 ||(gsPFC_Drive.sVoutCtrl.f16outFilt > FRAC16(VDC_NORMAL_OVERSHOOT_HIGH/VDC_SCALE)))
		{
			gsPFC_Drive.sFlag.PWM_enable = 0;
			DISABLE_ALL_GAN();
			DISABLE_ALLMOSFET();
			gsPFC_Drive.sFlag.EnLowFreSync = 0;
			u16PWMOffDurCnt = 0;
			gsPFC_Runsub  = LIGHTLOAD;
			gsPFC_Drive.sIinCtrl.sPIpAWParams.f32IAccK_1 = 0; /* avoid current spike when tube is turned on in lightload mode */
		}
		else if(gsPFC_Drive.sVoutCtrl.f16outFilt > FRAC16(VDC_FEEDFORWARD_COMP/VDC_SCALE))
		{
			gsPFC_Drive.sVoutCtrl.sPIpAWParams.f32IAccK_1 -= 40960; /* approximate output voltage feed forward to restrain the overshoot */
		}
	#endif
	}
	else if(gsPFC_Runsub  == LIGHTLOAD)
	{
		/* if reach the peak-valley controller peak value, close all tubes */ 
		if(gsPFC_Drive.sVoutCtrl.f16outFilt > gsPFC_Drive.sVoutCtrl.f16VBurstoff)
		{
			if(gsPFC_Drive.sFlag.PWM_enable)  u16PWMOffDurCnt = 0;
			gsPFC_Drive.sFlag.PWM_enable = 0;
			DISABLE_ALL_GAN();
		}
			
		/* burst on from zero crossing point */
		if(gsPFC_Drive.sFlag.Zerocross == 1)
		{
			if(u16PWMOffDurCnt < BURST_OFF_MIN_DURATION)
			{
				u16PWMOffDurCnt++;
			} 
			gsPFC_Drive.sFlag.Zerocross = 0;
			
			/* if reach the peak-valley controller valley value at the positive cycle, start converter again */
			if((gsPFC_Drive.sVoutCtrl.f16outFilt < gsPFC_Drive.sVoutCtrl.f16VBurston) && gsPFC_Drive.sFlag.Pol)
			{
				/* burst off time is less than the minimum duration, voltage drop too fast, directly return to normal mode */
				if(u16PWMOffDurCnt < BURST_OFF_MIN_DURATION)
				{ 
					/* fixed current reference is used in lightload mode, reset voltage PI integrator before back to normal mode to avoid current surge */
					gsPFC_Drive.sVoutCtrl.sPIpAWParams.f32IAccK_1 = (frac32_t)gsPFC_Drive.sVoutCtrl.sPIpAWParams.f16LowerLim << 16;
					gsPFC_Drive.sVoutCtrl.sPIpAWParams.f16InErrK_1 = 0;
					
					gsPFC_Runsub  = NORMAL;
					u16PWMOffDurCnt = 0;
				}
				gsPFC_Drive.sIinCtrl.sPIpAWParams.f32IAccK_1 = 0;
				gsPFC_Drive.sFlag.PWM_enable = 1;
			}
		}
		
		/* output voltage drop too low, directly return to normal mode */
		if(gsPFC_Drive.sVoutCtrl.f16outFilt < FRAC16(VDC_BURST_UNDER/VDC_SCALE))
		{
			u16PWMOffDurCnt = 0;
			/* fixed current reference is used in lightload mode, reset voltage PI integrator before back to normal mode to avoid current surge */
			gsPFC_Drive.sVoutCtrl.sPIpAWParams.f32IAccK_1 = ((frac32_t)gsPFC_Drive.sVoutCtrl.sPIpAWParams.f16LowerLim) << 16;
			gsPFC_Drive.sVoutCtrl.sPIpAWParams.f16InErrK_1 = 0;
			gsPFC_Runsub  = NORMAL;
			if(!gsPFC_Drive.sFlag.PWM_enable)
			{
				gsPFC_Drive.sIinCtrl.sPIpAWParams.f32IAccK_1 = 0;
				gsPFC_Drive.sFlag.PWM_enable = 1;
			}
		}
	}
	
#if !BOARD_TEST	
	if(bPFC_Run == 0) /* remote control */
	{
		gsPFC_Ctrl.uiCtrl |= SM_CTRL_STOP;
		PWMA_STOP(); // the estate is still RUN now, stop PWM to avoid opening the tube again during the transmission process */ 
	}
#endif
	FaultDetection();
}

/***************************************************************************//*!
*
* @brief   FAULT to INIT transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransFaultInit(void)
{
	bPFCVarInit = 0;
}
/***************************************************************************//*!
*
* @brief   INIT to FAULT transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransInitFault(void)
{
	
}
/***************************************************************************//*!
*
* @brief   INIT to STOP transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransInitStop(void)
{

}
/***************************************************************************//*!
*
* @brief   INIT to RUN transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransInitRun(void)
{
	
}
/***************************************************************************//*!
*
* @brief   STOP to FAULT transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransStopFault(void)
{
	
}
/***************************************************************************//*!
*
* @brief   STOP to RUN transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransStopRun(void)
{
	gsPFC_Drive.sFlag.PWM_enable = 1;//Enable PI control loops
	gsPFC_Drive.sFlag.Startup = 1;		 			    
	ENABLE_DOWNGAN();
	ENABLE_HARDWARE_CURRENT_PROTECTION(); /* enable hardware current protection when converter starts to avoid entering the fault state by mistake*/
	
	gsPFC_Drive.sVoutCtrl.f16outRef = gsPFC_Drive.sVoutCtrl.f16outFilt;
	GFLIB_RampInit_F16(gsPFC_Drive.sVoutCtrl.f16outFilt, &gsPFC_Drive.sVoutCtrl.sVoutRamp.sRamp);		 			   
	gsPFC_Ctrl.uiCtrl |= SM_CTRL_RUN_ACK;
}
/***************************************************************************//*!
*
* @brief   RUN to FAULT transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransRunFault(void)
{
	
}
/***************************************************************************//*!
*
* @brief   RUN to STOP transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransRunStop(void)
{
	gsPFC_Drive.sFlag.PWM_enable = 0;
	DISABLE_ALL_GAN();
	DISABLE_ALLMOSFET();
	gsPFC_Drive.sFlag.EnLowFreSync = 0;
	gsPFC_Drive.sFlag.EnHighFreSync = 0;
	gsPFC_Drive.sFlag.Startup = 0;
	gu16LLCVprimOK = 0; /* new LLC input voltage confirmation is needed because VDC may drop in stop mode */
	gsPFC_Runsub = SOFTSTART; /* always start from soft start mode */
	gsPFC_Drive.sFaultThresholds.f16UDcMin = 0; /* any output voltage is acceptable when converter is stopped, avoid entering fault state by mistake */
	
	/* reset PI controller to avoid current surge when converter works again */
	gsPFC_Drive.sVoutCtrl.sPIpAWParams.f32IAccK_1 = (frac32_t)gsPFC_Drive.sVoutCtrl.sPIpAWParams.f16LowerLim << 16;
	gsPFC_Drive.sVoutCtrl.sPIpAWParams.f16InErrK_1 = 0;
	gsPFC_Drive.sIinCtrl.sPIpAWParams.f32IAccK_1 = 0;
	
	gsPFC_Ctrl.uiCtrl |= SM_CTRL_STOP_ACK;
	PWMA_RUN(); /* enable PWM counter again for sampling and metering */
}

/***************************************************************************//*!
*
* @brief   RUN Soft start sub-state
*
* @param   void
*
* @return  none
*
******************************************************************************/

static void PFC_StateRun_Softstart(void)
{
	/* when voltage ref is less than the target, ramp up ref
	 * when voltage ref or output voltage is grater than the target, jump to normal mode
	 *  */
	if(gsPFC_Drive.sVoutCtrl.f16outRef < gsPFC_Drive.sVoutCtrl.sVoutRamp.f16Target)
	{
		u16SoftStartStepCnt++;
		if (u16SoftStartStepCnt >= RAMPUP_PERIOD)
		{
	       gsPFC_Drive.sVoutCtrl.f16outRef = GFLIB_Ramp_F16(gsPFC_Drive.sVoutCtrl.sVoutRamp.f16Target, &gsPFC_Drive.sVoutCtrl.sVoutRamp.sRamp);
	       u16SoftStartStepCnt = 0;
	       if(gsPFC_Drive.sVoutCtrl.f16outFilt >= gsPFC_Drive.sVoutCtrl.sVoutRamp.f16Target)
	       {
	           gsPFC_Runsub = NORMAL;
	           gsPFC_Drive.sFaultThresholds.f16UDcMin = FRAC16(VDC_LLIMIT/VDC_SCALE); /* make sure final voltage ref equals to the target */
	           gsPFC_Drive.sVoutCtrl.f16outRef = gsPFC_Drive.sVoutCtrl.sVoutRamp.f16Target;
	       }  
	    }
	}
	else
	{
		gsPFC_Runsub = NORMAL;
#if BOARD_TEST
		gsPFC_Drive.sFaultThresholds.f16UDcMin = 0;
#else
		gsPFC_Drive.sFaultThresholds.f16UDcMin = FRAC16(VDC_LLIMIT/VDC_SCALE);
#endif
	}
}


/***************************************************************************//*!
*
* @brief   RUN Normal sub-state
*
* @param   void
*
* @return  none
*
******************************************************************************/
#pragma section CODES_IN_RAM begin
static void PFC_StateRun_Normal(void)
{
}
#pragma section CODES_IN_RAM end
/***************************************************************************//*!
*
* @brief   RUN light-load sub-state
*
* @param   void
*
* @return  none
*
******************************************************************************/
#pragma section CODES_IN_RAM begin
static void PFC_StateRun_Lightload(void)
{
}
#pragma section CODES_IN_RAM end
/***************************************************************************//*!
*
* @brief   RUN SOFTSTART to NORMAL transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
void PFC_TransRun_Softstart_Normal(void)
{
	
}

/***************************************************************************//*!
*
* @brief   RUN normal to light-load transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransRun_Normal_Lightload(void)
{
	
}
/***************************************************************************//*!
*
* @brief   RUN light-load to normal transition
*
* @param   void
*
* @return  none
*
******************************************************************************/
static void PFC_TransRun_Lightload_Normal(void)
{
	
}
