/****************************************************************************
Copyright (c) 2018 Qualcomm Technologies International, Ltd.

FILE NAME
    sink_fast_pair.c

DESCRIPTION
    Interface with the fast pair library 
*/

#include <stdlib.h>
#include <connection.h>
#include <util.h>
#include <byte_utils.h>
#include <config_store.h>


/* Application headers */
#include "sink_main_task.h"
#include "sink_debug.h"
#include "sink_configmanager.h"
#include "sink_statemanager.h"
#include "sink_development.h"
#include "sink_utils.h"
#include "sink_events.h"
#include "sink_ble_gap.h"
#include "sink_ble.h"
#include "sink_slc.h"

#ifdef ENABLE_FAST_PAIR
#include "sink_fast_pair.h"
#include "sink_fast_pair_config_def.h"
#include "sink_ble_advertising.h"

#ifdef DEBUG_FAST_PAIR
#define FAST_PAIR_INFO(x) DEBUG(x)
#define FAST_PAIR_ERROR(x) DEBUG(x) TOLERATED_ERROR(x)
#else
#define FAST_PAIR_INFO(x)
#define FAST_PAIR_ERROR(x)
#endif

#define FAST_PAIR_STATE_TIMEOUT         (30)

/* Global Fast Pair Data instance */
sink_fast_pair_data_t    g_fast_pair_data;

/*******************************************************************************
NAME
    sinkFastPairGetAntiSpoofingPrivateKey

DESCRIPTION
    Get anti spoofing private key when kbp request is recieved.

PARAMETERS
    None

RETURNS
    True if private key is obtained
*/
static bool sinkFastPairGetAntiSpoofingPrivateKey(void)
{
    uint16 size;
    bool status = FALSE;
    fast_pair_readonly_private_key_config_def_t *data;
    uint8* private_key = PanicUnlessMalloc(FAST_PAIR_PRIVATE_KEY_LEN);

    size = configManagerGetReadOnlyConfig(FAST_PAIR_READONLY_PRIVATE_KEY_CONFIG_BLK_ID, (const void **)&data);
    if (size * sizeof(uint16) == sizeof(fast_pair_readonly_private_key_config_def_t))
    {
        ByteUtilsMemCpyUnpackString(&private_key[0], (const uint16 *)&data->private_key[0], FAST_PAIR_PRIVATE_KEY_LEN);
        status = TRUE;
    }
    else
    {
        FAST_PAIR_ERROR(("Fast Pair private key size invalid!\n"));
    }
    configManagerReleaseConfig(FAST_PAIR_READONLY_PRIVATE_KEY_CONFIG_BLK_ID);

    g_fast_pair_data.private_key = (uint16 *)private_key;
    return status;
}

/*******************************************************************************
NAME
    sinkFastPairStoreAccountKey

DESCRIPTION
    Store Account key provided by Fast Pair Seeker after fast pairing is completed.

PARAMETERS
    pointer to decrypted account key data

RETURNS
    None
*/
static void sinkFastPairStoreAccountKey(uint8* decrypted_data)
{
  
    fast_pair_writable_account_keys_config_def_t *account_keys;
    fast_pair_writable_account_keys_config_def_t *updated_keys;

    uint16 *buf;
    uint16 size = configManagerGetReadOnlyConfig(FAST_PAIR_WRITABLE_ACCOUNT_KEYS_CONFIG_BLK_ID, (const void **)&account_keys);
    uint8 num_keys = (size * sizeof(uint16)) / sizeof(account_keys->account_keys[0]);

    FAST_PAIR_INFO(("FAST_PAIR_ACCOUNT_KEY_IND curr_key_cnt=%d\n", num_keys));

    /* Calculate number of keys already present in config store */
    num_keys = (num_keys < MAX_FAST_PAIR_ACCOUNT_KEYS) ? (num_keys + 1) : MAX_FAST_PAIR_ACCOUNT_KEYS;

    updated_keys = PanicUnlessMalloc(num_keys * sizeof(updated_keys->account_keys[0]));
    ByteUtilsMemCpyPackString((uint16 *)&updated_keys->account_keys[0], decrypted_data, 16);

    if (num_keys > 1)
    {
        memmove(&updated_keys->account_keys[1], &account_keys->account_keys[0], (num_keys - 1) * sizeof(account_keys->account_keys[0]));
    }

    configManagerReleaseConfig(FAST_PAIR_WRITABLE_ACCOUNT_KEYS_CONFIG_BLK_ID);

    size = num_keys * sizeof(updated_keys->account_keys[0]);

    /* Ceil size */
    if (sizeof(uint16) == 2)
     size = (size + 1) / 2;

    size = configManagerGetWriteableConfig(FAST_PAIR_WRITABLE_ACCOUNT_KEYS_CONFIG_BLK_ID, (void **)&buf, size);

    FAST_PAIR_INFO(("Config mgr allocated for account key size=%d data_size=%d\n", size, num_keys * sizeof(updated_keys->account_keys[0])));

    memmove(buf, updated_keys, num_keys * sizeof(updated_keys->account_keys[0]));

    configManagerUpdateWriteableConfig(FAST_PAIR_WRITABLE_ACCOUNT_KEYS_CONFIG_BLK_ID);

    free(updated_keys);
 
}

