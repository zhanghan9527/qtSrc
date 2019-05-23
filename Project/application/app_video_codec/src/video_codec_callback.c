#include "video_codec_callback.h"
#include "data.h"

static INT8U sensor_frame_end_action = SENSOR_FEA_VIDEO_ENCODE;
INT32U video_codec_show_buffer;
INT32U video_codec_display_flag;

#if VIDEO_DISPALY_WITH_PPU == 1
#if APP_FACE_DETECTION_EN == 1
#define MD_DEMO_EN      			0
#define OBJECT_TRAINING_MEUN_EN		0
#define FACE_RECOGNIZE_MEUN_EN		0
#else
#define MD_DEMO_EN      			0
#define OBJECT_TRAINING_MEUN_EN		0
#define FACE_RECOGNIZE_MEUN_EN		0
#endif
#if OBJECT_TRAINING_MEUN_EN == 1
INT32S object_training_sprite_update(INT32U state,INT32U image_num);
extern INT32U obj_recognize_get_state(INT32U mode);
#elif FACE_RECOGNIZE_MEUN_EN == 1
INT32S face_recognize_sprite_update(void);
extern INT32U face_recognize_get_mode(INT32U mode);
#elif MD_DEMO_EN == 1
extern void sensor_md_demo(void);
#endif 
PPU_REGISTER_SETS turnkey_ppu_register_structure_1;
extern PPU_REGISTER_SETS *ppu_register_set;
#endif


//=======================================================================
//  Video decode end hook function  
//=======================================================================
//=======================================================================================
//		Video decode stop hook function 
//=======================================================================================
void video_decode_end(void)
{
	DBG_PRINT("AVI Decode play-end callback...\r\n");
}
extern INT16S Decora_Video,Video_Yuv;
//=======================================================================================
//		Video decode output buffer hook function 
//=======================================================================================
void video_decode_FrameReady(INT8U *FrameBufPtr)
{
#if	VIDEO_DISPALY_WITH_PPU == 0
    video_codec_show_buffer = (INT32U)FrameBufPtr;
 	video_codec_display_flag = 1;
#else	//display with PPU
	INT16U width, height;
	if(Decora_Video){
		View_Decora((INT32U)FrameBufPtr);
	}else{
		video_decode_get_display_size(&width, &height);	
		if(Video_Yuv==0){
			InitTextBmp(C_PPU_TEXT1,Tx1_buffer,TVOutBuff,TX_DEPTH0,0,0,3);
			Video_Yuv=1;
		}
		gplib_ppu_text_calculate_number_array(ppu_register_set, C_PPU_TEXT1, width, height, (INT32U)FrameBufPtr);	// Calculate Number array
	}
	paint_ppu_spriteram_cell4(ppu_register_set,0,Left_Top_coordinate_mode,Spritebuf_number);
	gplib_ppu_go(ppu_register_set);
#endif
}

//=======================================================================
//  Video encode hook function 
//=======================================================================
//=======================================================================
//		mipi isr function
//=======================================================================
#if (defined _DRV_L1_MIPI) && (_DRV_L1_MIPI == 1) 
#include "drv_l1_mipi.h"

void mipi_sot_sync_err(void)
{
	DBG_PRINT("%s\r\n", __func__);
}

void mipi_hd_1bit_err(void)
{
	DBG_PRINT("%s\r\n", __func__);
}

void mipi_hd_err(void)
{
	DBG_PRINT("%s\r\n", __func__);
}

void mipi_crc_err(void)
{
	DBG_PRINT("%s\r\n", __func__);
}

void mipi_sof(void)
{
	DBG_PRINT("%s\r\n", __func__);
}

void mipi_isr_enable(INT32U enable)
{
	mipi_user_isr_register(SOT_SYNC_ERR0_IDX, mipi_sot_sync_err);
	mipi_set_irq_enable(enable, SOT_SYNC_ERR0_IDX);
	
	mipi_user_isr_register(HD_ERR_1BIT_IDX, mipi_hd_1bit_err);
	mipi_set_irq_enable(enable, HD_ERR_1BIT_IDX);
	
	mipi_user_isr_register(HD_ERR_IDX, mipi_hd_err);
	mipi_set_irq_enable(enable, HD_ERR_IDX);
	
	mipi_user_isr_register(CRC_ERR_IDX, mipi_crc_err);
	mipi_set_irq_enable(enable, CRC_ERR_IDX);
	
	mipi_user_isr_register(SOF_FLAG_IDX, mipi_sof);
	mipi_set_irq_enable(enable, SOF_FLAG_IDX);
}
#endif

//=======================================================================
//		sensor start 
//=======================================================================
INT32U video_encode_sensor_start(INT16U width, INT16U height, INT32U csi_frame1, INT32U csi_frame2)
{
	OS_CPU_SR cpu_sr;
	
	// Setup CMOS sensor
	OS_ENTER_CRITICAL();
	CSI_Init(width, height, FT_CSI_YUVIN|FT_CSI_YUVOUT|FT_CSI_RGB1555, csi_frame1, csi_frame2);
	OS_EXIT_CRITICAL();
	user_defined_set_sensor_irq(1);
#if (defined _DRV_L1_MIPI) && (_DRV_L1_MIPI == 1)
	mipi_isr_enable(1);	
#endif
  	return 0;
}

//=======================================================================
//		sensor stop 
//=======================================================================
INT32U video_encode_sensor_stop(void)
{
  	//close sensor
	user_defined_set_sensor_irq(0);
	R_CSI_TG_CTRL0 = 0;
	R_CSI_TG_CTRL1 = 0;

#if (defined _DRV_L1_MIPI) && (_DRV_L1_MIPI == 1)
	mipi_uninit(); 	
	mipi_isr_enable(0);	
#endif	
    return 0;
}

//=======================================================================
//		preview frame buffer ready  
//=======================================================================
INT32S video_encode_display_frame_ready(INT32U frame_buffer)
{
	INT16U width, height;
	INT32U ready_frame;

#if	VIDEO_DISPALY_WITH_PPU == 0           
	video_codec_display_flag = 1;
  	video_codec_show_buffer = frame_buffer;
#else	//display with ppu
	//video_encode_get_display_size(&width, &height);	
	//gplib_ppu_text_calculate_number_array(ppu_register_set, C_PPU_TEXT1, width, height, frame_buffer);	// Calculate Number array
	
	//video_codec_display_flag = 1;
	//video_codec_show_buffer = frame_buffer;
	paint_ppu_spriteram_cell4(ppu_register_set,0,Left_Top_coordinate_mode,Spritebuf_number);
	gplib_ppu_go(ppu_register_set);

#endif
	return 0;
}

//=======================================================================
//		encode frame buffer ready when use user define mode 
//=======================================================================
INT32S  video_encode_frame_ready(void *workmem, unsigned long fourcc, long cbLen, const void *ptr, int nSamples, int ChunkFlag)
{
	if(fourcc == 0x63643030)
	{	//"00dc", video frame ready 
	
	}
	else if(fourcc == 0x62773130)
	{	//"01wb", audio frame ready
	
	}
	else
		return STATUS_FAIL;
	
	return STATUS_OK;
}

