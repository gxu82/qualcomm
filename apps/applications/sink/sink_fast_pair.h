/****************************************************************************
Copyright (c) 2018 Qualcomm Technologies International, Ltd.
Part of 6.3.2

FILE NAME
    sink_fast_pair.h

DESCRIPTION
    Header file for interface with fast pair application

NOTES

*/

#ifndef _SINK_FAST_PAIR_H_
#define _SINK_FAST_PAIR_H_

#include <csrtypes.h>
#include <message.h>
#include "sink_private_data.h"
#include "sink_gatt_db.h"

/* Work around gcc bug converting uppercase defines to lower case */
#ifdef enable_fast_pair
#define ENABLE_FAST_PAIR
#endif

/* Fast Pair requires GATT */
#ifndef GATT_ENABLED
#undef ENABLE_FAST_PAIR
#endif

#ifdef ENABLE_FAST_PAIR
/* Library headers */
#include <gatt_fast_pair_server.h>
#include <cryptoalgo.h>
#endif

#define PASSKEY_INVALID         0xFF000000UL


#define MAX_FAST_PAIR_ACCOUNT_KEYS              (5)

#define FAST_PAIR_ENCRYPTED_REQUEST_LEN         (16)
#define FAST_PAIR_ENCRYPTED_PASSKEY_BLOCK_LEN   (16)
#define FAST_PAIR_ENCRYPTED_ACCOUNT_KEY_LEN     (16)

#define FAST_PAIR_PUBLIC_KEY_LEN                (64)
#define FAST_PAIR_PRIVATE_KEY_LEN               (32)
#define FAST_PAIR_ACCOUNT_KEY_LEN               (16)


#define PROVIDER_ADDRESS_OFFSET                 (2)


typedef enum
{
    fast_pair_passkey_seeker = 2,
    fast_pair_passkey_provider = 3
} fast_pair_passkey_owner_t;

typedef enum
{
    fast_pair_idle,
    fast_pair_wait_aes_key,
    fast_pair_wait_key_based_pairing_response,
    fast_pair_wait_passkey,
    fast_pair_wait_passkey_response,
    fast_pair_wait_account_key
} fast_pair_state_t;

typedef enum
{
    fast_pair_success,
    fast_pair_io_cap_failure
} fast_pair_status_t;


typedef struct
{
    bdaddr provider_addr;
    bdaddr seeker_addr;
    bool request_discoverability;
    bool request_bonding;
} fast_pair_key_based_pairing_req_t;

typedef struct
{
    uint32 seeker_passkey;
    uint32 provider_passkey;
} fast_pair_passkey_t;

typedef struct
{
    uint8 num_keys;
    uint8 (*keys)[FAST_PAIR_ACCOUNT_KEY_LEN];
} fast_pair_account_keys_t;

typedef struct 
{
    TaskData                task;
    fast_pair_state_t       fp_state;           /* Fast pair state  */
    fast_pair_status_t      fp_status;          /* Fast pair status state */
    const GFPS              *fast_pair_server;
    uint16                  cid;
    uint8                   *aes_key;
    uint8                   *encrypted_data;
    uint16                   encrypted_data_size;
    uint16                  *public_key;
    uint16                  *private_key;
    fast_pair_key_based_pairing_req_t kbp_req;
    fast_pair_passkey_t     passkey;
}sink_fast_pair_data_t;

/****************************************************************************
NAME    
    sinkFastPairIsInProgress

DESCRIPTION
    Checks if Fast Pairing is in Progress or not
*/
#ifdef ENABLE_FAST_PAIR
bool sinkFastPairIsInProgress(void);
#else
#define sinkFastPairIsInProgress() ((void)(0))
#endif

/****************************************************************************
NAME
    sinkFastPairGetStatus

DESCRIPTION
    Checks if fast pair is in successful or not.
*/
#ifdef ENABLE_FAST_PAIR
bool sinkFastPairGetStatus(void);
#else
#define sinkFastPairGetStatus() ((void)(0))
#endif

/****************************************************************************
NAME
    sinkFastPairSetStatus

DESCRIPTION
    Sets the status for fast pair.
*/
#ifdef ENABLE_FAST_PAIR
void sinkFastPairSetStatus(fast_pair_status_t fp_status);
#else
#define sinkFastPairSetStatus(fp_status) ((void)(0))
#endif



