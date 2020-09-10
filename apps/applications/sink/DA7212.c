#include "DA7212.h"
#include "sink_debug.h"
#include "sink_main_task.h"
#include "sink_events.h"
#include "sink_private_data.h"

#define DUCO_Vol
#define Init_DA7212HP


#ifdef DEBUG_DA7212
    #define DA7212_DEBUG(x) DEBUG(x)
#else
    #define DA7212_DEBUG(x)
#endif

uint8 cnt;

//static const uint8 MIXIN_1_AMP_GAIN[]={0x00,/*0x01,*/0x02,0x03,0x04,0x05,0x06,0x07,0x08, 0x09, 0x0A, 0x0B, 0x0C ,0x0D, 0x0E,0x0F};
static const uint8 MIXIN_1_AMP_GAIN[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08, 0x09, 0x0A, 0x0B, 0x0C ,0x0D, 0x0E/*,0x0F*/};// jim 20200403
//static const uint8 MIXIN_1_AMP_GAIN[]={0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08, 0x09, 0x0A, 0x0B, 0x0C ,0x0c/*, 0x0E,0x0F*/};// jim 20200403

void DA7212_Init(void)
{
    DA7212_DEBUG(("HS:DA7212_Power_On\n"));

	uint8 test[2];

	test[0]=CIF_CTRL;//0x1d
	test[1]=0x80;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("1:%d \n",cnt));

	test[0]=DIG_ROUTING_DAI ;//0x21
	test[1]=DIG_ROUTING_DAI_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("2:%d \n",cnt));

	test[0]=0x21;//DIG_ROUTING_DAI
	test[1]=0x10;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("4:%d \n",cnt));

	test[0]=SR;//0x22
	test[1]=SR_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("5:%d \n",cnt));

	test[0]=REFERENCES;//0x23
	test[1]=REFERENCES_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("6:%d \n",cnt));

	test[0]=PLL_FRAC_TOP;//0x24
	test[1]=PLL_FRAC_TOP_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("7:%d \n",cnt));

	test[0]=PLL_FRAC_BOT;//0x25
	test[1]=PLL_FRAC_BOT_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("8:%d \n",cnt));

	test[0]=PLL_INTEGER;//0x26
	test[1]=PLL_INTEGER_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("9:%d \n",cnt));

	test[0]=PLL_CTRL;//0x27
	test[1]=PLL_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("10:%d \n",cnt));


	/* DAI*/
	test[0]=DAI_CLK_MODE;//0x28
	test[1]=DAI_CLK_MODE_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("11:%d \n",cnt));

	test[0]=DAI_CTRL;//0x29
	test[1]=DAI_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("12:%d \n",cnt));

	test[0]=DIG_ROUTING_DAC;
	test[1]=DIG_ROUTING_DAC_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("13:%d \n",cnt));

	test[0]=ALC_CTRL1;
	test[1]=ALC_CTRL1_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("14:%d \n",cnt));

	/*0x30*/
	test[0]=AUX_L_GAIN;
	test[1]=AUX_L_GAIN_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("15:%d \n",cnt));

	test[0]=AUX_R_GAIN;
	test[1]=AUX_R_GAIN_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("16:%d \n",cnt));

	test[0]=MIXIN_L_SELECT;
	test[1]=MIXIN_L_SELECT_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("17:%d \n",cnt));

	test[0]=MIXIN_R_SELECT;
	test[1]=MIXIN_R_SELECT_Data;
	I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("18:%d \n",cnt));


    test[0]=MIXIN_L_GAIN;
    test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];//20181225am
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("19:%d \n",cnt));

    test[0]=MIXIN_R_GAIN;
    test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];//20181225am
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("20:%d \n",cnt));


	test[0]=ADC_L_GAIN;
	test[1]=ADC_L_GAIN_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("21:%d \n",cnt));

	test[0]=ADC_R_GAIN;
	test[1]=ADC_R_GAIN_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("22:%d \n",cnt));

	test[0]=ADC_FILTERS1;
	test[1]=ADC_FILTERS1_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("23:%d \n",cnt));

	test[0]=MIC_1_GAIN;
	test[1]=MIC_1_GAIN_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("24:%d \n",cnt));

	test[0]=MIC_2_GAIN;
	test[1]=MIC_2_GAIN_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("25:%d \n",cnt));

	/*0x40*/	
	test[0]=DAC_L_GAIN;
	test[1]=DAC_L_GAIN_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("26:%d \n",cnt));

	test[0]=DAC_R_GAIN;
	test[1]=DAC_R_GAIN_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("27:%d \n",cnt));

	test[0]=CP_CTRL;
	test[1]=CP_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("28:%d \n",cnt));

	test[0]=HP_L_GAIN;
	test[1]=HP_L_GAIN_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("29:%d \n",cnt));

	test[0]=HP_R_GAIN;
	test[1]=HP_R_GAIN_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("30:%d \n",cnt));

	test[0]=LINE_GAIN;
	test[1]=LINE_GAIN_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("31:%d \n",cnt));

	test[0]=MIXOUT_L_SELECT;
	test[1]=MIXOUT_L_SELECT_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("32:%d \n",cnt));

	test[0]=MIXOUT_R_SELECT;
	test[1]=MIXOUT_R_SELECT_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("33:%d \n",cnt));

	/*0x50*/
	test[0]=SYSTEM_MODES_INPUT;
	test[1]=SYSTEM_MODES_INPUT_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("34:%d \n",cnt));

	test[0]=SYSTEM_MODES_OUTPUT;
	test[1]=SYSTEM_MODES_OUTPUT_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("35:%d \n",cnt));

	/*0x60*/
	test[0]=AUX_L_CTRL;
	test[1]=AUX_L_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("36:%d \n",cnt));

	test[0]=AUX_R_CTRL;
	test[1]=AUX_R_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("37:%d \n",cnt));

	test[0]=MICBIAS_CTRL;
	test[1]=MICBIAS_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("38:%d \n",cnt));

	test[0]=MIC_1_CTRL;
	test[1]=MIC_1_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("39:%d \n",cnt));

	test[0]=MIC_2_CTRL;
	test[1]=MIC_2_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("40:%d \n",cnt));

	test[0]=MIXIN_L_CTRL;
	test[1]=MIXIN_L_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("41:%d \n",cnt));

	test[0]=MIXIN_R_CTRL;
	test[1]=MIXIN_R_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("42:%d \n",cnt));

	test[0]=ADC_L_CTRL;
	test[1]=ADC_L_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("43:%d \n",cnt));

	test[0]=ADC_R_CTRL;
	test[1]=ADC_R_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("44:%d \n",cnt));

	test[0]=DAC_L_CTRL;
	test[1]=DAC_L_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("45:%d \n",cnt));

	test[0]=DAC_R_CTRL;
	test[1]=DAC_R_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("46:%d \n",cnt));