//=======================================================================
//		avi encode end, 
// ISRC: source; IART: Artist; ICOP: copyright; IARL: achival location
// ICMS: commissioned; ICMT: comments;  ICRD: create date; ICRP: cropped
// IDIM: dimensions; IDPI: dots per inch; IENG: engineer; IGNR: genre
// IKEY: keyword; ILGT: lightness; IMED: medium; INAM: name;
// IPLT: palette setting; IPRD: product; ISFT: software; ISHP: sharpness
// ISRC: source; ISRF: source form; ITCH: technician
//=======================================================================
void video_encode_end(void *workmem)
{
	//add description to avi file
	//int AviPackerV3_AddInfoStr(const char *fourcc, const char *info_string);
	AviPackerV3_AddInfoStr(workmem, "ISRC", "Generplus");
	AviPackerV3_AddInfoStr(workmem, "IART", "Generplus");
	AviPackerV3_AddInfoStr(workmem, "ICOP", "Generplus");
	AviPackerV3_AddInfoStr(workmem, "ICRD", "2010-06-29");	
}

//--------------------------------------------------------------------
//	user can add special effect in audio or video when use video encode
//  
//---------------------------------------------------------------------
#if AUDIO_SFX_HANDLE
extern INT16S   *pcm_bg_out[];
extern OS_EVENT    *aud_bg_send_q;

INT32U video_encode_audio_sfx(INT16U *PCM_Buf, INT32U cbLen)
{
	INT32S i, index;
	INT32U Temp;
	OS_Q_DATA OSQData; 
	AUDIO_ARGUMENT aud_arg;
	
	aud_arg.Main_Channel = 0;
	    
	if(audio_decode_status(aud_arg) == AUDIO_CODEC_PROCESSING)
	{
		OSQQuery(aud_bg_send_q, &OSQData);
		index = OSQData.OSNMsgs-1;
		if(index<0)
			index = 0;
		
		for(i = 0; i<(cbLen/2) ; i++)
		{
			Temp = (INT16U) pcm_bg_out[index][i];
			Temp +=  PCM_Buf[i];
			PCM_Buf[i] = Temp >> 1;
		}
	}
	
	return (INT32U)PCM_Buf;
}
#endif

#if VIDEO_SFX_HANDLE
INT32U video_encode_video_sfx(INT32U buf_addr, INT32U cbLen)
{
	INT32U i;
	INT8U *pdata = (INT8U *)buf_addr;
	
	for(i=0; i<cbLen ;i++)
	{
		if(i % 4 == 1)
		 *(pdata + i) <<= 1;	
	}

	return (INT32U)pdata;
}
#endif

//=======================================================================
//	Set Sensor irq enable/disable.
//=======================================================================
void user_defined_set_sensor_irq(INT32U enable)
{
	if(enable > 0) {
	#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ
		R_PPU_IRQ_STATUS = 0x40;
		R_PPU_IRQ_EN |= 0x40;	//enable csi frame end irq
	#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
		R_TGR_IRQ_STATUS = 0x1;
		R_TGR_IRQ_EN = 0x1;	//enable csi frame end irq
	#elif (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ) || (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ) || (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ)
		R_TGR_IRQ_STATUS = 0x21;
		R_TGR_IRQ_EN = 0x21;	//enable csi fifo irq, fifo under run and frame end irq
	#endif
	} else {
	#if CSI_IRQ_MODE == CSI_IRQ_PPU_IRQ
	 	R_PPU_IRQ_EN &= ~0x40;
	 	R_PPU_IRQ_STATUS = 0x40;
	#elif CSI_IRQ_MODE == CSI_IRQ_TG_IRQ
		R_TGR_IRQ_EN &= ~0x1;
		R_TGR_IRQ_STATUS = 0x1;
	#elif (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO8_IRQ) || (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO16_IRQ) || (CSI_IRQ_MODE == CSI_IRQ_TG_FIFO32_IRQ)
		R_TGR_IRQ_EN &= ~0x21;
		R_TGR_IRQ_STATUS = 0x31;
	#endif		
	}	
}

//=======================================================================
//	Set the action to take inside sensor frame end interrupt
//=======================================================================
void user_defined_set_sensor_fea(INT8U action)
{
	sensor_frame_end_action = action;
}

//=======================================================================
//	tft and sensor isr handle
//=======================================================================
void user_defined_video_codec_isr(void)
{
	if(R_PPU_IRQ_STATUS & R_PPU_IRQ_EN & 0x2000) {
		// tft Vertical-Blanking interrupt
		R_PPU_IRQ_STATUS |= 0x2000;
        if(video_codec_display_flag == 1) {
          video_codec_show_image(C_DISPLAY_DEVICE, (INT32U)video_codec_show_buffer, C_DISPLAY_DEVICE, DISPLAY_OUTPUT_FORMAT);
           video_codec_display_flag = 0;
        } 
	}
	
	if(R_PPU_IRQ_STATUS & R_PPU_IRQ_EN & 0x0800) {
		// tv Vertical-Blanking interrupt
		R_PPU_IRQ_STATUS |= 0x0800;
        if(video_codec_display_flag == 1) { 
          video_codec_show_image(C_DISPLAY_DEVICE, (INT32U)video_codec_show_buffer, C_DISPLAY_DEVICE, DISPLAY_OUTPUT_FORMAT);
          video_codec_display_flag = 0;
        } 
	}
	
	if(R_PPU_IRQ_STATUS & R_PPU_IRQ_EN & 0x0040) {
		// csi frame end irq
		R_PPU_IRQ_STATUS |= 0x0040;
		video_encode_auto_switch_csi_frame();
	}
	
	if(R_PPU_IRQ_STATUS & R_PPU_IRQ_EN & 0x1000) {
		//tft under run 
		R_PPU_IRQ_STATUS |= 0x1000;
		while(1);
	}
	
	if(R_PPU_IRQ_STATUS & R_PPU_IRQ_EN & 0x0400) {
		//tv under run 
		R_PPU_IRQ_STATUS |= 0x0400;
		while(1);
	}
	
	if(R_PPU_IRQ_STATUS & R_PPU_IRQ_EN & 0x8000) {
		//sensor under run 
		R_PPU_IRQ_STATUS |= 0x8000;
		while(1);
	}
	
#if (defined MCU_VERSION) && (MCU_VERSION >= GPL326XX) && (MCU_VERSION < GPL327XX)
	csi_tg_irq_handle();
#endif
}

void csi_tg_irq_handle(void) 
{
#if (CSI_IRQ_MODE & CSI_IRQ_TG_IRQ)
	INT32U status;  
	status = R_TGR_IRQ_STATUS;
	
	if(status & R_TGR_IRQ_EN) {
		if(status & R_TGR_IRQ_EN & 0x10) { 
			// fifo under run	
			R_TGR_IRQ_STATUS |= 0x10;
			while(1);	
		} else if(status & R_TGR_IRQ_EN & 0x20) { 
			// fifo end
			R_TGR_IRQ_STATUS |= 0x20;
	   		video_encode_auto_switch_csi_fifo_end();
		} 
		
		if(status & R_TGR_IRQ_EN & 0x01) {
			// frame end
			R_TGR_IRQ_STATUS |= 0x01; 
		#if CSI_IRQ_MODE == CSI_IRQ_TG_IRQ 
			video_encode_auto_switch_csi_frame();
		#else	//CSI_IRQ_TG_FIFO_IRQ
			video_encode_auto_switch_csi_fifo_frame_end();
		#endif
		}
	}
#endif
}

