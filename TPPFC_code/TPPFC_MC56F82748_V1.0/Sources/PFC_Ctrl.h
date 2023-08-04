/*
* Copyright 2021 NXP
* SPDX-License-Identifier: BSD-3-Clause
*/

/*
 * PFC_Ctrl.h
 *
 *  Created on: Jan 26, 2021
 *      Author: nxa16823
 */

#ifndef PFC_CTRL_H_
#define PFC_CTRL_H_

/******************************************************************************
* Includes
******************************************************************************/
#include "IO_Map.h"

/******************************************************************************
* Macros 
******************************************************************************/

/************************************** conditional compilation **************************************/
#define BOARD_TEST             0 /* BOARD_TEST=1, test board, don't connect the main power */

#define ISNS_HALL              1 /* If ISNS_Hall = 1, sense the input current with Hall, otherwise with current transformer.
                                    This code uses Hall, if CT is wanted, you need to change sampling and protection configurations */          
#define COMMUNICATION_EN       1 /* For communication with LLC*/         
#define VACJUMP_CORRECTION_EN  1 /* voltage loop correction to decrease the impact of input voltage jump to output bus voltage */
#define SPLL_EN                1 /* if SPLL_EN=1, SPLL is used, otherwise real-time ac sampling provides phase information */

/********************************* Voltage and current scales *****************************/
#define VAC_SCALE           1082.0 /* MAX measurable input AC voltage[V] */
#define VDC_SCALE           473.6 /* MAX measurable output voltage[V] */
#define VDC_TO_VAC          FRAC16(VDC_SCALE/VAC_SCALE) /* range conversion */
#define VAC_TO_VDC          ACC32(VAC_SCALE/VDC_SCALE)

#if ISNS_HALL
#define ISNS_SCALE          23.96 /* ISNS_SCALE = 3.3/(0.2083*39/59) = 23.96, maximum measurable current with Hall */
#define OFFSET_ERROR_LIMIT  FRAC16(0.2/ISNS_SCALE) /* acceptable offset error */
#else
#define ISNS_SCALE          16.9  /* ISNS_SCALE = 3.3*200/39=16.9, maximum measurable current with CT */
#endif

/********************************* application time base ********************************/
#define PFC_PWM_FREQ              80.0 /* [kHz] PFC switching frequency */
#define PWM_FREQ_VS_CTR_FREQ      1 /* PWM frequency vs fast current loop frequency */
#define PFC_FASTLOOP_FREQ         (PFC_PWM_FREQ/PWM_FREQ_VS_CTR_FREQ) /* [kHz] PFC fast current control loop frequency */
#define PFC_SLOWLOOP_FREQ         10.0 /* [kHz] PFC slow voltage control loop frequency */
#define PWM_PERIOD_CNT            (100000/PFC_PWM_FREQ) /* PWM period counter = 100M/(80K)= 1250 */

#define UDCBUS_RAMPUP_DURATION_BEFORE_RELAY_ON    2000 /* [0.1ms] when DC bus voltage reaches certain value, delay this amount of time before turning on relay */
#define UDCBUS_RAMPUP_DURATION_AFTER_RELAY_ON     2000 /* [0.1ms] after relay on, delay this amount of time before switching to RUN state */
#define FAULT_CLEAR_CONFIRM_DURATION              20 /* [0.1ms] when no fault is detected for this amount of time, exist FAULT state */

#define MESSAGE_MAX_INTERVAL       120 /* [0.1ms] when message time interval is longer than this value, restart to verify from the message head */

/********************************* Output voltage control ********************************/
#define VDC_REF           380.0 /* output voltage reference[V] */
#define VDC_BURSTON       (VDC_REF - 8.0)  /* lower threshold voltage of peak-valley controller in light load mode */   
#define VDC_BURSTOFF      (VDC_REF - 1.0)  /* upper threshold voltage of peak-valley controller in light load mode */
#define BURST_OFF_MIN_DURATION     3  /* [half-cycles], when output voltage drop to peak-valley controller valley within this 
                                         duration at light-load mode, converter returns to normal mode directly */
#define RAMPUP_VOL_VAL    1.0  /* output voltage reference change step in soft start */
#define RAMPUP_PERIOD     30  /* 1/10000*30 = 0.003s, output voltage reference change frequency in soft start */

