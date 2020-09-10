#ifndef DA7212_H
#define DA7212_H

#include "i2c.h"

#define     I2C_WRITE_ADDRESS               0x34

#define 	CIF_CTRL                        0x1D
#define 	CIF_CTRL_Data                   0x00

#define 	DIG_ROUTING_DAI                 0x21
#define 	DIG_ROUTING_DAI_Data            0x10

#define 	SR                              0x22
#define 	SR_Data                         0x0A

#define 	REFERENCES                      0x23
#define 	REFERENCES_Data                 0x08

#define 	PLL_FRAC_TOP                    0x24
#define 	PLL_FRAC_TOP_Data               0x00

#define 	PLL_FRAC_BOT                    0x25
#define 	PLL_FRAC_BOT_Data               0x00

#define 	PLL_INTEGER                     0x26
#define 	PLL_INTEGER_Data                0x20

#define 	PLL_CTRL                        0x27
#define 	PLL_CTRL_Data                   0x44

#define 	DAI_CLK_MODE                    0x28
#define 	DAI_CLK_MODE_Data               0x00

#define 	DAI_CTRL                        0x29
#define 	DAI_CTRL_Data                   0x00

#define 	DIG_ROUTING_DAC                 0x2A
#define 	DIG_ROUTING_DAC_Data            0x10

#define 	ALC_CTRL1                       0x2B
#define 	ALC_CTRL1_Data                  0x88

#define 	AUX_L_GAIN                      0x30
#define 	AUX_L_GAIN_Data                 0x35

#define 	AUX_R_GAIN                      0x31
#define 	AUX_R_GAIN_Data                 AUX_L_GAIN_Data

#define 	MIXIN_L_SELECT                  0x32
#define 	MIXIN_L_SELECT_Data             0x02

#define 	MIXIN_R_SELECT                  0x33
#define 	MIXIN_R_SELECT_Data             MIXIN_L_SELECT_Data

#define 	MIXIN_L_GAIN                    0x34
#define 	MIXIN_L_GAIN_Data               0x05

#define 	MIXIN_R_GAIN                    0x35
#define 	MIXIN_R_GAIN_Data               MIXIN_L_GAIN_Data

#define 	ADC_L_GAIN                      0x36
#define 	ADC_L_GAIN_Data                 0x6F

#define 	ADC_R_GAIN                      0x37
#define 	ADC_R_GAIN_Data                 ADC_L_GAIN_Data

#define 	ADC_FILTERS1                    0x38
#define 	ADC_FILTERS1_Data               0x80

#define 	MIC_1_GAIN                      0x39
//#define 	MIC_1_GAIN_Data                 0x04//xv3
#define 	MIC_1_GAIN_Data                 0x01//xv2

#define 	MIC_2_GAIN                      0x3A
#define 	MIC_2_GAIN_Data                 MIC_1_GAIN_Data

#define 	DAC_FILTERS5                    0x40
#define 	DAC_FILTERS5_Data               0x00

#define 	DAC_FILTERS2                    0x41
#define 	DAC_FILTERS2_Data               0x77

#define 	DAC_FILTERS3                    0x42
#define 	DAC_FILTERS3_Data               0x77

#define 	DAC_FILTERS4                    0x43
#define 	DAC_FILTERS4_Data               0x07

#define 	DAC_FILTERS1                    0x44
#define 	DAC_FILTERS1_Data               0x00

#define 	DAC_L_GAIN                      0x45
//#define 	DAC_L_GAIN_Data                 0x61//xv3
#define 	DAC_L_GAIN_Data                 0x6F//xv2

#define 	DAC_R_GAIN                      0x46
#define 	DAC_R_GAIN_Data                 DAC_L_GAIN_Data

#define 	CP_CTRL                         0x47
#define 	CP_CTRL_Data                    0xCD

#define 	HP_L_GAIN                       0x48
#define 	HP_L_GAIN_Data                  0x2F

#define 	HP_R_GAIN                       0x49
#define 	HP_R_GAIN_Data                  HP_L_GAIN_Data

#define 	LINE_GAIN                       0x4A
#define 	LINE_GAIN_Data                  0x00