void user_defined_video_codec_entrance(void)
{
#if VIDEO_DISPALY_WITH_PPU == 0
	video_codec_display_flag = 0;
	R_PPU_IRQ_EN = 0;			// disable all ppu interrupt
	R_PPU_IRQ_STATUS = 0x7FFF;	// Clear all PPU interrupts 
	vic_irq_register(VIC_PPU, user_defined_video_codec_isr);
	
	if(C_DISPLAY_DEVICE >= C_TV_QVGA) {
		R_PPU_IRQ_EN = 0x0800; // TV Vertical-Blanking IRQ enable register
	} else {	
		R_PPU_IRQ_EN = 0x2000; // TFT Vertical-Blanking IRQ enable register
	}
	vic_irq_enable(VIC_PPU);
#else
	//video_codec_ppu_configure();
#endif

#if (CSI_IRQ_MODE & CSI_IRQ_TG_IRQ)
	R_TGR_IRQ_EN = 0;			// disable all csi interrupt
	R_TGR_IRQ_STATUS = 0xFF;	// Clear all csi interrupts 
#endif

#if (defined MCU_VERSION) && (MCU_VERSION >= GPL327XX)	
	vic_irq_register(VIC_26, csi_tg_irq_handle);
	vic_irq_enable(VIC_26);
#endif
}

void video_codec_show_image(INT32U TV_TFT, INT32U BUF, INT32U DISPLAY_MODE ,INT32U SHOW_TYPE)
{
	INT32U reg = 0; 
	
	switch(SHOW_TYPE)
	{      
   	case IMAGE_OUTPUT_FORMAT_RGB565:
        if(DISPLAY_MODE == C_TV_QVGA) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|PPU_FRAME_BASE_MODE|PPU_RGB565_MODE);
        } else if(DISPLAY_MODE == C_TV_VGA) {
           	R_PPU_ENABLE=(PPU_VGA_MODE|PPU_FRAME_BASE_MODE|PPU_RGB565_MODE);
        } else if(DISPLAY_MODE == C_TV_D1) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|TFT_SIZE_720X480|PPU_FRAME_BASE_MODE|PPU_RGB565_MODE);
        } else {
        #if (C_DISPLAY_DEV_HPIXEL == 320 && C_DISPLAY_DEV_VPIXEL == 240)
        	reg = TFT_SIZE_320X240;
        #elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 234)
        	reg = TFT_SIZE_480X234;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 272)
	   		reg = TFT_SIZE_480X272;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 720 && C_DISPLAY_DEV_VPIXEL == 480)
	   		reg = TFT_SIZE_720X480;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 480)
	      	reg = TFT_SIZE_800X480;	
	    #elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 600)
	      	reg = TFT_SIZE_800X600;		
   		#elif (C_DISPLAY_DEV_HPIXEL == 1024 && C_DISPLAY_DEV_VPIXEL == 768)
	      	reg = TFT_SIZE_800X600;		
	    #else
	    	R_FREE_SIZE	= ((C_DISPLAY_DEV_HPIXEL & 0x7FF) << 16) | (C_DISPLAY_DEV_VPIXEL & 0x7FF);
   		#endif
   			R_PPU_ENABLE= PPU_QVGA_MODE|reg|PPU_FRAME_BASE_MODE|PPU_RGB565_MODE;	 
   		}
   		break;
    
   	case IMAGE_OUTPUT_FORMAT_RGBG:
        if(DISPLAY_MODE == C_TV_QVGA) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE2);
        } else if(DISPLAY_MODE == C_TV_VGA) {
           	R_PPU_ENABLE=(PPU_VGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE2);
        } else if(DISPLAY_MODE == C_TV_D1) {
        	R_PPU_ENABLE=(PPU_QVGA_MODE|TFT_SIZE_720X480|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE2);
        } else {
        #if (C_DISPLAY_DEV_HPIXEL == 320 && C_DISPLAY_DEV_VPIXEL == 240)
        	reg = TFT_SIZE_320X240;
        #elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 234)
        	reg = TFT_SIZE_480X234;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 272)
	   		reg = TFT_SIZE_480X272;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 720 && C_DISPLAY_DEV_VPIXEL == 480)
	   		reg = TFT_SIZE_720X480;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 480)
	      	reg = TFT_SIZE_800X480;	
	    #elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 600)
	      	reg = TFT_SIZE_800X600;		
   		#elif (C_DISPLAY_DEV_HPIXEL == 1024 && C_DISPLAY_DEV_VPIXEL == 768)
	      	reg = TFT_SIZE_800X600;		
	    #else
	    	R_FREE_SIZE	= ((C_DISPLAY_DEV_HPIXEL & 0x7FF) << 16) | (C_DISPLAY_DEV_VPIXEL & 0x7FF);	
   		#endif	
        	R_PPU_ENABLE=(PPU_QVGA_MODE|reg|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE2);
        } 
   		break;
   
   	case IMAGE_OUTPUT_FORMAT_GRGB:
   		if(DISPLAY_MODE == C_TV_QVGA) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE3);
        } else if(DISPLAY_MODE == C_TV_VGA) {
           	R_PPU_ENABLE=(PPU_VGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE3);
        } else if(DISPLAY_MODE == C_TV_D1) {
        	R_PPU_ENABLE=(PPU_QVGA_MODE|TFT_SIZE_720X480|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE3);
        } else {
        #if (C_DISPLAY_DEV_HPIXEL == 320 && C_DISPLAY_DEV_VPIXEL == 240)
        	reg = TFT_SIZE_320X240;
        #elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 234)
        	reg = TFT_SIZE_480X234;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 272)
	   		reg = TFT_SIZE_480X272;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 720 && C_DISPLAY_DEV_VPIXEL == 480)
	   		reg = TFT_SIZE_720X480;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 480)
	      	reg = TFT_SIZE_800X480;	
	    #elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 600)
	      	reg = TFT_SIZE_800X600;		
   		#elif (C_DISPLAY_DEV_HPIXEL == 1024 && C_DISPLAY_DEV_VPIXEL == 768)
	      	reg = TFT_SIZE_800X600;		
	    #else
	    	R_FREE_SIZE	= ((C_DISPLAY_DEV_HPIXEL & 0x7FF) << 16) | (C_DISPLAY_DEV_VPIXEL & 0x7FF);		
   		#endif	
			R_PPU_ENABLE=(PPU_QVGA_MODE|reg|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_RGBG_MODE|PPU_RGBG_TYPE3);
        }
   		break;	
   
   	case IMAGE_OUTPUT_FORMAT_UYVY:
        if(DISPLAY_MODE == C_TV_QVGA) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE2);
        } else if(DISPLAY_MODE == C_TV_VGA) {
           	R_PPU_ENABLE=(PPU_VGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE2);    
   		} else if(DISPLAY_MODE == C_TV_D1) {
   			R_PPU_ENABLE=(PPU_QVGA_MODE|TFT_SIZE_720X480|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE2);
   		} else {
   		#if (C_DISPLAY_DEV_HPIXEL == 320 && C_DISPLAY_DEV_VPIXEL == 240)
        	reg = TFT_SIZE_320X240;
        #elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 234)
        	reg = TFT_SIZE_480X234;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 272)
	   		reg = TFT_SIZE_480X272;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 720 && C_DISPLAY_DEV_VPIXEL == 480)
	   		reg = TFT_SIZE_720X480;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 480)
	      	reg = TFT_SIZE_800X480;	
	    #elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 600)
	      	reg = TFT_SIZE_800X600;		
   		#elif (C_DISPLAY_DEV_HPIXEL == 1024 && C_DISPLAY_DEV_VPIXEL == 768)
	      	reg = TFT_SIZE_800X600;		
	    #else
	    	R_FREE_SIZE	= ((C_DISPLAY_DEV_HPIXEL & 0x7FF) << 16) | (C_DISPLAY_DEV_VPIXEL & 0x7FF);	
   		#endif
   			R_PPU_ENABLE=(PPU_QVGA_MODE|reg|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE2);	
   		}
   		break;

   	case IMAGE_OUTPUT_FORMAT_YUYV:
        if(DISPLAY_MODE == C_TV_QVGA) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE3);
        } else if(DISPLAY_MODE == C_TV_VGA) {
           	R_PPU_ENABLE=(PPU_VGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE3);    
   		} else if(DISPLAY_MODE == C_TV_D1) {
   			R_PPU_ENABLE=(PPU_QVGA_MODE|TFT_SIZE_720X480|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE3);
   		} else {
   		#if (C_DISPLAY_DEV_HPIXEL == 320 && C_DISPLAY_DEV_VPIXEL == 240)
        	reg = TFT_SIZE_320X240;
        #elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 234)
        	reg = TFT_SIZE_480X234;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 272)
	   		reg = TFT_SIZE_480X272;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 720 && C_DISPLAY_DEV_VPIXEL == 480)
	   		reg = TFT_SIZE_720X480;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 480)
	      	reg = TFT_SIZE_800X480;	
	    #elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 600)
	      	reg = TFT_SIZE_800X600;		
   		#elif (C_DISPLAY_DEV_HPIXEL == 1024 && C_DISPLAY_DEV_VPIXEL == 768)
	      	reg = TFT_SIZE_800X600;		
	    #else
	    	R_FREE_SIZE	= ((C_DISPLAY_DEV_HPIXEL & 0x7FF) << 16) | (C_DISPLAY_DEV_VPIXEL & 0x7FF);		
   		#endif
   			R_PPU_ENABLE=(PPU_QVGA_MODE|reg|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE3);
   		}
   		break;
   	case IMAGE_OUTPUT_FORMAT_YVYU:
   		 if(DISPLAY_MODE == C_TV_QVGA) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE1);
        } else if(DISPLAY_MODE == C_TV_VGA) {
           	R_PPU_ENABLE=(PPU_VGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE1);    
   		} else if(DISPLAY_MODE == C_TV_D1) {
   			R_PPU_ENABLE=(PPU_QVGA_MODE|TFT_SIZE_720X480|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE1);
   		} else {
   		#if (C_DISPLAY_DEV_HPIXEL == 320 && C_DISPLAY_DEV_VPIXEL == 240)
        	reg = TFT_SIZE_320X240;
        #elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 234)
        	reg = TFT_SIZE_480X234;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 272)
	   		reg = TFT_SIZE_480X272;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 720 && C_DISPLAY_DEV_VPIXEL == 480)
	   		reg = TFT_SIZE_720X480;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 480)
	      	reg = TFT_SIZE_800X480;	
	    #elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 600)
	      	reg = TFT_SIZE_800X600;		
   		#elif (C_DISPLAY_DEV_HPIXEL == 1024 && C_DISPLAY_DEV_VPIXEL == 768)
	      	reg = TFT_SIZE_800X600;		
	    #else
	    	R_FREE_SIZE	= ((C_DISPLAY_DEV_HPIXEL & 0x7FF) << 16) | (C_DISPLAY_DEV_VPIXEL & 0x7FF);		
   		#endif
   			R_PPU_ENABLE=(PPU_QVGA_MODE|reg|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE1);
   		}
   		break;   		
   	case IMAGE_OUTPUT_FORMAT_VYUY://0xff11:
   		 if(DISPLAY_MODE == C_TV_QVGA) {
           	R_PPU_ENABLE=(PPU_QVGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE0);
        } else if(DISPLAY_MODE == C_TV_VGA) {
           	R_PPU_ENABLE=(PPU_VGA_MODE|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE0);    
   		} else if(DISPLAY_MODE == C_TV_D1) {
   			R_PPU_ENABLE=(PPU_QVGA_MODE|TFT_SIZE_720X480|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE0);
   		} else {
   		#if (C_DISPLAY_DEV_HPIXEL == 320 && C_DISPLAY_DEV_VPIXEL == 240)
        	reg = TFT_SIZE_320X240;
        #elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 234)
        	reg = TFT_SIZE_480X234;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 480 && C_DISPLAY_DEV_VPIXEL == 272)
	   		reg = TFT_SIZE_480X272;
	   	#elif (C_DISPLAY_DEV_HPIXEL == 720 && C_DISPLAY_DEV_VPIXEL == 480)
	   		reg = TFT_SIZE_720X480;	
	   	#elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 480)
	      	reg = TFT_SIZE_800X480;	
	    #elif (C_DISPLAY_DEV_HPIXEL == 800 && C_DISPLAY_DEV_VPIXEL == 600)
	      	reg = TFT_SIZE_800X600;		
   		#elif (C_DISPLAY_DEV_HPIXEL == 1024 && C_DISPLAY_DEV_VPIXEL == 768)
	      	reg = TFT_SIZE_800X600;		
	    #else
	    	R_FREE_SIZE	= ((C_DISPLAY_DEV_HPIXEL & 0x7FF) << 16) | (C_DISPLAY_DEV_VPIXEL & 0x7FF);		
   		#endif
   			R_PPU_ENABLE=(PPU_QVGA_MODE|reg|PPU_FRAME_BASE_MODE|PPU_YUYV_RGBG_FORMAT_MODE|PPU_YUYV_MODE|PPU_YUYV_TYPE0);
   		}
   		break;
	} 

	if (TV_TFT >= C_TV_QVGA) {
		R_TV_FBI_ADDR = BUF;
	} else {	
		R_TFT_FBI_ADDR = BUF;
	}
}

