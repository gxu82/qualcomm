/****************************************************************************
Copyright (c) 2018 Qualcomm Technologies International, Ltd.
Part of 6.3.2

FILE NAME
    sink_gatt_server_fps.c

DESCRIPTION
    Routines to handle messages sent from the GATT Fast Pair Service Task.
*/

/* Firmware headers */
#include <csrtypes.h>
#include <message.h>
#include <util.h>
#include <stdlib.h>
#include <byte_utils.h>
#include <config_store.h>

/* Application headers */
#include "sink_private_data.h"
#include "sink_gatt_common.h"
#include "sink_debug.h"
#include "sink_gatt_db.h"
#include "sink_ble.h"
#include "sink_gatt_server.h"
#include "sink_gatt_server_fps.h"
#include "sink_development.h"
#include "sink_events.h"
#include "sink_main_task.h"

#ifdef ENABLE_FAST_PAIR
#include "sink_fast_pair.h"
#endif

#ifdef GATT_FP_SERVER

#define DEBUG_GATT_FP_SERVER

#ifdef DEBUG_GATT_FP_SERVER
#define GATT_FP_SERVER_INFO(x) DEBUG(x)
#define GATT_FP_SERVER_ERROR(x) DEBUG(x) TOLERATED_ERROR(x)
static void GATT_FP_SERVER_MSG(
        const char *name, 
        uint16 size_value, 
        const uint8* data
        )
{
    uint16 idx;
    char buffer[61];        /* 3 char per number + null */
    char *bptr = buffer;

    *bptr = '\0';
    printf("GATT_SERVER_FP: %s\n", name);
    for (idx = 0; idx<size_value; idx++)
    {
        bptr += sprintf(bptr, "%02X ", data[idx]);
        if (idx > 0 && (idx % 20) == 0)
        {
            printf("%s\n", buffer);
            bptr = buffer;
            *bptr = '\0';
        }
    }
    printf("%s\n", buffer);
}
#else
#define GATT_FP_SERVER_INFO(x)
#define GATT_FP_SERVER_ERROR(x)
#define GATT_FP_SERVER_MSG(name, size_value, data)
#endif

/*******************************************************************************/
bool sinkGattFastPairServerInitialise(uint16 **ptr)
{
    if (
            GattFastPairServerInit(
                (GFPS *)*ptr, 
                sinkGetBleTask(),
                HANDLE_FAST_PAIR_SERVICE,
                HANDLE_FAST_PAIR_SERVICE_END
                )
       )
    {
        GATT_FP_SERVER_INFO(("GATT Fast Pair Server initialised\n"));
        gattServerSetServicePtr(ptr, gatt_server_service_fps);
        /* The size of FP is also calculated and memory is allocated.
         * So advance the ptr so that the next Server while initializing.
         * shall not over write the same memory */
        *ptr += ADJ_GATT_STRUCT_OFFSET(GFPS);
        
        return TRUE;
    }
    
    /* Heart Rate Service library initialization failed */
    GATT_FP_SERVER_INFO(("GATT Fast Pair Server initialisation failed \n"));
    return FALSE;
}

