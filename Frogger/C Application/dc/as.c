/////////////////////////////////////////////////////////////////////////////
//
// AS LAYER
//
//   - ac layer command buffering/scheduling routines for acDigi* functions
//
//
// Version 1.0 - David Harvey / Sega Europe 3/2000
//
//////////////////////////////////////////////////////////////////////////////


#include "as.h"

#include <string.h>

#include "bpamsfx.h"
#include "bpprintf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sn_fcntl.h>   /* LibCross file types. */
#include <usrsnasm.h>   /* LibCross I/O routines. */
#include "fixed.h"

//port flags
#define PF_STOPPED              1
#define PF_RESTARTPLAY          2
#define PF_OPENCLOSECHANGE      4
#define PF_FILTERCHANGE         8
#define PF_AMPCHANGE            16
#define PF_LFOCHANGE            32
#define PF_LOOPPOINTCHANGE      64
#define PF_MULTIPLAY            128
#define PF_MULTISTOP            256
#define PF_CALLBACK             512
#define PF_DSPMIXERCHANGE       1024

#define PF_MULTI                (PF_MULTIPLAY|PF_MULTISTOP)


//global flags
#define GF_CDVOLUMECHANGE       1
#define GF_CDPANCHANGE          2


//maximum number of ports to manage
#define AS_MAX_PORTS 64
#define AS_MAX_QSOUND_PORTS 8


//global parameters structure
typedef struct tagASGLOBALPARAMS
{
    KTU32 leftCDVolume;
    KTU32 rightCDVolume;
    KTU32 leftCDPan;
    KTU32 rightCDPan;
} ASGLOBALPARAMS, *PASGLOBALPARAMS;


//QSound parameters structure
typedef struct tagACQSOUNDPARAMS
{
    KTU32 angle;
} ACQSOUNDPARAMS, *PACQSOUNDPARAMS;


//port parameter structure
typedef struct tagASPORTPARAMS
{
    KTU32 nVolume;
    KTU32 nPan;
    KTU32 nSampleRate;
    KTU32* pSampleAddress;
    KTU32 nSizeInBytes;
    KTU32 nSizeInSamples;
    KTU32 dspMixerChannel;
    KTU32 dspSendLevel;
    KTU32 directLevel;
    KTS32 frequencyOrCentsOffset;
    KTU32 callbackOffsetInFrames;

    KTU32 filterOn;
    KTU32 filterQ;
    KTU32 filterValue0;
    KTU32 filterValue1;
    KTU32 filterValue2;
    KTU32 filterValue3;
    KTU32 filterValue4;
    KTU32 filterAttackRate;
    KTU32 filterDecayRate1;
    KTU32 filterDecayRate2;
    KTU32 filterReleaseRate;

    KTU32 ampAttackRate;
    KTU32 ampDecayRate1;
    KTU32 ampDecayRate2;
    KTU32 ampReleaseRate;
    KTU32 ampDecayLevel;
    KTU32 ampKeyRateScaling;
    KTU32 ampLoopStartLink;

    KTU32 lfoReset;
    KTU32 lfoFrequency;
    KTU32 pitchLfoWaveShape;
    KTU32 pitchLfoDepth;
    KTU32 ampLfoWaveShape;
    KTU32 ampLfoDepth;

    KTU16 loopStartOffsetInFrames;
    KTU16 loopEndOffsetInFrames;

    AC_AUDIO_TYPE aicaAudioType;
    AC_PITCH_SET_TYPE frequencyOrCentsFlag;
} ASPORTPARAMS, *PASPORTPARAMS;



//global as data
KTU32 g_asFlags; //global flags
ASGLOBALPARAMS g_asgpCurrent, g_asgpTarget; //current and target global parameters

//QSound data
ACQSOUNDPARAMS g_qspCurrent[AS_MAX_QSOUND_PORTS], g_qspTarget[AS_MAX_QSOUND_PORTS]; //current and target QSound parameters