#if VIDEO_DISPALY_WITH_PPU == 1
#include "Sprite_demo.h"
#include "Text_demo.h"

#if MD_DEMO_EN == 1
#include "sprite_data_16x16SP_HDR.h"
#elif OBJECT_TRAINING_MEUN_EN == 1
#include "SPRITE_OBJECT_TRAINING_HDR.h"
// PPU SP NUMBER
#define C_JPG_SAVE_NUMBER_1          0
#define C_JPG_SAVE_NUMBER_2          1
#define C_JPG_SAVE                   2
#define C_OBJECT_TRAIN_SAVE          3
const INT32U* SP_NUMBER_POOL[]={
 	(INT32U *)_Sprite001_IMG0000_CellIdx,
 	(INT32U *)_Sprite001_IMG0001_CellIdx,
 	(INT32U *)_Sprite001_IMG0002_CellIdx,
 	(INT32U *)_Sprite001_IMG0003_CellIdx,
 	(INT32U *)_Sprite001_IMG0004_CellIdx,
 	(INT32U *)_Sprite001_IMG0005_CellIdx,
 	(INT32U *)_Sprite001_IMG0006_CellIdx,
 	(INT32U *)_Sprite001_IMG0007_CellIdx,
 	(INT32U *)_Sprite001_IMG0008_CellIdx,
 	(INT32U *)_Sprite001_IMG0009_CellIdx
};
#elif FACE_RECOGNIZE_MEUN_EN == 1
#include "SPRITE_FACE_RECOGNIZE_HDR.h"
// PPU SP NUMBER
#define C_JPG_SAVE_NUMBER_1          0
#define C_JPG_SAVE_NUMBER_2          1
#define C_JPG_SAVE_NUMBER_3          2
#define C_JPG_SAVE_ICON              3
#define C_FACE_DEMO_STATE            4
#define C_FACE_IDENTIFY_END          5
#define C_FACE_IDENTIFY_LEVEL        6
#define C_FACE_IDENTIFY_ICON1        7
#define C_FACE_IDENTIFY_ICON2        8
// mode
#define FACE_MODE_GET                0
#define FACE_TRAINING_RESULT_GET     1
#define FACE_IDENTIFY_RESULT_GET     2
#define FACE_SECURITY_LEVEL_GET      3
#define FACE_TRAINING_COUNTER_GET    4
#define FACE_TRAINING_MODE           1
#define FACE_IDENTIFY_MODE           2
const INT32U* FACE_SP_NUMBER_POOL[]={
 	(INT32U *)_Sprite0001_IMG0000_CellIdx,
 	(INT32U *)_Sprite0001_IMG0001_CellIdx,
 	(INT32U *)_Sprite0001_IMG0002_CellIdx,
 	(INT32U *)_Sprite0001_IMG0003_CellIdx,
 	(INT32U *)_Sprite0001_IMG0004_CellIdx,
 	(INT32U *)_Sprite0001_IMG0005_CellIdx,
 	(INT32U *)_Sprite0001_IMG0006_CellIdx,
 	(INT32U *)_Sprite0001_IMG0007_CellIdx,
 	(INT32U *)_Sprite0001_IMG0008_CellIdx,
 	(INT32U *)_Sprite0001_IMG0009_CellIdx
};
#endif