#ifdef Init_DA7212HP
	test[0]=HP_L_CTRL;
	test[1]=HP_L_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("47:%d \n",cnt));

	test[0]=HP_R_CTRL;
	test[1]=HP_R_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("48:%d \n",cnt));
	
#else 
	test[0]=HP_L_CTRL;
	test[1]=0xe1;
	cnt=I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("47:%d \n",cnt));

	test[0]=HP_R_CTRL;
	test[1]=0xe0;
	cnt=I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("48:%d \n",cnt));
#endif

	test[0]=LINE_CTRL;
	test[1]=LINE_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("49:%d \n",cnt));

	test[0]=MIXOUT_L_CTRL;
	test[1]=MIXOUT_L_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("50:%d \n",cnt));

	test[0]=MIXOUT_R_CTRL;
	test[1]=MIXOUT_R_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("51:%d \n",cnt));

	/*0X80*/
	test[0]=MIXED_SAMPLE_MODE;
	test[1]=MIXED_SAMPLE_MODE_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("52:%d \n",cnt));

	/*0X90*/
	test[0]=LDO_CTRL;
	test[1]=LDO_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("53:%d \n",cnt));

	test[0]=IO_CTRL;
	test[1]=IO_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("54:%d \n",cnt));

	test[0]=GAIN_RAMP_CTRL;
	test[1]=GAIN_RAMP_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("55:%d \n",cnt));

	test[0]=MIC_CONFIG;
	test[1]=MIC_CONFIG_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("56:%d \n",cnt));

	test[0]=PC_COUNT;
	test[1]=PC_COUNT_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("57:%d \n",cnt));

	test[0]=CP_VOL_THRESHOLD1;
	test[1]=CP_VOL_THRESHOLD1_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("58:%d \n",cnt));

	test[0]=CP_DELAY;
	test[1]=CP_DELAY_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("59:%d \n",cnt));

	test[0]=CP_DETECTOR ;
	test[1]=CP_DETECTOR_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("60:%d \n",cnt));

	/* DAI*/
	test[0]=DAI_OFFSET;
	test[1]=0x00;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("61:%d \n",cnt));

	test[0]=DIG_CTRL;
	test[1]=0x00;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("62:%d \n",cnt));

	test[0]=ALC_CTRL2;
	test[1]=ALC_CTRL2_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("63:%d \n",cnt));

	test[0]=ALC_CTRL3;
	test[1]=ALC_CTRL3_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("64:%d \n",cnt));

	test[0]=ALC_NOISE;
	test[1]=ALC_NOISE_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("65:%d \n",cnt));

	test[0]=ALC_TARGET_MIN;
	test[1]=ALC_TARGET_MIN_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("66:%d \n",cnt));

	test[0]=ALC_THRESHOLD_MAX;
	test[1]=ALC_THRESHOLD_MAX_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("67:%d \n",cnt));

	test[0]=ALC_GAIN_LIMITS;
	test[1]=ALC_GAIN_LIMITS_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("68:%d \n",cnt));

	/*0xA0*/
	test[0]=ALC_ANA_GAIN_LIMITS;
	test[1]=ALC_ANA_GAIN_LIMITS_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("69:%d \n",cnt));

	test[0]=ALC_ANTICLIP_CTRL;
	test[1]=ALC_ANTICLIP_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("70:%d \n",cnt));

	test[0]=ALC_ANTICLIP_LEVEL;
	test[1]=ALC_ANTICLIP_LEVEL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("71:%d \n",cnt));

	test[0]=DAC_NG_SETUP_TIME;
	test[1]=DAC_NG_SETUP_TIME_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("72:%d \n",cnt));

	/*0xB0*/
	test[0]=DAC_NG_OFF_THRESHOLD;
	test[1]=DAC_NG_OFF_THRESHOLD_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("73:%d \n",cnt));

	test[0]=DAC_NG_ON_THRESHOLD;
	test[1]=DAC_NG_ON_THRESHOLD_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("74:%d \n",cnt));

	test[0]=DAC_NG_CTRL;
	test[1]=DAC_NG_CTRL_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("75:%d \n",cnt));

	/* TONE*/
	test[0]=TONE_GEN_CFG1;
	test[1]=TONE_GEN_CFG1_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("76:%d \n",cnt));

	test[0]=TONE_GEN_CFG2;
	test[1]=TONE_GEN_CFG2_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("77:%d \n",cnt));

	test[0]=TONE_GEN_CYCLES;
	test[1]=TONE_GEN_CYCLES_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("78:%d \n",cnt));

	test[0]=TONE_GEN_FREQ1_L;
	test[1]=TONE_GEN_FREQ1_L_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("79:%d \n",cnt));

	test[0]=TONE_GEN_FREQ1_U;
	test[1]=TONE_GEN_FREQ1_U_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("80:%d \n",cnt));

	test[0]=TONE_GEN_FREQ2_L;
	test[1]=TONE_GEN_FREQ2_L_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("81:%d \n",cnt));

	test[0]=TONE_GEN_FREQ2_U;
	test[1]=TONE_GEN_FREQ2_U_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("82:%d \n",cnt));

	test[0]=TONE_GEN_ON_PER;
	test[1]=TONE_GEN_ON_PER_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("83:%d \n",cnt));

	test[0]=TONE_GEN_OFF_PER;
	test[1]=TONE_GEN_OFF_PER_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("84:%d \n",cnt));

	DA7212_UpdateEQ_PlayVoice();

	/*0XF0*/
	test[0]=SYSTEM_ACTIVE;
	test[1]=SYSTEM_ACTIVE_Data;
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	//DA7212_DEBUG(("85:%d \n",cnt));
}