/*******************************************************************************
NAME
    sinkFastPairStoreEncryptedBlock

DESCRIPTION
    Store encrypted block provided by Fast Pair Seeker until AES key is derived.

PARAMETERS
    pointer to encrypted block

RETURNS
    None
*/
static void sinkFastPairStoreEncryptedBlock(uint8 *ptr)
{
    uint8* encrypted_data = PanicUnlessMalloc(FAST_PAIR_ENCRYPTED_REQUEST_LEN);

    g_fast_pair_data.encrypted_data = encrypted_data;
    g_fast_pair_data.encrypted_data_size = FAST_PAIR_ENCRYPTED_REQUEST_LEN;

    memmove(g_fast_pair_data.encrypted_data, ptr, g_fast_pair_data.encrypted_data_size);
}

/*******************************************************************************
NAME
    sinkFastPairStorePublicKey

DESCRIPTION
    Store public key provided by Fast Pair Seeker.

PARAMETERS
    pointer to public keys

RETURNS
    None
*/
static void sinkFastPairStorePublicKey(uint8 *ptr)
{
    uint8* public_key = PanicUnlessMalloc(FAST_PAIR_PUBLIC_KEY_LEN);

    g_fast_pair_data.public_key = (uint16 *)public_key;

    memmove(g_fast_pair_data.public_key, ptr, FAST_PAIR_PUBLIC_KEY_LEN);
}

/*******************************************************************************
NAME
    sinkFastPairCalculateSharedSecret

DESCRIPTION
    Calculate shared secret using public and private key on ECDH sec256p curve.

PARAMETERS
    pointer to public and private keys

RETURNS
    None
*/
static void sinkFastPairCalculateSharedSecret(uint16 *private_key, uint16 *public_key)
{
    ConnectionGenerateSharedSecretKey(sinkGetMainTask(), cl_crypto_ecc_p256, private_key, public_key);
}

/*******************************************************************************
NAME
    sinkFastPairMatchProviderAddress

DESCRIPTION
    Match provider address from decrypted key based pairing request.

PARAMETERS
    pointer to decrypted_data

RETURNS
    True if Provider address matches else False
*/
static bool sinkFastPairMatchProviderAddress(uint16 *decrypted_data)
{
    bool status = FALSE;

    bdaddr local_addr;

    uint8 *decrypted_packet_be;

    bdaddr provider_addr;

    decrypted_packet_be = (uint8 *)decrypted_data;
    
     /* Check local addrss */
     sinkDataGetLocalBdAddress(&local_addr);

     provider_addr.nap = (uint16)(decrypted_packet_be[PROVIDER_ADDRESS_OFFSET]<<8)| decrypted_packet_be[PROVIDER_ADDRESS_OFFSET+1];
     provider_addr.uap = (uint8)(decrypted_packet_be[PROVIDER_ADDRESS_OFFSET+2]);
     provider_addr.lap = (uint24)(decrypted_packet_be[PROVIDER_ADDRESS_OFFSET+3] & 0xFF) << 16 | (uint24)(decrypted_packet_be[PROVIDER_ADDRESS_OFFSET+4]) << 8 | decrypted_packet_be[PROVIDER_ADDRESS_OFFSET+5];
     
     if (!BdaddrIsSame(&local_addr, &provider_addr))
     {
         FAST_PAIR_ERROR(("Fast Pair provider addr mismatch!\n"));
     }
     else
     {  
        status = TRUE;
     }

     return status;
}