void video_codec_ppu_configure(void)
{
	INT16U ppu_frame_width, ppu_frame_height;
	INT32S i, j;
	INT32U frame_size, buffer_ptr;
	#if C_DISPLAY_DEVICE < C_TV_QVGA
		INT16U width, height;
	#endif
	
	/* initial ppu register parameter set structure */
   	ppu_register_set = &turnkey_ppu_register_structure_1;
	gplib_ppu_init(ppu_register_set);
	gplib_ppu_enable_set(ppu_register_set, 1);							// Enable PPU
	gplib_ppu_char0_transparent_set(ppu_register_set, 1);					// Enable character 0 transparent mode
	
#if 1	// TV/tft frame mode
  	gplib_ppu_non_interlace_set(ppu_register_set, 0);			            // Set interlace mode
  	gplib_ppu_frame_buffer_mode_set(ppu_register_set, 1, 0);		        // Enable TV/TFT frame buffer mode
#else	// TFT line mode
	gplib_ppu_non_interlace_set(ppu_register_set, 0);          		 	// Set non-interlace mode
	gplib_ppu_frame_buffer_mode_set(ppu_register_set, 0, 1);    			// Enable TFT line mode
#endif	
	gplib_ppu_fb_format_set(ppu_register_set, 1, 1);			            // Set PPU output frame buffer format to YUYV
   
   	#if C_DISPLAY_DEVICE >= C_TV_QVGA
   		#if C_DISPLAY_DEVICE == C_TV_QVGA
    		ppu_frame_width = 320; 
   			ppu_frame_height = 240;
    		gplib_ppu_vga_mode_set(ppu_register_set, 0);						// Disable VGA mode
    		gplib_ppu_resolution_set(ppu_register_set, C_TV_RESOLUTION_320X240);// Set display resolution to 320X240
   		#elif C_DISPLAY_DEVICE == C_TV_VGA
   			ppu_frame_width = 640; 
   			ppu_frame_height = 480;
    		gplib_ppu_vga_mode_set(ppu_register_set, 1);						// Enable VGA mode
    		gplib_ppu_resolution_set(ppu_register_set, C_TV_RESOLUTION_640X480);// Set display resolution to 640X480
    	#else
   			ppu_frame_width = 720; 
   			ppu_frame_height = 480;
    		gplib_ppu_vga_mode_set(ppu_register_set, 0);						// Enable VGA mode
    		gplib_ppu_resolution_set(ppu_register_set, C_TV_RESOLUTION_720X480);// Set display resolution to 640X480
        #endif    
    #else//tft
  		video_encode_get_display_size(&width, &height);
  		if(width==640&&height==480)
  		{
   			ppu_frame_width = 640; 
   			ppu_frame_height = 480;
    		gplib_ppu_resolution_set(ppu_register_set, C_TFT_RESOLUTION_640X480);// Set display resolution to 640X480  		
  		}
  		else if(width==800&&height==480)
  		{
   			ppu_frame_width = 800; 
   			ppu_frame_height = 480;
    		gplib_ppu_resolution_set(ppu_register_set, C_TFT_RESOLUTION_800X480);// Set display resolution to 640X480    		
  		}
  		else
  		{
    		ppu_frame_width = 320; 
   			ppu_frame_height = 240;
    		gplib_ppu_resolution_set(ppu_register_set, C_TFT_RESOLUTION_320X240);// Set display resolution to 320X240  		
  		}
  		gplib_ppu_vga_mode_set(ppu_register_set, 0);						// Enable VGA mode	
    #endif
    
    gplib_ppu_bottom_up_mode_set(ppu_register_set, 1);                    // Disable bottom to top
    gplib_ppu_palette_type_set(ppu_register_set, 0, 0);                   // text and sprite palette mode
    gplib_ppu_text_direct_mode_set(ppu_register_set, 0);			        // Enable TEXT direct address mode
    
    gplib_ppu_long_burst_set(ppu_register_set, 1);						// Disable PPU long burst
    if(C_DISPLAY_DEVICE < C_TV_QVGA)//tft 	
    	gplib_ppu_tft_long_burst_set(ppu_register_set, 1);    			// Enable TFT long burst
    
    gplib_ppu_deflicker_mode_set(0);			            					// Disable De-flicker is only valid under VGA + Interlace + Frame mode	
    gplib_ppu_yuv_type_set(ppu_register_set, 3);							// Set 32-bit color format to Y1UY0V
    
    frame_size = ppu_frame_width * ppu_frame_height * 2; 
    for (i=0; i<PPU_FRAME_NO; i++) 
    {
		buffer_ptr = (INT32U) gp_malloc_align(frame_size, 64);
		DBG_PRINT("ppu_frame = 0x%x\r\n", buffer_ptr);
		if (buffer_ptr) 
		{
	    	INT32U *ptr;
	    	ptr = (INT32U *) buffer_ptr;
	    	for (j=0; j<frame_size; j++) 
	    	{
	    	    *ptr++ = 0x00800080;              // Black for YUYV color mode
	    	}
			gplib_ppu_frame_buffer_add(ppu_register_set, buffer_ptr);
		} 
		else 
			DBG_PRINT("Failed to allocate frame buffer %d for PPU driver!\r\n", i);
	}
	
	/* display at text1 or video decode at text1*/
	buffer_ptr = (INT32U) gp_malloc(1024*4);	// 4K character number array is enough for char mode(Character 32x32) and bitmap mode(TEXT 1024x1024)
	if (!buffer_ptr) 
	{
		DBG_PRINT("Photo display task failed to allocate character number array memory\r\n");
		while(1);
	}

	gplib_ppu_text_enable_set(ppu_register_set, C_PPU_TEXT1, 1);	                // Enable TEXT
	gplib_ppu_text_compress_disable_set(ppu_register_set, 1);	                    // Disable TEXT1/TEXT2 horizontal/vertical compress function
	gplib_ppu_text_attribute_source_select(ppu_register_set, C_PPU_TEXT1, 1);	    // Get TEXT attribute from register
	gplib_ppu_text_size_set(ppu_register_set, C_PPU_TEXT1, C_TXN_SIZE_1024X512);	// Set TEXT size to 1024x512
	gplib_ppu_text_segment_set(ppu_register_set, C_PPU_TEXT1, 0);				    // Set TEXT segment address	

	gplib_ppu_text_number_array_ptr_set(ppu_register_set, C_PPU_TEXT1, buffer_ptr); // Set TEXT number array address
	gplib_ppu_text_bitmap_mode_set(ppu_register_set, C_PPU_TEXT1, 1);			     // Enable bitmap mode
	gplib_ppu_text_color_set(ppu_register_set, C_PPU_TEXT1, 1, 3);				     // Set TEXT color to YUYV 
	
	gplib_ppu_text_position_set(ppu_register_set, C_PPU_TEXT1, 0, 0);
	gplib_ppu_text_offset_set(ppu_register_set, C_PPU_TEXT1, 0, 0);		

	#if OBJECT_TRAINING_MEUN_EN == 1
	 	gplib_ppu_palette_type_set(ppu_register_set, 0,1);
		gplib_ppu_palette_ram_ptr_set(ppu_register_set, 1, (INT32U)_SPRITE_OBJECT_TRAINING_Palette1); 	
		gplib_ppu_sprite_enable_set(ppu_register_set, 1);			                     // Disable Sprite
		gplib_ppu_sprite_coordinate_set(ppu_register_set, 0);                          // set sprite coordinate
		gplib_ppu_sprite_direct_mode_set(ppu_register_set, 0);		                 // Set sprite address mode
		gplib_ppu_sprite_number_set(ppu_register_set, 256);                             // Set sprite number
		gplib_ppu_sprite_attribute_ram_ptr_set(ppu_register_set, (INT32U)SpriteRAM);   // set sprite ram buffer
		gplib_ppu_sprite_extend_attribute_ram_ptr_set(ppu_register_set, (INT32U)SpriteExRAM); // value: 32-bit pointer to sprite extend attribute ram
	    gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)_SPRITE_OBJECT_TRAININ_CellData);      // sprite cell data
		gplib_ppu_sprite_blend_mode_set(ppu_register_set, 1); 	
	#elif FACE_RECOGNIZE_MEUN_EN == 1
		gplib_ppu_palette_type_set(ppu_register_set, 0,1);
		gplib_ppu_palette_ram_ptr_set(ppu_register_set, 1, (INT32U)_SPRITE_FACE_RECOGNIZE_Palette1); 	
		gplib_ppu_sprite_enable_set(ppu_register_set, 1);			                     	// Disable Sprite
		gplib_ppu_sprite_coordinate_set(ppu_register_set, 0);                          // set sprite coordinate
		gplib_ppu_sprite_direct_mode_set(ppu_register_set, 0);		                 // Set sprite address mode
		gplib_ppu_sprite_number_set(ppu_register_set, 256);                             // Set sprite number
		gplib_ppu_sprite_attribute_ram_ptr_set(ppu_register_set, (INT32U)SpriteRAM);   // set sprite ram buffer
		gplib_ppu_sprite_extend_attribute_ram_ptr_set(ppu_register_set, (INT32U)SpriteExRAM); // value: 32-bit pointer to sprite extend attribute ram
	    gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)_SPRITE_FACE_RECOGNIZE_CellData);      // sprite cell data
		gplib_ppu_sprite_blend_mode_set(ppu_register_set, 1);	
	#elif MD_DEMO_EN == 1
	    // Now configure Sprite
		gplib_ppu_sprite_init(ppu_register_set);
		gplib_ppu_sprite_enable_set(ppu_register_set, 1);			                        // Enable Sprite
		gplib_ppu_sprite_coordinate_set(ppu_register_set, 0);                               // set sprite center coordinate
		gplib_ppu_sprite_direct_mode_set(ppu_register_set, 0);		                        // Set sprite relative address mode
		gplib_ppu_sprite_number_set(ppu_register_set, 256);                                 // Set sprite number
		gplib_ppu_sprite_attribute_ram_ptr_set(ppu_register_set, (INT32U)SpriteRAM);        // set sprite ram buffer
		gplib_ppu_sprite_segment_set(ppu_register_set, (INT32U)_r16x16_CellData);           // sprite cell data
		gplib_ppu_palette_type_set(ppu_register_set, 1,0);                                  // P1024 mode
	    gplib_ppu_palette_ram_ptr_set(ppu_register_set, 0,(INT32U)_sprite_data_16X16SP_Palette0);    // sprite palette		
	#else // Disable Sprite
		gplib_ppu_sprite_init(ppu_register_set);
		gplib_ppu_sprite_enable_set(ppu_register_set, 0);	
	#endif	
}

