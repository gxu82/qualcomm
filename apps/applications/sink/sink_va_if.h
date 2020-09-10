/****************************************************************************
Copyright (c) 2018 Qualcomm Technologies International, Ltd.
Part of 6.3.2

FILE NAME
    sink_va_if.h

DESCRIPTION
    Header file for the interface to the sink VA functionality

*/

/*!
@file   sink_va_if.h
@brief  Interface to the voice assistant functionality in the sink app

*/

#ifndef SINK_VA_IF_H_
#define SINK_VA_IF_H_

#include <csrtypes.h>

#if (defined(ENABLE_AMA))
#define ENABLE_VOICE_ASSISTANT
#endif /*ENABLE_AMA*/

/*!
    @brief Initialize the Voice Assistant Application module

    @param None

    @return None
*/
#ifdef ENABLE_VOICE_ASSISTANT
void SinkVaInit(void);
#else
#define SinkVaInit() ((void)(0))
#endif


/*!
    @brief Calls for the cleaning up of transport state after Link Loss event. 

    @param None

    @return None
*/
#ifdef ENABLE_VOICE_ASSISTANT
void SinkVaHandleLinkLoss(void);
#else
#define SinkVaHandleLinkLoss() ((void)(0))
#endif


/*!
    @brief Triggers the event to start the button tap VA session

    @param None

    @return None
*/
#ifdef ENABLE_VOICE_ASSISTANT
void SinkVaTriggerTaptoTalk(void);
#else
#define SinkVaTriggerTaptoTalk() ((void)(0))
#endif

/*!
    @brief Triggers the event to start or stop the Push to talk VA session
    based on the VA session state.

    @param None

    @return None
*/
#ifdef ENABLE_VOICE_ASSISTANT
void SinkVaTriggerPushToTalk(void);
#else
#define SinkVaTriggerPushToTalk() ((void)(0))
#endif

/*!
    @brief Triggers the event to send the Push to talk release

    @param None

    @return None
*/
#ifdef ENABLE_VOICE_ASSISTANT
void SinkVaTriggerPushToTalkRelease(void);
#else
#define SinkVaTriggerPushToTalkRelease() ((void)(0))
#endif


/*!
    @brief Triggers the user event to Cancel the VA session

    @param None

    @return None
*/
#ifdef ENABLE_VOICE_ASSISTANT
void SinkVaUsrCancelSession(void);
#else
#define SinkVaUsrCancelSession() ((void)(0))
#endif

/*!
    @brief Abort the VA session when HFP incoming/outgoing call.

    @param None

    @return None
*/
#ifdef ENABLE_VOICE_ASSISTANT
void SinkVaCancelSession(void);
#else
#define SinkVaCancelSession() ((void)(0))
#endif /* ENABLE_VOICE_ASSISTANT */
/*!
    @brief Abort the VA session when HFP incoming/outgoing call.

    @param None

    @return None
*/
#ifdef ENABLE_VOICE_ASSISTANT
void SinkVaResumeSession(void);
#else
#define SinkVaResumeSession() ((void)(0))
#endif /* ENABLE_VOICE_ASSISTANT */

#endif /* SINK_VA_IF_H_ */