/****************************************************************************
NAME    
    sinkFastPairHandleProviderPasskey

DESCRIPTION
    Saves the Fast Pair Provider Passkey to checking later during paring
*/
#ifdef ENABLE_FAST_PAIR
void sinkFastPairHandleProviderPasskey(uint32 passkey);
#else
#define sinkFastPairHandleProviderPasskey(passkey) ((void)(0))
#endif

/****************************************************************************
NAME
    sinkFastPairDisconnectLELink

DESCRIPTION
    Disconnects LE link and resets fastpair state back to idle.
*/
#ifdef ENABLE_FAST_PAIR
void sinkFastPairDisconnectLELink(void);
#else
#define sinkFastPairDisconnectLELink() ((void)(0))
#endif

/****************************************************************************
NAME
    sinkFastPairGetFastPairState

DESCRIPTION
    Gets the Fast Pair state
*/
#ifdef ENABLE_FAST_PAIR
fast_pair_state_t sinkFastPairGetFastPairState(void);
#else
#define sinkFastPairGetFastPairState() ((void)(0))
#endif

/****************************************************************************
NAME
    sinkFastPairSetFastPairState

DESCRIPTION
    Sets the Fast Pair state
*/
#ifdef ENABLE_FAST_PAIR
void sinkFastPairSetFastPairState(fast_pair_state_t new_state);
#else
#define sinkFastPairSetFastPairState(new_state) ((void)(0))
#endif

/****************************************************************************
NAME    
    sinkFastPairKeybasedPairingWrite

DESCRIPTION
    Handle Keybased Pairing Write request from Fast Pair seeker. The AES key is 
    calculated and notification will be send if decryption is susccessful. 
*/
#ifdef ENABLE_FAST_PAIR
void sinkFastPairKeybasedPairingWrite(const GFPS* fast_pair_server, uint16 cid, uint8* enc_data, uint16 size);
#else
#define sinkFastPairKeybasedPairingWrite(fast_pair_server, cid, enc_data, size) ((void)(0))
#endif

/****************************************************************************
NAME    
    sinkFastPairPasskeyWrite

DESCRIPTION
    Handle Passkey Write request from Fast Pair seeker. The passkey is 
    decrypted and matched with provider's passkey and encrypted 
    notification will be send if decryption is susccessful. 
*/
#ifdef ENABLE_FAST_PAIR
void sinkFastPairPasskeyWrite(const GFPS* fast_pair_server, uint16 cid, uint8* enc_data);
#else
#define sinkFastPairPasskeyWrite(fast_pair_server, cid, enc_data) ((void)(0))
#endif

/****************************************************************************
NAME    
    sinkFastPairAccountKeyWrite

DESCRIPTION
    Handle account key Write request from Fast Pair seeker. The account key is 
    decrypted and stored in NVM if decryption is susccessful. 
*/
#ifdef ENABLE_FAST_PAIR
void sinkFastPairAccountKeyWrite(const GFPS* fast_pair_server, uint16 cid, uint8* enc_data);
#else
#define sinkFastPairAccountKeyWrite(fast_pair_server, cid, enc_data) ((void)(0))
#endif

/****************************************************************************
NAME    
    sinkFastPairHandleMessages

DESCRIPTION
    Handle fast pair messages from crypto library. 
*/
#ifdef ENABLE_FAST_PAIR
void sinkFastPairHandleMessages(Task task, MessageId id, Message message);
#else
#define sinkFastPairHandleMessages(task, id, message) ((void)(0))
#endif

/****************************************************************************
NAME    
    sinkFastPairInit

DESCRIPTION
    Init Fast Pair application. 
*/
#ifdef ENABLE_FAST_PAIR
void sinkFastPairInit(void);
#else
#define sinkFastPairInit() ((void)(0))
#endif

/****************************************************************************
NAME
    sinkFastPairClearAccountKeys

DESCRIPTION
    Clear Fast Pair account keys if any.
*/
#ifdef ENABLE_FAST_PAIR
void sinkFastPairClearAccountKeys(void);
#else
#define sinkFastPairClearAccountKeys() ((void)(0))
#endif

#endif /* _SINK_FAST_PAIR_H_ */