void md_sprite_set(INT32U en)
{
  if(en)
	paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_640X480,PPU_hardware_coordinate,512);
  else	
	paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_320X240,PPU_hardware_coordinate,512);
}

#if OBJECT_TRAINING_MEUN_EN == 1
INT32S object_training_icon_set_update(INT32U en)
{
	SpN_ptr sp_ptr;
	INT32U i,sp_num_addr;

    if(en == 1)
    {
		//get sprite character number of image and sprite start ptr of sprite ram
		Get_sprite_image_info(C_OBJECT_TRAIN_SAVE,(SpN_ptr *)&sp_ptr);
		sp_num_addr=sp_ptr.nSPNum_ptr;
		for(i=0;i<sp_ptr.nSP_CharNum;i++)
		{
			gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,32); 
			sp_num_addr+=sizeof(SpN_RAM); 
		}	    
    }
    else if(en == 2)
    {
		//get sprite character number of image and sprite start ptr of sprite ram
		Get_sprite_image_info(C_OBJECT_TRAIN_SAVE,(SpN_ptr *)&sp_ptr);
		sp_num_addr=sp_ptr.nSPNum_ptr;
		for(i=0;i<sp_ptr.nSP_CharNum;i++)
		{
			gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,63);
			sp_num_addr+=sizeof(SpN_RAM);
		}	      
    }
    else if(en == 3)
    {
		//get sprite character number of image and sprite start ptr of sprite ram
		Get_sprite_image_info(C_OBJECT_TRAIN_SAVE,(SpN_ptr *)&sp_ptr);
		sp_num_addr=sp_ptr.nSPNum_ptr;
		for(i=0;i<sp_ptr.nSP_CharNum;i++)
		{
			gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,32);
			gplib_ppu_sprite_attribute_palette_set((SpN_RAM *)sp_num_addr,0,3);
			sp_num_addr+=sizeof(SpN_RAM);
		}	      
    }    

	return STATUS_OK;
}