//port data
KTU32 g_asPortFlags[AS_MAX_PORTS]; //port flags
ASPORTPARAMS g_asppCurrent[AS_MAX_PORTS], g_asppTarget[AS_MAX_PORTS]; //current and target port parameters


//multiplay data
KTU32	g_MultiStopUpperMask;
KTU32	g_MultiStopLowerMask;
KTU32	g_MultiPlayUpperMask[2];
KTU32	g_MultiPlayLowerMask[2]; //[0] == AC_LOOP_OFF and [1] == AC_LOOP_ON
KTU32	g_MultiPlayUpperMaskReady[2];
KTU32	g_MultiPlayLowerMaskReady[2]; //all port initialisation completed



//quick command buffer overflow error check
#define asCheckBufferOverflow() ( acErrorGetLast()->number == AC_CANT_SEND_DRIVER_COMMAND )



//main as layer update function
void asUpdateManager()
{
    static int s_iPort = 0; //current port index
    static int s_iQSoundChannel = 0; //current QSound channel
    register PASPORTPARAMS pasppCurrent = &g_asppCurrent[s_iPort];
    register PASPORTPARAMS pasppTarget = &g_asppTarget[s_iPort];
    register int nLoopCounter;
    register KTU32* pflags = &g_asPortFlags[s_iPort];
    KTU32 uppermask, lowermask;


    //check the global parameters
    if( g_asFlags & GF_CDVOLUMECHANGE )
    {
        if( !acCdSetVolume( g_asgpTarget.leftCDVolume, g_asgpTarget.rightCDVolume ) ) if( asCheckBufferOverflow() ) return;
        g_asgpCurrent.leftCDVolume = g_asgpTarget.leftCDVolume;
        g_asgpCurrent.rightCDVolume = g_asgpTarget.rightCDVolume;
        g_asFlags &= ~GF_CDVOLUMECHANGE;
    }
    if( g_asFlags & GF_CDPANCHANGE )
    {
        if( !acCdSetPan( g_asgpTarget.leftCDPan, g_asgpTarget.rightCDPan) ) if( asCheckBufferOverflow() ) return;
        g_asgpCurrent.leftCDPan = g_asgpTarget.leftCDPan;
        g_asgpCurrent.rightCDPan = g_asgpTarget.rightCDPan;
        g_asFlags &= ~GF_CDPANCHANGE;
    }


    //check each QSound parameter
    for( nLoopCounter = AS_MAX_QSOUND_PORTS; nLoopCounter; nLoopCounter-- )
    {
        //see if it's changed
        if( g_qspCurrent[s_iQSoundChannel].angle != g_qspTarget[s_iQSoundChannel].angle )
        {
            if( !acDspSetQSoundAngle( s_iQSoundChannel, g_qspTarget[s_iQSoundChannel].angle ) ) if( asCheckBufferOverflow() ) return;
            g_qspCurrent[s_iQSoundChannel].angle = g_qspTarget[s_iQSoundChannel].angle;
        }
        s_iQSoundChannel++;
    }


    //check each channel's parameters
    for( nLoopCounter = AS_MAX_PORTS; nLoopCounter; nLoopCounter-- )
    {
        //see if the sound sample address has changed
        if( *pflags & PF_OPENCLOSECHANGE )
        {
            if( pasppCurrent->pSampleAddress != NULL )
            {
                //it's NULL, close the port
                if( !acDigiClose( s_iPort ) ) if( asCheckBufferOverflow() ) return;
                memset(&pasppCurrent[s_iPort], 0, sizeof(ASPORTPARAMS));
            }

            if( pasppTarget->pSampleAddress != NULL )
            {
                //it's non-NULL, open the port
                if( !acDigiOpen( s_iPort, (KTU32)pasppTarget->pSampleAddress, pasppTarget->nSizeInBytes, pasppTarget->aicaAudioType, pasppTarget->nSampleRate ) ) if( asCheckBufferOverflow() ) return;
                pasppCurrent->pSampleAddress = pasppTarget->pSampleAddress;
                pasppCurrent->nSizeInBytes   = pasppTarget->nSizeInBytes;
                pasppCurrent->aicaAudioType  = pasppTarget->aicaAudioType;
                pasppCurrent->nSampleRate    = pasppTarget->nSampleRate;
            }

            *pflags &= ~PF_OPENCLOSECHANGE;
        }

        if( pasppCurrent->pSampleAddress != NULL )
        {
            //are we ready to play?
            if( *pflags & PF_RESTARTPLAY )
            {
                if( !acDigiPlayWithParameters( s_iPort, pasppTarget->nVolume, pasppTarget->nPan, pasppTarget->dspMixerChannel, pasppTarget->dspSendLevel, pasppTarget->directLevel, pasppTarget->frequencyOrCentsOffset, pasppTarget->frequencyOrCentsFlag, pasppTarget->callbackOffsetInFrames, pasppTarget->loopStartOffsetInFrames, pasppTarget->loopEndOffsetInFrames ) ) if( asCheckBufferOverflow() ) return;
                *pflags &= ~(PF_RESTARTPLAY|PF_STOPPED|PF_LOOPPOINTCHANGE);
                pasppCurrent->nVolume                 = pasppTarget->nVolume;
                pasppCurrent->nPan                    = pasppTarget->nPan;
                pasppCurrent->dspMixerChannel         = pasppTarget->dspMixerChannel;
                pasppCurrent->dspSendLevel            = pasppTarget->dspSendLevel;
                pasppCurrent->directLevel             = pasppTarget->directLevel;
                pasppCurrent->frequencyOrCentsOffset  = pasppTarget->frequencyOrCentsOffset;
                pasppCurrent->frequencyOrCentsFlag    = pasppTarget->frequencyOrCentsFlag;
                pasppCurrent->callbackOffsetInFrames  = pasppTarget->callbackOffsetInFrames;
                pasppCurrent->loopStartOffsetInFrames = pasppTarget->loopStartOffsetInFrames;
                pasppCurrent->loopEndOffsetInFrames   = pasppTarget->loopEndOffsetInFrames;
            }
            else
            {
                //should we stop it?
                if( *pflags & PF_STOPPED )
                {
                    if( !acDigiStop( s_iPort ) ) if( asCheckBufferOverflow() ) return;
                    *pflags &= ~PF_STOPPED;
                }


                //change volume if needed
                if( pasppCurrent->nVolume != pasppTarget->nVolume )
                {
                    if( !acDigiSetVolume( s_iPort, pasppTarget->nVolume ) ) if( asCheckBufferOverflow() ) return;
                    pasppCurrent->nVolume = pasppTarget->nVolume;
                }

                //change pan if needed
                if( pasppCurrent->nPan != pasppTarget->nPan )
                {
                    if( !acDigiSetPan( s_iPort, pasppTarget->nPan ) ) if( asCheckBufferOverflow() ) return;
                    pasppCurrent->nPan = pasppTarget->nPan;
                }

                //change sample rate if needed
                if( pasppCurrent->nSampleRate != pasppTarget->nSampleRate )
                {
                    if( !acDigiSetSampleRate( s_iPort, pasppTarget->nSampleRate ) ) if( asCheckBufferOverflow() ) return;
                    pasppCurrent->nSampleRate = pasppTarget->nSampleRate;
                    pasppCurrent->frequencyOrCentsOffset = 0;
                }

                //change pitch if needed
                if( pasppCurrent->frequencyOrCentsOffset != pasppTarget->frequencyOrCentsOffset )
                {
                    if( !acDigiSetCurrentPitch( s_iPort, pasppTarget->frequencyOrCentsOffset ) ) if( asCheckBufferOverflow() ) return;
                    pasppCurrent->frequencyOrCentsOffset = pasppTarget->frequencyOrCentsOffset;
                }

                //change loop points if needed
                if( *pflags & PF_LOOPPOINTCHANGE )
                {
                    if( !acDigiSetLoopPoints( s_iPort, pasppTarget->loopStartOffsetInFrames, pasppTarget->loopEndOffsetInFrames ) ) if( asCheckBufferOverflow() ) return;
                        
                    pasppTarget->loopStartOffsetInFrames = pasppCurrent->loopStartOffsetInFrames;
                    pasppTarget->loopEndOffsetInFrames = pasppCurrent->loopEndOffsetInFrames;
                    *pflags &= ~PF_LOOPPOINTCHANGE;
                }
                
                //set callback event if needed
                if( *pflags & PF_CALLBACK )
                {
                    if( !acDigiRequestEvent( s_iPort, pasppTarget->callbackOffsetInFrames ) ) if( asCheckBufferOverflow() ) return;
                    pasppCurrent->callbackOffsetInFrames = pasppTarget->callbackOffsetInFrames;
                    *pflags &= ~PF_CALLBACK;
                }
                
                //set filter envelope if needed
                if( *pflags & PF_FILTERCHANGE )
                {
                    if( !acDigiSetFilterEnvelope( s_iPort, pasppTarget->filterOn, pasppTarget->filterQ, pasppTarget->filterValue0, pasppTarget->filterValue1, pasppTarget->filterValue2, pasppTarget->filterValue3, pasppTarget->filterValue4, pasppTarget->filterAttackRate, pasppTarget->filterDecayRate1, pasppTarget->filterDecayRate2, pasppTarget->filterReleaseRate ) ) if( asCheckBufferOverflow() ) return;
                    *pflags &= ~PF_FILTERCHANGE;
                }

                //set amplitude envelope if needed
                if( *pflags & PF_AMPCHANGE )
                {
                    if( !acDigiSetAmplitudeEnvelope( s_iPort, pasppTarget->ampAttackRate, pasppTarget->ampDecayRate1, pasppTarget->ampDecayRate2, pasppTarget->ampReleaseRate, pasppTarget->ampDecayLevel, pasppTarget->ampKeyRateScaling, pasppTarget->ampLoopStartLink ) ) if( asCheckBufferOverflow() ) return;
                    *pflags &= ~PF_AMPCHANGE;
                }

                //set LFO registers if needed
                if( *pflags & PF_LFOCHANGE )
                {
                    if( !acDigiSetLfoRegisters( s_iPort, pasppTarget->lfoReset, pasppTarget->lfoFrequency, pasppTarget->pitchLfoWaveShape, pasppTarget->pitchLfoDepth, pasppTarget->ampLfoWaveShape, pasppTarget->ampLfoDepth ) ) if( asCheckBufferOverflow() ) return;
                    *pflags &= ~PF_LFOCHANGE;
                }

                //check DSP
                if( *pflags & PF_DSPMIXERCHANGE )
                {
                    if( !acDspSetMixerChannel( s_iPort, pasppTarget->dspMixerChannel, pasppTarget->dspSendLevel, pasppTarget->directLevel ) ) if( asCheckBufferOverflow() ) return;
                    pasppCurrent->dspMixerChannel = pasppTarget->dspMixerChannel;
                    pasppCurrent->dspSendLevel = pasppTarget->dspSendLevel;
                    pasppCurrent->directLevel = pasppTarget->directLevel;
                    *pflags &= ~PF_DSPMIXERCHANGE;
                }


                //check multiplay
                if( *pflags & PF_MULTI )
                {
                    //if this port is multiplay then set muliplayready
                    uppermask = lowermask = 0;
                    acDigiMultiSetMask( s_iPort, &uppermask, &lowermask );

                    if( (g_MultiPlayUpperMask[0] & uppermask) || (g_MultiPlayLowerMask[0] & lowermask) )
                    {
                        g_MultiPlayUpperMaskReady[0] |= uppermask;
                        g_MultiPlayLowerMaskReady[0] |= lowermask;
                    }
                    else if( (g_MultiPlayUpperMask[1] & uppermask) || (g_MultiPlayLowerMask[1] & lowermask) )
                    {
                        g_MultiPlayUpperMaskReady[1] |= uppermask;
                        g_MultiPlayLowerMaskReady[1] |= lowermask;
                    }

                    //check unlooped multiplay
                    if( (g_MultiPlayUpperMask[0] == g_MultiPlayUpperMaskReady[0]) && (g_MultiPlayLowerMask[0] == g_MultiPlayLowerMaskReady[0]) )
                    {
                        //set multiplay and clear masks
                        if( !acDigiMultiPlay( AC_LOOP_OFF, g_MultiPlayUpperMask[0], g_MultiPlayLowerMask[0] ) ) if( asCheckBufferOverflow() ) return;
                        g_MultiPlayUpperMask[0] = g_MultiPlayLowerMask[0] = g_MultiPlayUpperMaskReady[0] = g_MultiPlayLowerMaskReady[0] = 0;
                        *pflags &= ~PF_MULTIPLAY;
                    }
                    //[else] check looped multiplay
                    else if( (g_MultiPlayUpperMask[1] == g_MultiPlayUpperMaskReady[1]) && (g_MultiPlayLowerMask[1] == g_MultiPlayLowerMaskReady[1]) )
                    {
                        //set multiplay and clear masks
                        if( !acDigiMultiPlay( AC_LOOP_ON, g_MultiPlayUpperMask[1], g_MultiPlayLowerMask[1] ) ) if( asCheckBufferOverflow() ) return;
                        g_MultiPlayUpperMask[1] = g_MultiPlayLowerMask[1] = g_MultiPlayUpperMaskReady[1] = g_MultiPlayLowerMaskReady[1] = 0;
                        *pflags &= ~PF_MULTIPLAY;
                    }

                    //check multistop
                    if( g_MultiStopUpperMask || g_MultiStopLowerMask )
                    {
                        //set multiplay and clear masks
                        if( !acDigiMultiStop( g_MultiStopUpperMask, g_MultiStopLowerMask ) ) if( asCheckBufferOverflow() ) return;
                        g_MultiStopUpperMask = g_MultiStopLowerMask = 0;
                        *pflags &= ~PF_MULTISTOP;
                    }
                }
            }
        }


        //advance to the next port
        if( ++s_iPort == AS_MAX_PORTS ) 
        {
            s_iPort = 0;
            pasppCurrent = g_asppCurrent;
            pasppTarget = g_asppTarget;
            pflags = g_asPortFlags;
        }
        else
        {
            pasppCurrent++;
            pasppTarget++;
            pflags++;
        }
    }
}




