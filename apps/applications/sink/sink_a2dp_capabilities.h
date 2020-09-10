/*
Copyright (c) 2018 Qualcomm Technologies International, Ltd.
*/

/*!
@file    sink_a2dp_capabilities.h
@brief   The data structures used for the capability exchange with a source device.
*/

#ifndef _SINK_A2DP_CAPABILITIES_H_
#define _SINK_A2DP_CAPABILITIES_H_

#include <csrtypes.h>

typedef enum {
    aptx_ad_channel_mode_stereo       = (1 << 1),
    aptx_ad_channel_mode_tws_stereo   = (1 << 2),
    aptx_ad_channel_mode_joint_stereo = (1 << 3),
    aptx_ad_channel_mode_tws_mono     = (1 << 4)
} aptx_ad_channel_mode_masks_t;

/*!
 * Octet offset from AVDTP_SERVICE_MEDIA_CODEC in aptX Adaptive decoder service capability.
 */
typedef enum {
    aptx_ad_channel_mode_offset = 11,
    aptx_ad_ll_ttp_min_offset   = 12,
    aptx_ad_ll_ttp_max_offset   = 13,
    aptx_ad_hq_ttp_min_offset   = 14,
    aptx_ad_hq_ttp_max_offset   = 15,
    aptx_ad_tws_ttp_min_offset  = 16,
    aptx_ad_tws_ttp_max_offset  = 17
} octet_offsets_in_aptx_ad_decoder_specific_caps_t;

extern const uint8 sbc_caps_sink[16];
extern const uint8 sbc_med_caps_sink[16];
extern const uint8 mp3_caps_sink[16];
extern const uint8 aac_caps_sink[18];
extern const uint8 faststream_caps_sink[14];
extern const uint8 aptx_caps_sink[19];
extern const uint8 aptx_acl_sprint_caps_sink[27];
extern const uint8 aptx_ll_caps_sink[27];
extern const uint8 aptxhd_caps_sink[23];
#ifdef INCLUDE_APTX_ADAPTIVE
#ifdef USE_DEPRECATED_APTX_AD_SERVICE_CAP
extern uint8 aptx_ad_caps_sink[29];
#else
extern uint8 aptx_ad_caps_sink[52];
#endif
#endif

extern const uint8 tws_sbc_caps[26];
extern const uint8 tws_mp3_caps[26];
extern const uint8 tws_aac_caps[28];
extern const uint8 tws_aptx_caps[29];
#ifdef INCLUDE_APTX_ADAPTIVE
#ifdef USE_DEPRECATED_APTX_AD_SERVICE_CAP
extern const uint8 tws_aptx_ad_caps[39];
#else
extern const uint8 tws_aptx_ad_caps[62];
#endif
#endif

#endif /* _SINK_A2DP_CAPABILITIES_H_ */