#define     MIXOUT_L_SELECT                 0x4B
#define     MIXOUT_L_SELECT_Data            0x09

#define     MIXOUT_R_SELECT                 0x4C
#define     MIXOUT_R_SELECT_Data            0x09

#define     SYSTEM_MODES_INPUT              0x50
#define     SYSTEM_MODES_INPUT_Data         0x0e

#define     SYSTEM_MODES_OUTPUT             0x51
#define     SYSTEM_MODES_OUTPUT_Data        0X00

#define 	AUX_L_CTRL                      0x60
#define 	AUX_L_CTRL_Data                 0x80

#define 	AUX_R_CTRL                      0x61
#define 	AUX_R_CTRL_Data                 0x80

#define 	MICBIAS_CTRL                    0x62
#define 	MICBIAS_CTRL_Data               0x99

#define 	MIC_1_CTRL                      0x63
#define 	MIC_1_CTRL_Data                 0x88

#define 	MIC_2_CTRL                      0x64
#define 	MIC_2_CTRL_Data                 0x88

#define 	MIXIN_L_CTRL                    0x65
#define 	MIXIN_L_CTRL_Data               0xA8

#define 	MIXIN_R_CTRL                    0x66
#define 	MIXIN_R_CTRL_Data               0xA8

#define 	ADC_L_CTRL                      0x67
#define 	ADC_L_CTRL_Data                 0xa0

#define 	ADC_R_CTRL                      0x68
#define 	ADC_R_CTRL_Data                 0xa0

#define 	DAC_L_CTRL                      0x69
#define 	DAC_L_CTRL_Data                 0xA0

#define 	DAC_R_CTRL                      0x6A
#define 	DAC_R_CTRL_Data                 0xA0

#define 	HP_L_CTRL                       0x6B
#define 	HP_L_CTRL_Data                  0xA8

#define 	HP_R_CTRL                       0x6C
#define 	HP_R_CTRL_Data                  0xA8

#define 	LINE_CTRL                       0x6D
#define 	LINE_CTRL_Data                  0x00

#define 	MIXOUT_L_CTRL                   0x6E
#define 	MIXOUT_L_CTRL_Data              0x88

#define 	MIXOUT_R_CTRL                   0x6F
#define 	MIXOUT_R_CTRL_Data              0x88

#define 	MIXED_SAMPLE_MODE               0x84
#define 	MIXED_SAMPLE_MODE_Data          0x00

#define 	LDO_CTRL                        0x90
#define 	LDO_CTRL_Data                   0xB0

#define 	IO_CTRL                         0x91
#define 	IO_CTRL_Data                    0x00

#define 	GAIN_RAMP_CTRL                  0x92
#define 	GAIN_RAMP_CTRL_Data             0x00

#define 	MIC_CONFIG                      0x93
#define 	MIC_CONFIG_Data                 0x00

#define 	PC_COUNT                        0x94
#define 	PC_COUNT_Data                   0x01

#define 	CP_VOL_THRESHOLD1               0x95
#define 	CP_VOL_THRESHOLD1_Data          0x36

#define 	CP_DELAY                        0x96
#define 	CP_DELAY_Data                   0xa5

#define 	CP_DETECTOR                     0x97
#define 	CP_DETECTOR_Data                0x00

#define 	DAI_OFFSET                      0x98
#define 	DAI_OFFSET_Data                 0x00

#define 	DIG_CTRL                        0x99
#define 	DIG_CTRL_Data                   0x00

#define 	ALC_CTRL2                       0x9A
#define 	ALC_CTRL2_Data                  0x50

#define 	ALC_CTRL3                       0x9B
#define 	ALC_CTRL3_Data                  0x52

#define 	ALC_NOISE                       0x9C
#define 	ALC_NOISE_Data                  0x3F

#define 	ALC_TARGET_MIN                  0x9D
//#define 	ALC_TARGET_MIN_Data             0x0e
#define 	ALC_TARGET_MIN_Data             0x12//modify 20200108
//#define 	ALC_TARGET_MIN_Data             0x13//modify jim 20200403