void Remember_Vol0_Status(void)
{
    //uint8 test[2];
    //DA7212_PlayEQVoicePrompt_3s();

#if 0
    test[0]=MIXIN_L_GAIN;
    test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];//20181225am
    cnt=I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
    //DA7212_DEBUG(("19:%d \n",cnt));

    test[0]=MIXIN_R_GAIN;
    test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];//20181225am
    cnt=I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
    //DA7212_DEBUG(("20:%d \n",cnt));
#endif

	if(My_theSink.DA7212_Vol==0)
	{
		DA7212_Mute();	
	}
	
}

void Volume_Up(void)
{
	uint8 test[2];
	if(My_theSink.DA7212_Vol==14)
	{
        MessageSendLater(&theSink.task, EventSysTone1, 0, 0);//Tone_Vol_Max
        //DA7212_DEBUG(("volmax_info:%d\n",My_theSink.DA7212_Vol));
		
		test[0]=MIXIN_L_GAIN;
		test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
		I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
		
		test[0]=MIXIN_R_GAIN;
		test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
		I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);		

	}
	
	if(My_theSink.DA7212_Vol<14)
	{
		My_theSink.DA7212_Vol++;
        //DA7212_DEBUG(("volup++_info:%d\n",My_theSink.DA7212_Vol));
		switch(My_theSink.DA7212_Vol)
		{
			case 0:
			case 1:
					DA7212_Close_Mute();//enable input channel					
                    MessageSendLater(&theSink.task, EventSysTone2, 0, 0);//Tone_Vol_Up
                    //DA7212_DEBUG(("vol_0-1_info:%d\n",My_theSink.DA7212_Vol));
				break;

			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:					
                    MessageSendLater(&theSink.task, EventSysTone2, 0, 0);//Tone_Vol_Up
                    //DA7212_DEBUG(("vol_2-14_info:%d\n",My_theSink.DA7212_Vol));
				break;

			case 14:
                    MessageSendLater(&theSink.task, EventSysTone1, 0, 0);//Tone_Vol_Max
                    //DA7212_DEBUG(("volmax_info:%d\n",My_theSink.DA7212_Vol));
				break;
				
			default:
				break;
		}
		
        test[0]=MIXIN_L_GAIN;
        test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

        test[0]=MIXIN_R_GAIN;
        test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

        //DA7212_DEBUG(("vol_up_info:%d\n",My_theSink.DA7212_Vol));

	}

    //DA7212_DEBUG(("HS:void Volume_Up(void)\n"));
}

