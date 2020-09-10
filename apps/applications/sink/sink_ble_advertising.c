/****************************************************************************
Copyright (c) 2014 - 2015 Qualcomm Technologies International, Ltd.

FILE NAME
    sink_ble_advertising.c

DESCRIPTION
    BLE Advertising functionality
*/

#include "sink_ble_advertising.h"


#ifdef ENABLE_FAST_PAIR
#include "sink_main_task.h"
#include "sink_fast_pair_config_def.h"
#include<byte_utils.h>
#include<util.h>
#include<cryptoalgo.h>
#define FAST_PAIR_GOOGLE_IDENTIFIER 0xFE2C
#define NEW_PAIR_AD_DATA_LENGTH 10
#define NEW_PAIR_AD_SERVICE_DATA_LENGTH 6
#define TX_POWER_AD_DATA_LENGTH 2
#define FAST_PAIR_ACCOUNT_DATA_FLAG_VALUE 0
/* global Fast Pair Account Key Data instance */
key_filter_t *g_key_filter = NULL ;
#endif

#include "sink_ble.h"
#include "sink_gatt_device.h"
#include "sink_gatt_server_battery.h"
#include "sink_gatt_server_lls.h"
#include "sink_gatt_server_tps.h"
#include "sink_gatt_server_ias.h"
#include "sink_gatt_server_hrs.h"
#ifdef ACTIVITY_MONITORING
#include "sink_gatt_server_rscs.h"
#include "sink_gatt_server_logging.h"
#include "gatt_logging_server_uuids.h"
#endif
#include "sink_gatt_server_dis.h"
#include "sink_debug.h"
#include "sink_development.h"
#include "sink_utils.h"
#include "sink_configmanager.h"
#include "sink_va_if.h"
#include "sink_ba.h"
#include "sink_ba_ble_gap.h"

#include <connection.h>
#include <gatt.h>
#include <local_device.h>

#include <csrtypes.h>
#include <stdlib.h>
#include <string.h>
#include "sink_statemanager.h"

#ifdef GATT_ENABLED


#ifdef ENABLE_AMA
#include "sink_ama.h"
#define LONG_AMA_ADV
#define SHORT_AMA_ADV
#endif

/* Macro for BLE AD Data Debug */
#ifdef DEBUG_BLE
#include <stdio.h>
#define BLE_AD_INFO(x) DEBUG(x)
#define BLE_AD_INFO_STRING(name, len) {unsigned i; for(i=0;i<len;i++) BLE_AD_INFO(("%c", name[i]));}
#define BLE_AD_ERROR(x) DEBUG(x) TOLERATED_ERROR(x)
#else
#define BLE_AD_INFO(x)
#define BLE_AD_INFO_STRING(name, len)
#define BLE_AD_ERROR(x)
#endif

#ifndef MIN
#define MIN(a, b)   ((a < b) ? a : b)
#endif

#define MODE_TO_MASK(mode)  (1 << mode)

#define SIZE_UUID16                         (2)
#define AD_FIELD_LENGTH(data_length)        (data_length + 1)
#define USABLE_SPACE(space)                 ((*space) > AD_DATA_HEADER_SIZE ? (*space) - AD_DATA_HEADER_SIZE : 0)

#define SERVICE_DATA_LENGTH(num_services)   (num_services * OCTETS_PER_SERVICE)
#define NUM_SERVICES_THAT_FIT(space)        (USABLE_SPACE(space) / OCTETS_PER_SERVICE)