/*******************************************************************************
NAME
    sinkFastPairGenerateResponse

DESCRIPTION
    Generate key based pairing response.

PARAMETERS
    None

RETURNS
    pointer to raw response block
*/
static uint8* sinkFastPairGenerateResponse(void)
{
    uint8 *response = PanicUnlessMalloc(FAST_PAIR_ENCRYPTED_REQUEST_LEN);
    uint16 i;
    bdaddr local_addr;

    /* Check local addrss */
    sinkDataGetLocalBdAddress(&local_addr);
    
    response[0] = 0x01;
    response[1] = (local_addr.nap >> 8) & 0xFF;
    response[2] = local_addr.nap & 0xFF;
    response[3] = local_addr.uap;
    response[4] = (local_addr.lap >> 16) & 0xFF;
    response[5] = (local_addr.lap >> 8) & 0xFF;
    response[6] = local_addr.lap & 0xFF;
    for (i = 7; i < 16; i++)
    {
        response[i] = UtilRandom() & 0xFF;
    }

    return response;

}

/*******************************************************************************
NAME
    sinkFastPairPasskeyResponse

DESCRIPTION
    Generate provider passkey response.

PARAMETERS
    None

RETURNS
    pointer to raw passkey block
*/
static uint8* sinkFastPairPasskeyResponse(void)
{
    uint8 *response = PanicUnlessMalloc(FAST_PAIR_ENCRYPTED_PASSKEY_BLOCK_LEN);
    uint16 i;


    response[0] = fast_pair_passkey_provider;
    response[1] = (g_fast_pair_data.passkey.provider_passkey >> 16) & 0xFF;
    response[2] = (g_fast_pair_data.passkey.provider_passkey >> 8) & 0xFF;
    response[3] = g_fast_pair_data.passkey.provider_passkey & 0xFF;
    for (i = 4; i < 16; i++)
    {
        response[i] = UtilRandom() & 0xFF;
    }

    return response;
}

/*******************************************************************************
NAME
    checkSeekerPasskey

DESCRIPTION
    Matches seeker passkey with provider passkey and sends Accept or Reject Ind.

PARAMETERS
    passkey - Seeker encrypted passkey in decrypted format

RETURNS
    True or False
*/
static bool checkSeekerPasskey(uint32 passkey)
{
    if (g_fast_pair_data.passkey.provider_passkey != PASSKEY_INVALID)
    {
        MessageCancelAll(&theSink.task, EventUsrConfirmationReject);

        FAST_PAIR_INFO(("Fast Pair passkey received seeker=%06ld provider=%06ld\n", passkey, g_fast_pair_data.passkey.provider_passkey));

        MessageSend(&theSink.task, (g_fast_pair_data.passkey.provider_passkey == passkey) ? EventUsrConfirmationAccept : EventUsrConfirmationReject, NULL);
        return (g_fast_pair_data.passkey.provider_passkey == passkey) ? TRUE: FALSE;
    }

    return FALSE;
}

/*******************************************************************************
NAME
    sinkFastPairStartTimer

DESCRIPTION
    Starts a timer when the Fast Pair state apaprt from Idle is entered.

PARAMETERS
    None

RETURNS
    None
*/
static void sinkFastPairStartTimer(void)
{
    uint16 timeout_s = FAST_PAIR_STATE_TIMEOUT;

    FAST_PAIR_INFO(("sinkFastPairStartTimer timeout=[%u s]\n", timeout_s));

    /* Make sure any pending messages are cancelled */
    MessageCancelFirst(sinkGetMainTask(), EventSysFastPairStateTimeout);

    /* Start Fast Pair timeer */
    MessageSendLater(sinkGetMainTask(), EventSysFastPairStateTimeout, 0, D_SEC(timeout_s));
}

/*******************************************************************************
NAME
    sinkFastPairStopTimer

DESCRIPTION
    Stops a timer when the fast pair state changes.

PARAMETERS
    None

RETURNS
    None
*/
static void sinkFastPairStopTimer(void)
{
    FAST_PAIR_INFO(("sinkFastPairStopTimer\n"));

    /* End Fast Pairing timer */
    MessageCancelFirst(sinkGetMainTask(), EventSysFastPairStateTimeout);
}

