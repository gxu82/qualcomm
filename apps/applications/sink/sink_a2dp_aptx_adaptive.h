/*
Copyright (c) 2018 Qualcomm Technologies International, Ltd.
*/

/*!
@file
@ingroup sink_app
@brief   sink a2dp related functions specific to aptX Adaptive
*/

#ifndef _SINK_A2DP_APTX_ADAPTIVE_H_
#define _SINK_A2DP_APTX_ADAPTIVE_H_

#ifdef INCLUDE_APTX_ADAPTIVE

/* Cause a build error if ENABLE_AUDIO_TTP is not defined */
#ifndef ENABLE_AUDIO_TTP
#error ENABLE_AUDIO_TTP must be enabled for aptX Adaptive
#endif

/*************************************************************************
NAME
    sinkA2dpAptxAdInitServiceCapability

DESCRIPTION
    Initialise the aptx adaptive service capability based on configuration
    data from the Config Tool.

RETURNS
    None

**************************************************************************/
void sinkA2dpAptxAdInitServiceCapability(void);

/*************************************************************************
NAME
    sinkA2dpAptxAdUpdateServiceCapWhenEnteringTwsMode

DESCRIPTION
    Update the aptX adaptive service capability for TWS mode

RETURNS
    TRUE when capability changed, FALSE otherwise

**************************************************************************/
bool sinkA2dpAptxAdUpdateServiceCapWhenEnteringTwsMode(void);

/*************************************************************************
NAME
    sinkA2dpAptxAdRestoreServiceCapWhenExitingTwsMode

DESCRIPTION
    Restore aptX adaptive service capability when exiting TWS mode

RETURNS
    TRUE when capability changed, FALSE otherwise

**************************************************************************/
bool sinkA2dpAptxAdRestoreServiceCapWhenExitingTwsMode(void);

#else

#define sinkA2dpAptxAdInitServiceCapability() ((void)(0))
#define sinkA2dpAptxAdUpdateServiceCapWhenEnteringTwsMode() (FALSE)
#define sinkA2dpAptxAdRestoreServiceCapWhenExitingTwsMode() (FALSE)

#endif /* INCLUDE_APTX_ADAPTIVE */
#endif /* _SINK_A2DP_APTX_ADAPTIVE_H_ */