/******************************************************************************/
void sinkGattFastPairServerMsgHandler(Task task, MessageId id, Message message)
{
    UNUSED(task);
    switch(id)
    {
        case GATT_FAST_PAIR_SERVER_WRITE_KEYBASED_PAIRING_IND:
            {   
                GATT_FAST_PAIR_SERVER_WRITE_KEYBASED_PAIRING_IND_T *m = 
                    (GATT_FAST_PAIR_SERVER_WRITE_KEYBASED_PAIRING_IND_T *)message;
                GATT_FP_SERVER_MSG(
                        "Write Key-Based Pairing",
                        m->size_value,
                        m->value
                        );
                /* Send the Write request sink fast pair task for decrytion */
                sinkFastPairKeybasedPairingWrite(m->fast_pair_server, m->cid, m->value, m->size_value);
                
                GattFastPairServerWriteKeybasedPairingResponse(
                        m->fast_pair_server,
                        m->cid,
                        gatt_status_success
                        );

            }
            break;
        case GATT_FAST_PAIR_SERVER_READ_KEYBASED_PAIRING_CONFIG_IND:
            {
                GATT_FAST_PAIR_SERVER_READ_KEYBASED_PAIRING_CONFIG_IND_T *m = 
                    (GATT_FAST_PAIR_SERVER_READ_KEYBASED_PAIRING_CONFIG_IND_T *)message;
                GATT_FP_SERVER_INFO((
                        "GATT_FP_SERVER: Read Key-Based Pairing Client Config Descriptor\n"
                        ));
                GattFastPairServerReadKeybasedPairingConfigResponse(
                        m->fast_pair_server,
                        m->cid,
                        0x0000
                        );
            }
            break;
        case GATT_FAST_PAIR_SERVER_WRITE_KEYBASED_PAIRING_CONFIG_IND:
            {
                GATT_FAST_PAIR_SERVER_WRITE_KEYBASED_PAIRING_CONFIG_IND_T *m = 
                    (GATT_FAST_PAIR_SERVER_WRITE_KEYBASED_PAIRING_CONFIG_IND_T *)message;
                GATT_FP_SERVER_INFO((
                        "GATT_FP_SERVER: Write Key-Based Pairing Client Config Descriptor 0x%04x\n",
                        (int)m->config_value
                        ));
                GattFastPairServerWriteKeybasedPairingConfigResponse(
                        m->fast_pair_server,
                        m->cid,
                        gatt_status_success
                        );
            }
            break;
        case GATT_FAST_PAIR_SERVER_WRITE_PASSKEY_IND:
            {
                GATT_FAST_PAIR_SERVER_WRITE_PASSKEY_IND_T *m = 
                    (GATT_FAST_PAIR_SERVER_WRITE_PASSKEY_IND_T *)message;
                GATT_FP_SERVER_MSG(
                        "Write Passkey",
                        FAST_PAIR_VALUE_SIZE,
                        m->value
                        );

                /* Send the Write request sink fast pair task for decrytion */
                sinkFastPairPasskeyWrite(m->fast_pair_server, m->cid, m->value);
                
                GattFastPairServerWritePasskeyResponse(
                        m->fast_pair_server,
                        m->cid,
                        gatt_status_success
                        );
       
            }
            break;
        case GATT_FAST_PAIR_SERVER_READ_PASSKEY_CONFIG_IND:
            {
                GATT_FAST_PAIR_SERVER_READ_PASSKEY_CONFIG_IND_T *m = 
                    (GATT_FAST_PAIR_SERVER_READ_PASSKEY_CONFIG_IND_T *)message;
                GATT_FP_SERVER_INFO((
                        "GATT_FP_SERVER: Read Passkey Client Config Descriptor\n"
                        ));
                GattFastPairServerReadPasskeyConfigResponse(
                        m->fast_pair_server,
                        m->cid,
                        0x0000
                        );
            }
            break;
        case GATT_FAST_PAIR_SERVER_WRITE_PASSKEY_CONFIG_IND:
            {
                GATT_FAST_PAIR_SERVER_WRITE_PASSKEY_CONFIG_IND_T *m = 
                    (GATT_FAST_PAIR_SERVER_WRITE_PASSKEY_CONFIG_IND_T *)message;
                GATT_FP_SERVER_INFO((
                        "GATT_FP_SERVER: Write Passkey Client Config Descriptor 0x%04x\n",
                        (int)m->config_value
                        ));
                GattFastPairServerWritePasskeyConfigResponse(
                        m->fast_pair_server,
                        m->cid,
                        gatt_status_success
                        );
            }
            break;
        case GATT_FAST_PAIR_SERVER_WRITE_ACCOUNT_KEY_IND:
            {
                GATT_FAST_PAIR_SERVER_WRITE_ACCOUNT_KEY_IND_T *m = 
                    (GATT_FAST_PAIR_SERVER_WRITE_ACCOUNT_KEY_IND_T *)message;
                GATT_FP_SERVER_MSG(
                        "Write Account Key",
                        FAST_PAIR_VALUE_SIZE,
                        m->value
                        );
                
                /* Send the Write request sink fast pair task for decrytion */
                sinkFastPairAccountKeyWrite(m->fast_pair_server, m->cid, m->value);
                
                GattFastPairServerWriteAccountKeyResponse(
                        m->fast_pair_server,
                        m->cid,
                        gatt_status_success
                        );            
            }
            break;
        case GATT_FAST_PAIR_SERVER_KEYBASED_PAIRING_NOTIFICATION_CFM:
            {
                GATT_FP_SERVER_INFO((
                        "GATT_FP_SERVER: Key-based Pairing Notify Cfm\n"
                        ));
            }
            break;
        case GATT_FAST_PAIR_SERVER_PASSKEY_NOTIFICATION_CFM:
            {
                GATT_FP_SERVER_INFO((
                        "GATT_FP_SERVER: Passkey Notify Cfm\n"
                        ));
            }
            break;
        default:
        {
            /* Un-handled messages are not critical errors */
            GATT_FP_SERVER_INFO(("GATT Fast Pair Unhandled msg id[%x]\n", id));
        }
    }
}

bool sinkGattFastPairServiceEnabled(void)
{
    return TRUE;
}


#endif /* GATT_FP_SERVER */