void Volume_Down(void)
{
	uint8 test[2];
	if(My_theSink.DA7212_Vol==0)
	{
        test[0]=MIXIN_L_GAIN;
        test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

        test[0]=MIXIN_R_GAIN;
        test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);	
        
        DA7212_Mute();//mute input channel
        MessageSendLater(&theSink.task, EventSysToneDigit8, 0, 0);//Tone_Vol_Min
	}
	if(My_theSink.DA7212_Vol>0)
	{
		My_theSink.DA7212_Vol--;
        //DA7212_DEBUG(("vol--_info:%d\n",My_theSink.DA7212_Vol));
        switch(My_theSink.DA7212_Vol)
		{
			case 0:		
					DA7212_Mute();//mute input channel
                    MessageSendLater(&theSink.task, EventSysToneDigit8, 0, 0);//Tone_Vol_Min
                    //DA7212_DEBUG(("vol_0__info:%d\n",My_theSink.DA7212_Vol));
				break;
				
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
                    MessageSendLater(&theSink.task, EventSysToneDigit9, 0, 0);//Tone_Vol_Down
                    //DA7212_DEBUG(("vol_1-14__info:%d\n",My_theSink.DA7212_Vol));
				break;

			default:
				break;
			
		}
		
		test[0]=MIXIN_L_GAIN;
        test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

        test[0]=MIXIN_R_GAIN;
        test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
        I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

        //DA7212_DEBUG(("vol_down_info:%d\n",My_theSink.DA7212_Vol));

	}

    //DA7212_DEBUG(("HS:void Volume_Down(void)\n"));
}


void DA7212_Mute_HP(void)
{
    uint8 test[2];

    //DA7212_Mute();
    
    test[0]=0x6B;
    test[1]=0x61;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);//output mute
    
    test[0]=0x6C;
    test[1]=0x60;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);//output mute   
	
    //DA7212_DEBUG(("HS:void DA7212_Mute_HP(void)\n"));
}

void DA7212_CloseMute_HP(void)
{
	uint8 test[2];
    
    test[0]=0x6B;
    test[1]=0xA8;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);//output mute
    
    test[0]=0x6C;
    test[1]=0xA8;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);//output mute 
     
    //DA7212_DEBUG(("HS:void DA7212_CloseMute_HP(void)\n"));
}

void DA7212_Power_Off(void)
{
    uint8 test[2];
    test[0]=0x23;
    test[1]=0x00;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    //DA7212_DEBUG(("HS:void DA7212_Power_Off(void)\n"));
}

//function 3s play mode voice prompts
void DA7212_PlayEQVoicePrompt_Delay(void)
{
	switch(My_theSink.DA7212_EQ)
	{
            case 0:
                    MessageSendLater(&theSink.task, EventSysToneDigit0, 0, 0);//voice prompt universal 
                    //DA7212_DEBUG(("DA7212_EQ_HS: universal mode\n") ) ;
                break;

            case 1:
                    MessageSendLater(&theSink.task, EventSysToneDigit1, 0, 0);//voice prompt clear voice 
                    //DA7212_DEBUG(("DA7212_EQ_HS: clear voice mode\n") ) ;
                break;

            case 2:
                    MessageSendLater(&theSink.task, EventSysToneDigit2, 0, 0);//voice prompt high frequency boost
                    //DA7212_DEBUG(("DA7212_EQ_HS: high frequency boost mode\n") ) ;
                break;

            case 3:
                    MessageSendLater(&theSink.task, EventSysToneDigit3, 0, 0);//voice prompt power boost
                    //DA7212_DEBUG(("DA7212_EQ_HS: power boost mode\n") ) ;
                break;

            default:
                break;	
    }

    //DA7212_DEBUG(("HS:void DA7212_UpdateEQ_3s(void)\n"));
}