#define VDC_NORMAL_OVERSHOOT_DELTA_LOW    8.0   /* Enter burst mode when output voltage is larger than command this value and voltage controller outputs lower limit [V] */
#define VDC_NORMAL_OVERSHOOT_LOW          (VDC_REF+VDC_NORMAL_OVERSHOOT_DELTA_LOW)
#define VDC_NORMAL_OVERSHOOT_DELTA_HIGH   12.0  /* Enter burst mode when output voltage is larger than command this value [V] */
#define VDC_NORMAL_OVERSHOOT_HIGH         (VDC_REF+VDC_NORMAL_OVERSHOOT_DELTA_HIGH)
#define VDC_BURST_UNDER_DELTA             15.0  /* During burst mode, when output voltage is lesser than command this value , go to normal sub-state directly*/
#define VDC_BURST_UNDER                   (VDC_REF-VDC_BURST_UNDER_DELTA)
#define VDC_FEEDFORWARD_COMP_DELTA        5.0   /* During normal mode, when output voltage is larger than command this value, extra feed-forward compensation is added */
#define VDC_FEEDFORWARD_COMP              (VDC_REF+VDC_FEEDFORWARD_COMP_DELTA)
//--------------------------------------------------------------------
//loop sample time                          = 0.0001[sec](10kHz)
//--------------------------------------------------------------------
#define VDC_IIR_B0      FRAC32(0.1367/2) // fc = 1k
#define VDC_IIR_B1      FRAC32(0.1367/2)
#define VDC_IIR_A1      FRAC32(-0.7266/-2)

#if SPLL_EN
#define VDC_KP_GAIN     ACC32(0.06)//ACC32(0.9)/16//
#define VDC_KI_GAIN     ACC32(0.0004)//ACC32(0.007)/16//
#else
#define VDC_KP_GAIN     ACC32(0.056)//ACC32(0.9)/16//
#define VDC_KI_GAIN     ACC32(0.00044)//ACC32(0.007)/16//
#endif

#define VOL_LOOP_ERROR_LIMIT_VALUE 50.0  
#define VOL_LOOP_ERROR_LIMIT       FRAC16(VOL_LOOP_ERROR_LIMIT_VALUE/VDC_SCALE) 

/********************************* Input voltage ********************************/
//--------------------------------------------------------------------
//loop sample time                          = 0.0000125[sec](80kHz)
//--------------------------------------------------------------------
#define VAC_IIR_B0      FRAC32(0.0378/2) 
#define VAC_IIR_B1      FRAC32(0.0378/2)
#define VAC_IIR_A1      FRAC32(-0.9244/-2)

#define VRMS_LOW_TH         FRAC16(140.0/VAC_SCALE)
#define VRMS_HIGH_TH        FRAC16(160.0/VAC_SCALE)
#define VAC_LOWRMS_LOWTH    20 /* input voltage threshold for syn tubes shutdown and different PI parameters when vrms<VRMST_LOW_VALUE */
#define VAC_HIGHRMS_LOWTH   30 /* input voltage threshold for syn tubes shutdown and different PI parameters when vrms>VRMS_HIGH_VALUE */
#define VAC_CHANGTHRESHOLD  FRAC16(40.0/VAC_SCALE) /* VAC change limit for voltage loop correction, when the 
                                                      input voltage change compared with the same angle point in previous
                                                      period is larger than this threshold, voltage loop correction is enabled */

/********************************* Input current control ********************************/
//--------------------------------------------------------------------
//loop sample time                          = 0.0000125[sec](80kHz)
//--------------------------------------------------------------------
#define ISNS_KP_GAIN_LVIN   ACC32(1.28) /* PI parameters when vin is low for zero-crossing soft start */
#define ISNS_KI_GAIN_LVIN   ACC32(0.16)
#define ISNS_KP_GAIN_LVRMS  ACC32(0.544) /*  PI parameters when vrms<VRMS_LOW_TH */
#define ISNS_KI_GAIN_LVRMS  ACC32(0.14)
#define ISNS_KP_GAIN_HVRMS  ACC32(0.544) /*  PI parameters when vrms>VRMS_HIGH_TH */
#define ISNS_KI_GAIN_HVRMS  ACC32(0.192)

#define ISNS_LOOP_ERROR_LIMIT_VALUE 2.0  
#define ISNS_LOOP_ERROR_LIMIT       FRAC16(ISNS_LOOP_ERROR_LIMIT_VALUE/ISNS_SCALE) 

