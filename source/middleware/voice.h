/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-03 dczhang
* Initial revision.
*
************************************************/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include "ssz_def.h"
#include "ssz_common.h"
#include "drv_isd2360.h"



//音元
typedef enum{
    kVoiceBeepIndex,        //播放beep音元索引
    kVoiceLowIndex,
    kVoiceHighIndex,

    kVoiceIndexMax,

    kVoiceBeepMacroIndex,   //播放beep宏索引
    kVoiceLowMacroIndex,
    kVoiceHighMacroIndex,

    kVoiceMacroIndexMax
}VoiceElementIndex;

//通道
typedef enum{
    kVoiceBeepChannel,      //beep声音通道
    kVoiceLowChannel,
    kVoiceHighChannel,
    
    kVoiceChannelMax
}VoiceChannelControl;

//音量
typedef enum{
    kVoiceVolumeLowest,
    kVoiceVolumeLow,
    kVoiceVolumeNormal,
    kVoiceVolumeHigh,
    kVoiceVolumeHighest,

    kvoiceVolumeMax
    
}VoiceVolume;

/************************************************
* Declaration
************************************************/

void voice_device_open();
void voice_device_close();

//获取播放通道
uint8_t voice_get_channel(VoiceChannelControl channel);

//立即播放音元索引
void voice_play_once(VoiceElementIndex index, VoiceChannelControl channel);

//播放宏索引
void voice_play_cycle(VoiceElementIndex index, VoiceChannelControl channel);
//设置音量大小
void voice_set_volume(VoiceVolume volume); 
//stop the channel voice
void voice_stop_play_current_channel(VoiceChannelControl channel);

void voice_stop_play_beep_channel();

void voice_stop_play_low_channel();

void voice_stop_play_high_channel();

void voice_play_beep_once();

void voice_play_low_once();

void voice_play_high_once();

void voice_play_beep_cycle();

void voice_play_low_cycle();

void voice_play_high_cycle();



#ifdef __cplusplus
}
#endif





