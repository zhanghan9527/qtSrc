#include "audio_encoder.h"

extern OS_EVENT 	*aud_enc_reqQ;
//===================================================================================
// audio encode entry
//===================================================================================
void audio_encode_entrance(void)
{
	adc_record_task_create(AUD_ENC_PRIORITY);
}

void audio_encode_exit(void)
{
	 adc_record_task_del(AUD_ENC_PRIORITY);
}

//===================================================================================
// start record
//===================================================================================
CODEC_START_STATUS audio_encode_start(MEDIA_SOURCE src, INT16U SampleRate, INT32U BitRate)
{	
	INT64U temp;
	
	if(audio_record_get_status() == C_START_RECORD) {
		return REENTRY_ERROR;
	}
		
	if((src.type != SOURCE_TYPE_FS) && 
		(src.type != SOURCE_TYPE_USER_DEFINE)) {
		return RESOURCE_NO_FOUND_ERROR;
	}
	
	if(src.type == SOURCE_TYPE_FS) {
		if(src.type_ID.FileHandle < 0) {	
			return RESOURCE_NO_FOUND_ERROR;
		}
		
		audio_record_set_status(C_STOP_RECORD);
		audio_record_set_source_type(C_GP_FS);
		temp = audio_record_set_file_handle_free_size(src.type_ID.FileHandle);
		if(temp <= (1024 * 10)) { 
			return 	RESOURCE_WRITE_ERROR;
		}
	} else {
		audio_record_set_status(C_STOP_RECORD);
		audio_record_set_source_type(C_USER_DEFINE);
		audio_record_set_file_handle_free_size(-1);
	}
		
	if(src.Format.AudioFormat == A1800) {
		audio_record_set_info(WAVE_FORMAT_A1800, 16000, BitRate);
		audio_record_set_down_sample(TRUE, 2);	
	} else if(src.Format.AudioFormat == WAV) {
		audio_record_set_info(WAVE_FORMAT_PCM, SampleRate, BitRate);
	} else if(src.Format.AudioFormat == IMA_ADPCM) {
		audio_record_set_info(WAVE_FORMAT_IMA_ADPCM, SampleRate, BitRate);
	} else if(src.Format.AudioFormat == MICROSOFT_ADPCM) {
		audio_record_set_info(WAVE_FORMAT_ADPCM, SampleRate, BitRate);
	} else if(src.Format.AudioFormat == ALAW) {
		audio_record_set_info(WAVE_FORMAT_ALAW, SampleRate, BitRate);
	} else if(src.Format.AudioFormat == MULAW) {
		audio_record_set_info(WAVE_FORMAT_MULAW, SampleRate, BitRate);
	} else if(src.Format.AudioFormat == MP3) {
		audio_record_set_info(WAVE_FORMAT_MP3, SampleRate, BitRate);
	} else if(src.Format.AudioFormat == VR_PCM) {
		audio_record_set_info(PCM_FORMAT_VR, SampleRate, BitRate);
	} else {
		return AUDIO_ALGORITHM_NO_FOUND_ERROR;
	}

	if(adc_record_task_start() < 0) {
		return CODEC_START_STATUS_ERROR_MAX;
	}

	return START_OK;
}

//===================================================================================
// stop record
//===================================================================================
void audio_encode_stop(void)
{
	if(audio_record_get_status() == C_START_RECORD) {
		adc_record_task_stop();
	}
}

//====================================================================================
//	status
//====================================================================================
AUDIO_CODEC_STATUS audio_encode_status(void)
{
	if(audio_record_get_status() == C_START_RECORD)
		return AUDIO_CODEC_PROCESSING;
	else if(audio_record_get_status() == C_STOP_RECORD)
		return AUDIO_CODEC_PROCESS_END;
	else if(audio_record_get_status() == C_STOP_RECORDING)
		return AUDIO_CODEC_PROCESS_END;
	else if(audio_record_get_status() == C_START_FAIL)
		return AUDIO_CODEC_BREAK_OFF;
	else
		return AUDIO_CODEC_STATUS_MAX;
}

//====================================================================================
//	Set DownSample when audio record
//  bEnable: enable or disable
//  DownSampleFactor: 2, 3 or 4
//====================================================================================
CODEC_START_STATUS audio_encode_set_downsample(INT8U bEnable, INT8U DownSampleFactor)
{
	INT32S nRet;
	
	if(DownSampleFactor > 4)
		DownSampleFactor = 4;
	
	if(DownSampleFactor < 2)
		DownSampleFactor = 2;
	
	nRet = audio_record_set_down_sample(bEnable, DownSampleFactor);
	if(nRet >= 0)
		return START_OK;
	else
		return  RESOURCE_NO_FOUND_ERROR;
}	

//====================================================================================
//	Set channel number
//  channel: 1 or 2
//====================================================================================
CODEC_START_STATUS audio_encode_set_channel(INT8U channel)
{
	INT8U device;
	
	if(channel == 0 || channel > 2) {
		channel = C_MONO_RECORD;
	}
	
	if(channel == 1) {
	#if ((defined MCU_VERSION) && (MCU_VERSION < GPL326XX))	
		device = ADC_LINE_IN;
	#else
		device = BUILD_IN_MIC;
	#endif	
	} else {
		device = DOUBLE_LINEIN;
	}
	
	audio_record_set_channel(device, channel);
	return START_OK;
}