static void asClearMultiPlayReadyFlag( KTU32 port )
{
    //if this port is multiplay then set muliplayready
    KTU32 uppermask = 0;
    KTU32 lowermask = 0;

    acDigiMultiSetMask( port, &uppermask, &lowermask );
    
    uppermask = ~uppermask;
    lowermask = ~lowermask;

    g_MultiPlayUpperMaskReady[0] &= uppermask;
    g_MultiPlayLowerMaskReady[0] &= lowermask;
    g_MultiPlayUpperMaskReady[1] &= uppermask;
    g_MultiPlayLowerMaskReady[1] &= lowermask;
}



//////////////////////////////////////////////////////////////////////////////////////
// scheduled digi functions //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
KTBOOL asDigiOpen( KTU32 port, KTU32 address, KTU32 sizeInBytes, AC_AUDIO_TYPE aicaAudioType, KTU32 sampleRate )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    pasppTarget->nVolume = 255;
    pasppTarget->nPan = 64;
    pasppTarget->dspSendLevel = 15;

    pasppTarget->pSampleAddress = (KTU32*)address;
    pasppTarget->nSizeInBytes = sizeInBytes;
    pasppTarget->aicaAudioType = aicaAudioType;
    pasppTarget->nSampleRate = sampleRate;

    switch( aicaAudioType )
    {
        case AC_16BIT:
            pasppTarget->nSizeInSamples = sizeInBytes >> 1;
            break;

        default:
        case AC_8BIT:
            pasppTarget->nSizeInSamples = sizeInBytes;
            break;

        case AC_ADPCM:
        case AC_ADPCM_LOOP:
            pasppTarget->nSizeInSamples = sizeInBytes << 1;
            break;
    }

    g_asPortFlags[port] |= PF_OPENCLOSECHANGE;
    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}