#define LOW_CURRENT         0.25 /* minimum rms value of current reference, fixed current reference rms used in light load mode */
#define HIGH_CURRENT        4 /* maximum rms value of current reference */
#define LOOP_SCALE          (1/ISNS_SCALE)//FRAC16(1/ISNS_SCALE)//0.6767// 
#define IREF_HLIMIT         FRAC16(HIGH_CURRENT*1.414/ISNS_SCALE) /* High limit of Iref, FRAC16(4*1.414/ISNS_SCALE) = 7735 */

#define DUTY_HLIMIT         1.0  /* MAX applicable duty cycle */
#define DUTY_LLIMIT         0    /* MIN applicable duty cycle */
#define ISNS_PI_UP_LIMIT    DUTY_HLIMIT*(PWM_PERIOD_CNT/2-1)*32 /* upper limit of current PI controller output, PI output is 32 times of the final duty */ 
#define ISNS_PI_LOW_LIMIT   DUTY_LLIMIT*(PWM_PERIOD_CNT/2-1)*32 /* lower limit of current PI controller output */

#define DUTYBIGRATIO        (0.3*624)
#define DUTY_OPEN_LOOP      200 /* 62 for 10% PWM duty */

/********************************* relay control ********************************/
#define VDC_OPEN_RELAY           100.0 /* open relay if Vout < VDC_OPEN_RELAY [V] */
#define VDC_CLOSE_RELAY_COEFF    0.8 /* relay can be closed when output voltage is above vin_max multiplied with this coefficient */
#define VDC_CLOSE_RELAY_TO_VRMS_COEFF  ACC32(VDC_CLOSE_RELAY_COEFF*1.414*VAC_TO_VDC)
#define VDC_OPEN_RELAY_WHEN_ACDROP     365.0 /* open relay if Vout is less than this value when ac is drop for energy saving [V] */

/********************************* Fault thresholds *****************************/
#define VDC_HLIMIT          (VDC_REF + 15) /* MAX applicable output voltage[V] */
#define VDC_LLIMIT          300/* MIN applicable output voltage[V], for cooperate with LLC */
#define VAC_RMS_UPLIMIT     261 /* input voltage rms high threshold */
#define VAC_RMS_LOWLIMIT    30 /* input voltage low rms threshold */
#define ISNS_LIMIT          6/* MAX applicable input current[A] */

#define PFC_VIN_FREQOVER_LIMIT    65.0    /* [Hz] Max frequency on input voltage */
#define PFC_VIN_FREQUNDER_LIMIT   45.0    /* [Hz] Min frequency on input voltage */
#define PFC_VIN_FREQOVER_CNT      (uint16_t)(PFC_FASTLOOP_FREQ/PFC_VIN_FREQOVER_LIMIT*500) 
#define PFC_VIN_FREQUNDER_CNT     (uint16_t)(PFC_FASTLOOP_FREQ/PFC_VIN_FREQUNDER_LIMIT*500)

/************************************** Faults indicators *****************************************/
#define VOUT_OVER         0x1 /* Output voltage over happens */
#define IIN_OVER          0x2 /* Software input current over happens */
#define VOUT_HW_OVER      0x4 /* Hardware output voltage over happens */
#define IIN_HW_OVER       0x8 /* Hardware input current over happens */
#define VAC_OVER          0x10 /* Input voltage RMS over happens */
#define VAC_LACK          0x20 /* Input voltage RMS lack happens */
#define FREQ_UNDER        0x40 /* Counter-over within a half cycle happens */
#define FREQ_OVER         0x80 /* Hardware fault happens */
#define CURRENTDISTORTION 0x100 /* When input voltage is positive and current is negative, current distortion happens, and vice versa */
#define VOUT_LACK  0x200

#define FAULT_SET(faults, faultid)    (faults |= faultid)
#define FAULT_CLEAR(faults, faultid)  (faults &= ~faultid)
#define FAULT_ALL_CLEAR(faults)        faults = 0

#define FAULT_RELEASE_DURATION         5000 /* [0.1ms] */

/************************************** Sync tube control *****************************************/
#define TURNOFF_HIGHFRE_SYNC_CURR_VAL   1.3 /* [A] lower limit of current to turn off high frequency arm sync tube */
#define TURNOFF_HIGHFRE_SYNC_CURR       FRAC16(TURNOFF_HIGHFRE_SYNC_CURR_VAL/ISNS_SCALE)

