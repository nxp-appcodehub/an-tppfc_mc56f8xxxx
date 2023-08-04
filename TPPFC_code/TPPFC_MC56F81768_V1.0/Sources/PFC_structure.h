/******************************************************************************
* 
* Copyright 2021 NXP
* SPDX-License-Identifier: BSD-3-Clause                    
*
******************************************************************************* 
*
* @file      Loop.h
*
* @author    
* 
* @version   1.0.0.0

* @date      Dec-02-2016
* 
* @brief     Loops code head
*
*******************************************************************************/
#ifndef PFC_STRUCTURE_H_
#define PFC_STRUCTURE_H_

#include "mlib_types.h"
#include "gflib.h"
#include "gdflib.h"
#include "mlib.h"

typedef struct
{
	GFLIB_CTRL_PI_P_AW_T_A32  sPIpAWParams;  /* current PI controller parameters */
	bool_t               StopIntegFlag; /* PI controller integartion stop flag */
	acc32_t              a32HVinkp; /* kp used when input voltage is greater than a certain threshold, 
	                                   different from the zero-crossing point */
	acc32_t              a32HVinki; /* ki used when input voltage is greater than a certain threshold, 
	                                   different from the zero-crossing point */
	frac16_t             f16outError; /* controlled value error */
	frac16_t             f16out; /* sampled current value */
	frac16_t             f16outRef; /* required current value */
	frac16_t             f16PIResult; /* current PI controller output */
	frac16_t			 f16RefHlim; /* high limit for current reference */
	uint16_t             u16HigherthanOnsynccurCnt1; /* turn on high frequency sync tube at least this number of current that higher than the limit have been detected */
	uint16_t			 u16HigherthanOnsynccurCnt2; 
	frac16_t             f16TurnOnHighFreSyncCur; /* f16TurnOnRightSynCu; */
	frac16_t             f16TurnOffHighFreSyncCur; /* f16TurnOffRightSynCu; */
	
	frac16_t             f16IsnsRec; /* rectified current */
	frac16_t             f16IsnsOffset; /* current offset */
}PFC_CURRENT_CTRL_T;

typedef struct
{
	GFLIB_RAMP_T_F16       sRamp; /* output voltage ramp parameters, used on softstart sub-state*/
	frac16_t               f16InitVal; /* ramp initial value */
	frac16_t               f16Target; /* required value, ramp limit*/
}PFC_UDC_RAMP_T;

typedef struct
{
	GDFLIB_FILTER_IIR1_T_F32  sFilterParam; /* output voltage IIR filter parameters */
	GFLIB_CTRL_PI_P_AW_T_A32  sPIpAWParams; /* output voltage PI controller parameters */
	bool_t                 StopIntegFlag; /* PI controller integartion stop flag */
	PFC_UDC_RAMP_T         sVoutRamp; /* output voltage ramp parameters */
	frac16_t               f16out; /* sampled value */
	frac16_t               f16outFilt; /* filtered sample value */
	frac16_t               f16outRef; /* required value, output voltage reference */
	frac16_t               f16outError; /* controlled value error */
	frac16_t               f16PIResult; /* PI controller output */
	frac16_t               f16UDcCtrlout; /* output of voltage controller for current reference generation*/
	
	frac16_t              f16VBurston; /* Start PFC operation when output voltage is less than this value in light-load mode */
	frac16_t              f16VBurstoff; /* Stop PFC operation when output voltage is larger than this value in light-load mode */
	frac16_t              f16VOpenRelay; /* DC bus voltage threshold to turn on the relay */
	frac16_t              f16VCloseRelay; /* DC bus voltage threshold to turn off the relay */
}PFC_VOLTAGE_CTRL_T;