KTBOOL asDigiClose( KTU32 port )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    memset(&g_asppTarget[port], 0, sizeof(ASPORTPARAMS));

    g_asPortFlags[port] |= PF_OPENCLOSECHANGE;
    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}


KTBOOL asDigiSetCurrentPitch( KTU32 port, KTS32 pitchOffsetInCents )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    pasppTarget->frequencyOrCentsOffset = pitchOffsetInCents;
    pasppTarget->frequencyOrCentsFlag = AC_PITCH_AS_CENT_VALUE;
    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}


KTBOOL asDigiSetSampleRate( KTU32 port, KTU32 sampleRate )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    pasppTarget->nSampleRate = sampleRate;
    pasppTarget->frequencyOrCentsOffset = 0;
    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}


KTBOOL asDigiSetVolume( KTU32 port, KTU32 aicaVolume )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    pasppTarget->nVolume = aicaVolume;
    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}


KTBOOL asDigiSetPan( KTU32 port, KTU32 aicaPan )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    pasppTarget->nPan = aicaPan;
    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}


KTBOOL asDigiStop( KTU32 port )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    g_asPortFlags[port] |= PF_STOPPED;
    g_asPortFlags[port] &= ~PF_RESTARTPLAY;
    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}


KTBOOL asDigiPlay( KTU32 port, KTU32 startOffset, KTS16 aicaLoopFlag )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    pasppTarget->loopStartOffsetInFrames = 0;
    pasppTarget->loopEndOffsetInFrames = ( aicaLoopFlag == AC_LOOP_ON ) ? pasppTarget->nSizeInSamples-1 : 0;

    g_asPortFlags[port] |= PF_RESTARTPLAY;
    g_asPortFlags[port] &= ~(PF_STOPPED|PF_MULTI);
    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}