//function enter EQ but no voice prompts only for init
void DA7212_InitEQ(void)
{    
	switch(My_theSink.DA7212_EQ)
	{
            case 0:
                    DA7212_UNIVERSAL_Mode();
                    //DA7212_DEBUG(("DA7212_EQ_HS: universal mode\n") ) ;
                break;

            case 1:
                    DA7212_CLEARVOICE_Mode();
                    //DA7212_DEBUG(("DA7212_EQ_HS: clear voice mode\n") ) ;
                break;

            case 2:
                    DA7212_HIGH_FREQUENCY_BOOST_Mode();
                    //DA7212_DEBUG(("DA7212_EQ_HS: high frequency boost mode\n") ) ;
                break;

            case 3:
                    DA7212_POWER_BOOST_Mode();
                    //DA7212_DEBUG(("DA7212_EQ_HS: power boost mode\n") ) ;
                break;

            default:
                break;
	}	
    //DA7212_DEBUG(("HS:void DA7212_UpdateEQ(void)\n"));
}

void DA7212_UpdateEQ(void)
{    
	switch(My_theSink.DA7212_EQ)
	{
            case 0:
                    DA7212_UNIVERSAL_Mode();
                    MessageSendLater(&theSink.task, EventSysToneDigit0, 0, 0);//voice prompt universal
                    //DA7212_DEBUG(("DA7212_EQ_HS: universal mode\n") ) ;
                break;

            case 1:
                    DA7212_CLEARVOICE_Mode();
                    MessageSendLater(&theSink.task, EventSysToneDigit1, 0, 0);//voice prompt clear voice
                    //DA7212_DEBUG(("DA7212_EQ_HS: clear voice mode\n") ) ;
                break;

            case 2:
                    DA7212_HIGH_FREQUENCY_BOOST_Mode();
                    MessageSendLater(&theSink.task, EventSysToneDigit2, 0, 0);//voice prompt high frequency boost
                    //DA7212_DEBUG(("DA7212_EQ_HS: high frequency boost mode\n") ) ;
                break;

            case 3:
                    DA7212_POWER_BOOST_Mode();
                    MessageSendLater(&theSink.task, EventSysToneDigit3, 0, 0);//voice prompt power boost
                    //DA7212_DEBUG(("DA7212_EQ_HS: power boost mode\n") ) ;
                break;

            default:
                break;
	}	
    //DA7212_DEBUG(("HS:void DA7212_UpdateEQ(void)\n"));
}

void DA7212_EQ_Mode_Up(void)
{
    My_theSink.DA7212_EQ++;
    //DA7212_DEBUG(("DA7212_EQvalue_info: %d\n",My_theSink.DA7212_EQ)) ;
    if(My_theSink.DA7212_EQ>3)
	{
    	My_theSink.DA7212_EQ=0;
    }
    DA7212_UpdateEQ_PlayVoice();
    MessageSendLater(&theSink.task, EventUsrGaiaUser13, 0, 100);
}

void DA7212_UpdateEQ_PlayVoice(void)
{    
	switch(My_theSink.DA7212_EQ)
	{
            case 0:
                    DA7212_UNIVERSAL_Mode();
                    MessageSendLater(&theSink.task, EventSysToneDigit0, 0, 0);//voice prompt universal
                    //DA7212_DEBUG(("DA7212_EQ_HS: universal mode\n") ) ;
                break;

            case 1:
                    DA7212_CLEARVOICE_Mode();
                    MessageSendLater(&theSink.task, EventSysToneDigit1, 0, 0);//voice prompt clear voice
                    //DA7212_DEBUG(("DA7212_EQ_HS: clear voice mode\n") ) ;
                break;

            case 2:
                    DA7212_HIGH_FREQUENCY_BOOST_Mode();
                    MessageSendLater(&theSink.task, EventSysToneDigit2, 0, 0);//voice prompt high frequency boost
                    //DA7212_DEBUG(("DA7212_EQ_HS: high frequency boost mode\n") ) ;
                break;

            case 3:
                    DA7212_POWER_BOOST_Mode();
                    MessageSendLater(&theSink.task, EventSysToneDigit3, 0, 0);//voice prompt power boost
                    //DA7212_DEBUG(("DA7212_EQ_HS: power boost mode\n") ) ;
                break;

            default:
                break;
	}	
    //DA7212_DEBUG(("HS:void DA7212_UpdateEQ(void)\n"));  
}


/* function only mute DA7212 input*/
void DA7212_Mute(void)
{
    uint8 test[2];
    test[0]=0x63;//mic 1
    test[1]=0xc8;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=0x64;//mic 2
    test[1]=0xc8;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=0x67;//adc 1
    test[1]=0x40;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=0x68;//adc 2
    test[1]=0x40;  
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    //DA7212_DEBUG(("HS:void DA7212_Mute(void)\n"));
}

