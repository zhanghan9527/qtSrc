

/*
 *
 *  iOSAudio
 *
 *  Created by Generalplus SA1 on 2011/5/27.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif




void  fn_GPTD8_Init(int *frequency_table, int SamplingRate);
void  fn_GPTD8_ToneDetection_Proc(int t_num,short *InBuf_Ptr, int *FREQ_ENERGY, short *FREQ_SQRT_ENERGY);
// InBuf_Ptr = &InputBuf[256]; // Input Frame Size = 256 Sample

short fnSqrt(int temp_long_lX);

#ifdef __cplusplus
}
#endif