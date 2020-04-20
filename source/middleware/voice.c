/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-03 dczhang
* Initial revision.
*
************************************************/
#include "voice.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
static bool g_voice_device_is_opened = false;

/************************************************
* Function 
************************************************/

//open device for paly voice
void voice_device_open() {
	if (!g_voice_device_is_opened) {
		g_voice_device_is_opened = true;
		drv_isd2360_initial();
	}
}

//close device for reduce power
void voice_device_close() {
	if (g_voice_device_is_opened) {
		g_voice_device_is_opened = false;
		drv_isd2360_deinitial();
	}
}


//获取播放通道
uint8_t voice_get_channel(VoiceChannelControl channel)
{
    uint8_t voice_channel;
    
    ssz_assert(channel < kVoiceChannelMax);
    
    switch(channel){
        case kVoiceBeepChannel:
            voice_channel = ISD2360_CHANNELCONTROL_SPICMDCH0;
            break;

        case kVoiceLowChannel:
            voice_channel = ISD2360_CHANNELCONTROL_SPICMDCH1;    
            break;
    
        case kVoiceHighChannel:
            voice_channel = ISD2360_CHANNELCONTROL_SPICMDCH2;    
            break;

        default:
            ssz_assert_fail();
            break;
            
    } 

    return voice_channel;
}

//立即播放音元索引
void voice_play_once(VoiceElementIndex index, VoiceChannelControl channel)
{
    uint8_t voice_index, voice_channel;
	
	voice_device_open();

    //只能传递音元索引
    ssz_assert(index < kVoiceIndexMax);

    switch(index){
        case kVoiceBeepIndex:
            voice_index = VOPROMPT_BEEP_ALARM;
            break;

        case kVoiceLowIndex:
            voice_index = VOPROMPT_LOW_ALARM;
            break;

        case kVoiceHighIndex:
            voice_index = VOPROMPT_HIGH_ALARM;
            break;

        default:
            ssz_assert_fail();
            break;
    }

    voice_channel = voice_get_channel(channel);
    
    drv_isd2360_play_voice_prompts(voice_index, voice_channel);
}

//播放宏索引
void voice_play_cycle(VoiceElementIndex index, VoiceChannelControl channel)
{
    uint8_t voice_macro, voice_channel;
	
	voice_device_open();
    //只能传递宏索引
    ssz_assert(kVoiceIndexMax < index && index < kVoiceMacroIndexMax);

    switch(index){
        case kVoiceBeepMacroIndex:
            voice_macro = VOMACRO_BEEP_ALARM;
            break;

        case kVoiceLowMacroIndex:
            voice_macro = VOMACRO_LOW_ALARM;
            break;

        case kVoiceHighMacroIndex:
            voice_macro = VOMACRO_HIGH_ALARM;
            break;

        default:
            ssz_assert_fail();
            break;
    }

    voice_channel = voice_get_channel(channel);

    drv_isd2360_exe_voice_macros(voice_macro, voice_channel);
}

//设置音量大小
void voice_set_volume(VoiceVolume volume)
{
    uint8_t voice_volume;

    ssz_assert(volume < kvoiceVolumeMax);

	voice_device_open();
    switch(volume){
        case kVoiceVolumeLowest:
            voice_volume = VOICE_VOLUME_LOWEST;
            break;

        case kVoiceVolumeLow:
            voice_volume = VOICE_VOLUME_LOW;
            break;

        case kVoiceVolumeNormal:
            voice_volume = VOICE_VOLUME_NORMAL;
            break;

        case kVoiceVolumeHigh:
            voice_volume = VOICE_VOLUME_HIGH;
            break;

        case kVoiceVolumeHighest:
            voice_volume = VOICE_VOLUME_HIGHEST;
            break;

        default:
            ssz_assert_fail();
            break;
    }

    drv_isd2360_setvolume(voice_volume);
}


//stop the channel voice
void voice_stop_play_current_channel(VoiceChannelControl channel)
{
    uint8_t voice_channel;
	
    voice_device_open();
    voice_channel = voice_get_channel(channel);

    drv_isd2360_stop(voice_channel);
}

void voice_stop_play_beep_channel()
{
    voice_stop_play_current_channel(kVoiceBeepChannel);
}

void voice_stop_play_low_channel()
{
    voice_stop_play_current_channel(kVoiceLowChannel);
}

void voice_stop_play_high_channel()
{
    voice_stop_play_current_channel(kVoiceHighChannel);
}

//play beep immediately and return at once
void voice_play_beep_once()
{
    voice_play_once(kVoiceBeepIndex,kVoiceBeepChannel);
}

void voice_play_low_once()
{
    voice_play_once(kVoiceLowIndex,kVoiceLowChannel);
}

void voice_play_high_once()
{
    voice_play_once(kVoiceHighIndex,kVoiceHighChannel);
}

void voice_play_beep_cycle()
{
    voice_play_cycle(kVoiceBeepMacroIndex,kVoiceBeepChannel);
}

void voice_play_low_cycle()
{
    voice_play_cycle(kVoiceLowMacroIndex,kVoiceLowChannel);
}

void voice_play_high_cycle()
{
    voice_play_cycle(kVoiceHighMacroIndex,kVoiceHighChannel);
}




