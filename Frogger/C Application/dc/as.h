
#ifndef _AS_H
#define _AS_H

#include <ac.h>

#ifdef __SHC__
#include <math.h> // for pow( )
#include <string.h> // for strcpy in asError.c and memset
#include <stdlib.h> // for malloc in amMem.c
#endif

#if defined(__MWERKS__)
#include <sh4_mwerks.h>
#elif defined(__GNUC__)
#include <sh4_gnu.h>
#else
#include <machine.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif


void asUpdateManager();



// digital audio functions
KTBOOL asDigiOpen( KTU32 port, KTU32 address, KTU32 sizeInBytes, AC_AUDIO_TYPE aicaAudioType, KTU32 sampleRate );
KTBOOL asDigiClose( KTU32 port );
KTBOOL asDigiSetCurrentPitch( KTU32 port, KTS32 pitchOffsetInCents );
KTBOOL asDigiSetSampleRate( KTU32 port, KTU32 sampleRate );
KTBOOL asDigiSetVolume( KTU32 port, KTU32 aicaVolume );
KTBOOL asDigiSetPan( KTU32 port, KTU32 aicaPan );
KTBOOL asDigiRequestEvent( volatile KTU32 port, volatile KTU32 offsetFromBeginningInFrames );
KTBOOL asDigiStop( KTU32 port );
KTBOOL asDigiMultiStop( KTU32 upperMask, KTU32 lowerMask );
KTBOOL asDigiMultiPlay( KTS32 aicaLoopFlag, KTU32 upperMask, KTU32 lowerMask );
KTBOOL asDigiPlay( KTU32 port, KTU32 startOffset, KTS16 aicaLoopFlag );
KTBOOL asDigiPlayWithParameters(KTU32 port, KTU32 volume, KTU32 pan, KTU32 dspMixerChannel, KTU32 dspSendLevel, KTU32 directLevel, KTS32 frequencyOrCentsOffset, AC_PITCH_SET_TYPE frequencyOrCentsFlag, KTU32 callbackOffsetInFrames, KTU16 loopStartOffsetInFrames, KTU16 loopEndOffsetInFrames );
KTBOOL asDigiPlayWithLoopParameters( KTU32 port, KTU32 startOffset, KTS32 aicaLoopFlag, KTU16 loopStartOffsetInFrames, KTU16 loopEndOffsetInFrames );
KTBOOL asDigiSetFilterEnvelope(KTU32 port, KTU32 filterOn, KTU32 filterQ, KTU32 filterValue0, KTU32 filterValue1, KTU32 filterValue2, KTU32 filterValue3, KTU32 filterValue4, KTU32 filterAttackRate, KTU32 filterDecayRate1, KTU32 filterDecayRate2, KTU32 filterReleaseRate );
KTBOOL asDigiSetAmplitudeEnvelope(KTU32 port, KTU32 ampAttackRate, KTU32 ampDecayRate1, KTU32 ampDecayRate2, KTU32 ampReleaseRate, KTU32 ampDecayLevel, KTU32 ampKeyRateScaling, KTU32 ampLoopStartLink );
KTBOOL asDigiSetLfoRegisters(KTU32 port, KTU32 lfoReset, KTU32 lfoFrequency, KTU32 pitchLfoWaveShape, KTU32 pitchLfoDepth, KTU32 ampLfoWaveShape, KTU32 ampLfoDepth );
KTBOOL asDigiSetLoopPoints(KTU32 port, KTU32 loopStartAddress, KTU32 loopEndAddress ); // override default loop points

#define asDigiMultiSetMask  acDigiMultiSetMask
#define asDigiGetCurrentReadPosition acDigiGetCurrentReadPosition
#define asSystemSetStereoOrMono acSystemSetStereoOrMono // mode is 0 for stereo, 1 for mono
#define asDigiPortsAvailable acDigiPortsAvailable

// DSP functions
#define asDspInstallOutputMixer acDspInstallOutputMixer
#define asDspInstallProgram acDspInstallProgram
KTBOOL asDspSetMixerChannel( KTU32 port, KTU32 toMixerChannel, KTU32 sendLevel, KTU32 directLevel );
KTBOOL asDspSetQSoundAngle( KTU32 qSoundChannel, KTU32 angle );



// Redbook Functions
KTBOOL asCdSetVolume( KTU32 leftVolume, KTU32 rightVolume );
KTBOOL asCdSetPan( KTU32 leftPan, KTU32 rightPan );
#define asCdInit acCdInit



/*
    //FIXME...

// MIDI functions
KTBOOL asMidiClose( KTU32 port );
KTBOOL asMidiOpen( KTU32 port, KTU8 gmMode, KTU32 address, KTU32 sizeInBytes, KTU32 pulsesPerQuarterNote );
KTBOOL asMidiPlay( KTU32 port, KTU32 startOffset, KTS16 aicaLoopFlag );
KTBOOL asMidiStop( KTU32 port );
KTBOOL asMidiPause( KTU32 port );
KTBOOL asMidiResume( KTU32 port );
KTBOOL asMidiSetVolume( KTU32 port, KTU32 portMasterVolume );
KTBOOL asMidiSetTempo( KTU32 port, KTU32 microSecondsPerQuarterNote );
KTBOOL asMidiReset( KTU32 port );
KTBOOL asMidiSendMessage( KTU32 port, KTU32 channel, KTU32 command, KTU32 value1, KTU32 value2 );
KTBOOL asMidiRequestEvent( KTU32 port, KTU32 offsetFromBeginningInBytes );
KTBOOL asMidiSetTonebank( KTU8 toneBank, AC_BANK_TYPE bankType, KTU32 address, KTU32 sizeInBytes, KTU32 mttPtr );

*/

#ifdef __cplusplus
}
#endif


#endif // end of #ifndef _AS_H
