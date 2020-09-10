/*******************************************************************************
Copyright (c) 2017 - 2018 Qualcomm Technologies International, Ltd.
Part of 6.2

FILE NAME
    sink_va_if.c

DESCRIPTION
    Interface to the selected voice assistant

NOTES

*/

#include "sink_va_if.h"
#include "sink_ama.h"
#include <vmtypes.h>

#ifdef ENABLE_VOICE_ASSISTANT

/*
    Where more than one assistant's handler function is called the appropriate
    assistant is currently selected by not defining the function of the unused
    voice assistant in the header. Mutual exclusivity is enforced by the project
    settings.
*/


/*****************************************************************************/
void SinkVaTriggerTaptoTalk(void)
{
    SinkAmaTriggerTaptoTalk();
}

/*****************************************************************************/
void SinkVaUsrCancelSession(void)
{
    SinkAmaVoiceStop();

}

/*****************************************************************************/
void SinkVaTriggerPushToTalk(void)
{
    SinkAmaTriggerPushtoTalk();
}

/*****************************************************************************/
void SinkVaTriggerPushToTalkRelease(void)
{
    SinkAmaTriggerPushtoTalkRelease();
}

/*****************************************************************************/
void SinkVaCancelSession(void)
{
    SinkAmaHFPCallInd();
}

/*****************************************************************************/
void SinkVaResumeSession(void)
{
    SinkAmaResumeSession();
}

/*****************************************************************************/
void SinkVaInit(void)
{
    SinkAmaInit();
}

/*****************************************************************************/
void SinkVaHandleLinkLoss(void)
{
    SinkAmaResetTransportAfterLinkLoss();
}

#endif /* ENABLE_VOICE_ASSISTANT*/