#define 	ALC_THRESHOLD_MAX               0x9E
//#define 	ALC_THRESHOLD_MAX_Data          0x0d
#define 	ALC_THRESHOLD_MAX_Data          0x11//modify 20200108
//#define 	ALC_THRESHOLD_MAX_Data          0x12//modify jim 20200403


#define 	ALC_GAIN_LIMITS                 0x9F
//#define 	ALC_GAIN_LIMITS_Data            0x48//xv3
#define 	ALC_GAIN_LIMITS_Data            0x23//xv2
//#define 	ALC_GAIN_LIMITS_Data            0x36//modify 20200108

#define 	ALC_ANA_GAIN_LIMITS             0xA0
#define     ALC_ANA_GAIN_LIMITS_Data        0x44

#define 	ALC_ANTICLIP_CTRL               0xA1
#define     ALC_ANTICLIP_CTRL_Data          0x00

#define 	ALC_ANTICLIP_LEVEL              0xA2
#define     ALC_ANTICLIP_LEVEL_Data         0x00

#define 	DAC_NG_SETUP_TIME               0xAF
#define     DAC_NG_SETUP_TIME_Data          0x00

#define 	DAC_NG_OFF_THRESHOLD            0xB0
#define     DAC_NG_OFF_THRESHOLD_Data       0x00

#define 	DAC_NG_ON_THRESHOLD             0xB1
#define     DAC_NG_ON_THRESHOLD_Data        0x00

#define 	DAC_NG_CTRL                     0xB2
#define 	DAC_NG_CTRL_Data                0x00

#define 	TONE_GEN_CFG1                   0xB4
#define 	TONE_GEN_CFG1_Data              0x00

#define 	TONE_GEN_CFG2                   0xB5
#define 	TONE_GEN_CFG2_Data              0x00

#define 	TONE_GEN_CYCLES                 0xB6
#define 	TONE_GEN_CYCLES_Data            0x00

#define 	TONE_GEN_FREQ1_L                0xB7
#define 	TONE_GEN_FREQ1_L_Data           0x00

#define 	TONE_GEN_FREQ1_U                0xB8
#define 	TONE_GEN_FREQ1_U_Data           0x00

#define 	TONE_GEN_FREQ2_L                0xB9
#define 	TONE_GEN_FREQ2_L_Data           0x00

#define 	TONE_GEN_FREQ2_U                0xBA
#define 	TONE_GEN_FREQ2_U_Data           0x00

#define 	TONE_GEN_ON_PER                 0xBB
#define 	TONE_GEN_ON_PER_Data            0x00

#define 	TONE_GEN_OFF_PER                0xBC
#define 	TONE_GEN_OFF_PER_Data           0x00

#define 	SYSTEM_ACTIVE                   0xFD
#define 	SYSTEM_ACTIVE_Data              0x01

void DA7212_Init(void);
void Volume_Up(void);
void Volume_Down(void);

void DA7212_Mute_HP(void);
void DA7212_CloseMute_HP(void);


void DA7212_Power_Off(void);
void DA7212_UpdateEQ(void);
void DA7212_EQ_Mode_Up(void);

void DA7212_Mute(void);
void DA7212_Close_Mute(void);

void Tone_Vol_Up(void);
void Tone_Vol_Max(uint8 cycle);
void Tone_Vol_Down(void);
void Tone_Vol_Min(uint8 cycle);

void Open_ADC_DAC(void);

void DA7212_UNIVERSAL_Mode(void);
void DA7212_CLEARVOICE_Mode(void);
void DA7212_HIGH_FREQUENCY_BOOST_Mode(void);
void DA7212_POWER_BOOST_Mode(void);

void App_control_DA7212_Vol(void);

extern uint8 DA7212_VOL;
extern uint8 DA7212_EQ;
extern uint8 DA7212_Power_Flag;


void AVRCP_Update_Sleeptime(void);
#define Two_hour 	2
#define Four_hour	4
#define Six_hour	6


void DA7212_PlayEQVoicePrompt_Delay(void);

void DA7212_UpdateEQ_PlayVoice(void);
void DA7212_InitEQ(void);


void Remember_Vol0_Status(void);

void AVRCP_Update_Sleeptime_NoneVoiceTone(void);



#endif // DA7212_H