void DA7212_Close_Mute(void)
{
    uint8 test[2];
    test[0]=0x63;//mic 1
    test[1]=0x88;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=0x64;//mic 2
    test[1]=0x88;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=0x67;//adc 1
    test[1]=0xa0;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=0x68;//adc 2
    test[1]=0xa0; 
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    //DA7212_DEBUG(("HS:void DA7212_Close_Mute(void)\n"));

}

/*
void DA7212_UpdateEQ(void)
{
 u8 test[2];
 u8 FILTERS[5];
 
    //Close_Mic_Mute();
    switch(DA7212_EQ)
    {
    case 0:
      //Universal_Mode();
   FILTERS[0] = 0x80;
   FILTERS[1] = 0XBB;
   FILTERS[2] = 0XBB;
   //FILTERS[3] = 0X80;//0DB
   FILTERS[3] = 0X87;//0DB
   FILTERS[4] = 0;
   //tone_EQ(0x00);
    test[0]=ALC_TARGET_MIN;
    test[1]=ALC_TARGET_MIN_Data;
    da7212_send(test[0],test[1]); 
 
    test[0]=ALC_THRESHOLD_MAX;
    test[1]=ALC_THRESHOLD_MAX_Data;
    da7212_send(test[0],test[1]);
      break;  
    case 1:
      //Clear_Voice_Mode();
      FILTERS[0] = 0x80;
      FILTERS[1] = 0X00;
      //FILTERS[2] = 0XEF;// 新外壳
      FILTERS[2] = 0XC0;  //20191221
      FILTERS[3] = 0X80;//-10.5DB
      FILTERS[4] = 0;
      //tone_EQ(0x01);
       test[0]=ALC_TARGET_MIN;
    test[1]=ALC_TARGET_MIN_Data;
    da7212_send(test[0],test[1]); 
 
    test[0]=ALC_THRESHOLD_MAX;
    test[1]=ALC_THRESHOLD_MAX_Data;
    da7212_send(test[0],test[1]);
      break;
      
    case 2:
        //High_Frequency_Boost_Mode();
        FILTERS[0] = 0x80;
        FILTERS[1] = 0X00;
        FILTERS[2] = 0XE0;
        //FILTERS[3] = 0X8E;//3DB新外壳
        FILTERS[3] = 0X89;//3DB
        FILTERS[4] = 0;  
        //tone_EQ(0x02);
         test[0]=ALC_TARGET_MIN;
     test[1]=ALC_TARGET_MIN_Data+2;
     da7212_send(test[0],test[1]); 
 
     test[0]=ALC_THRESHOLD_MAX;
     test[1]=ALC_THRESHOLD_MAX_Data+2;
     da7212_send(test[0],test[1]); 
      break;
      
    case 3:
        //Power_Boost_Mode();
        FILTERS[0] = 0x80;
        FILTERS[1] = 0XFF;
        FILTERS[2] = 0XFF;
        //FILTERS[3] = 0X8F;//3DB新外壳
        FILTERS[3] = 0X89;//3DB
        FILTERS[4] = 0; 
        //tone_EQ(0x03);  
         test[0]=ALC_TARGET_MIN;
     test[1]=ALC_TARGET_MIN_Data+4;
     da7212_send(test[0],test[1]); 
 
     test[0]=ALC_THRESHOLD_MAX;
     test[1]=ALC_THRESHOLD_MAX_Data+4;
     da7212_send(test[0],test[1]);
      break;
      
    default:
      break;

    }
      test[0]=DAC_FILTERS5;//0x40
      test[1]=FILTERS[4];
      

*/

void DA7212_UNIVERSAL_Mode(void)
{
    uint8 test[2];
    uint8 FILTERS[5];
    
#if 1
    FILTERS[0] = 0x80;
    FILTERS[1] = 0XBB;//band1-6  band2-6
    FILTERS[2] = 0XBB;//band3-6  band4-6
    FILTERS[3] = 0X87;//band5-0
#endif	
    FILTERS[4] = 0;

    test[0]=DAC_FILTERS5;//0x40
    test[1]=FILTERS[4];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS2;//0x41
    test[1]=FILTERS[1];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS3;//0x42
    test[1]=FILTERS[2];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS4;//0x43
    test[1]=FILTERS[3];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS1;//0x44
    test[1]=FILTERS[0];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

/*add by lwy 20200903 sf version*/
    test[0]=ALC_TARGET_MIN;
    test[1]=ALC_TARGET_MIN_Data;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=ALC_THRESHOLD_MAX;
    test[1]=ALC_THRESHOLD_MAX_Data;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

   
    //DA7212_DEBUG(("HS:void DA7212_UNIVERSAL_Mode(void)\n"));
}