/*******************************************************************************
NAME
    sinkFastPairIsInProgress

DESCRIPTION
    Checks if fast pairing is in progress.

*/
bool sinkFastPairIsInProgress(void)
{

    if (g_fast_pair_data.fp_state != fast_pair_idle)
        return TRUE;
    else
        return FALSE;
}

/*******************************************************************************
NAME
    sinkFastPairGetStatus

DESCRIPTION
    Checks if fast pair is in successful or not.

*/
bool sinkFastPairGetStatus(void)
{

    if (g_fast_pair_data.fp_status == fast_pair_success)
        return TRUE;
    else
        return FALSE;
}

/*******************************************************************************
NAME
    sinkFastPairSetStatus

DESCRIPTION
    Sets the status of fast pair.

*/
void sinkFastPairSetStatus(fast_pair_status_t fp_status)
{
    g_fast_pair_data.fp_status= fp_status;
}


/*******************************************************************************
NAME
    sinkFastPairHandleProviderPasskey

DESCRIPTION
    Save provider passkey when User confirmation Indication is invoked.

*/
void sinkFastPairHandleProviderPasskey(uint32 passkey)
{
    FAST_PAIR_INFO(("FPS: Provider Passkey Provided\n"));
    if (sinkFastPairIsInProgress())
    {
        /* Wait for passkey */
        g_fast_pair_data.passkey.provider_passkey = passkey;

        FAST_PAIR_INFO(("Provider passkey=%06ld\n", g_fast_pair_data.passkey.provider_passkey));
    }
    else
        g_fast_pair_data.passkey.provider_passkey = PASSKEY_INVALID;
}

/*******************************************************************************
NAME
    sinkFastPairDisconnectLELink

DESCRIPTION
    Disconnects LE link and resets fastpair state back to idle.

*/
void sinkFastPairDisconnectLELink(void)
{
    FAST_PAIR_INFO(("sinkFastPairDisconnectLELink: Fastpair State=%d\n", g_fast_pair_data.fp_state));
    FAST_PAIR_INFO(("sinkFastPairDisconnectLELink: Fastpair cid=%d\n", g_fast_pair_data.cid));

    if(g_fast_pair_data.cid != INVALID_CID)
    {
        FAST_PAIR_INFO(("sinkFastPairDisconnectLELink: Entered\n"));
        GattManagerDisconnectRequest(g_fast_pair_data.cid);
        sinkFastPairSetFastPairState(fast_pair_idle);
    }
}

/*******************************************************************************
NAME
    sinkFastPairGetFastPairState

DESCRIPTION
    Gets new fast pair state.

*/
fast_pair_state_t sinkFastPairGetFastPairState(void)
{
    return g_fast_pair_data.fp_state;
}

/*******************************************************************************
NAME
    sinkFastPairSetFastPairState

DESCRIPTION
    Sets new fast pair state. Reset fast pair data if state moves back to Idle

*/
void sinkFastPairSetFastPairState(fast_pair_state_t new_state)
{
    g_fast_pair_data.fp_state = new_state;

    sinkFastPairStopTimer();

    if(g_fast_pair_data.fp_state != fast_pair_idle)
    {
        /* Restart Fast Pair State timer */
        sinkFastPairStartTimer();
    }
    else
    {
        /* Free allocated Memory */
        if(g_fast_pair_data.private_key)
        {
            free(g_fast_pair_data.private_key);
            g_fast_pair_data.private_key = NULL;
        }

        if(g_fast_pair_data.public_key)
        {
            free(g_fast_pair_data.public_key);
             g_fast_pair_data.public_key = NULL;
        }

        if(g_fast_pair_data.aes_key)
        {
            free(g_fast_pair_data.aes_key);
            g_fast_pair_data.aes_key = NULL;
        }

        memset(&g_fast_pair_data.kbp_req, 0, sizeof(g_fast_pair_data.kbp_req));

        g_fast_pair_data.fast_pair_server = NULL;
        g_fast_pair_data.cid = INVALID_CID;
        g_fast_pair_data.encrypted_data_size = 0;
        g_fast_pair_data.fp_status = fast_pair_success;
    }
}