#define TURNON_HIGHFRE_SYNC_CURR_VAL    1.4 /* [A] lower limit of current to turn on high frequency arm sync tube */
#define TURNON_HIGHFRE_SYNC_CURR        FRAC16(TURNON_HIGHFRE_SYNC_CURR_VAL/ISNS_SCALE)

#define TURNOFF_SYNC_REVERSE_CURR_VAL   0.5 /* [A] when the reverse current is greater than this value, turn off sync tubes */
#define TURNOFF_SYNC_REVERSE_CURR       FRAC16(TURNOFF_SYNC_REVERSE_CURR_VAL/ISNS_SCALE)

#define TURNOFF_CONVERTER_REVERSE_CURR_VAL  1.1  /* [A] when the reverse current is greater than this value, the converter enters fault mode */
#define TURNOFF_CONVERTER_REVERSE_CURR      FRAC16(TURNOFF_CONVERTER_REVERSE_CURR_VAL/ISNS_SCALE)

#define TURNON_HIGH_FRE_SYNC_CUR_HIGH_NUM 	5 /* high frequency sync tube won't be enabled until the input current is greater than a certain value for this number of consecutive times */


/********************************* vac detection ********************************/
#define VAC_POLARITY_CONFIRM_NUM   3  /* a  polarity is accepted when this number of consecutive same polarities are detected */
#define INPUT_VRMS_DET_NUM         5  /* PFC starts to work at least this number of vrms have been detected*/

#define ZERO_CROSS_SOFT_START_PWM_CYCLES 10 /* a gradually increasing duty cycle is applied instead of PI controller output for this number of PWM cycles after zero crossing of input voltage */
#define MIN_QUARTER_CYCLE_POINT_COUNTER  320 /* mask interval, ensure the zero crossing detection and sync tube off control are in the last quarter cycle of each half cycle */

#define VAC_FAIL_THRESHOLD         FRAC16(5.0/VAC_SCALE) /* ac drop judgement threshold */ 
#define VAC_FAIL_CONFIRM_NUM 	   15 /*when ac sampling value is less than the drop threshold for this 
                                        number of consecutive times, the input voltage is considered power down. */
#define VAC_RECOVERY_THRESHOLD     FRAC16(100.0/VAC_SCALE)
#define VAC_RECOVERY_CONFIRM_NUM   5  /*when ac sampling value is larger than the recovery threshold for
                                        this number of consecutive times, the input voltage is considered power up again. */

/********************************* vac jump correction ********************************/
#define VAC_JUMP_COR_EN_DURATION   200 /* [0.1ms] maximum duration of voltage loop correction work */
#define VAC_JUMP_COR_DIS_DURATION  1000 /* [0.1ms] voltage loop correction will not be enabled again until this time pass away */
#define F16K1 		110
#define F16K2 		5

/********************************* SPLL parameters ********************************/
#if SPLL_EN
#define SPLL_SOGI_LF_B0     ACC32(3.3342)//ACC32(2.2228)//ACC32(0.5557)
#define SPLL_SOGI_LF_B1     ACC32(-3.3306)//ACC32(-2.2204)//ACC32(-0.5551)
#define SPLL_CTRL_FREQ             80000.0 /* [Hz] SPLL run frequency */
#define SOGI_K_COEFF               0.5 /* second order generalized integrator(SOGI) based orthogonal system coefficient 
                                         the smaller the coefficient, the narrower the bandpass and the slower the dynamic response*/
#endif

/********************************* peripheral configurations ********************************/
#define RELAY_STATE()    GPIOF_DR & 0x80
#define CLOSE_RELAY()    GPIOF_DR |= 0x80
#define OPEN_RELAY()     GPIOF_DR &= ~0x80

#define PWMA_RUN()               PWMA_MCTRL |= PWM_MCTRL_RUN(7) /*Enable PWM generator clock*/
#define PWMA_STOP()              PWMA_MCTRL &= ~PWM_MCTRL_RUN(7) /*Disable PWM generator clock*/
#define ENABLE_DUTYLOAD          PWMA_MCTRL |= PWM_MCTRL_LDOK(5)
#define INVERTOUTPUTPOLARITY     PWMA_SM0OCTRL |= 0x600/*Inverse polarity of PWM output in input negative cycle*/ 
#define RESETOUTPUTPOLARITY      PWMA_SM0OCTRL &= ~0x600/*Not invert polarity of PWM output in input positive cycle*/ 
#define PWMFAULTSTATE_0          PWMA_SM0OCTRL &= ~(PWM_OCTRL_PWMAFS_MASK|PWM_OCTRL_PWMBFS_MASK)
#define PWMFAULTSTATE_1          PWMA_SM0OCTRL = ((PWMA_SM0OCTRL & ~(PWM_OCTRL_PWMAFS_MASK|PWM_OCTRL_PWMBFS_MASK)) | 0x14)  