INT32S object_training_icon_set(INT32U en,INT32U frame_counter)
{
    if(en == 1)
    {
	    set_sprite_init(C_OBJECT_TRAIN_SAVE,(INT32U)&Sprite003_SP);   
    	set_sprite_display_init(C_OBJECT_TRAIN_SAVE,0,0,(INT32U)_Sprite003_IMG0000_CellIdx);       
    }
    else if(en == 2)
    {
	    set_sprite_init(C_OBJECT_TRAIN_SAVE,(INT32U)&Sprite004_SP);        
    	set_sprite_display_init(C_OBJECT_TRAIN_SAVE,0,0,(INT32U)_Sprite004_IMG0000_CellIdx);
    }
    else if(en == 3)
    {
	    set_sprite_init(C_OBJECT_TRAIN_SAVE,(INT32U)&Sprite002_SP);        
    	set_sprite_display_init(C_OBJECT_TRAIN_SAVE,0,0,(INT32U)_Sprite002_IMG0006_CellIdx);
    }    
    else
    {   
	    set_sprite_init(C_OBJECT_TRAIN_SAVE,(INT32U)&Sprite003_SP);   
    	set_sprite_display_init(C_OBJECT_TRAIN_SAVE,0,0,(INT32U)0);    
    }
    
    return STATUS_OK;
}

INT32S jpg_save_number_set_update(INT32U en)
{
	SpN_ptr sp_ptr;
	INT32U i,sp_num_addr;
	     
	//get sprite character number of image and sprite start ptr of sprite ram
	Get_sprite_image_info(C_JPG_SAVE_NUMBER_1,(SpN_ptr *)&sp_ptr);
	sp_num_addr=sp_ptr.nSPNum_ptr;
	for(i=0;i<sp_ptr.nSP_CharNum;i++)
	{
		gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,en);
		sp_num_addr+=sizeof(SpN_RAM);
	}
	//get sprite character number of image and sprite start ptr of sprite ram
	Get_sprite_image_info(C_JPG_SAVE_NUMBER_2,(SpN_ptr *)&sp_ptr);
	sp_num_addr=sp_ptr.nSPNum_ptr;
	for(i=0;i<sp_ptr.nSP_CharNum;i++)
	{
		gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,en);
		sp_num_addr+=sizeof(SpN_RAM);
	}
	//get sprite character number of image and sprite start ptr of sprite ram
	Get_sprite_image_info(C_JPG_SAVE,(SpN_ptr *)&sp_ptr);
	sp_num_addr=sp_ptr.nSPNum_ptr;
	for(i=0;i<sp_ptr.nSP_CharNum;i++)
	{
		gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,en);
		gplib_ppu_sprite_attribute_palette_set((SpN_RAM *)sp_num_addr,0,3);
		sp_num_addr+=sizeof(SpN_RAM);
	
	}
	return STATUS_OK;		
}

INT32S jpg_save_number_set(INT32U en)
{
    INT32U temp;

    // image and position init    
    set_sprite_init(C_JPG_SAVE_NUMBER_1,(INT32U)&Sprite001_SP);
    set_sprite_init(C_JPG_SAVE_NUMBER_2,(INT32U)&Sprite001_SP); 
    set_sprite_init(C_JPG_SAVE,(INT32U)&Sprite002_SP);
    set_sprite_display_init(C_JPG_SAVE,160,6,(INT32U)_Sprite002_IMG0001_CellIdx);
    temp = (en / 10);
    set_sprite_display_init(C_JPG_SAVE_NUMBER_2,268,6,(INT32U)SP_NUMBER_POOL[temp]);
    temp = (en % 10);
    set_sprite_display_init(C_JPG_SAVE_NUMBER_1,288,6,(INT32U)SP_NUMBER_POOL[temp]);   
    
    return STATUS_OK;
}

INT32S object_training_sprite_update(INT32U state,INT32U image_num)
{
	jpg_save_number_set(image_num);
	object_training_icon_set(state, image_num);
	paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_320X240,LeftTop2Center_coordinate,20);
	jpg_save_number_set_update(32);
	object_training_icon_set_update(state);
	
	return STATUS_OK;
}

INT32S object_training_ppu_update(INT32U frame_buf, INT32U mode)
{
	INT32U temp;
	
	temp = 0;
	
	if(mode)
	{
		gplib_ppu_text_enable_set(ppu_register_set, C_PPU_TEXT1, 0);
		temp = gplib_ppu_go_and_wait_done(ppu_register_set);
	
	}
	else
	{
		gplib_ppu_text_enable_set(ppu_register_set, C_PPU_TEXT1, 1);
		temp = gplib_ppu_go_and_wait_done(ppu_register_set);
	}
	
	return temp;
}
#endif

#if FACE_RECOGNIZE_MEUN_EN == 1
INT32S face_training_mode_update(INT32U en)
{
	SpN_ptr sp_ptr;
	INT32U i,sp_num_addr;
   
    if(face_recognize_get_mode(FACE_MODE_GET) == FACE_TRAINING_MODE)
    {	     
		//get sprite character number of image and sprite start ptr of sprite ram
		Get_sprite_image_info(C_JPG_SAVE_NUMBER_1,(SpN_ptr *)&sp_ptr);
		sp_num_addr=sp_ptr.nSPNum_ptr;
		for(i=0;i<sp_ptr.nSP_CharNum;i++)
		{
			gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,en);
			sp_num_addr+=sizeof(SpN_RAM);
		}
		//get sprite character number of image and sprite start ptr of sprite ram
		Get_sprite_image_info(C_JPG_SAVE_NUMBER_2,(SpN_ptr *)&sp_ptr);
		sp_num_addr=sp_ptr.nSPNum_ptr;
		for(i=0;i<sp_ptr.nSP_CharNum;i++)
		{
			gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,en);
			sp_num_addr+=sizeof(SpN_RAM);
		}
		//get sprite character number of image and sprite start ptr of sprite ram
		Get_sprite_image_info(C_JPG_SAVE_NUMBER_3,(SpN_ptr *)&sp_ptr);
		sp_num_addr=sp_ptr.nSPNum_ptr;
		for(i=0;i<sp_ptr.nSP_CharNum;i++)
		{
			gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,en);
			sp_num_addr+=sizeof(SpN_RAM);
		}	
		//get sprite character number of image and sprite start ptr of sprite ram
		Get_sprite_image_info(C_JPG_SAVE_ICON,(SpN_ptr *)&sp_ptr);
		sp_num_addr=sp_ptr.nSPNum_ptr;
		for(i=0;i<sp_ptr.nSP_CharNum;i++)
		{
			gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,en);
			sp_num_addr+=sizeof(SpN_RAM);
		
		}
	}
	
	return STATUS_OK;
}