#define WRITE_AD_DATA(ad_data, space, value) \
{ \
    *ad_data = value; \
    BLE_AD_INFO(("0x%02x ", *ad_data)); \
    ad_data++; \
    (*space)--; \
}
#ifdef ENABLE_FAST_PAIR
static fp_advertising_mode_t fp_advertising_mode = fp_advertising_none;
#endif
/******************************************************************************/
static bool reserveSpaceForLocalName(uint8* space, uint16 name_length)
{
    uint8 required_space = MIN(name_length, MIN_LOCAL_NAME_LENGTH);

    if((*space) >= required_space)
    {
        *space -= required_space;
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************/
static void restoreSpaceForLocalName(uint8* space, uint16 name_length)
{
    *space += MIN(name_length, MIN_LOCAL_NAME_LENGTH);
}

/******************************************************************************/
static uint8* setupFlagsAdData(uint8* ad_data, uint8* space, adv_discoverable_mode_t mode, ble_gap_read_name_t reason)
{
    uint16 flags = 0;

    if(reason == ble_gap_read_name_broadcasting || reason == ble_gap_read_name_associating)
        flags = BLE_FLAGS_DUAL_HOST;

    if (mode == adv_discoverable_mode_general)
        flags |= BLE_FLAGS_GENERAL_DISCOVERABLE_MODE;
    else if (mode == adv_discoverable_mode_limited)
        flags |= BLE_FLAGS_LIMITED_DISCOVERABLE_MODE;

#ifdef ENABLE_FAST_PAIR
    flags |= BLE_FLAGS_DUAL_HOST;
    flags |= BLE_FLAGS_DUAL_CONTROLLER;
#endif 
	
    /* According to CSSv6 Part A, section 1.3 "FLAGS" states:
        "The Flags data type shall be included when any of the Flag bits are non-zero and the advertising packet
        is connectable, otherwise the Flags data type may be omitted"
     */
    if(flags)
    {
        BLE_AD_INFO(("AD Data: flags = ["));

        ad_data = bleAddHeaderToAdData(ad_data, space, FLAGS_DATA_LENGTH, ble_ad_type_flags);
        WRITE_AD_DATA(ad_data, space, flags);

        BLE_AD_INFO(("]\n"));
    }
    return ad_data;
}

/******************************************************************************/
static uint8* updateServicesAdData(uint8* ad_data, uint8* space)
{
#ifdef GATT_AMA_SERVER
/* AMA TODO : Revisit for future improvement */
#define AMA_SERVICE_ADV (0xFE03)

#ifdef SHORT_AMA_ADV
    if(SinkAmaIsAdvertiseAmaEnabled())
    {
        ad_data = bleAddServiceUuidToAdData(ad_data, space, AMA_SERVICE_ADV);
    }
#endif

#endif

    if (sinkGattBatteryServiceEnabled() && (*space))
    {
        BLE_AD_INFO(("BAS "));
        ad_data = bleAddServiceUuidToAdData(ad_data, space, GATT_SERVICE_UUID_BATTERY_SERVICE);
    }

    if (sinkGattLinkLossServiceEnabled() && (*space))
    {
        BLE_AD_INFO(("LLS "));
        ad_data = bleAddServiceUuidToAdData(ad_data, space, GATT_SERVICE_UUID_LINK_LOSS);
    }

    if (sinkGattTxPowerServiceEnabled() && (*space))
    {
        BLE_AD_INFO(("TPS "));
        ad_data = bleAddServiceUuidToAdData(ad_data, space, GATT_SERVICE_UUID_TX_POWER);
    }

    if (sinkGattImmAlertServiceEnabled() && (*space))
    {
        BLE_AD_INFO(("IAS "));
        ad_data = bleAddServiceUuidToAdData(ad_data, space, GATT_SERVICE_UUID_IMMEDIATE_ALERT);
    }

    if (sinkGattHeartRateServiceEnabled() && (*space))
    {
        BLE_AD_INFO(("HRS "));
        ad_data = bleAddServiceUuidToAdData(ad_data, space, GATT_SERVICE_UUID_HEART_RATE);
    }

#ifdef ACTIVITY_MONITORING
    if (sinkGattRSCServiceEnabled() && (*space))
    {
        BLE_AD_INFO(("RSCS"));
        ad_data = bleAddServiceUuidToAdData(ad_data, space, GATT_SERVICE_UUID_RUNNING_SPEED_AND_CADENCE);
    }
    // Advertising of 128 bit UUIDS is not currently supported by the Sink App
/*
    if (sinkGattLoggingServiceEnabled() && (*space))
    {
        BLE_AD_INFO(("LOGGING"));
        ad_data = bleAddServiceUuidToAdData(ad_data, space, GATT_SERVICE_UUID_LOGGING);
    }
*/
#endif

    if (sinkGattDeviceInfoServiceEnabled() && (*space))
    {
        BLE_AD_INFO(("DIS "));
        ad_data = bleAddServiceUuidToAdData(ad_data, space, GATT_SERVICE_UUID_DEVICE_INFORMATION);
    }

    return ad_data;
}

/******************************************************************************/
static uint16 getNumberOfServersEnabled(void)
{
    uint16 num_services = 0;

#ifdef GATT_AMA_SERVER
#ifdef SHORT_AMA_ADV

    if(SinkAmaIsAdvertiseAmaEnabled())
    {
        /* AMA TODO : Revisit for future improvement */
        num_services++;
    }
#endif
#endif


    if (sinkGattBatteryServiceEnabled())
        num_services++;

    if (sinkGattLinkLossServiceEnabled())
        num_services++;

    if (sinkGattTxPowerServiceEnabled())
        num_services++;

    if (sinkGattImmAlertServiceEnabled())
        num_services++;

    if (sinkGattHeartRateServiceEnabled())
        num_services++;

#ifdef ACTIVITY_MONITORING
    if (sinkGattRSCServiceEnabled())
        num_services++;
// We must not add this until 128 bit UUIDs are supported in Sink App!
//    if (sinkGattLoggingServiceEnabled())
//        num_services++;
#endif

    if (sinkGattDeviceInfoServiceEnabled())
        num_services++;

    return num_services;
}

/******************************************************************************/
static uint8* setupServicesAdData(uint8* ad_data, uint8* space, ble_gap_read_name_t reason)
{
    if(reason == ble_gap_read_name_broadcasting || reason == ble_gap_read_name_advertising_broadcasting)
    {
        /* Add the broadcaster variant IV service data */
        ad_data = setupBroadcasterIvServiceData(ad_data, space);
        if(reason == ble_gap_read_name_broadcasting)
            return ad_data;
    }

    if(reason == ble_gap_read_name_associating)
    {
        /* Set up receiver association service data */
        ad_data = setupReceiverAssociationServiceData(ad_data, space);
    }
    else
    {
        uint16 num_services = getNumberOfServersEnabled();
        uint8 num_services_that_fit = NUM_SERVICES_THAT_FIT(space);

        if (num_services && num_services_that_fit)
        {
            uint8 service_data_length;
            uint8 service_field_length;
            uint8 ad_tag = ble_ad_type_complete_uuid16;

            /* Is there enough room to store the complete list of services defined for the device? */
            if(num_services > num_services_that_fit)
            {
                /* Advertise incomplete list */
                ad_tag = ble_ad_type_more_uuid16;
                num_services = num_services_that_fit;
            }

            /* Setup AD data for the services */
            BLE_AD_INFO(("AD Data: services = ["));
            service_data_length = SERVICE_DATA_LENGTH(num_services);
            service_field_length = AD_FIELD_LENGTH(service_data_length);
            ad_data = bleAddHeaderToAdData(ad_data, space, service_field_length, ad_tag);

            /* Add UUID of enabled services to advertising list */
            ad_data = updateServicesAdData(ad_data, space);

            BLE_AD_INFO(("]\n"));
        }
    }
    /* return the advertising data counter as next available index based on configured number of services */
    return ad_data;
}

/******************************************************************************/
static uint8* setupLocalNameAdvertisingData(uint8 *ad_data, uint8* space, uint16 size_local_name, const uint8 * local_name)
{
    uint8 name_field_length;
    uint8 name_data_length = size_local_name;
    uint8 ad_tag = ble_ad_type_complete_local_name;
    uint8 usable_space = USABLE_SPACE(space);

    if((name_data_length == 0) || usable_space <= 1)
        return ad_data;

    if(name_data_length > usable_space)
    {
        ad_tag = ble_ad_type_shortened_local_name;
        name_data_length = usable_space;
    }

    BLE_AD_INFO(("AD Data: local name = ["));

    name_field_length = AD_FIELD_LENGTH(name_data_length);
    ad_data = bleAddHeaderToAdData(ad_data, space, name_field_length, ad_tag);

    /* Setup the local name advertising data */
    memmove(ad_data, local_name, name_data_length);
    BLE_AD_INFO_STRING(ad_data, name_data_length);
    ad_data += name_data_length;
    *space -= name_data_length;

    BLE_AD_INFO(("]\n"));
    return ad_data;
}


#ifdef ENABLE_FAST_PAIR
/******************************************************************************/
static bool getFastPairModelInfoConfig(fast_pair_model_info_config_def_t* model_info_config)
{
    uint16 size;
    fast_pair_model_info_config_def_t* fast_pair_ad_config;

    size = configManagerGetReadOnlyConfig(FAST_PAIR_MODEL_INFO_CONFIG_BLK_ID, (const void **)&fast_pair_ad_config);

    *model_info_config = *fast_pair_ad_config;

    configManagerReleaseConfig(FAST_PAIR_MODEL_INFO_CONFIG_BLK_ID);

    return(size !=0);    
}

/******************************************************************************/
static bool getFastPairAccountKeyConfig(fast_pair_writable_account_keys_config_def_t *account_keys_config)
{
    uint16 size;
    fast_pair_writable_account_keys_config_def_t *account_keys;
    size = configManagerGetReadOnlyConfig(FAST_PAIR_WRITABLE_ACCOUNT_KEYS_CONFIG_BLK_ID, (const void **)&account_keys);
    g_key_filter->account_keys.num_keys = (size * sizeof(uint16)) / sizeof(account_keys->account_keys[0]);

    *account_keys_config = *account_keys;

    configManagerReleaseConfig(FAST_PAIR_WRITABLE_ACCOUNT_KEYS_CONFIG_BLK_ID);

    return(size != 0);
}

/*******************************************************************************
NAME
    startFastPairAdvTimer
    
DESCRIPTION
    Starts the timer for fast pair advertising.
    
PARAMETERS
    None
    
RETURNS
    None
*/
static void startFastPairAdvTimer(void)
{
    MessageCancelFirst(sinkGetBleTask(), BLE_INTERNAL_MESSAGE_FAST_PAIR_ADV_TIMER);
    MessageSendLater(sinkGetBleTask(), BLE_INTERNAL_MESSAGE_FAST_PAIR_ADV_TIMER, 0, D_SEC(sinkBleGetConfiguration()->fast_pair_adv_timer_s));
}

void bleAccountKeySha256Req(uint8 input_array[SHA256_INPUT_ARRAY_LENGTH])
{
    uint16 temp[(SHA256_INPUT_ARRAY_LENGTH/2) + 1];
    int count;

    temp[0] = MAKEWORD_HI_LO(input_array[SHA256_INPUT_ARRAY_LENGTH - 2], input_array[SHA256_INPUT_ARRAY_LENGTH - 1]);
    
    for (count = 1; count < (SHA256_INPUT_ARRAY_LENGTH/2); count++)
    {
        temp[count] =
            MAKEWORD_HI_LO(input_array[SHA256_INPUT_ARRAY_LENGTH - 2 * count - 2],
                           input_array[SHA256_INPUT_ARRAY_LENGTH - 2 * count - 1]);
    }

    temp[SHA256_INPUT_ARRAY_LENGTH/2] = input_array[0];

    ConnectionEncryptBlockSha256(&theSink.task, temp, (SHA256_INPUT_ARRAY_LENGTH/2) + 1);
}

void bleSetupFastPairAdvertisingDataReconnection(adv_discoverable_mode_t mode, ble_gap_read_name_t reason)
{
    uint8 space = MAX_AD_DATA_SIZE_IN_OCTETS * sizeof(uint8);
    uint8 *ad_start = malloc(space);
    uint8 i;
    if(ad_start)
    {
        uint8* ad_head = ad_start;

        ad_head = setupFlagsAdData(ad_head, &space, mode, reason);

        BLE_AD_INFO(("Fast Pair AD Data for Reconnection: ["));
        for(i = 0; i < g_key_filter->len + 8; i++)
            WRITE_AD_DATA(ad_head, &space, g_key_filter->ad_data[i]);
        BLE_AD_INFO(("]\n"));

        ConnectionDmBleSetAdvertisingDataReq(ad_head - ad_start, ad_start);

        free (ad_start);
        free(g_key_filter->ad_data);
    }
}

void bleFastPairHandleAccountKeys(CL_CRYPTO_HASH_CFM_T *cfm)
{
    uint32 hash_value, hash_account_key;
    uint8 j;
    for (j = 0; j < 16; j += 2)
    {
        hash_value = (((uint32)cfm->hash[j] << 24) & 0xFF000000UL) |
                            (((uint32)cfm->hash[j] << 16) & 0x00FF0000UL) |
                            (((uint32)cfm->hash[j + 1] << 8) & 0x0000FF00UL) |
                            (((uint32)cfm->hash[j + 1]) & 0x000000FFUL);
        hash_account_key = hash_value % (g_key_filter->s * 8);
        g_key_filter->key_filter[hash_account_key/ 8] = g_key_filter->key_filter[hash_account_key / 8] | (1 << (hash_account_key % 8));
    }
    
     if(g_key_filter->number_of_keys_processed == g_key_filter->number_of_keys)
     {
         bleSetupFastPairAdvertisingDataReconnection(g_key_filter->mode, g_key_filter->reason);
         startFastPairAdvTimer();
         return;
     }  
      
     if(g_key_filter->number_of_keys_processed < g_key_filter->number_of_keys)
     {          
         memmove(g_key_filter->temp, &g_key_filter->account_keys.keys[g_key_filter->number_of_keys_processed][0], 16);
         bleAccountKeySha256Req(g_key_filter->temp);
         g_key_filter->number_of_keys_processed++;
     }

}

uint8 bleFastPairCalcAccountKeyData(fast_pair_account_keys_t *keys, uint8 *buf)
{
    uint8 size;

    if (keys->num_keys == 0)
    {
        /* Nothing to do when we don't have account keys */
        size = 0;
    }
    else
    {
        /* size of account key filter */
        g_key_filter->s = keys->num_keys * 6 / 5 + 3;
        size = g_key_filter->s + 3;

        if (buf)
        {         
            /* 1 Byte Salt */
            uint8 salt = UtilRandom() & 0xFF;
            g_key_filter->key_filter = &buf[1];
            memset(buf, 0, size);

            buf[0] = (g_key_filter->s << 4) & 0xF0;
            buf[size - 2] = 0x11;
            buf[size - 1] = salt;

            g_key_filter->temp[16] = salt;

            g_key_filter->number_of_keys_processed = 0;
            g_key_filter->number_of_keys = keys->num_keys;
            memmove(g_key_filter->temp, &keys->keys[g_key_filter->number_of_keys_processed][0], 16);
            bleAccountKeySha256Req(g_key_filter->temp);
            g_key_filter->number_of_keys_processed++;              
        }
    }

    return size;
}

/* Set up fast pair advertising data for new pair*/
static uint8* setupFastPairAdvertisingDataNewPair(uint8 *fast_pair_ad_data, uint8 *space)
{
    fast_pair_model_info_config_def_t fast_pair_ad_config;
    uint32 model_id = 0;
    int8 tx_power = 0;

    if(getFastPairModelInfoConfig(&fast_pair_ad_config))
    {
        model_id = (((uint32)fast_pair_ad_config.fast_pair_model_id_hi << 16) | (uint32)fast_pair_ad_config.fast_pair_model_id_lo) & 0xFFFFFFUL;
        tx_power = (int8)fast_pair_ad_config.fast_pair_tx_power;
    }
    else
    {
        BLE_AD_INFO(("Fast Pair readonly config invalid!\n"));
    }

    if(NEW_PAIR_AD_DATA_LENGTH <= (*space))
    {
        BLE_AD_INFO(("Fast Pair AD Data for new Pair: ["));
        /*Service Data*/
        WRITE_AD_DATA(fast_pair_ad_data, space, (uint8)NEW_PAIR_AD_SERVICE_DATA_LENGTH);
        WRITE_AD_DATA(fast_pair_ad_data, space, (uint8)ble_ad_type_service_data);
        WRITE_AD_DATA(fast_pair_ad_data, space, (uint8)(FAST_PAIR_GOOGLE_IDENTIFIER & 0xFF));
        WRITE_AD_DATA(fast_pair_ad_data, space, (uint8)((FAST_PAIR_GOOGLE_IDENTIFIER >> 8) & 0xFF));
        WRITE_AD_DATA(fast_pair_ad_data, space, (uint8)((model_id >> 16) & 0xFF));
        WRITE_AD_DATA(fast_pair_ad_data, space, (uint8)((model_id >> 8) & 0xFF));
        WRITE_AD_DATA(fast_pair_ad_data, space, (uint8)(model_id & 0xFF));
        
        /* TX power */
        WRITE_AD_DATA(fast_pair_ad_data, space, (uint8)TX_POWER_AD_DATA_LENGTH);
        WRITE_AD_DATA(fast_pair_ad_data, space, (uint8)ble_ad_type_tx_power_level);
        WRITE_AD_DATA(fast_pair_ad_data, space,(uint8)tx_power);
        BLE_AD_INFO(("]\n"));
    }
    return fast_pair_ad_data;    
}

/* Set up fast pair advertising data for reconnection */
void bleFastPairAdvertisingDataReconnection(void)
{
    int8 tx_power = 0;
    uint8 i;
    fast_pair_writable_account_keys_config_def_t account_keys;
    fast_pair_model_info_config_def_t fast_pair_ad_config;

    if(getFastPairModelInfoConfig(&fast_pair_ad_config))
    {
        tx_power = (int8)fast_pair_ad_config.fast_pair_tx_power;
    }

    if(getFastPairAccountKeyConfig(&account_keys))
    {          
        BLE_AD_INFO(("Fast Pair account keys num: %d\n", g_key_filter->account_keys.num_keys));
              
        if (g_key_filter->account_keys.num_keys)
        {
            g_key_filter->account_keys.keys = PanicUnlessMalloc(g_key_filter->account_keys.num_keys * sizeof(g_key_filter->account_keys.keys[0]));
            for (i = 0; i < g_key_filter->account_keys.num_keys; i++)
            {
                ByteUtilsMemCpyUnpackString(&g_key_filter->account_keys.keys[i][0], (const uint16 *)&account_keys.account_keys[i], FAST_PAIR_ACCOUNT_KEY_LEN);
            }
        }
        else
        {
            g_key_filter->account_keys.keys = NULL;
        }
    }
          
    g_key_filter->len = bleFastPairCalcAccountKeyData(&g_key_filter->account_keys, NULL);
    g_key_filter->ad_data = PanicUnlessMalloc(g_key_filter->len + 8);
          
    /* TX power */
    g_key_filter->ad_data[0] = TX_POWER_AD_DATA_LENGTH;
    g_key_filter->ad_data[1] = ble_ad_type_tx_power_level;
    g_key_filter->ad_data[2] = (uint8)tx_power;
          
    /* Service Data */      
    g_key_filter->ad_data[3] = g_key_filter->len + 4;
    g_key_filter->ad_data[4] = ble_ad_type_service_data;
    g_key_filter->ad_data[5] = FAST_PAIR_GOOGLE_IDENTIFIER & 0xFF;
    g_key_filter->ad_data[6] = (FAST_PAIR_GOOGLE_IDENTIFIER >> 8) & 0xFF;
    g_key_filter->ad_data[7] = FAST_PAIR_ACCOUNT_DATA_FLAG_VALUE;

    if (g_key_filter->len)
    {
       bleFastPairCalcAccountKeyData(&g_key_filter->account_keys, &g_key_filter->ad_data[8]);
    }
}
/******************************************************************************/
static void bleFastPairChangeAddressRPA(void)
{
    /*Stop advertising as well if going on*/
    ConnectionDmBleSetAdvertiseEnable(FALSE);

    ConnectionDmBleConfigureLocalAddressReq(ble_local_addr_generate_resolvable, NULL);    
}

void bleSetFPadvertMode(fp_advertising_mode_t mode)
{
    fp_advertising_mode = mode;
}

fp_advertising_mode_t bleGetFPadvertMode(void)
{
    return fp_advertising_mode;
}

#endif /*ENABLE_FAST_PAIR*/

/******************************************************************************/
uint8* bleAddHeaderToAdData(uint8* ad_data, uint8* space, uint8 size, uint8 type)
{
    WRITE_AD_DATA(ad_data, space, size);
    WRITE_AD_DATA(ad_data, space, type);

    return ad_data;
}

/******************************************************************************/
uint8* bleAddServiceUuidToAdData(uint8* ad_data, uint8* space, uint16 uuid)
{
    *ad_data = (uuid & 0xFF);
    *(ad_data + 1) = (uuid >> 8);

    BLE_AD_INFO(("0x%02x%02x ", ad_data[1], ad_data[0]));
    ad_data += SIZE_UUID16;
    *space -= SIZE_UUID16;

    return ad_data;
}


#ifdef GATT_AMA_SERVER
static void bleSetAmaServiceDataAD(void)
{
    // 0x17    Length for Service Data AD Type (23 bytes)
    // 0x16    Service Data AD Type Identifier.   // ble_ad_type_service_data
    // (QTIL) 0x000A  Vendor Id assigned by BT
    // 0x0001 for Alexa-enabled Headphones
    // Color of the Accessory ????????
    // Device State bit mask.  Bit 1: 1, if classic bluetooth is discoverable
    // Preference of transport for initial connection RFCOMM is preferred

                                                                  /* 0xFE03 */    /* (QTIL) 0x000A */    /* Product Identifier */
    uint8 scan_res_data[0x18]  = {0x17, ble_ad_type_service_data ,   0x03, 0xFE  ,    0x0A, 0x00,             0x01, 0x00 ,
                   /* Color?? */ /* Device State bit mask *//* RFCOMM is preffered*//* reserved*/ /* Product specific (should be set to x0) */
                     0x0,             0x2 ,                        0x1,                 0,0,0,         0,0,0,0,0,0,0,0,0,0 };
    ConnectionDmBleSetScanResponseDataReq(0x18, scan_res_data);
}
#endif

/******************************************************************************/
void bleSetupAdvertisingData(uint16 size_local_name, const uint8 *local_name, adv_discoverable_mode_t mode, ble_gap_read_name_t reason)
{
    advertising_data_type_t ad_type = le_normal_adverts;
    
    if(ConnectionDmBleCheckTdlDeviceAvailable())
    {
        if( (stateManagerGetState() == deviceConnDiscoverable) && (mode  == adv_discoverable_mode_limited))
        {
#ifdef ENABLE_FAST_PAIR     
            if(sinkFastPairGetFastPairState() == fast_pair_idle)
            {
                /* new pair fast pair data */
                gapStopFastPairAdvTimer();
                ad_type = le_new_pair_fp_data_adverts;
            }
#else
            ad_type = le_normal_adverts;
#endif
        }
        else if(mode  == adv_discoverable_mode_limited)
        {
#ifdef ENABLE_FAST_PAIR          
            /*normal adverts*/
            gapStopFastPairAdvTimer();
#endif            
            ad_type = le_normal_adverts;
        }
#ifdef ENABLE_FAST_PAIR
        else  if(gapFastPairAccountKeysPresent() && (gapGetAdvSpeed() == ble_gap_adv_speed_fast)
                    && (mode  == adv_non_discoverable_mode) && (bleGetFPadvertMode() == fp_advertising_none))
        {
            /* reconnection fp data */
            ad_type = le_reconn_fp_data_adverts;
            bleSetFPadvertMode(fp_advertising_reconnection);
        }
#endif        
        else 
        {
            /* normal adv*/
            ad_type = le_normal_adverts;
        }
    }
    else
    {
#ifdef ENABLE_FAST_PAIR
        if( (stateManagerGetState() == deviceConnDiscoverable) && (mode  == adv_discoverable_mode_limited) && (sinkFastPairGetFastPairState() == fast_pair_idle))
        {            
            /* new pair fast pair data */
            ad_type = le_new_pair_fp_data_adverts;
        }
        else 
#endif    
        {
            /*normal advertising*/
            ad_type = le_normal_adverts;
        }
    }

    if(ad_type != le_reconn_fp_data_adverts)
    {
        uint8 space = MAX_AD_DATA_SIZE_IN_OCTETS * sizeof(uint8);
        uint8 *ad_start = malloc(space);
#ifdef ENABLE_FAST_PAIR
         bleSetFPadvertMode(fp_advertising_none);
#endif
        if(ad_start)
        {
            uint8* ad_head = ad_start;
            bool name_space_reserved;

            ad_head = setupFlagsAdData(ad_head, &space, mode, reason);


#ifdef ENABLE_FAST_PAIR
            if(ad_type == le_new_pair_fp_data_adverts)
            {
                ad_head = setupFastPairAdvertisingDataNewPair(ad_head, &space);
                bleSetFPadvertMode(fp_advertising_new);
            }
#endif

            name_space_reserved = reserveSpaceForLocalName(&space, size_local_name);

            ad_head = setupServicesAdData(ad_head, &space, reason);

            if(name_space_reserved)
                restoreSpaceForLocalName(&space, size_local_name);

            ad_head = setupLocalNameAdvertisingData(ad_head, &space, size_local_name, local_name);

            ConnectionDmBleSetAdvertisingDataReq(ad_head - ad_start, ad_start);
#ifdef GATT_AMA_SERVER /* AMA TODO */
#ifdef LONG_AMA_ADV
            if(SinkAmaIsAdvertiseAmaEnabled())
            {
                bleSetAmaServiceDataAD();
            }
#endif
#endif
            free (ad_start);
        }
    }
#ifdef ENABLE_FAST_PAIR      
    else
    {
         g_key_filter = PanicUnlessMalloc(sizeof(key_filter_t));
   
         g_key_filter->mode = mode;
         g_key_filter->reason = reason; 

         /*Change to RPA for reconnection*/
         bleFastPairChangeAddressRPA();
         
         bleFastPairAdvertisingDataReconnection();
    }

#endif
}

/******************************************************************************/
void bleHandleSetAdvertisingData(const CL_DM_BLE_SET_ADVERTISING_DATA_CFM_T * cfm)
{
    ble_gap_event_t event;

    BLE_AD_INFO(("CL_DM_BLE_SET_ADVERTISING_DATA_CFM [%x]\n", cfm->status));

    if (cfm->status != success)
    {
        BLE_AD_ERROR(("  Failed!\n"));
    }

    /* Send GAP event after set of advertising data */
    event.id = ble_gap_event_set_advertising_complete;
    event.args = NULL;
    sinkBleGapEvent(event);
}


#endif /* GATT_ENABLED */
