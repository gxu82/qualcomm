/****************************************************************************
Copyright (c) 2014 - 2015 Qualcomm Technologies International, Ltd.
Part of 6.3.2

FILE NAME
    sink_ble_advertising.h

DESCRIPTION
    BLE Advertising functionality
*/

#ifndef _SINK_BLE_ADVERTISING_H_
#define _SINK_BLE_ADVERTISING_H_

#include "sink_ble_gap.h"
#include <connection.h>

#include <csrtypes.h>
#ifdef ENABLE_FAST_PAIR
#include "sink_fast_pair.h"
#include "sink_ble_gap.h"
#endif

#define ADVERTISING gBleData->ble.gap.advertising


#define MAX_AD_DATA_SIZE_IN_OCTETS  (0x1F)   /* AD Data max size = 31 octets (defined by BT spec) */
#define AD_DATA_HEADER_SIZE         (0x02)   /* AD header{Octet[0]=length, Octet[1]=Tag} AD data{Octets[2]..[n]} */
#define OCTETS_PER_SERVICE          (0x02)   /* 2 octets per uint16 service UUID */
#ifdef ENABLE_FAST_PAIR
#define MIN_LOCAL_NAME_LENGTH       (0x0A)   /* Minimum length of the local name being advertised when Fast Pair is enabled*/
#else
#define MIN_LOCAL_NAME_LENGTH       (0x10)   /* Minimum length of the local name being advertised*/
#endif

/* Discoverable mode */
typedef enum __adv_discoverable_mode
{
    adv_non_discoverable_mode,
    adv_discoverable_mode_general,
    adv_discoverable_mode_limited
} adv_discoverable_mode_t;

typedef enum
{
    le_normal_adverts ,
    le_new_pair_fp_data_adverts,
    le_reconn_fp_data_adverts,
    le_invalid_adverts
} advertising_data_type_t;

#ifdef ENABLE_FAST_PAIR
#define SHA256_INPUT_ARRAY_LENGTH 17 /*Here 0-15 Bytes are used to store account key data and remaining 1 Byte for salt*/

typedef enum
{
    fp_advertising_none ,
    fp_advertising_reconnection,
    fp_advertising_new    
} fp_advertising_mode_t;

typedef struct 
{
    uint8 s;
    uint8 number_of_keys;
    uint8 number_of_keys_processed;
    uint8 *ad_data;
    uint8 len;
    adv_discoverable_mode_t mode;
    ble_gap_read_name_t reason;
    uint8 temp[SHA256_INPUT_ARRAY_LENGTH];
    uint8 *key_filter;
    fast_pair_account_keys_t account_keys;
} key_filter_t;

#endif

#if ENABLE_FAST_PAIR
void bleFastPairHandleAccountKeys(CL_CRYPTO_HASH_CFM_T *cfm);
#else
#define bleFastPairHandleAccountKeys(cfm) (void(0))
#endif
#ifdef ENABLE_FAST_PAIR
void bleAccountKeySha256Req(uint8 input_array[SHA256_INPUT_ARRAY_LENGTH]);
#else
#define bleAccountKeySha256Req(input_array) (void(0))
#endif

#ifdef ENABLE_FAST_PAIR
void bleSetupFastPairAdvertisingDataReconnection(adv_discoverable_mode_t mode, ble_gap_read_name_t reason);
#else 
#define bleSetupFastPairAdvertisingDataReconnection(mode,reason) (void(0))
#endif

#ifdef ENABLE_FAST_PAIR
void bleFastPairAdvertisingDataReconnection(void);
#else 
#define bleFastPairAdvertisingDataReconnection(void) (void(0))
#endif

#ifdef ENABLE_FAST_PAIR
uint8 bleFastPairCalcAccountKeyData(fast_pair_account_keys_t *keys, uint8 *buf);
#else 
#define bleFastPairCalcAccountKeyData(keys, buf) (void(0))
#endif

#ifdef ENABLE_FAST_PAIR
void bleSetFPadvertMode(fp_advertising_mode_t mode);
#else 
#define bleSetFPadvertMode(mode) (void(0))
#endif

#ifdef ENABLE_FAST_PAIR
fp_advertising_mode_t bleGetFPadvertMode(void);
#else 
#define bleGetFPadvertMode(void) (void(0))
#endif


/*******************************************************************************
NAME    
    bleSetupAdvertisingData
    
DESCRIPTION
    Function to setup the BLE Advertising data for the device.
    
PARAMETERS
    size_local_name Length of the local name buffer.
    local_name      Buffer containing the local name.
    mode            Mode
    reason          Reason to set advertising data
RETURN
    None
*/
#ifdef GATT_ENABLED
void bleSetupAdvertisingData(uint16 size_local_name, const uint8 *local_name, adv_discoverable_mode_t mode, ble_gap_read_name_t reason);
#else
#define bleSetupAdvertisingData(size_local_name, local_name, mode, reason) (void(0))
#endif


/*******************************************************************************
NAME    
    bleHandleSetAdvertisingData
    
DESCRIPTION
    Function to handle when BLE advertising data has been registered with CL.
    
PARAMETERS
    cfm     pointer to a CL_DM_BLE_SET_ADVERTISING_DATA_CFM message.
    
RETURN
    None
*/
#ifdef GATT_ENABLED
void bleHandleSetAdvertisingData(const CL_DM_BLE_SET_ADVERTISING_DATA_CFM_T * cfm);
#else
#define bleHandleSetAdvertisingData(cfm) ((void)(0))
#endif

#define bleAdvertiseForBredrDiscovery() ((void)0)

/******************************************************************************
NAME    
    bleAddHeaderToAdData
    
DESCRIPTION
    Function to add BLE advertising data header.
    
PARAMETERS
    ad_data    pointer advertisement data buffer
    space       space left in the advertisement buffer
    type         AD type 
    
RETURN
    uint8*      updated pointer of ad_data buffer
*/
uint8* bleAddHeaderToAdData(uint8* ad_data, uint8* space, uint8 size, uint8 type);

/******************************************************************************
NAME    
    bleAddServiceUuidToAdData
    
DESCRIPTION
    Function to add the BLE service UUID in advertisement data.
    
PARAMETERS
    ad_data    pointer advertisement data buffer
    space       space left in the advertisement buffer
    uuid         BLE service UUID
    
RETURN
    uint8*      updated pointer of ad_data buffer
*/
uint8* bleAddServiceUuidToAdData(uint8* ad_data, uint8* space, uint16 uuid);

#endif /* _SINK_BLE_ADVERTISING_H_ */