void DA7212_CLEARVOICE_Mode(void)
{
    uint8 test[2];
    uint8 FILTERS[5];

#if 1
    FILTERS[0] = 0x80;
    FILTERS[1] = 0x00;//band1- -10.5  band2- -10.5

    FILTERS[2] = 0xc0;//band3: -10.5db     band4: 7.5db
    //FILTERS[2] = 0xef;//band3- 12     band4- 10.5
    
    FILTERS[3] = 0x80;//band5- -10.5
#endif

    FILTERS[4] = 0;

    test[0]=DAC_FILTERS5;//0x40
    test[1]=FILTERS[4];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS2;//0x41
    test[1]=FILTERS[1];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS3;//0x42
    test[1]=FILTERS[2];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS4;//0x43
    test[1]=FILTERS[3];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS1;//0x44
    test[1]=FILTERS[0];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

/*add by lwy 20200903 sf version*/
    test[0]=ALC_TARGET_MIN;
    test[1]=ALC_TARGET_MIN_Data;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=ALC_THRESHOLD_MAX;
    test[1]=ALC_THRESHOLD_MAX_Data;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
    
    //DA7212_DEBUG(("HS:void DA7212_CLEARVOICE_Mode(void)\n"));
}

void DA7212_HIGH_FREQUENCY_BOOST_Mode(void)
{
    uint8 test[2];
    uint8 FILTERS[5];
    
#if 1
    FILTERS[0] = 0x80;
    FILTERS[1] = 0X00;//band1- -10.5  band2- -10.5
    FILTERS[2] = 0Xe0;//band3- -10.5  band4- 10.5

    FILTERS[3] = 0X89;//band5: 3db
    //FILTERS[3] = 0X8e;//band5- 10.5
    
#endif

    FILTERS[4] = 0;

    test[0]=DAC_FILTERS5;//0x40
    test[1]=FILTERS[4];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS2;//0x41
    test[1]=FILTERS[1];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS3;//0x42
    test[1]=FILTERS[2];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS4;//0x43
    test[1]=FILTERS[3];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS1;//0x44
    test[1]=FILTERS[0];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
	
	 /*add by lwy 20200903 sf version*/
    test[0]=ALC_TARGET_MIN;
    test[1]=ALC_TARGET_MIN_Data+2;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
 
    test[0]=ALC_THRESHOLD_MAX;
    test[1]=ALC_THRESHOLD_MAX_Data+2;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    //DA7212_DEBUG(("HS:void DA7212_HIGH_FREQUENCY_BOOST_Mode(void)\n"));
}

void DA7212_POWER_BOOST_Mode(void)
{
    uint8 test[2];
    uint8 FILTERS[5];

    FILTERS[0] = 0x80;
    FILTERS[1] = 0XFF;//band1-12  band2-12
    FILTERS[2] = 0XFF;//band3-12  band4-12
    
    FILTERS[3] = 0x89;//band5:  3db
    //FILTERS[3] = 0x8F;//band5-12
    
    FILTERS[4] = 0;

    test[0]=DAC_FILTERS5;//0x40
    test[1]=FILTERS[4];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS2;//0x41
    test[1]=FILTERS[1];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS3;//0x42
    test[1]=FILTERS[2];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS4;//0x43
    test[1]=FILTERS[3];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    test[0]=DAC_FILTERS1;//0x44
    test[1]=FILTERS[0];
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

	/*add by lwy 20200903 sf version*/
    test[0]=ALC_TARGET_MIN;
    test[1]=ALC_TARGET_MIN_Data+4;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
 
    test[0]=ALC_THRESHOLD_MAX;
    test[1]=ALC_THRESHOLD_MAX_Data+4;
    I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

    //DA7212_DEBUG(("HS:void DA7212_POWER_BOOST_Mode(void)\n"));

}

