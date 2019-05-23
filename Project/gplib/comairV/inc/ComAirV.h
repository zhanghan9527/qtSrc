#ifndef __COMAIR_V__H__
#define __COMAIR_V__H__
#include "gplib.h"

extern unsigned int MCLK;
#define COMAIR_V_SAMPLERATE					 48000
#define COMAIR_V_COMAIR_V_FREQ_TABLE_SIZE	 6
#define COMAIR_V_ENG_AVG_HISTORY_SIZE		 16
#define COMAIR_V_CMD_ARRAY_SIZE				 5
#define COMAIR_V_CMD_TABLE_SIZE				 80
#define COMAIR_V_FRAME						 256
#define COMAIR_V_MIN_ENG_VALUE	             2
#define COMAIR_V_MISSTONE_INDEX				 8
#define COMAIR_V_SILENCE_INDEX				 0	                                    //index 0 == silence
#define COMAIR_V_TONE_START_INDEX			 1
#define COMAIR_V_VALID_TONE_START            COMAIR_V_TONE_START_INDEX
#define COMAIR_V_MIN_CMD_REPEAT              12
#define COMAIR_V_MS_PER_SEC                  1000
#define COMAIR_V_BACKWARD_STEP				 (COMAIR_V_SAMPLERATE / COMAIR_V_MS_PER_SEC) * 240 / COMAIR_V_FRAME // 48(KHz) * 240ms / 256(Pt)
#define COMAIR_V_HISTORY_ENGSIZE             (COMAIR_V_CMD_ARRAY_SIZE*COMAIR_V_BACKWARD_STEP) //1800
#define COMAIR_V_RETAIN_SAMPLE               COMAIR_V_SAMPLERATE/COMAIR_V_MS_PER_SEC * 100				//48(KHz) * 100ms
#define COMAIR_V_MAX_POSSIBLECMD_SIZE		 30
#define COMAIR_V_TONE_NUMBER_PER_COM	     46

#define	C_COMAIR_V_IOTX_SEND_MAX_COUNT	    98		/* 1.2 sec * 4096(Hz) * 0.04(s) = 196. Every 1/4096 sec will execute COMAIR_IOTX_Send() once, so 40ms will execute 196 times. */
#define	C_COMAIR_V_IOTX_RAMP_TIMER		    3		/* In order to prevent generating beep sounds between each tones in one command, ramp up/down 3 preloads in 1/4096 sec. */

#define COMAIR_V_FREQ_TABLE_SIZE		    6
#define COMAIR_V_SAMPLERATE					48000

#if 0
static int frequency_table[COMAIR_V_FREQ_TABLE_SIZE]=
{
	COMAIR_V_FREQ_1, COMAIR_V_FREQ_2, COMAIR_V_FREQ_3, COMAIR_V_FREQ_4, COMAIR_V_FREQ_5, COMAIR_V_FREQ_6
}; // Hz
#endif
static unsigned char comair_v_cmd_idx[COMAIR_V_CMD_TABLE_SIZE][COMAIR_V_CMD_ARRAY_SIZE]={
	 1, 2, 3, 2, 3
	,1, 2, 3, 4, 3
	,1, 2, 3, 5, 3
	,1, 2, 3, 6, 3
	,1, 2, 4, 2, 4
	,1, 2, 4, 3, 4
	,1, 2, 4, 5, 4
	,1, 2, 4, 6, 4
	,1, 2, 5, 2, 5
	,1, 2, 5, 3, 5
	,1, 2, 5, 4, 5
	,1, 2, 5, 6, 5
	,1, 2, 6, 2, 6
	,1, 2, 6, 3, 6
	,1, 2, 6, 4, 6
	,1, 2, 6, 5, 6
	,1, 3, 2, 3, 2
	,1, 3, 2, 4, 2
	,1, 3, 2, 5, 2
	,1, 3, 2, 6, 2
	,1, 3, 4, 2, 4
	,1, 3, 4, 3, 4
	,1, 3, 4, 5, 4
	,1, 3, 4, 6, 4
	,1, 3, 5, 2, 5
	,1, 3, 5, 3, 5
	,1, 3, 5, 4, 5
	,1, 3, 5, 6, 5
	,1, 3, 6, 2, 6
	,1, 3, 6, 3, 6
	,1, 3, 6, 4, 6
	,1, 3, 6, 5, 6
	,1, 4, 2, 3, 2
	,1, 4, 2, 4, 2
	,1, 4, 2, 5, 2
	,1, 4, 2, 6, 2
	,1, 4, 3, 2, 3
	,1, 4, 3, 4, 3
	,1, 4, 3, 5, 3
	,1, 4, 3, 6, 3
	,1, 4, 5, 2, 5
	,1, 4, 5, 3, 5
	,1, 4, 5, 4, 5
	,1, 4, 5, 6, 5
	,1, 4, 6, 2, 6
	,1, 4, 6, 3, 6
	,1, 4, 6, 4, 6
	,1, 4, 6, 5, 6
	,1, 5, 2, 3, 2
	,1, 5, 2, 4, 2
	,1, 5, 2, 5, 2
	,1, 5, 2, 6, 2
	,1, 5, 3, 2, 3
	,1, 5, 3, 4, 3
	,1, 5, 3, 5, 3
	,1, 5, 3, 6, 3
	,1, 5, 4, 2, 4
	,1, 5, 4, 3, 4
	,1, 5, 4, 5, 4
	,1, 5, 4, 6, 4
	,1, 5, 6, 2, 6
	,1, 5, 6, 3, 6
	,1, 5, 6, 4, 6
	,1, 5, 6, 5, 6
	,1, 6, 2, 3, 2
	,1, 6, 2, 4, 2
	,1, 6, 2, 5, 2
	,1, 6, 2, 6, 2
	,1, 6, 3, 2, 3
	,1, 6, 3, 4, 3
	,1, 6, 3, 5, 3
	,1, 6, 3, 6, 3
	,1, 6, 4, 2, 4
	,1, 6, 4, 3, 4
	,1, 6, 4, 5, 4
	,1, 6, 4, 6, 4
	,1, 6, 5, 2, 5
	,1, 6, 5, 3, 5
	,1, 6, 5, 4, 5
	,1, 6, 5, 6, 5
};

void comair_v_tx_init(void);
void set_comair_v_pincode_cmd_gen(INT16U pin);
void stop_comair_v_timer(void);
void start_comair_v_timer(void);
void ComAir_V_SendCmd(int updown);
BOOLEAN get_comair_v_timer_status(void);
COMAIR_SEND_STATUS send_comair_v_cmd_single_cmd(INT8U cmd);
void send_comair_v_command(void);
COMAIR_CMD_CHECK comair_v_cmd_validation(INT16U cmd);
void comair_v_iotx_send_isr(void);
void comair_v_update_pwm_timer(void);
void set_comair_v_cmd_gen(INT16U pin);
void comair_v_pincode_set(INT16U pin);
void comair_v_cmd_generator(void);
int TryToFindCommnad(int i32FrameSize,char *pCommnad,int *pFoundCommnad);
void comair_v_init_cmdlist(void);
#endif