/*******************************************************************************
NAME
    sinkFastPairKeybasedPairingWrite

DESCRIPTION
    Invoked when new key based pairing write request is recieved. Fetches private key
    stores encrypted data block, public key until shared secret is calculated.

*/
void sinkFastPairKeybasedPairingWrite(const GFPS* fast_pair_server, uint16 cid, uint8* enc_data, uint16 size)
{
    /* Store Encrytped Write request */
    g_fast_pair_data.fast_pair_server = fast_pair_server;
    g_fast_pair_data.cid = cid;

    /* Get private Key */
    sinkFastPairGetAntiSpoofingPrivateKey();

    /* Store encrypted data for later use */
    sinkFastPairStoreEncryptedBlock(enc_data);

    if(size > FAST_PAIR_ENCRYPTED_REQUEST_LEN)
    {
       sinkFastPairStorePublicKey(&(enc_data[FAST_PAIR_ENCRYPTED_REQUEST_LEN]));
    }
    
    /* Initiate calculation of shared secret Key */
    sinkFastPairCalculateSharedSecret(g_fast_pair_data.private_key, g_fast_pair_data.public_key);

    sinkFastPairSetFastPairState(fast_pair_wait_aes_key);
    
}

/*******************************************************************************
NAME
    sinkFastPairPasskeyWrite

DESCRIPTION
    Invoked when new passkey write request is recieved. .

*/
void sinkFastPairPasskeyWrite(const GFPS* fast_pair_server, uint16 cid, uint8* enc_data)
{
    /* Store Encrytped Write request */
    g_fast_pair_data.fast_pair_server = fast_pair_server;
    g_fast_pair_data.cid = cid;

    /* Decrypt passkey Block */
    ConnectionDecryptBlockAes(sinkGetMainTask(), (uint16 *)enc_data, (uint16 *)g_fast_pair_data.aes_key);
}

/*******************************************************************************
NAME
    sinkFastPairAccountKeyWrite

DESCRIPTION
    Invoked when new account key write request is recieved. .

*/
void sinkFastPairAccountKeyWrite(const GFPS* fast_pair_server, uint16 cid, uint8* enc_data)
{    
    /* Store Encrytped Write request */
    g_fast_pair_data.fast_pair_server = fast_pair_server;
    g_fast_pair_data.cid = cid;

    /* Decrypt passkey Block */
    ConnectionDecryptBlockAes(sinkGetMainTask(), (uint16 *)enc_data, (uint16 *)g_fast_pair_data.aes_key);
}