void App_control_DA7212_Vol(void)
{
    uint8 test[2];

    if(My_theSink.DA7212_Vol == 0)
    {
        DA7212_DEBUG(("APP Vol equl 0\n"));

		test[0]=MIXIN_L_GAIN;
		test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
		I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

		test[0]=MIXIN_R_GAIN;
		test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
		I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
		
        //DA7212_DEBUG(("show app control vol value :%d\n",My_theSink.DA7212_Vol));

        //modify by lwy 20200305
        //MessageSendLater(&theSink.task, EventSysToneDigit8, 0, 0);//Tone_Vol_Min


        MessageSendLater(&theSink.task, EventUsrGaiaUser12, 0, D_SEC(1));//DA7212_Mute();
    }
    else if(My_theSink.DA7212_Vol == 14)
    {
        DA7212_DEBUG(("APP Vol equl 14\n"));

		test[0]=MIXIN_L_GAIN;
		test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
		I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
		
		test[0]=MIXIN_R_GAIN;
		test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
		I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);
    
        DA7212_Close_Mute();

        MessageCancelAll(&theSink.task, EventUsrGaiaUser12);

        //DA7212_DEBUG(("show app control vol value :%d\n",My_theSink.DA7212_Vol));

        //modify by lwy 20200305
        //MessageSendLater(&theSink.task, EventSysTone1, 0, 0);//Tone_Vol_Max
    }
    else if(My_theSink.DA7212_Vol > 0)
    {
        DA7212_DEBUG(("APP Vol > 0\n"));

        DA7212_Close_Mute();

		test[0]=MIXIN_L_GAIN;
		test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
		I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

		test[0]=MIXIN_R_GAIN;
		test[1]=MIXIN_1_AMP_GAIN[My_theSink.DA7212_Vol];
		I2cTransfer(I2C_WRITE_ADDRESS, test, 2, 0, 0);

        MessageCancelAll(&theSink.task, EventUsrGaiaUser12);
    }

    //DA7212_DEBUG(("show app control vol value :%d\n",My_theSink.DA7212_Vol));
}

void AVRCP_Update_Sleeptime(void)
{
    MessageCancelAll(&theSink.task, EventUsrPowerOff);
    //DA7212_DEBUG(("cancel poweroff \n"));
    DA7212_DEBUG(("da7212_HS: My_theSink.time_BToff= 0x%x \n",My_theSink.time_BToff));
    switch(My_theSink.time_BToff)
    {
        case 0:
	        	MessageCancelAll(&theSink.task,EventUsrGaiaUser17);//add by lwy 20200903
	        	MessageSendLater(&theSink.task, EventSysToneDigit4, 0, 0);//voice tone '2h'
	            MessageSendLater(&theSink.task, EventUsrGaiaUser17, 0, D_HOUR(Two_hour));
            break;

        case 1:
                MessageCancelAll(&theSink.task,EventUsrGaiaUser17);//add by lwy 20200903
                MessageSendLater(&theSink.task, EventSysToneDigit5, 0, 0);//voice tone '4h'
            	MessageSendLater(&theSink.task, EventUsrGaiaUser17, 0, D_HOUR(Four_hour));
            break;

        case 2:
				MessageCancelAll(&theSink.task,EventUsrGaiaUser17);//add by lwy 20200903
				MessageSendLater(&theSink.task, EventSysToneDigit6, 0, 0);//voice tone '6h'
	            MessageSendLater(&theSink.task, EventUsrGaiaUser17, 0, D_HOUR(Six_hour));

            break;

        case 3:
	        	MessageCancelAll(&theSink.task,EventUsrGaiaUser17);//add by lwy 20200903
	        	MessageSendLater(&theSink.task, EventSysToneDigit7, 0, 0);//voice tone 'disable'
	            DA7212_DEBUG(("DA7212_HS: My_theSink.time_BToff= 0x%x \n",My_theSink.time_BToff));
	            MessageCancelAll(&theSink.task, EventUsrPowerOff);
            break;
    }
   
    //DA7212_DEBUG(("AVRCP_Update_Sleeptime \n"));
}

void AVRCP_Update_Sleeptime_NoneVoiceTone(void)
{
    MessageCancelAll(&theSink.task, EventUsrPowerOff);
    //DA7212_DEBUG(("cancel poweroff \n"));
    DA7212_DEBUG(("da7212_HS: My_theSink.time_BToff= 0x%x \n",My_theSink.time_BToff));
    switch(My_theSink.time_BToff)
    {
        case 0:	 
			    MessageCancelAll(&theSink.task,EventUsrGaiaUser17);//add by lwy 20200903
	            MessageSendLater(&theSink.task, EventUsrGaiaUser17, 0, D_HOUR(Two_hour));
            break;

        case 1:     
			    MessageCancelAll(&theSink.task,EventUsrGaiaUser17);//add by lwy 20200903
            	MessageSendLater(&theSink.task, EventUsrGaiaUser17, 0, D_HOUR(Four_hour));
            break;

        case 2:		
				MessageCancelAll(&theSink.task,EventUsrGaiaUser17);//add by lwy 20200903
	            MessageSendLater(&theSink.task, EventUsrGaiaUser17, 0, D_HOUR(Six_hour));

            break;

        case 3:	 	
				MessageCancelAll(&theSink.task,EventUsrGaiaUser17);//add by lwy 20200903
	            DA7212_DEBUG(("DA7212_HS: My_theSink.time_BToff= 0x%x \n",My_theSink.time_BToff));
	            MessageCancelAll(&theSink.task, EventUsrPowerOff);
            break;
    }
   
    //DA7212_DEBUG(("AVRCP_Update_Sleeptime_NoneVoiceTone \n"));
}
