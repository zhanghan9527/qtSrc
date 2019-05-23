
//#include "os_res.h"


//=================================================================
#define THUM_W		152
#define THUM_H		114

#define GBOOK_W		80
#define GBOOK_H		80

//=================================================================
#define PICTURE_MAX		500

#define Frame_Num	12
#define ACTMAX	(Frame_Num+10+3)

#define PAGE_NUM	4

#define	POW_SP		210
#define	VOL_SP		220

//=================================================================

extern int pic_Max;
extern INT16S Pic_No,imgmode;

extern IMAGE_SCALE_STRUCT img_scale;
extern INT32U ScalerBuff,scaler_task_state;
extern INT16S Avi_Encode_Flag;

extern int pic_Max;
extern struct f_info	file_info;

extern INT16S PowerOff_Video;
extern INT16U VideoThum;
extern INT32U PIC_Nums,PicSd_Nums,AUDIO_Nums,VID_Nums,VidSd_Nums;
extern INT16S Decora_Video;
//=================================================================