KTBOOL asDigiPlayWithParameters( KTU32 port, KTU32 volume, KTU32 pan, KTU32 dspMixerChannel, KTU32 dspSendLevel, KTU32 directLevel, KTS32 frequencyOrCentsOffset, AC_PITCH_SET_TYPE frequencyOrCentsFlag, KTU32 callbackOffsetInFrames, KTU16 loopStartOffsetInFrames, KTU16 loopEndOffsetInFrames )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    pasppTarget->nVolume = volume;
    pasppTarget->nPan = pan;
    pasppTarget->dspMixerChannel = dspMixerChannel;
    pasppTarget->dspSendLevel = dspSendLevel;
    pasppTarget->directLevel = directLevel;
    pasppTarget->frequencyOrCentsOffset = frequencyOrCentsOffset;
    pasppTarget->frequencyOrCentsFlag = frequencyOrCentsFlag;
    pasppTarget->callbackOffsetInFrames = callbackOffsetInFrames;
    pasppTarget->loopStartOffsetInFrames = loopStartOffsetInFrames;
    pasppTarget->loopEndOffsetInFrames = loopEndOffsetInFrames;
    
    g_asPortFlags[port] |= PF_RESTARTPLAY;
    g_asPortFlags[port] &= ~(PF_STOPPED|PF_MULTI);
    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}


