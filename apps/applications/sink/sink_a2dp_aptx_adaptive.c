/*
Copyright (c) 2018 Qualcomm Technologies International, Ltd.
*/

/*!
@file
@ingroup sink_app
@brief   sink a2dp related functions specific to aptX Adaptive
*/

#include "sink_a2dp_aptx_adaptive.h"
#include "sink_a2dp_capabilities.h"

#include <audio_config.h>
#include <a2dp.h>

#ifdef INCLUDE_APTX_ADAPTIVE

/* Max TTP latency values in the service capability are not in units of 1ms */
#define MAX_TTP_LATENCY_UNIT_IN_MS (4)


static uint8 original_channel_mode;


static bool isSinkInMonoChannelMode(void)
{
    return (AudioConfigGetRenderingMode() == single_channel_rendering);
}

static uint8 getChannelModeToBeUsedForTwsMode(void)
{
    if (isSinkInMonoChannelMode())
        return aptx_ad_channel_mode_tws_mono;
    else
        return aptx_ad_channel_mode_tws_stereo;
}

static uint8 * getStartOfCodecSpecificInformation(void)
{
    uint8 *service_caps = aptx_ad_caps_sink;

    while (service_caps[0] != AVDTP_SERVICE_MEDIA_CODEC)
        service_caps += service_caps[1] + 2;

    return service_caps;
}

static uint8 getChannelModeFromServiceCapability(void)
{
    uint8 * aptx_adaptive_codec_caps = getStartOfCodecSpecificInformation();
    return aptx_adaptive_codec_caps[aptx_ad_channel_mode_offset];
}

static void setChannelModeInServiceCapability(uint8 channel_mode)
{
    uint8 * aptx_adaptive_codec_caps = getStartOfCodecSpecificInformation();
    aptx_adaptive_codec_caps[aptx_ad_channel_mode_offset] = channel_mode;
}

static void setTtpBoundsInServiceCapsFromConfigToolValues(void)
{
    uint8 * aptx_adaptive_codec_caps = getStartOfCodecSpecificInformation();

    ttp_latency_t ttp_latencies = AudioConfigGetA2DPTtpLatency();
    aptx_adaptive_codec_caps[aptx_ad_ll_ttp_min_offset] = ttp_latencies.min_in_ms;
    aptx_adaptive_codec_caps[aptx_ad_ll_ttp_max_offset] = ttp_latencies.max_in_ms / MAX_TTP_LATENCY_UNIT_IN_MS;
    aptx_adaptive_codec_caps[aptx_ad_hq_ttp_min_offset] = aptx_adaptive_codec_caps[aptx_ad_ll_ttp_min_offset];
    aptx_adaptive_codec_caps[aptx_ad_hq_ttp_max_offset] = aptx_adaptive_codec_caps[aptx_ad_ll_ttp_max_offset];

    ttp_latencies = AudioConfigGetTWSTtpLatency();
    aptx_adaptive_codec_caps[aptx_ad_tws_ttp_min_offset] = ttp_latencies.min_in_ms;
    aptx_adaptive_codec_caps[aptx_ad_tws_ttp_max_offset] = ttp_latencies.max_in_ms / MAX_TTP_LATENCY_UNIT_IN_MS;
}

void sinkA2dpAptxAdInitServiceCapability(void)
{
    if (isSinkInMonoChannelMode())
        setChannelModeInServiceCapability(getChannelModeToBeUsedForTwsMode());

    setTtpBoundsInServiceCapsFromConfigToolValues();
    original_channel_mode = getChannelModeFromServiceCapability();
}

bool sinkA2dpAptxAdUpdateServiceCapWhenEnteringTwsMode(void)
{
    uint8 tws_channel_mode = getChannelModeToBeUsedForTwsMode();

    if (getChannelModeFromServiceCapability() != tws_channel_mode)
    {
        setChannelModeInServiceCapability(tws_channel_mode);
        return TRUE;
    }

    return FALSE;
}

bool sinkA2dpAptxAdRestoreServiceCapWhenExitingTwsMode(void)
{
    if (getChannelModeFromServiceCapability() != original_channel_mode)
    {
        setChannelModeInServiceCapability(original_channel_mode);
        return TRUE;
    }

    return FALSE;
}

#endif /* INCLUDE_APTX_ADAPTIVE */