INT32S face_training_mode(INT32U image_num)
{
    INT32U temp,temp1;

    if(face_recognize_get_mode(FACE_MODE_GET) == FACE_TRAINING_MODE)
    {
	    // image and position init    
	    set_sprite_init(C_JPG_SAVE_NUMBER_1,(INT32U)&Sprite00001_SP);
	    set_sprite_init(C_JPG_SAVE_NUMBER_2,(INT32U)&Sprite00001_SP); 
	    set_sprite_init(C_JPG_SAVE_NUMBER_3,(INT32U)&Sprite00001_SP);
	    set_sprite_init(C_JPG_SAVE_ICON ,(INT32U)&Sprite0030_SP);
	 	set_sprite_display_init(C_JPG_SAVE_ICON,184,0,(INT32U)_Sprite0003_IMG0000_CellIdx);
	 	
	 	temp = (image_num / 100);
		set_sprite_display_init(C_JPG_SAVE_NUMBER_3,240,16,(INT32U)FACE_SP_NUMBER_POOL[temp]);
	    temp1 = temp * 100;
	    temp = (image_num-temp1);
		temp1 = (temp / 10);
		set_sprite_display_init(C_JPG_SAVE_NUMBER_2,264,16,(INT32U)FACE_SP_NUMBER_POOL[temp1]);
		temp1 = (temp % 10);
		set_sprite_display_init(C_JPG_SAVE_NUMBER_1,288,16,(INT32U)FACE_SP_NUMBER_POOL[temp1]);   
    }
    else
    {
	    // image and position init    
	    set_sprite_init(C_JPG_SAVE_NUMBER_1,(INT32U)&Sprite00001_SP);
	    set_sprite_init(C_JPG_SAVE_NUMBER_2,(INT32U)&Sprite00001_SP); 
	    set_sprite_init(C_JPG_SAVE_NUMBER_3,(INT32U)&Sprite00001_SP);
	    set_sprite_init(C_JPG_SAVE_ICON ,(INT32U)&Sprite0030_SP);
	 	set_sprite_display_init(C_JPG_SAVE_ICON,184,0,0);
		set_sprite_display_init(C_JPG_SAVE_NUMBER_3,240,16,0);
		set_sprite_display_init(C_JPG_SAVE_NUMBER_2,264,16,0);
		set_sprite_display_init(C_JPG_SAVE_NUMBER_1,288,16,0);     
    }
    
    return STATUS_OK;
}

INT32S face_demo_icon_mode_update(INT32U en)
{
	SpN_ptr sp_ptr;
	INT32U i,sp_num_addr,state;
	     
	//get sprite character number of image and sprite start ptr of sprite ram
	Get_sprite_image_info(C_FACE_DEMO_STATE,(SpN_ptr *)&sp_ptr);
	sp_num_addr=sp_ptr.nSPNum_ptr;
	for(i=0;i<sp_ptr.nSP_CharNum;i++)
	{
		gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,en);
		sp_num_addr+=sizeof(SpN_RAM);
	}
	
	state = face_recognize_get_mode(FACE_MODE_GET);
	if(state != FACE_TRAINING_MODE)
	{
		state = face_recognize_get_mode(FACE_IDENTIFY_RESULT_GET);
		if(state)
		{	
			//get sprite character number of image and sprite start ptr of sprite ram
			Get_sprite_image_info(C_JPG_SAVE_NUMBER_2,(SpN_ptr *)&sp_ptr);
			sp_num_addr=sp_ptr.nSPNum_ptr;
			for(i=0;i<sp_ptr.nSP_CharNum;i++)
			{
				gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,en);
				sp_num_addr+=sizeof(SpN_RAM);
			}
		}
	}
		
	//get sprite character number of image and sprite start ptr of sprite ram
	Get_sprite_image_info(C_FACE_IDENTIFY_LEVEL,(SpN_ptr *)&sp_ptr);
	sp_num_addr=sp_ptr.nSPNum_ptr;
	for(i=0;i<sp_ptr.nSP_CharNum;i++)
	{
		gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,en);
		sp_num_addr+=sizeof(SpN_RAM);
	}
	
	//get sprite character number of image and sprite start ptr of sprite ram
	Get_sprite_image_info(C_FACE_IDENTIFY_ICON1,(SpN_ptr *)&sp_ptr);
	sp_num_addr=sp_ptr.nSPNum_ptr;
	for(i=0;i<sp_ptr.nSP_CharNum;i++)
	{
		gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,en);
		sp_num_addr+=sizeof(SpN_RAM);
	}
	
	//get sprite character number of image and sprite start ptr of sprite ram
	Get_sprite_image_info(C_FACE_IDENTIFY_ICON2,(SpN_ptr *)&sp_ptr);
	sp_num_addr=sp_ptr.nSPNum_ptr;
	for(i=0;i<sp_ptr.nSP_CharNum;i++)
	{
		gplib_ppu_sprite_attribute_blend64_set((SpN_RAM *)sp_num_addr,1,en);
		sp_num_addr+=sizeof(SpN_RAM);
	}		
	
	return STATUS_OK;
}

INT32S face_demo_icon_mode(INT32U mode)
{
	INT32U state;
	
	if(mode == FACE_TRAINING_MODE)
	{
		set_sprite_init(C_FACE_DEMO_STATE ,(INT32U)&Sprite005_SP);
	    set_sprite_display_init(C_FACE_DEMO_STATE,0,0,(INT32U)_Sprite005_IMG0002_CellIdx);		
	}
	else if(mode == FACE_IDENTIFY_MODE)
	{
		set_sprite_init(C_FACE_DEMO_STATE ,(INT32U)&Sprite005_SP);
	    set_sprite_display_init(C_FACE_DEMO_STATE,0,0,(INT32U)_Sprite005_IMG0001_CellIdx);	
	}
	else
	{
		set_sprite_init(C_FACE_DEMO_STATE ,(INT32U)&Sprite005_SP);
	    set_sprite_display_init(C_FACE_DEMO_STATE,0,0,(INT32U)_Sprite005_IMG0000_CellIdx);	
	}
	
	if(mode != FACE_TRAINING_MODE)
	{
		state = face_recognize_get_mode(FACE_IDENTIFY_RESULT_GET);
		if(state)
		{
			set_sprite_init(C_FACE_IDENTIFY_END ,(INT32U)&Sprite006_SP);
			set_sprite_display_init(C_FACE_IDENTIFY_END,96,56,(INT32U)_Sprite006_IMG0000_CellIdx);						
		}
		else
		{
		    set_sprite_init(C_FACE_IDENTIFY_END ,(INT32U)&Sprite006_SP);
		 	set_sprite_display_init(C_FACE_IDENTIFY_END,96,56,(INT32U)0);		
		}
    }
    else
    {
	    set_sprite_init(C_FACE_IDENTIFY_END ,(INT32U)&Sprite006_SP);
	 	set_sprite_display_init(C_FACE_IDENTIFY_END,96,56,(INT32U)0);  
    }
	set_sprite_init(C_FACE_IDENTIFY_LEVEL ,(INT32U)&Sprite00001_SP);
	set_sprite_init(C_FACE_IDENTIFY_ICON1 ,(INT32U)&Sprite005_SP);
	set_sprite_init(C_FACE_IDENTIFY_ICON2 ,(INT32U)&Sprite005_SP);
    set_sprite_display_init(C_FACE_IDENTIFY_LEVEL,120,206,(INT32U)FACE_SP_NUMBER_POOL[face_recognize_get_mode(FACE_SECURITY_LEVEL_GET)]);	
    set_sprite_display_init(C_FACE_IDENTIFY_ICON1,0,198,(INT32U)(INT32U)_Sprite005_IMG0004_CellIdx);	
    set_sprite_display_init(C_FACE_IDENTIFY_ICON2,192,198,(INT32U)(INT32U)_Sprite005_IMG0005_CellIdx);	 	
	
	return 0;
}

INT32S face_recognize_sprite_update(void)
{
	face_training_mode(face_recognize_get_mode(FACE_TRAINING_COUNTER_GET));
	face_demo_icon_mode(face_recognize_get_mode(FACE_MODE_GET));
	paint_ppu_spriteram(ppu_register_set,Sprite_Coordinate_320X240,LeftTop2Center_coordinate,20);
	face_training_mode_update(32);
	face_demo_icon_mode_update(32);

	return 0;
}
#endif
#endif