KTBOOL asDigiPlayWithLoopParameters( KTU32 port, KTU32 startOffset, KTS32 aicaLoopFlag, KTU16 loopStartOffsetInFrames, KTU16 loopEndOffsetInFrames )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    if( aicaLoopFlag == AC_LOOP_OFF ) loopStartOffsetInFrames = loopEndOffsetInFrames = 0;
    pasppTarget->loopStartOffsetInFrames = loopStartOffsetInFrames;
    pasppTarget->loopEndOffsetInFrames = loopEndOffsetInFrames;

    g_asPortFlags[port] |= PF_RESTARTPLAY;
    g_asPortFlags[port] &= ~(PF_STOPPED|PF_MULTI);
    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}


KTBOOL asDigiSetLoopPoints( KTU32 port, KTU32 loopStartAddress, KTU32 loopEndAddress )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    g_asPortFlags[port] |= PF_LOOPPOINTCHANGE;
    pasppTarget->loopStartOffsetInFrames = loopStartAddress;
    pasppTarget->loopEndOffsetInFrames = loopEndAddress;

    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}

KTBOOL asDigiRequestEvent( volatile KTU32 port, volatile KTU32 offsetFromBeginningInFrames )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];


    g_asPortFlags[port] |= PF_CALLBACK;
    pasppTarget->callbackOffsetInFrames = offsetFromBeginningInFrames;

    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}