/************* operation for low speed sync MOS *************/
#define LOWERMOSFET_STATE       (GPIOF_DR & 0x10)
#define HIGHERMOSFET_STATE      (GPIOF_DR & 0x20)
#define SETLOWERMOSFET_HIGH     GPIOF_DR |= GPIO_DR_D(0x10) 
#define SETLOWERMOSFET_LOW      GPIOF_DR &= ~GPIO_DR_D(0x10)
#define SETHIGHERMOSFET_HIGH    GPIOF_DR |= GPIO_DR_D(0x20)
#define SETHIGHERMOSFET_LOW     GPIOF_DR &= ~GPIO_DR_D(0x20)
#define DISABLE_ALLMOSFET()     GPIOF_DR &= ~GPIO_DR_D(0x30)
/************* operation for high speed GaN *************/
#define DISABLE_UPGAN()          AOI_BFCRT011 &= ~AOI_BFCRT01_PT0_DC_MASK/*Disable up-side GaN MOSFET.*/
#define ENABLE_UPGAN()           AOI_BFCRT011 |= AOI_BFCRT01_PT0_DC_MASK/*Enable up-side GaN MOSFET.*/  
#define ENABLE_DOWNGAN()         AOI_BFCRT010 |= AOI_BFCRT01_PT0_DC_MASK /*Enable low-side GaN MOSFET.*/
#define DISABLE_DOWNGAN()        AOI_BFCRT010 &= ~AOI_BFCRT01_PT0_DC_MASK /*Disable low-side GaN MOSFET.*/
#define DISABLE_ALL_GAN()        {AOI_BFCRT011 &= ~AOI_BFCRT01_PT0_DC_MASK;\
                                  AOI_BFCRT010 &= ~AOI_BFCRT01_PT0_DC_MASK;}
#define UPGAN_STATE              (AOI_BFCRT011 & AOI_BFCRT01_PT0_DC_MASK)
#define DOWNGAN_STATE            (AOI_BFCRT010 & AOI_BFCRT01_PT0_DC_MASK)


#define DUTY_UPDATE(Duty)        {PWMA_SM0VAL3 = Duty; PWMA_SM0VAL2 = -Duty;}//Update PWM VAL2&3 registers

#define CLR_PWM1_ISR_FLAG           PWMA_SM1STS |= PWM_STS_CMPF(1)
#define CLR_PWM0_ISR_FLAG           PWMA_SM0STS |= PWM_STS_CMPF(1)

#define ENABLE_HARDWARE_CURRENT_PROTECTION()   PWMA_SM0DISMAP0 |= (PWM_DISMAP_DIS0A(3) | PWM_DISMAP_DIS0B(3))//Enable Hardware current protection
#define DISABLE_HARDWARE_CURRENT_PROTECTION()  PWMA_SM0DISMAP0 &= ~(PWM_DISMAP_DIS0A(3)| PWM_DISMAP_DIS0B(3)) //disable Hardware current protection
#define HW_FAULT()                     PWMA_FSTS0 & PWM_FSTS_FFLAG(7)
#define IIN_HW_FAULT()                 PWMA_FSTS0 & PWM_FSTS_FFLAG(3)
#define VOUT_HW_FAULT()                PWMA_FSTS0 & PWM_FSTS_FFLAG(4)
#define HW_FAULT_FLAG_CLEAR()  	       PWMA_FSTS0 |= PWM_FSTS_FFLAG(7)

/************************ polarity detection ************************/ 
#define POLARITY(pol)  if(GPIOC_DR & 0x10) pol = 1; else pol = 0;

/***************** Test point settings *****************/
#define TP9_UP()      GPIOC_DR |= 0x2000
#define TP9_DOWN()    GPIOC_DR &= ~0x2000
#define TP11_UP()     GPIOE_DR |= 0x80
#define TP11_DOWN()   GPIOE_DR &= ~0x80
#endif /* PFC_CTRL_H_ */
