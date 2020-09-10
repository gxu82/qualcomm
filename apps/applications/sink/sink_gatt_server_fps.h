/****************************************************************************
Copyright (c) 2018 Qualcomm Technologies International, Ltd.
Part of 6.3.2

FILE NAME
    sink_gatt_server_fps.h

DESCRIPTION
    Routines to handle messages sent from the GATT Running Fast Pair Server Task.
    
NOTES

*/

#ifndef _SINK_GATT_SERVER_FP_H_
#define _SINK_GATT_SERVER_FP_H_

#if defined GATT_ENABLED && defined enable_fast_pair
#define GATT_FP_SERVER
#else
#undef GATT_FP_SERVER
#endif

/* Library headers */
#ifdef GATT_FP_SERVER
#include <gatt_fast_pair_server.h>
#endif

#include <csrtypes.h>
#include <message.h>

#ifdef GATT_FP_SERVER
#define sinkGattFastPairServerGetSize() sizeof(GFPS)
#else
#define sinkGattFastPairServerGetSize() 0
#endif

/*******************************************************************************
NAME
    sinkGattFastPairServerInitialise
    
DESCRIPTION
    Initialize Fast Pair server task.
    
PARAMETERS
    ptr - pointer to allocated memory to store server task's Fast Pair data.
    
RETURNS
    TRUE if the Fast Pair server task was initialized, FALSE otherwise.
*/
#ifdef GATT_FP_SERVER
bool sinkGattFastPairServerInitialise(uint16 **ptr);
#else
/* This is set to TRUE if the server is not defined, so as not to block the flow
 * of execution for initialiseOptionalServerTasks in sink_gatt_init.c. If it was
 * false, that function would return prematurely denoting an error in
 * initialization instead of merely skipping an undefined service. */
#define sinkGattFastPairServerInitialise(ptr) (TRUE)
#endif


/*******************************************************************************
NAME
    sinkGattFastPairServerMsgHandler
    
DESCRIPTION
    Handle messages from the GATT Fast Pair Service library
    
PARAMETERS
    task    The task the message is delivered
    id      The ID for the GATT message
    message The message payload
    
RETURNS
    void
*/
#ifdef GATT_FP_SERVER
void sinkGattFastPairServerMsgHandler(Task task, MessageId id, Message message);
#else
#define sinkGattFastPairServerMsgHandler(task, id, message) ((void)(0))
#endif

/*******************************************************************************
NAME
    sinkGattFastPairServiceEnabled
    
DESCRIPTION
    Returns whether Fast Pair service is enabled or not.
*/
bool sinkGattFastPairServiceEnabled(void);

#endif /* _SINK_GATT_SERVER_FP_H_ */