KTBOOL asDigiSetFilterEnvelope( KTU32 port, KTU32 filterOn, KTU32 filterQ, KTU32 filterValue0, KTU32 filterValue1, KTU32 filterValue2, KTU32 filterValue3, KTU32 filterValue4, KTU32 filterAttackRate, KTU32 filterDecayRate1, KTU32 filterDecayRate2, KTU32 filterReleaseRate )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    pasppTarget->filterOn = filterOn;
    pasppTarget->filterQ = filterQ;
    pasppTarget->filterValue0 = filterValue0;
    pasppTarget->filterValue1 = filterValue1;
    pasppTarget->filterValue2 = filterValue2;
    pasppTarget->filterValue3 = filterValue3;
    pasppTarget->filterValue4 = filterValue4;
    pasppTarget->filterAttackRate = filterAttackRate;
    pasppTarget->filterDecayRate1 = filterDecayRate1;
    pasppTarget->filterDecayRate2 = filterDecayRate2;
    pasppTarget->filterReleaseRate = filterReleaseRate;

    g_asPortFlags[port] |= PF_FILTERCHANGE;
    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}


KTBOOL asDigiSetAmplitudeEnvelope( KTU32 port, KTU32 ampAttackRate, KTU32 ampDecayRate1, KTU32 ampDecayRate2, KTU32 ampReleaseRate, KTU32 ampDecayLevel, KTU32 ampKeyRateScaling, KTU32 ampLoopStartLink )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    pasppTarget->ampAttackRate = ampAttackRate;
    pasppTarget->ampDecayRate1 = ampDecayRate1;
    pasppTarget->ampDecayRate2 = ampDecayRate2;
    pasppTarget->ampReleaseRate = ampReleaseRate;
    pasppTarget->ampDecayLevel = ampDecayLevel;
    pasppTarget->ampKeyRateScaling = ampKeyRateScaling;
    pasppTarget->ampLoopStartLink = ampLoopStartLink;
    
    g_asPortFlags[port] |= PF_AMPCHANGE;
    asClearMultiPlayReadyFlag( port );
    
    return KTTRUE;
}


KTBOOL asDigiSetLfoRegisters( KTU32 port, KTU32 lfoReset, KTU32 lfoFrequency, KTU32 pitchLfoWaveShape, KTU32 pitchLfoDepth, KTU32 ampLfoWaveShape, KTU32 ampLfoDepth )
{
    register PASPORTPARAMS pasppTarget = &g_asppTarget[port];

    pasppTarget->lfoReset = lfoReset;
    pasppTarget->lfoFrequency = lfoFrequency;
    pasppTarget->pitchLfoWaveShape = pitchLfoWaveShape;
    pasppTarget->pitchLfoDepth = pitchLfoDepth;
    pasppTarget->ampLfoWaveShape = ampLfoWaveShape;
    pasppTarget->ampLfoDepth = ampLfoDepth;
    
    g_asPortFlags[port] |= PF_LFOCHANGE;
    asClearMultiPlayReadyFlag( port );

    return KTTRUE;
}