/*******************************************************************************
NAME
    sinkFastPairHandleMessages

DESCRIPTION
    Message handler for sink fast pair

*/
void sinkFastPairHandleMessages(Task task, MessageId id, Message message)
{
    UNUSED(task);

    switch(id)
    {
        case CL_CRYPTO_GENERATE_SHARED_SECRET_KEY_CFM:
        {
            CL_CRYPTO_GENERATE_SHARED_SECRET_KEY_CFM_T *cfm = (CL_CRYPTO_GENERATE_SHARED_SECRET_KEY_CFM_T *)message;

            if(cfm->status == success)
            {
                ConnectionEncryptBlockSha256(sinkGetMainTask(), cfm->shared_secret_key, CL_CRYPTO_SHA_DATA_LEN);
            }
            else
            {
                FAST_PAIR_ERROR(("Shared Secret Key calcuation failed!\n", id));
            }
        }
        break;

        case CL_CRYPTO_HASH_CFM:
        {
            CL_CRYPTO_HASH_CFM_T *cfm = (CL_CRYPTO_HASH_CFM_T *)message;
            
            if(cfm->status == success)
            {
                if(g_fast_pair_data.fp_state == fast_pair_wait_aes_key)
                {
                    sinkFastPairSetFastPairState(fast_pair_wait_key_based_pairing_response);

                    /* Store AES Key */
                    g_fast_pair_data.aes_key = PanicUnlessMalloc(AES_BLOCK_SIZE);
                    memcpy(g_fast_pair_data.aes_key, cfm->hash, AES_BLOCK_SIZE);

                    /*Fetch Encrypted Block */
                    ConnectionDecryptBlockAes(sinkGetMainTask(), (uint16 *)g_fast_pair_data.encrypted_data, (uint16 *)g_fast_pair_data.aes_key);

                }
                else
                {

                    /* Handle Account Key filter generation */
                    bleFastPairHandleAccountKeys(cfm);
                }
            }
            else
            {
                FAST_PAIR_ERROR(("Hash calcuation failed!\n", id));
            }
        }
        break;

        case CL_CRYPTO_ENCRYPT_CFM:
        {
             CL_CRYPTO_ENCRYPT_CFM_T *cfm = (CL_CRYPTO_ENCRYPT_CFM_T *)message;

            if(cfm->status == success)
            {
                if(g_fast_pair_data.fp_state == fast_pair_wait_key_based_pairing_response)
                {
                   /* Send Notification */
                   GattFastPairServerKeybasedPairingNotification(
                        g_fast_pair_data.fast_pair_server,
                        g_fast_pair_data.cid,
                        (uint8 *)cfm->encrypted_data
                        );  
                    sinkFastPairSetFastPairState(fast_pair_wait_passkey);
		      sinkBleGapStartReadLocalName(ble_gap_read_name_advertising);
                }
                else if(g_fast_pair_data.fp_state == fast_pair_wait_passkey_response)
                {
                    /* Send Encrypted Passkey */
                    GattFastPairServerPasskeyNotification(
                        g_fast_pair_data.fast_pair_server,
                        g_fast_pair_data.cid,
                        (uint8 *)cfm->encrypted_data
                        );
                    /* Go to account key write state */
                   sinkFastPairSetFastPairState(fast_pair_wait_account_key);
                }
            }
            else
            {
                FAST_PAIR_ERROR(("AES Encrypt failed!\n", id));
            }  
        }
        break;

        case CL_CRYPTO_DECRYPT_CFM:
        {
             CL_CRYPTO_DECRYPT_CFM_T *cfm = (CL_CRYPTO_DECRYPT_CFM_T *)message;

            if(cfm->status == success)
            {
                if(g_fast_pair_data.fp_state == fast_pair_wait_key_based_pairing_response)
                {
                    if(sinkFastPairMatchProviderAddress(cfm->decrypted_data))
                    {
                        uint8* raw_response = sinkFastPairGenerateResponse();
                        uint8* decrypted_data_be = (uint8 *)cfm->decrypted_data;

                        /* Encrypt Raw Response with AES Key */
                        ConnectionEncryptBlockAes(sinkGetMainTask(), (uint16 *)raw_response, (uint16 *)g_fast_pair_data.aes_key);

                        free(raw_response);

                        g_fast_pair_data.kbp_req.request_discoverability = (decrypted_data_be[1] & 0x1) ? TRUE : FALSE;
                        
                        g_fast_pair_data.kbp_req.request_bonding = (decrypted_data_be[1] & 0x2) ? TRUE : FALSE;
                        g_fast_pair_data.kbp_req.provider_addr.nap = ((uint16)decrypted_data_be[2] << 8) | (uint16)decrypted_data_be[3];
                        g_fast_pair_data.kbp_req.provider_addr.uap = decrypted_data_be[4];
                        g_fast_pair_data.kbp_req.provider_addr.lap = ((uint32)decrypted_data_be[5] << 16 ) | ((uint32)decrypted_data_be[6] << 8 ) | (uint32)decrypted_data_be[7];

                        /* We always go to pairable mode */                        
                        sinkInquirySetInquirySession(inquiry_session_normal);
                        stateManagerEnterConnDiscoverableState( TRUE );
                        
                        if (g_fast_pair_data.kbp_req.request_bonding)
                        {
                            g_fast_pair_data.kbp_req.seeker_addr.nap = ((uint16)decrypted_data_be[8] << 8) | (uint16)decrypted_data_be[9];
                            g_fast_pair_data.kbp_req.seeker_addr.uap = decrypted_data_be[10];
                            g_fast_pair_data.kbp_req.seeker_addr.lap = ((uint32)decrypted_data_be[11] << 16 ) | ((uint32)decrypted_data_be[12] << 8 ) | (uint32)decrypted_data_be[13];

                            FAST_PAIR_INFO(("Send SecurityRequest to peer_addr %04x%02x%06lx\n", g_fast_pair_data.kbp_req.seeker_addr.nap, g_fast_pair_data.kbp_req.seeker_addr.uap, g_fast_pair_data.kbp_req.seeker_addr.lap));
                            gapStartEncryption(FALSE,g_fast_pair_data.cid);

                        }
                        else
                        {
                           memset(&g_fast_pair_data.kbp_req.seeker_addr, 0, sizeof(bdaddr));
                        }
                        

                    }
                    else
                    {
                        /* AES Key Not Valid!. Free it and set fast Pair state to state to Idle */
                        sinkFastPairSetFastPairState(fast_pair_idle);
                    }
                }
                else if(g_fast_pair_data.fp_state == fast_pair_wait_passkey)
                {
                    uint8* decrypted_data_be = (uint8 *)cfm->decrypted_data;
                    if (decrypted_data_be[0] != fast_pair_passkey_seeker)
                    {
                        /* We failed to decrypt passkey */
                        FAST_PAIR_ERROR(("Failed to decrypt passkey!\n"));

                        /* AES Key Not Valid!. Free it and set fast Pair state to state to Idle */
                        sinkFastPairSetFastPairState(fast_pair_idle);

                    }
                    else
                    {
                        g_fast_pair_data.passkey.seeker_passkey = ((uint32)decrypted_data_be[1] << 16 ) |
                                                  ((uint32)decrypted_data_be[2] << 8 ) |
                                                  (uint32)decrypted_data_be[3];
                    }

                    if(checkSeekerPasskey(g_fast_pair_data.passkey.seeker_passkey))
                    {
                    
                        uint8* passkey_response =  sinkFastPairPasskeyResponse();
                    
                        /* Encrypt Raw Response with AES Key */
                        ConnectionEncryptBlockAes(sinkGetMainTask(), (uint16 *)passkey_response, (uint16 *)g_fast_pair_data.aes_key);

                        free(passkey_response);

                        sinkFastPairSetFastPairState(fast_pair_wait_passkey_response);
                         
                    }
                    else
                    {
                        /* We failed to match passkey */
                        FAST_PAIR_ERROR(("Failed to match passkey!\n"));
                    }

                }
                else if(g_fast_pair_data.fp_state == fast_pair_wait_account_key)
                {
                    sinkFastPairStoreAccountKey((uint8 *)cfm->decrypted_data);

                    /* since we are with fast pairing set BLE bonding state to non-bondable */
                    sinkBleSetGapBondState(ble_gap_bond_state_non_bondable);

                    /* Go to Fast pair Idle state for next conenction */
                    sinkFastPairSetFastPairState(fast_pair_idle);
                }
            }
            else
            {
                FAST_PAIR_ERROR(("AES Decrypt failed!\n", id));
            }  
        }
        break;
        
        default:
        {
            FAST_PAIR_ERROR(("handleFastPairCryptoMessage: unhandled (0x%04x)!\n", id));
        }
        break;
    }

}