typedef struct
{
	GDFLIB_FILTER_IIR1_T_F32  sFilterParam; /*input voltage filter parameters */
	frac16_t        f16Vac; /* sampled input voltage */
	frac16_t        f16Vac_L;
	frac16_t        f16Vac_N;
	frac16_t        f16VacRec; /* rectified input voltage */
	frac16_t        f16VacRms; /* RMS value of input voltage */ 
	frac32_t        f32VacSquare; /* square of input voltage, used for RMS calculation */
	frac32_t        f32VacSquareSum; 
	frac32_t        f32VrmsSquare; /* square of input voltage RMS value */
	frac32_t        f32InvVrmsSquare; /* the inverse of f32VrmsSquare */	
	
	frac16_t        f16VacLowTh; /* input voltage low threshold, sync tubes are close and different PI parameters are used when input voltage is less than this threshold */
	frac16_t        f16VacRecoveryTh; /* input voltage recovery judgement threshold  */
    frac16_t        f16VacLackTh; /* input voltage power down judgement threshold */
    
    uint16_t        u16PointCnt; /* a counter to record the time duration from last zero crossing point of input voltage, accumulate in the fast loop */
    uint16_t	    u16PeriodCnt; /* input voltage period duration, which is derived from u16PointCnt */
    uint16_t        u16HalfCycleCntforMetering; /* half cycle point counter for metering */
    uint16_t		u16VrmsDoneCnt;	/* a counter to record how many times input voltage RMS value has been calculated */
    frac16_t        f16VacPhase; /* electrical phase of input voltage */
    uint16_t		u16PolChangeConfirmCnt; /* a counter to record how many consecutive different polarities are detected */
    uint16_t		u16VacFailCnt; /* a counter to record how many consecutive times input voltage is lower than the drop threshold */
    uint16_t		u16VacRecoveryOkCnt; /* a counter to record how many consecutive times input voltage is higher than the recovery threshold */
    uint16_t		u16VacJumpCorCnt; /* a counter to record the voltage loop correction duration when Vac jump is detected */
    uint16_t		u16VacJumpCorDisCnt; /* a counter to record the duration after last voltage loop correction */
    	
    bool_t          bSeveralVrmsDetectionFlag;	/* this flag will be set after Vrms has been calculated for certain times */
}PFC_INPUT_VOLTAGE_T;

typedef struct
{
	frac16_t              f16IacRec; /* rectified current */
	frac32_t              f32IacSquare; /* square of input current, used for Irms calculation*/
	frac32_t              f32IacSquareSum;
	frac16_t              f16IacRms; /* RMS value of input current */
	frac32_t              f32IrmsSquare;	
}PFC_IAC_METERING_T;

typedef struct 
{
	uint16_t  Pol:1;               /* Pol=1,positive cycle; Pol=0, negative cycle */
	uint16_t  PolLast:1;           /* Last polarity */
	uint16_t  PolTemp:1;           /* Temp polarity */
	uint16_t  Zerocross:1;         /* zero crossing flag */
	uint16_t  EnMetering:1;        /* metering is enabled every half cycle after zero crossing is detected */
	uint16_t  Ensoftzero:1;        /* when ensoftzero = 1, enable soft start drivers at detected zero crossing point */
	uint16_t  EnHighFreSync:1;     /* when it's set, enable high frequency arm sync tube */
	uint16_t  EnLowFreSync:1;      /* when it's set, enable low frequency arm sync tube */
	uint16_t  AcDrop:1;            /* ac input drop flag */
	uint16_t  FisrtZerocrossingDetect:1; /* first cycle after input power up, the first zero-crossing is discard to avoid wrong RMS */
	uint16_t  RelayFlag:1;         /* relay status flag */
	uint16_t  EnVolLoopCor:1;      /* flag indicate whether voltage loop correction is enabled when input voltage jumps*/
	uint16_t  PWM_enable:1;        /* flag indicate whether the controllers are working  */ 
	uint16_t  Startup:1;           /* vac jump correction works only when this flag is set */
	uint16_t  SPLLStart:1;         /* SPLL starts when this flag is set*/
	uint16_t  Reserved:1;
}PFC_FLAG_T;

typedef struct
{
	frac16_t        f16VacMax; /* input voltage maximum applicable level */ 
	frac16_t        f16VacMin; /* input voltage manimum applicable level */
	frac16_t        f16UDcMax; /* output voltage maximum applicable value */
	frac16_t        f16UDcMin; /* output voltage minimum applicable value */
	frac16_t        f16IinMax; /* input current maximum applicable value */	
}PFC_FAULT_THRESHOLDS_T;

typedef struct
{
	PFC_FAULT_THRESHOLDS_T     sFaultThresholds;
	PFC_VOLTAGE_CTRL_T         sVoutCtrl;
	PFC_CURRENT_CTRL_T         sIinCtrl;
	PFC_INPUT_VOLTAGE_T        sVacTest;
	PFC_IAC_METERING_T         sIacTest;
	PFC_FLAG_T                 sFlag;	
   
	frac32_t              f32IrefReq;
	acc32_t               a32IrefReq;     /* f16UDcCtrlout/vrms^2, used for current reference generation */
    frac16_t              f16Duty;        /* PWM duty cycle */
    frac16_t              f16PowerFactor; 
    frac16_t              f16ActivePower;
    frac16_t              f16ApparentPower;
    frac32_t              f32InstPower;   /* instantaneous power */ 
    frac32_t              f32InstPowerSum;/* sum of instantaneous power, use for active power calculation */
    frac16_t              f16FaultIDPending; /* fault identification */
    frac16_t              f16FaultID; /* record fault */
}PFC_STRUC_T;

#endif /* PFC_STRUCTURE_H_ */