KTBOOL asDigiMultiStop( KTU32 uppermask, KTU32 lowermask )
{
	int	i;

    g_MultiStopUpperMask |= uppermask;
    g_MultiStopLowerMask |= lowermask;

    //set channel flags etc.
    for( i = 31; i; i-- )
    {
        if( lowermask & (1 << i) ) { g_asPortFlags[i]    &= ~(PF_STOPPED|PF_RESTARTPLAY|PF_MULTIPLAY);  g_asPortFlags[i]    |= PF_MULTISTOP; }
        if( uppermask & (1 << i) ) { g_asPortFlags[i+32] &= ~(PF_STOPPED|PF_RESTARTPLAY|PF_MULTIPLAY);  g_asPortFlags[i+32] |= PF_MULTISTOP; }
    }

    uppermask = ~uppermask;
    lowermask = ~lowermask;
    g_MultiPlayUpperMask[0] &= uppermask;
    g_MultiPlayLowerMask[0] &= lowermask;
    g_MultiPlayUpperMask[1] &= uppermask;
    g_MultiPlayLowerMask[1] &= lowermask;
    
    return KTTRUE;
}


KTBOOL asDigiMultiPlay( KTS32 aicaLoopFlag, KTU32 uppermask, KTU32 lowermask )
{
    register int i = (aicaLoopFlag == AC_LOOP_OFF) ? 0 : 1;

    g_MultiPlayUpperMask[i] |= uppermask;
    g_MultiPlayLowerMask[i] |= lowermask;

    i ^= 1; //select other array index and clear bits
    g_MultiPlayUpperMask[i] &= ~uppermask;
    g_MultiPlayLowerMask[i] &= ~lowermask;

    //set channel flags etc.
    for( i = 31; i; i-- )
    {
        if( lowermask & (1 << i) ) { g_asPortFlags[i]    &= ~(PF_STOPPED|PF_RESTARTPLAY|PF_MULTISTOP);  g_asPortFlags[i]    |= PF_MULTIPLAY; }
        if( uppermask & (1 << i) ) { g_asPortFlags[i+32] &= ~(PF_STOPPED|PF_RESTARTPLAY|PF_MULTISTOP);  g_asPortFlags[i+32] |= PF_MULTIPLAY; }
    }

    return KTTRUE;
}







//////////////////////////////////////////////////////////////////////////////////////
// scheduled DSP functions ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

KTBOOL asDspSetMixerChannel( KTU32 port, KTU32 toMixerChannel, KTU32 sendLevel, KTU32 directLevel )
{
    g_asppTarget[port].dspMixerChannel = toMixerChannel;
    g_asppTarget[port].dspSendLevel = sendLevel;
    g_asppTarget[port].directLevel = directLevel;
    g_asPortFlags[port] |= PF_DSPMIXERCHANGE;

    return KTTRUE;
}


KTBOOL asDspSetQSoundAngle( KTU32 qSoundChannel, KTU32 angle )
{
    g_qspTarget[qSoundChannel].angle = angle;

    return KTTRUE;
}




//////////////////////////////////////////////////////////////////////////////////////
// scheduled redbook audio functions /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
KTBOOL asCdSetVolume( KTU32 leftVolume, KTU32 rightVolume )
{
    g_asgpTarget.leftCDVolume = leftVolume;
    g_asgpTarget.rightCDVolume = rightVolume;
    g_asFlags |= GF_CDVOLUMECHANGE;

    return KTTRUE;
}

KTBOOL asCdSetPan( KTU32 leftPan, KTU32 rightPan )
{
    g_asgpTarget.leftCDPan = leftPan;
    g_asgpTarget.rightCDPan = rightPan;
    g_asFlags |= GF_CDPANCHANGE;

    return KTTRUE;
}