/*************************************************************************
NAME
    SinkFastPairInit

DESCRIPTION
    Initialize Fast Pair by allocating memory for fast pair global structure
*/
void sinkFastPairInit(void)
{
    /* Initialise Fast Pair global structure */
    memset(&g_fast_pair_data, 0, sizeof(g_fast_pair_data));
	
    g_fast_pair_data.cid = INVALID_CID;
	
    /* Set the handler */
    g_fast_pair_data.task.handler = sinkFastPairHandleMessages;
}

/*************************************************************************
NAME
    sinkFastPairClearAccountKeys

DESCRIPTION
    Clear all the fast pair account keys from PS store
*/
void sinkFastPairClearAccountKeys(void)
{
    config_store_status_t status;
    status = ConfigStoreRemoveConfig(FAST_PAIR_WRITABLE_ACCOUNT_KEYS_CONFIG_BLK_ID);

    FAST_PAIR_INFO(("Try to delete Fast Pair Account Keys\n"));

    switch(status)
    {
        case config_store_success :
            FAST_PAIR_INFO(("Deleted Fast Pair Account Keys successfully\n"));
        break;
        case config_store_error_config_block_not_found :
            FAST_PAIR_INFO(("Failed to delete Fast Pair Account Keys. Config block not found\n"));
        break;
        default :
            FAST_PAIR_ERROR(("This should not have happened at all\n", FALSE));
        break;
    }
}

#endif
