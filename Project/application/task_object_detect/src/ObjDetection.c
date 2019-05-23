#include <stdio.h>
#include "task_object_detect.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it =====================//
#if (APP_FACE_DETECTION_EN == 1) 
//==================================================================================//
#include "math.h"
#include "ObjDetection.h"
#include "RFMS.h"

/****************************************************
*		define										*
****************************************************/
#define C_AE_VALUE		 	0x48
#define FD_DEBUG_MSG_EN		0
#define FACE_ROI			0
#define LEYE_ROI			1
#define REYE_ROI			2
#define BESTFACE_ROI		3

/****************************************************
*		extern function								*
****************************************************/
extern INT32S scalerStart( gpImage *src, gpImage *dst);
extern INT32S scaler_wait_done(void);
extern INT32S scaler_clip(INT8U wait_done, gpImage *src, gpImage *dst, gpRect *clip);
extern void image_rotate(gpImage *src, gpImage *dst, gpPoint *pt, int angle, int scale);
extern ObjDetect_t *obj_detect_get_workmem(void);

#ifdef	__GC0308_DRV_C_	
extern INT8U GC0308_ReadY(void);
extern INT8U GC0308_SetY(INT8U Value);
extern INT8U GC0308_SetGain(INT8U Value1, INT8U Value2);
#endif

extern  const unsigned char gamma_34[256];
extern  const unsigned char gamma_30[256];
extern  const unsigned char gamma_44[256];
extern  const unsigned char gamma_58[256];

/****************************************************
*		varaible									*
****************************************************/
static ObjResult_t obj_result;
static INT32U face_recognize_state = 0;

////////////////////////////////////////////////////////////////
// scale function
////////////////////////////////////////////////////////////////
INT32S scalerStart(gpImage *src, gpImage *dst)
{
	gpRect clip;
	
	clip.x = 0;
	clip.y = 0;
	clip.width = src->width;
	clip.height = src->height;
	
	return scaler_clip(0, src, dst, &clip);	
}

INT32S scalerEnd(void)
{
	return scaler_wait_done();
}

INT32S scalerClip(gpImage *src, gpImage *dst, gpRect *clip)
{
	return scaler_clip(0, src, dst, clip);
}

////////////////////////////////////////////////////////////////
// sensor ae function
////////////////////////////////////////////////////////////////
void sensor_control_ae(void)
{
#ifdef	__GC0308_DRV_C_	
	INT32S YValue, temp;
	
	YValue = GC0308_ReadY();
	temp = ABS(YValue - C_AE_VALUE);	
	if(temp >= 10) {
		temp = temp/10;
		if(YValue > C_AE_VALUE) {	
			YValue -= 16 * temp;
		} else {
			YValue += 16 * temp;
		}
		
		GC0308_SetY(YValue);
	} 
#endif
}

////////////////////////////////////////////////////////////////
// get illumination
// input: orgimg, _roi
// output: lum
////////////////////////////////////////////////////////////////
static void getIllumination(gpImage *orgimg, gpRect *_roi, LumStr *lum)
{
	INT32S i, j;
	gpRect roi;
	INT8U *org_ptr;

	if(_roi == NULL) {
		roi.x = 0;
		roi.y = 0;
		roi.width = orgimg->width;
		roi.height = orgimg->height;
	} else {
		roi.x = _roi->x;
		roi.y = _roi->y;
		roi.width = _roi->width;
		roi.height = _roi->height;
	}

	lum->cnt[0] = lum->cnt[1] = lum->cnt[2] = 0;
	lum->l[0] = lum->l[1] = lum->l[2] = 0;
	org_ptr = orgimg->ptr + roi.x + roi.y * orgimg->widthStep;
	for(j=0; j<roi.height; j++) {
		INT8U *p_org;
		
		p_org = org_ptr + j * orgimg->widthStep;
		for(i=0; i<roi.width; i++) {
			INT8U t = *p_org++;

			if(t >= lum->level[0]) {
				if(t < lum->level[1]) {
					lum->l[0] += t;
					lum->cnt[0] += 1;
				} else if (t < lum->level[2]) {
					lum->l[1] += t;
					lum->cnt[1] += 1;
				} else if	(t < lum->level[3]) {
					lum->l[2] += t;
					lum->cnt[2] += 1;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////
// Gamma Correction for Gray Image
// input: orgimg
// output: outimg
////////////////////////////////////////////////////////////////
static INT32S GammaCorrection(gpImage *orgimg, gpImage *outimg, gpRect *_roi)
{
	INT32S i, j, dif;	
	const unsigned char *gamma_table;
	INT8U *org_ptr, *out_ptr;
	gpRect roi;
	LumStr lum;

	if(_roi == NULL) {
		roi.x = 0;
		roi.y = 0;
		roi.width = orgimg->width;
		roi.height = orgimg->height;
	} else {
		roi.x = _roi->x;
		roi.y = _roi->y;
		roi.width = _roi->width;
		roi.height = _roi->height;
	}

	lum.level[0] = 0;
	lum.level[1] = 85;
	lum.level[2] = 170;
	lum.level[3] = 255;
	getIllumination(orgimg, &roi, &lum);
	
	dif = ((lum.cnt[0] + lum.cnt[1] + lum.cnt[2]) * 1638) >> 15;
	gamma_table = gamma_34;
	if((lum.cnt[0] - lum.cnt[1]) >= dif && (lum.cnt[0] - lum.cnt[2]) >= dif) {
		gamma_table = gamma_30;
	} else if((lum.cnt[1] - lum.cnt[0]) >= dif && (lum.cnt[1] - lum.cnt[2]) >= dif) {
		gamma_table = gamma_44;
	} else if((lum.cnt[2] - lum.cnt[0]) >= dif && (lum.cnt[2] - lum.cnt[1]) >= dif) {
		gamma_table = gamma_58;
	}

	org_ptr = orgimg->ptr + roi.x + roi.y * orgimg->widthStep;
	out_ptr = outimg->ptr + roi.x + roi.y * outimg->widthStep;
	for(j = 0; j < roi.height ; j++) {
		unsigned char *p_org, *p_out;
		
		p_org = org_ptr + j * orgimg->widthStep;
		p_out = out_ptr + j * outimg->widthStep;
		for( i = 0; i < roi.width; i ++ ) {
			unsigned char t;

			t = *p_org++;			
			*p_out++ = gamma_table[t];
		}		
	}
	
	return 0;
}

////////////////////////////////////////////////////////////////
// Set to zero
// input: addr, size
// output: nonr
////////////////////////////////////////////////////////////////
static void mem_set_zero_int32(void *addr, INT32S size)
{
	INT32S i;
	INT32U *p_addr;

	if(size > 0) {
		INT32S t0, t1, t2, t3, t4, t5, t6, t7;

		t0 = 0;
		p_addr = (unsigned int *)addr;

		if(size >= 8) {
			t1 = t2 = t3 = t4 = t5 = t6 = t7 = 0;
			i = size >> 3;
			size = size - (i << 3);
			
			do {
				*p_addr++ = t0;
				*p_addr++ = t1;
				*p_addr++ = t2;
				*p_addr++ = t3;
				*p_addr++ = t4;
				*p_addr++ = t5;
				*p_addr++ = t6;
				*p_addr++ = t7;
				i--;
			} while( i != 0);			
		}

		if(size > 0) {
			i = size;
			do {
				*p_addr++ = t0;
				i--;
			} while(i != 0);
		}
	}
}

////////////////////////////////////////////////////////////////
// Modified Histogram Equalization for Gray Image
// input: orgimg
// output: outimg
////////////////////////////////////////////////////////////////
static INT32S ModEqualizeHist_fix(gpImage *orgimg, gpImage *outimg, gpRect *roi)
{
	INT32S i, x;
	INT32S bins[256], *p_bins;	
	INT32S width, height;
	INT32S widthStep, out_widthStep;
	INT32S sum;
	gpRect local_roi;
	INT32U scale;
	INT32S mean_bins;
	INT32S mean_bins_cnt;
	INT8U lut[256], *p_lut;
	INT8U *out_ptr, *org_ptr;

	if(orgimg->ch != 1) {
		return -1;
	}
	
	mem_set_zero_int32(bins, 256);
	if(roi == NULL) {
		local_roi.x = 0;
		local_roi.y = 0;
		local_roi.width = orgimg->width;
		local_roi.height = orgimg->height;
	} else {
		local_roi.x = roi->x;
		local_roi.y = roi->y;
		local_roi.width = roi->width;
		local_roi.height = roi->height;
	}

	
	width = local_roi.width;
	height = local_roi.height;
	widthStep = orgimg->widthStep;
	org_ptr = orgimg->ptr + local_roi.x + local_roi.y * widthStep;
	do {
		INT8U *p = org_ptr;

		for(x=0; x<= width-4; x+=4) {
			int v0 = *p++;
			int v1 = *p++;

			bins[v0]++;
			bins[v1]++;

			v0 = *p++;
			v1 = *p++;

			bins[v0]++;
			bins[v1]++;
		}

		for( ; x<width; x++) {
			int v0 = *p++;
			bins[v0]++;
		}

		org_ptr += widthStep;
		height-- ; 
	} while(height != 0);

	mean_bins = 0.0;
	mean_bins_cnt = 0;
	p_bins = bins;
	i = 256;
	do {
		INT32S v = *p_bins++;
		
		if(v != 0) {
			mean_bins += v;
			mean_bins_cnt++;
		}

		i--;
	} while(i != 0);
	mean_bins = mean_bins / mean_bins_cnt;

	sum = 0;
	p_bins = bins;
	i = 256;
	do {
		INT32S v = *p_bins;
		
		v = MIN(mean_bins, v);
		sum += v;
		*p_bins++ = v;
		i--;
	} while(i != 0);
	
	scale = (255 << 16)/sum;
	sum = 0;
	p_bins = bins;
	p_lut = lut;
	i = 256;
	do {
		INT32S v = *p_bins++;
		
		sum += v;
        *p_lut++ = (sum*scale) >> 16;//(unsigned char)gpRound(sum*scale);
		i--;
	} while(i != 0);


	lut[0] = 0;
	out_widthStep = outimg->widthStep;
	org_ptr = orgimg->ptr + local_roi.x + local_roi.y * widthStep;
	out_ptr = outimg->ptr + local_roi.x + local_roi.y * out_widthStep;
	height = local_roi.height;
	//for(; height-- ; out_ptr += out_widthStep, org_ptr += widthStep)
	do {
		INT8U *p_org, *p_out;
		
		p_org = org_ptr;
		p_out = out_ptr;
		for(i=0; i<=width-4; i+=4) {
			INT32S p0, p1, p2, p3;
			INT8U t0, t1, t2, t3;

			p0 = *p_org++;
			p1 = *p_org++;
			p2 = *p_org++;
			p3 = *p_org++;
			t0 = lut[p0];          
			t1 = lut[p1];			
			t2 = lut[p2];          
			t3 = lut[p3];     
			*p_out++ = t0;                        
			*p_out++ = t1;                      
			*p_out++ = t2;
			*p_out++ = t3;
		}
		for( ; i<width; i++)
		{
			INT32S p0 = *p_org++;
			INT8U t0 = lut[p0];
			
			*p_out++ = t0;
		}
		
		out_ptr += out_widthStep; 
		org_ptr += widthStep;
		height-- ; 
	} while(height != 0);

	return 0;
}

////////////////////////////////////////////////////////////////
// tracking
////////////////////////////////////////////////////////////////
INT32S obj_track_init(ObjDetect_t *odWorkMem)
{
	INT32U mem_size;
	gpImage *img = &odWorkMem->image;
	
	mem_size = track_get_memory_size();
	odWorkMem->obj_track_WorkMem = gp_malloc_align(mem_size, 16);
	if(odWorkMem->obj_track_WorkMem == 0) {
		DBG_PRINT("Fail to allocate obj_detect_WorkMem\r\n");
		return -1;
	}
	gp_memset((INT8S *)odWorkMem->obj_track_WorkMem, 0x00, mem_size);
	
	track_init(odWorkMem->obj_track_WorkMem, img->width, img->height);
	return 0;
}

////////////////////////////////////////////////////////////////
// mouth detect
////////////////////////////////////////////////////////////////
#if MOUTH_NOSE_DETECT_EN == 1
void mouth_detect_init(MouthNoseDetect_t *mdWorkMem)
{
	MouthDetect_init(mdWorkMem->mouth_detect_WorkMem);
}

void mouth_detect_reset(void *para)
{
	MouthNoseDetect_t *mdWorkMem = (MouthNoseDetect_t *)para;
	
	mouth_detect_init(mdWorkMem);
}

INT32S mouth_detect_alloc(void)
{
	MouthNoseDetect_t *mdWorkMem;
	INT32S nRet;
	INT32U mem_size;
	
	// mouth detect variable allocate
	mdWorkMem = gp_malloc_align(sizeof(MouthNoseDetect_t), 32);
	if(mdWorkMem == 0) {
		DBG_PRINT("Fail to allocate mdWorkMem\r\n");
		return 0;
	}
	DBG_PRINT("mdWorkMem = 0x%x\r\n", mdWorkMem);
	gp_memset((INT8S *)mdWorkMem, 0, sizeof(MouthNoseDetect_t));
	
	// mouth detect buffer allocate
	mdWorkMem->upright_face_img.width     = RESIZE_FACE_IMG_WIDTH;
	mdWorkMem->upright_face_img.height    = (int) (RESIZE_FACE_IMG_HEIGHT*1.2);
	mdWorkMem->upright_face_img.widthStep = RESIZE_FACE_IMG_WIDTH;
	mdWorkMem->upright_face_img.format    = IMG_FMT_GRAY;
	mdWorkMem->upright_face_img.ch        = 1;
	
	mem_size = mdWorkMem->upright_face_img.widthStep * mdWorkMem->upright_face_img.height;
	mdWorkMem->upright_face_img.ptr = gp_malloc_align(mem_size, 32);
	if(mdWorkMem->upright_face_img.ptr == 0) {
		DBG_PRINT("Fail to allocate mdWorkMem->upright_face_img.ptr\r\n");
		RETURN(STATUS_FAIL);
	}
	
	// mouth detect work memory allocate
	mem_size = MouthDetect_get_work_mem_size();
	mdWorkMem->mouth_detect_WorkMem = gp_malloc_align(mem_size, 32);
	if(mdWorkMem->mouth_detect_WorkMem == 0) {
		DEBUG_MSG("Fail to allocate mdWork->mouth_detect_WorkMem memory\r\n");
		RETURN(STATUS_FAIL);
	}
	DEBUG_MSG("mouth_detect_WorkMem = 0x%x\r\n", mdWorkMem->mouth_detect_WorkMem);
	gp_memset((INT8S *)mdWorkMem->mouth_detect_WorkMem, 0, mem_size);
	
	// mouth init
	mouth_detect_init(mdWorkMem);
	
	nRet = STATUS_OK;
Return:
	if(nRet < 0) {
		mouth_detect_free(mdWorkMem);
		return 0;
	}

	return (INT32S)mdWorkMem;
}

void mouth_detect_free(void *md_work_mem)
{
	MouthNoseDetect_t *mdWorkMem = (MouthNoseDetect_t *)md_work_mem;

	if(mdWorkMem) {
		if(mdWorkMem->upright_face_img.ptr) {
			gp_free(mdWorkMem->upright_face_img.ptr);
			mdWorkMem->upright_face_img.ptr = 0;
		}
		
		if(mdWorkMem->mouth_detect_WorkMem) {
			gp_free(mdWorkMem->mouth_detect_WorkMem);	
			mdWorkMem->mouth_detect_WorkMem = 0;			
		}
		
		gp_free(mdWorkMem);
		mdWorkMem = 0;
	}
}

void mouth_detect_proc(void *md_work_mem, gpImage *face_img, void *pre_result)
{	
	MouthNoseDetect_t *mdWorkMem = (MouthNoseDetect_t *)md_work_mem;
	FaceEyeResult_t *p_result = (FaceEyeResult_t *)pre_result;	
	INT32S nose_ret, mouth_ret, int_angle, int_scale;
	INT32S eye_distance;
	double angle;
	gpPoint *leye, *reye;
	gpPoint rotated_reye;
	gpFeaturePoint *mouth_pt, *nose_pt;

	leye = &p_result->left_eye;
	reye = &p_result->right_eye;	

	mouth_pt = &mdWorkMem->mouth_pt;
	nose_pt = &mdWorkMem->nose_pt;

	// get upright face
	angle = p_result->eye_angle;

	int_angle = (INT32S)(angle * 180 / PI + 0.5);
	int_scale = (INT32S)(1.0 * 1024 + 0.5);
	image_rotate(face_img, &mdWorkMem->upright_face_img, leye, int_angle, int_scale);

	eye_distance = sqrt((double)((reye->x - leye->x)*(reye->x - leye->x) 
						+ (reye->y - leye->y)*(reye->y - leye->y)));

	rotated_reye.y = reye->y - eye_distance * sin(angle);
	rotated_reye.x = leye->x + eye_distance;

	// Nose Detection
	nose_ret = NoseDetect(mdWorkMem->mouth_detect_WorkMem, mdWorkMem->upright_face_img, *leye, rotated_reye, nose_pt);
	
	// Mouth Detection
	mouth_ret = 0;
	if(nose_ret) {
		mouth_ret = MouthDetect(mdWorkMem->mouth_detect_WorkMem, mdWorkMem->upright_face_img, 
							leye, &rotated_reye, nose_pt, eye_distance, mouth_pt);
	}
	
	if(mouth_ret) {
		obj_result.is_smile = MouthDetect_is_smile(mdWorkMem->mouth_detect_WorkMem);
	} else { 
		obj_result.is_smile = 0;
	}

	obj_result.is_get_mouth = mdWorkMem->is_get_mouth = mouth_ret;
	obj_result.is_get_nose = mdWorkMem->is_get_nose = nose_ret;
}

void mouth_detect_get_result(void *md_work_mem, void *result)
{
	MouthNoseDetect_t *mdWorkMem = (MouthNoseDetect_t *) md_work_mem;
	ObjResult_t *p_obj_result = result;

	if(mdWorkMem->is_get_nose) {
		//left
		p_obj_result->nose[0].x = mdWorkMem->nose_pt.left_pt.x;
		p_obj_result->nose[0].y = mdWorkMem->nose_pt.left_pt.y;
		p_obj_result->nose[0].width = 4;
		p_obj_result->nose[0].height = 2;
		//right
		p_obj_result->nose[1].x = mdWorkMem->nose_pt.right_pt.x;
		p_obj_result->nose[1].y = mdWorkMem->nose_pt.right_pt.y;
		p_obj_result->nose[1].width = 4;
		p_obj_result->nose[1].height = 2;	
	}

	if(mdWorkMem->is_get_mouth) {
		//top
		p_obj_result->mouth[0].x = mdWorkMem->mouth_pt.top_pt.x;
		p_obj_result->mouth[0].y = mdWorkMem->mouth_pt.top_pt.y;
		p_obj_result->mouth[0].width = 4;
		p_obj_result->mouth[0].height = 2;
		//right
		p_obj_result->mouth[1].x = mdWorkMem->mouth_pt.right_pt.x;
		p_obj_result->mouth[1].y = mdWorkMem->mouth_pt.right_pt.y;
		p_obj_result->mouth[1].width = 4;
		p_obj_result->mouth[1].height = 2;
		//bottom		
		p_obj_result->mouth[2].x = mdWorkMem->mouth_pt.bottom_pt.x;
		p_obj_result->mouth[2].y = mdWorkMem->mouth_pt.bottom_pt.y;
		p_obj_result->mouth[2].width = 4;
		p_obj_result->mouth[2].height = 2;
		//left
		p_obj_result->mouth[3].x = mdWorkMem->mouth_pt.left_pt.x;
		p_obj_result->mouth[3].y = mdWorkMem->mouth_pt.left_pt.y;
		p_obj_result->mouth[3].width = 4;
		p_obj_result->mouth[3].height = 2;
	}	
}
#endif

////////////////////////////////////////////////////////////////
// eyes detect
////////////////////////////////////////////////////////////////
#if EYE_DETECT_EN == 1
void eye_detect_init(EyeDetect_t *edWorkMem)
{
	edWorkMem->scale = (int) (1.1*65536);
	edWorkMem->xstep = 2;
	
	FaceDetect_set_ScalerFn(edWorkMem->eye_detect_WorkMem, scalerStart, scalerEnd, scalerClip); 
}

void eye_detect_reset(void *ed_work_mem)
{
	EyeDetect_t *edWorkMem = (EyeDetect_t *)ed_work_mem;

	if(edWorkMem->post_proc.flag == POST_PROC_ENABLE) {
		edWorkMem->post_proc.reset(edWorkMem->post_proc.workmem);
	}
}

INT32S eye_detect_alloc(void)
{
	EyeDetect_t *edWorkMem;
	INT32S nRet;
	INT32U mem_size;
	
	// eye detect variable allocate
	edWorkMem = gp_malloc_align(sizeof(EyeDetect_t), 32);
	if(edWorkMem == 0) {
		DEBUG_MSG("Fail to allocate edWorkMem\r\n");
		return 0;
	}
	DEBUG_MSG("edWorkMem = 0x%x\r\n", edWorkMem);
	gp_memset((INT8S *)edWorkMem, 0, sizeof(EyeDetect_t));
	
	// eye detect buffer allocate 
	edWorkMem->face_img.width     = RESIZE_FACE_IMG_WIDTH;
	edWorkMem->face_img.height    = (int)(RESIZE_FACE_IMG_HEIGHT*1.2);
	edWorkMem->face_img.widthStep = RESIZE_FACE_IMG_WIDTH;
	edWorkMem->face_img.format    = IMG_FMT_GRAY;
	edWorkMem->face_img.ch        = 1;
	
	mem_size = edWorkMem->face_img.widthStep * edWorkMem->face_img.height;
	edWorkMem->face_img.ptr = gp_malloc_align(mem_size, 32);
	if(edWorkMem->face_img.ptr == 0) {
		DEBUG_MSG("Fail to allocate edWorkMem->face_img.ptr\r\n");
		RETURN(STATUS_FAIL);
	}

	edWorkMem->eq_face_img.width     = RESIZE_FACE_IMG_WIDTH;
	edWorkMem->eq_face_img.height    = (int)(RESIZE_FACE_IMG_HEIGHT*1.2);
	edWorkMem->eq_face_img.widthStep = RESIZE_FACE_IMG_WIDTH;
	edWorkMem->eq_face_img.format    = IMG_FMT_GRAY;
	edWorkMem->eq_face_img.ch        = 1;
	
	mem_size = edWorkMem->eq_face_img.widthStep * edWorkMem->eq_face_img.height;
	edWorkMem->eq_face_img.ptr = gp_malloc_align(mem_size, 32);
	if(edWorkMem->eq_face_img.ptr == 0) {
		DEBUG_MSG("Fail to allocate edWorkMem->eq_face_img.ptr\r\n");
		RETURN(STATUS_FAIL);
	}
	
	// eye detect work memory
	mem_size = FaceDetect_Config(0, 0, edWorkMem->face_img.width, edWorkMem->face_img.height , 
								edWorkMem->face_img.ch, OBJDETECT_MAX_RESULT, 2, 2);
	edWorkMem->eye_detect_WorkMem = gp_malloc_align(mem_size, 32);
	if(edWorkMem->eye_detect_WorkMem == 0) {
		DEBUG_MSG("Fail to allocate edWorkMem->eye_detect_WorkMem\r\n");
		RETURN(STATUS_FAIL);
	}
	DEBUG_MSG("eye_detect_WorkMem = 0x%x\r\n", edWorkMem->eye_detect_WorkMem);
	gp_memset((INT8S *)edWorkMem->eye_detect_WorkMem, 0, mem_size);
	
	// eye detect init
	mem_size = FaceDetect_Config(edWorkMem->eye_detect_WorkMem, mem_size, edWorkMem->face_img.width, edWorkMem->face_img.height, 
								edWorkMem->face_img.ch, OBJDETECT_MAX_RESULT, 2, 2);
	
	eye_detect_init(edWorkMem);
	
	// post process register
#if MOUTH_NOSE_DETECT_EN == 1	
	edWorkMem->post_proc.flag       = POST_PROC_ENABLE;
	edWorkMem->post_proc.alloc      = mouth_detect_alloc;
	edWorkMem->post_proc.run        = mouth_detect_proc;
	edWorkMem->post_proc.free       = mouth_detect_free;
	edWorkMem->post_proc.get_result = mouth_detect_get_result;
	edWorkMem->post_proc.reset      = mouth_detect_reset;
#else
	edWorkMem->post_proc.flag       = POST_PROC_DISABLE;
#endif
	
	// post process allocate  
	if(edWorkMem->post_proc.flag == POST_PROC_ENABLE) {
		edWorkMem->post_proc.workmem = (void *)edWorkMem->post_proc.alloc();
		if(edWorkMem->post_proc.workmem == 0) {
			RETURN(STATUS_FAIL);
		}
	}
	
	face_recognize_state = 0;
	nRet = STATUS_OK;
Return:
	if(nRet < 0) {
		if(edWorkMem) {
			eye_detect_free(edWorkMem);
		}
		return 0;
	}
	
	return (INT32S)edWorkMem;
}

void eye_detect_free(void *ed_work_mem)
{
	EyeDetect_t *edWorkMem = (EyeDetect_t *)ed_work_mem;

	if(edWorkMem) {
		if(edWorkMem->face_img.ptr)  {
			gp_free(edWorkMem->face_img.ptr);
			edWorkMem->face_img.ptr = 0;
		}
		
		if(edWorkMem->eq_face_img.ptr) {
			gp_free(edWorkMem->eq_face_img.ptr);
			edWorkMem->eq_face_img.ptr = 0;
		}

		if(edWorkMem->eye_detect_WorkMem) {
			gp_free(edWorkMem->eye_detect_WorkMem);
			edWorkMem->eye_detect_WorkMem = 0;
		}
			
		if(edWorkMem->post_proc.flag == POST_PROC_ENABLE) {
			edWorkMem->post_proc.free(edWorkMem->post_proc.workmem);
		}
		
		gp_free(edWorkMem);
		edWorkMem = 0;
	}

}

void eye_detect_proc(void *ed_work_mem, gpImage *org_img, void *pre_result)
{
	ObjResult_t *p_obj_result = (ObjResult_t *)pre_result;
	EyeDetect_t *edWorkMem = (EyeDetect_t *)ed_work_mem;
	gpPoint pos;
	gpRect RectEye;
	float scale_factor;
	INT32S max_wnd, k;
	INT32S eye_nbr;		
	
	edWorkMem->is_get_eye = 0;	
	if(p_obj_result->result_cnt <= 0) {
		goto Return;
	}
	
	// set information and detect first face 
	edWorkMem->fe_result.face.x = p_obj_result->rect[0].x;
	edWorkMem->fe_result.face.y = p_obj_result->rect[0].y;
	edWorkMem->fe_result.face.width = p_obj_result->rect[0].width;
	edWorkMem->fe_result.face.height = (INT32S)(p_obj_result->rect[0].height*1.2);
	if(edWorkMem->fe_result.face.width < (RESIZE_FACE_IMG_WIDTH/2)) {
		eye_nbr = 1;
	} else {
		eye_nbr = 2;
	}
		
	scalerClip(org_img, &edWorkMem->face_img, &edWorkMem->fe_result.face);

	scale_factor = (float)p_obj_result->rect[0].width / (float)edWorkMem->face_img.width;
	
	RectEye.x = HAAR_X_MARGIN;
	RectEye.y = HAAR_Y_MARGIN;
	RectEye.width = RESIZE_FACE_IMG_WIDTH / 2;
	RectEye.height = RESIZE_FACE_IMG_HEIGHT / 2;

	max_wnd = MIN(MIN(RectEye.width, RectEye.height), 36);

	FaceDetect_SetScale(edWorkMem->eye_detect_WorkMem, edWorkMem->scale, 24, max_wnd,SCALE_NONLINEAR);

	// wait for scaler ending
	scalerEnd();

	for(k=0; k<2; k++) {
		INT32S N, i, max_idx, max_cnt;
		gpPoint *p_eye;
		gpRect *p_eyebrow;
		INT32S do_eq, pre_cnt;

		if(k == 0)  {
			edWorkMem->clf.obj_type = OBJ_LEYE;
			p_eye = &edWorkMem->fe_result.left_eye;
			p_eyebrow = &edWorkMem->fe_result.left_eyebrow;
		} else {
			edWorkMem->clf.obj_type = OBJ_REYE;
			p_eye = &edWorkMem->fe_result.right_eye;
			p_eyebrow = &edWorkMem->fe_result.right_eyebrow;
			RectEye.x = RESIZE_FACE_IMG_WIDTH / 2 - HAAR_X_MARGIN;
		}
		
		FaceDetect_set_detect_obj(edWorkMem->eye_detect_WorkMem, &edWorkMem->clf);
		
		//R_IOA_O_DATA |= (1 << 8);
		pre_cnt = 0;
		do_eq = 0;
		do {
			if(do_eq == 0) {
				N = FaceDetect(edWorkMem->eye_detect_WorkMem, &edWorkMem->face_img, &RectEye, 
							OBJDETECT_MAX_RESULT, edWorkMem->eye_rect, edWorkMem->eye_cnt);
			} else {
				// do EQ, and search again...					
				ModEqualizeHist_fix(&edWorkMem->face_img, &edWorkMem->eq_face_img, &RectEye);
				N = FaceDetect(edWorkMem->eye_detect_WorkMem, &edWorkMem->eq_face_img, &RectEye, 
							OBJDETECT_MAX_RESULT, edWorkMem->eye_rect, edWorkMem->eye_cnt);
							
				// do gamma						
				if(N <= 0) {	
					GammaCorrection(&edWorkMem->eq_face_img, &edWorkMem->eq_face_img, &RectEye);
					N = FaceDetect(edWorkMem->eye_detect_WorkMem, &edWorkMem->eq_face_img, &RectEye, OBJDETECT_MAX_RESULT, edWorkMem->eye_rect, edWorkMem->eye_cnt);			
				}
				
				if(N > 0) {
					eye_nbr = 1;
				}
			}
			
			// find the best eye
			max_idx = 0;
			max_cnt = 0;
			for(i=0; i<N; i++) {
				if(edWorkMem->eye_cnt[i] > max_cnt) {
					max_cnt = edWorkMem->eye_cnt[i];
					max_idx = i;
				}
			}

			// assume that there is eye but it's not easy detected!!!
			max_cnt += pre_cnt;
			if(max_cnt < eye_nbr) {
				if(do_eq == 1) {
					goto Return;
				} else {
					do_eq = 1;
				}
				
				pre_cnt = max_cnt;
			} else { 
				do_eq = 0;
			}			
		} while(do_eq == 1);
		//R_IOA_O_DATA &= ~(1 << 8);
		
		if(edWorkMem->eye_rect[max_idx].width > EYES_WIDTH) {
			INT32S val = (edWorkMem->eye_rect[max_idx].width - EYES_WIDTH + 1) / 2;
			
			edWorkMem->eye_rect[max_idx].x += val;
			edWorkMem->eye_rect[max_idx].width -= val * 2;
		
			val = ((edWorkMem->eye_rect[max_idx].height - EYES_WIDTH) * 5 + 7) / 8;
			edWorkMem->eye_rect[max_idx].y += val;
			edWorkMem->eye_rect[max_idx].height = edWorkMem->eye_rect[max_idx].width;
		}
		
		p_eye->x = edWorkMem->eye_rect[max_idx].x + (edWorkMem->eye_rect[max_idx].width >> 1);
		p_eye->y = edWorkMem->eye_rect[max_idx].y + (INT32S)(edWorkMem->eye_rect[max_idx].height * 0.6 + 0.5);

		p_eyebrow->x = edWorkMem->eye_rect[max_idx].x + 3;
		p_eyebrow->y = edWorkMem->eye_rect[max_idx].y + (INT32S)(edWorkMem->eye_rect[max_idx].height * 0.16 + 0.5);
		p_eyebrow->width = edWorkMem->eye_rect[max_idx].width - 6;
		p_eyebrow->height = 2;		
	}

	edWorkMem->is_get_eye = 1;
Return:	
	p_obj_result->is_get_eye = edWorkMem->is_get_eye;
	p_obj_result->is_get_nose = 0;
	p_obj_result->is_get_mouth = 0;
	if(edWorkMem->is_get_eye) {
		// push eye result to obj result
		pos.x = edWorkMem->fe_result.face.x;
		pos.y = edWorkMem->fe_result.face.y;
        
		// map coordinate of eye to org image
		p_obj_result->eye[0].x = (int)(edWorkMem->fe_result.left_eye.x * scale_factor + 0.5) + pos.x;
		p_obj_result->eye[0].y = (int)(edWorkMem->fe_result.left_eye.y * scale_factor + 0.5) + pos.y;
		p_obj_result->eye[0].width= 4;
		p_obj_result->eye[0].height= 4;
			
		p_obj_result->eye[1].x = (int)(edWorkMem->fe_result.right_eye.x * scale_factor + 0.5) + pos.x;
		p_obj_result->eye[1].y = (int)(edWorkMem->fe_result.right_eye.y * scale_factor + 0.5) + pos.y;
		p_obj_result->eye[1].width= 4;
		p_obj_result->eye[1].height= 4;	
		
		p_obj_result->eyebrow[0].x = (int)(edWorkMem->fe_result.left_eyebrow.x * scale_factor + 0.5) + pos.x;
		p_obj_result->eyebrow[0].y = (int)(edWorkMem->fe_result.left_eyebrow.y * scale_factor + 0.5) + pos.y;
		p_obj_result->eyebrow[0].width = (int)(edWorkMem->fe_result.left_eyebrow.width * scale_factor + 0.5);
		p_obj_result->eyebrow[0].height= 2;
		
		p_obj_result->eyebrow[1].x = (int)(edWorkMem->fe_result.right_eyebrow.x * scale_factor + 0.5) + pos.x;
		p_obj_result->eyebrow[1].y = (int)(edWorkMem->fe_result.right_eyebrow.y * scale_factor + 0.5) + pos.y;
		p_obj_result->eyebrow[1].width= (int)(edWorkMem->fe_result.right_eyebrow.width * scale_factor + 0.5);
		p_obj_result->eyebrow[1].height= 2;		
	}
	
	if(edWorkMem->is_get_eye == 0) {
		edWorkMem->no_eye_cnt++;
		if(edWorkMem->no_eye_cnt >= 3) {
			edWorkMem->no_eye_cnt = 3;
			obj_result.is_smile = 0;
			edWorkMem->post_proc.reset(edWorkMem->post_proc.workmem);
		}
	} else {
		edWorkMem->no_eye_cnt = 0;
		if(edWorkMem->fe_result.left_eye.y == edWorkMem->fe_result.right_eye.y) {
			edWorkMem->fe_result.eye_angle = 0;
		} else {
			edWorkMem->fe_result.eye_angle = atan2((edWorkMem->fe_result.right_eye.y - edWorkMem->fe_result.left_eye.y), 
												(edWorkMem->fe_result.right_eye.x - edWorkMem->fe_result.left_eye.x));
		}
		
		// mouth detection	
		if(edWorkMem->post_proc.flag == POST_PROC_ENABLE) {
			INT32S i, x, y;
			float angle;
			double x_dif, y_dif;
			gpPoint *leye;
			gpRect *r;
				 
			edWorkMem->post_proc.run(edWorkMem->post_proc.workmem, &edWorkMem->face_img, &edWorkMem->fe_result);			
			
			//get mouth, noise and smile 	
			edWorkMem->post_proc.get_result(edWorkMem->post_proc.workmem, p_obj_result);
			leye = &edWorkMem->fe_result.left_eye;
			angle = -edWorkMem->fe_result.eye_angle;

			//nose 
			if(p_obj_result->is_get_nose) {
				for(i=0; i<2; i++) {
					r = &p_obj_result->nose[i];
					x_dif = (double)(r->x - leye->x);
					y_dif = (double)(r->y - leye->y);
					x = (INT32S)(x_dif * cos(angle) + y_dif * sin(angle) + 0.5) + leye->x;
					y = (INT32S)(y_dif * cos(angle) - x_dif * sin(angle) + 0.5) + leye->y;
					r->x = (x * scale_factor + 0.5) + pos.x;
					r->y = (y * scale_factor + 0.5) + pos.y;
				}
			}
			
			//mouth
			if(p_obj_result->is_get_mouth) {
				for(i=0; i<4; i++) {
					r = &p_obj_result->mouth[i];
					x_dif = (double)(r->x - leye->x);
					y_dif = (double)(r->y - leye->y);
					x = (INT32S)(x_dif * cos(angle) + y_dif * sin(angle) + 0.5) + leye->x;
					y = (INT32S)(y_dif * cos(angle) - x_dif * sin(angle) + 0.5) + leye->y;
					r->x = (x * scale_factor + 0.5) + pos.x;
					r->y = (y * scale_factor + 0.5) + pos.y;
				}
			}
		}
	}	
}

void eye_detect_get_result(void *ed_work_mem, void *result)
{

}
#endif

////////////////////////////////////////////////////////////////
// face identify
////////////////////////////////////////////////////////////////
#if FACE_RECOGNIZE_EN == 1
#define FD_TRAINING_IMAGE_NUMBER		20
#define NO_FACEID_MAX_CNT				5
static INT32U *idWorkMem_ptr;
static INT32U fd_idWorkMem_get(void)
{
	return (INT32U)idWorkMem_ptr;
}

static INT32S face_recognize_load(FaceIdentify_t *idWorkMem)
{
#if 0
	INT16S fd;
	INT32S ret;
	
	chdir("C:\\");
	fd = open("Face_info.bin", O_RDONLY);
	if(fd < 0) {
		return -1;
	} 
	
	ret = read(fd, (INT32U)idWorkMem->ownerULBP, SZ_ULBP*20);
	close(fd);
		
	if(ret != SZ_ULBP*20) {
		return -1;
	}
	
	return 0;
#else
	return -1;
#endif
}

INT32S face_recognize_init(FaceIdentify_t *idWorkMem)
{
	face_recognize_set_security_level(3);
	return 0;
}

void face_recognize_reset(void *para)
{
	//face_recognize_init((FaceIdentify_t *)para);
}

INT32S face_recognize_alloc(void)
{
	FaceIdentify_t *idWorkMem;
	INT32U mem_size;
	INT32S nRet;
	
	idWorkMem = gp_malloc_align(sizeof(FaceIdentify_t), 32);
	if(idWorkMem == 0) {
		DEBUG_MSG("Fail to allocate edWorkMem\r\n");
		return 0;
	}
	DEBUG_MSG("idWorkMem = 0x%x\r\n", idWorkMem);
	idWorkMem_ptr = (INT32U *)idWorkMem;
	gp_memset((INT8S *)idWorkMem, 0, sizeof(FaceIdentify_t));
	
	idWorkMem->image.width     = OBJDETECT_IMG_WIDTH;
	idWorkMem->image.height    = OBJDETECT_IMG_HEIGHT;
	idWorkMem->image.ch        = 2;
	idWorkMem->image.widthStep = OBJDETECT_IMG_WIDTH;
	idWorkMem->image.format    = IMG_FMT_GRAY;
	
	mem_size = FaceIdentify_MemCalc();
	idWorkMem->identify_WorkMem = (INT32U *)gp_malloc_align((mem_size), 4); 
	if(idWorkMem->identify_WorkMem == 0) {
		DEBUG_MSG("Fail to allocate idWorkMem->identify_WorkMem\r\n");
		RETURN(STATUS_FAIL);
	}
	gp_memset((INT8S *)idWorkMem->identify_WorkMem, 0, mem_size);
	
	mem_size = SZ_ULBP*(FD_TRAINING_IMAGE_NUMBER + 1);
	idWorkMem->ownerULBP = (INT32U *)gp_malloc_align(mem_size, 4);
	if(idWorkMem->ownerULBP == 0) {
		DEBUG_MSG("Fail to allocate idWorkMem->ownerULBP\r\n");
		RETURN(STATUS_FAIL);
	}
	gp_memset((INT8S *)idWorkMem->ownerULBP, 0, mem_size);
	
	//initial
	face_recognize_init(idWorkMem);
	if(face_recognize_load(idWorkMem) < 0) {
		idWorkMem->id_mode = 1; //training mode
	} else {
		idWorkMem->id_mode = 2; //identify mode
	}
	
	face_recognize_state = 1;
	nRet = STATUS_OK;
Return:
	if(nRet < 0) {
		face_recognize_free(idWorkMem);
		return 0;
	}
	return (INT32S)idWorkMem;
}

void face_recognize_free(void *id_work_mem)
{
	FaceIdentify_t *idWorkMem = (FaceIdentify_t *)id_work_mem;
	
	if(idWorkMem) {
		if(idWorkMem->identify_WorkMem) {
			gp_free((void *)idWorkMem->identify_WorkMem);
			idWorkMem->identify_WorkMem = 0;
		}
		
		if(idWorkMem->ownerULBP) {
			gp_free((void*)idWorkMem->ownerULBP);
			idWorkMem->ownerULBP = 0;
		}
		
		if(idWorkMem->eye_detect_WorkMem)
		{
			eye_detect_free(idWorkMem->eye_detect_WorkMem);
			idWorkMem->eye_detect_WorkMem = 0;
		}
				
		gp_free((void *)idWorkMem);
		idWorkMem = 0;
	}
		
	face_recognize_state = 0;
}

void face_recognize_proc(void *id_work_mem, gpImage *org_img, void *pre_result)
{
	ObjResult_t *p_obj_result = (ObjResult_t *)pre_result;
	FaceIdentify_t *idWorkMem = (FaceIdentify_t *)id_work_mem;
	INT32S ret,identify_score[FD_TRAINING_IMAGE_NUMBER];	
	INT32U identify_index;
	gpRect image[3];

	if(p_obj_result->is_best_face && (idWorkMem->id_mode == 1 || idWorkMem->id_mode == 2))
	{
		// face ROI
		image[0] = p_obj_result->rect[FACE_ROI];
		// leye	ROI
		image[1] = p_obj_result->rect[LEYE_ROI];
		// reye ROI
		image[2] = p_obj_result->rect[REYE_ROI];		
	}	

	if(p_obj_result->is_best_face == 0) {
		//no face
		idWorkMem->identify_cnt = 0;
		return;
	} else if(idWorkMem->id_mode == 0) {
		// idle 
		idWorkMem->training_cnt = 0;
		idWorkMem->identify_cnt = 0;
		return;
	} else if(idWorkMem->id_mode == 1) {
		// traning	
		FaceIdentify_Train(org_img, 
						&image[0], 
						idWorkMem->ownerULBP, 
						idWorkMem->training_cnt, 
						idWorkMem->identify_WorkMem);															
		idWorkMem->training_cnt++;
		DEBUG_MSG("training_cnt = %d\r\n", idWorkMem->training_cnt);
		if(idWorkMem->training_cnt >= FD_TRAINING_IMAGE_NUMBER) {
			DEBUG_MSG("Training Success\r\n");
			idWorkMem->id_mode = 0; //idle mode
			idWorkMem->training_cnt = 0;
			idWorkMem->identify_cnt = 0;
			idWorkMem->training_state = 1;
			obj_result.training_ok = 1;
			if(idWorkMem->pFuncStore) {
				idWorkMem->pFuncStore((INT32U)idWorkMem->ownerULBP, SZ_ULBP*(FD_TRAINING_IMAGE_NUMBER + 1));
			}
		} 			
	} else if(idWorkMem->id_mode == 2) {		
		// identify	
		ret = FaceIdentify_Verify(org_img, 
							&image[0], 
							idWorkMem->ownerULBP, 
							idWorkMem->security_value, 
							idWorkMem->identify_WorkMem,
							FD_TRAINING_IMAGE_NUMBER,
							(int *)&identify_index,							
						    (int *)&identify_score[0]);				
		
		idWorkMem->recognize_score = identify_score[0];
		#if FD_DEBUG_MSG_EN == 1
			DBG_PRINT("Face_Identify_Index = %d\r\n", (FD_TRAINING_IMAGE_NUMBER - identify_index));
			DBG_PRINT("Face_Identify_Score = %d\r\n", identify_score[0]);		
		#endif
		
		if(ret == 0) {
			DEBUG_MSG("NoID\r\n");
			return;
		}
			
		idWorkMem->identify_cnt++;
		DEBUG_MSG("identify_cnt = %d\r\n", idWorkMem->identify_cnt);
		if(idWorkMem->identify_cnt >= 2) {
			DEBUG_MSG("Identify Success\r\n");
			idWorkMem->id_mode = 0; //idle mode
			idWorkMem->training_cnt = 0;
			idWorkMem->identify_cnt = 0;
			idWorkMem->identify_state = 1;
			obj_result.identify_ok = 1;
		}
	} else {
		while(1);
	}
}

void face_recognize_get_result(void *id_work_mem, void *result)
{
	
}

void face_recognize_set_security_level(INT8U value)
{
	INT32U temp;
	FaceIdentify_t *idWorkMem = (FaceIdentify_t *)fd_idWorkMem_get();
	
	idWorkMem->security_level = value;
	
	switch(idWorkMem->security_level)
	{
		case 1:
			temp = 75112;
			break;
			
		case 2:
			temp = 90112;
			break;
			
		case 4:
			temp = 106112;
			break;
			
		case 5:
			temp = 128612;
			break;
			
		default:
			temp = 93112;
			break;		  	        	          	          	
	}
	
	idWorkMem->security_value = temp;  
}

INT32U face_recognize_get_buffer(void)
{	
	ObjDetect_t *odWorkMem = obj_detect_get_workmem();
	FaceIdentify_t *idWorkMem = odWorkMem->post_proc.workmem;
	return (INT32U)idWorkMem->ownerULBP;
}

INT32U face_recognize_get_training_cnt(void)
{
	ObjDetect_t *odWorkMem = obj_detect_get_workmem();
	FaceIdentify_t *idWorkMem = odWorkMem->post_proc.workmem;	
	return idWorkMem->training_cnt;
}

void face_recognize_set_mode(INT32U mode)
{
	ObjDetect_t *odWorkMem = obj_detect_get_workmem();
	FaceIdentify_t *idWorkMem = odWorkMem->post_proc.workmem;
	
	if(mode == 1) { 
		//training
		DBG_PRINT("training\r\n");
		idWorkMem->id_mode = 1; 
		idWorkMem->training_cnt = 0;
		idWorkMem->training_state = 0;
		idWorkMem->identify_cnt = 0;
		idWorkMem->identify_state = 0;
		obj_result.training_ok = 0;
		obj_result.identify_ok = 0;
	} else if(mode == 2) { 
		//identify
		DBG_PRINT("recognize\r\n");
		idWorkMem->id_mode = 2; 
		idWorkMem->identify_cnt = 0;
		idWorkMem->identify_state = 0;
		obj_result.identify_ok = 0;
	} else {
		//not do any thing
		DBG_PRINT("idle\r\n");
		idWorkMem->id_mode = 0; 
	}
}

INT32U face_recognize_get_mode(INT32U mode)
{
	INT32U temp;
	ObjDetect_t *odWorkMem = obj_detect_get_workmem();
	FaceIdentify_t *idWorkMem = odWorkMem->post_proc.workmem;
	
	switch(mode)
	{
		case FD_TRAINING_STATE_GET:
			temp = idWorkMem->training_state;
			break;
		
		case FD_IDENTIFY_STATE_GET:
			temp = idWorkMem->identify_state;
			break;
						
		case FD_SECURITY_LEVEL_GET:
			temp = idWorkMem->security_level;	
			break;	
		
		case FD_TRAINING_CNT_GET:
			if(idWorkMem->id_mode == 1 || idWorkMem->training_state)
			{
				if(idWorkMem->training_state)
					temp = FD_TRAINING_IMAGE_NUMBER;
				else
					temp = idWorkMem->training_cnt;	
			}
			else
				temp = 0;
			break;
		
		case FD_IDENTIFY_SCORE_GET:
			temp = idWorkMem->recognize_score;	
			break;					
		
		default:		
			temp = idWorkMem->id_mode;
			break;					
	}
	
	return temp;
}

void face_recognize_register_store_func(INT32S (*pFuncStore)(INT32U, INT32U))
{
	ObjDetect_t *odWorkMem = obj_detect_get_workmem();
	FaceIdentify_t *idWorkMem = odWorkMem->post_proc.workmem;
	
	idWorkMem->pFuncStore = pFuncStore;
}
#endif

////////////////////////////////////////////////////////////////
// face detect
////////////////////////////////////////////////////////////////
#if FACE_RECOGNIZE_EN == 1
#define INT_MAX							2147483647
INT32S face_detect_init(ObjDetect_t *odWorkMem)
{
	INT32U mem_size;
	gpImage *image;
	
	image = &odWorkMem->image;
	mem_size = FaceDetect_Config(0, 0, image->width, image->height, 
								image->ch, OBJDETECT_MAX_RESULT, odWorkMem->xstep, odWorkMem->ystep);
								
	odWorkMem->obj_detect_WorkMem = gp_malloc_align(mem_size, 16);
	if(odWorkMem->obj_detect_WorkMem == 0) {
		DEBUG_MSG("Fail to allocate obj_detect_WorkMem\r\n");
		return -1;
	}
	gp_memset((INT8S*)odWorkMem->obj_detect_WorkMem, 0x00, mem_size);
	odWorkMem->WorkMemSize = mem_size;
	mem_size = FaceDetect_Config(odWorkMem->obj_detect_WorkMem, mem_size, image->width, image->height, 
								image->ch, OBJDETECT_MAX_RESULT, odWorkMem->xstep, odWorkMem->ystep);
	
	FaceDetect_set_ScalerFn(odWorkMem->obj_detect_WorkMem, scalerStart, scalerEnd, scalerClip); 
	
	// setting cascade type 	
	FaceDetect_set_detect_obj(odWorkMem->obj_detect_WorkMem, &odWorkMem->clf);	
	
	return 0;
}

INT32S face_detect_alloc(INT32S obj_type, 
					INT32S post_flag, 
					void(*post_proc_reset)(void*para),
					INT32S (*post_proc_alloc)(void), 
					void (*post_proc_free)(void *para), 
					void (*post_proc_run)(void *para, gpImage *org_img, void *pre_result) , 
					void(*post_proc_get_result)(void *para, void *result))
{
	INT32S nRet;
	ObjDetect_t *odWorkMem;
	
	odWorkMem = gp_malloc_align(sizeof(ObjDetect_t), 16);
	if(odWorkMem == 0) {
		DEBUG_MSG("Fail to allocate odWorkMem\r\n");
		return 0;
	}
	DEBUG_MSG("odWorkMem = 0x%x\r\n", odWorkMem);
	gp_memset((INT8S*)odWorkMem, 0, sizeof(ObjDetect_t));
	
	// image info setting
	odWorkMem->image.width     = OBJDETECT_IMG_WIDTH;
	odWorkMem->image.height    = OBJDETECT_IMG_HEIGHT;
	odWorkMem->image.ch        = 1;
	odWorkMem->image.widthStep = OBJDETECT_IMG_WIDTH;
	odWorkMem->image.format    = IMG_FMT_GRAY;

	//post proc setting
	odWorkMem->post_proc.flag  = post_flag;
	odWorkMem->post_proc.run   = post_proc_run;
	odWorkMem->post_proc.alloc = post_proc_alloc;
	odWorkMem->post_proc.free  = post_proc_free;
	odWorkMem->post_proc.reset = post_proc_reset;
	odWorkMem->post_proc.get_result = post_proc_get_result;
	
	// setting cascade type 
	odWorkMem->clf.obj_type 		 = obj_type;
	odWorkMem->clf.data              = NULL;
	odWorkMem->clf.stage_of_classify = NULL;
	odWorkMem->clf.num_of_classify   = NULL;
	
	//scan step and scale size
	odWorkMem->xstep        		 = 2;
	odWorkMem->ystep        		 = 2;
	odWorkMem->scale                 = (INT32S)(1.1*65536);
	
	// allocate memory & initial
	if(face_detect_init(odWorkMem) < 0) {
		RETURN(STATUS_FAIL);
	}

	if(odWorkMem->post_proc.flag == POST_PROC_ENABLE) {
		odWorkMem->post_proc.workmem = (void*) odWorkMem->post_proc.alloc();
		if(odWorkMem->post_proc.workmem == 0) {
			RETURN(STATUS_FAIL);
		}
	}
	
	nRet = STATUS_OK;
Return:
	if(nRet < 0) {
		face_detect_free((void *)odWorkMem);
		return 0;
	}
	
	return (INT32S)odWorkMem;
}

void face_detect_free(ObjDetect_t *odWorkMem)
{
	// free memory
	if(odWorkMem) {
	    if(odWorkMem->obj_detect_WorkMem) {
			gp_free(odWorkMem->obj_detect_WorkMem);
			odWorkMem->obj_detect_WorkMem = 0;
		}
		
		if(odWorkMem->post_proc.flag == POST_PROC_ENABLE) {
			odWorkMem->post_proc.free(odWorkMem->post_proc.workmem);
		}

		gp_free(odWorkMem);
		odWorkMem = 0;
	}
}

static gpRect GPRect_utils(int _x, int _y, int _width, int _height)
{
	gpRect rect;
	
	rect.x = _x;
	rect.y = _y;
	rect.width = _width;
	rect.height = _height;
	return rect;
}

static int faceRoiDetect(ObjDetect_t *odWorkMem,const gpImage* gray, gpRect* userROI, int mode)
{	
	#define MAX_RESULT 		OBJDETECT_MAX_RESULT	
	FaceIdentify_t *idWorkMem = odWorkMem->post_proc.workmem;
	// detect type setting
	ClassifyData clfface;
	ClassifyData clfreye;
	ClassifyData clfleye;
	// best face reye and leye
	gpRect* faceROI = &userROI[FACE_ROI];
	gpRect* lEyeROI = &userROI[LEYE_ROI];
	gpRect* rEyeROI = &userROI[REYE_ROI];
	gpRect* best_faceROI = &userROI[BESTFACE_ROI];
	
	// detect result and count
	gpRect Rect,rFace,lFace;
	gpRect faceResult[MAX_RESULT];
	gpRect rEyeResult[MAX_RESULT];
	gpRect lEyeResult[MAX_RESULT];
	int faceCount[MAX_RESULT];
	int rEyeCount[MAX_RESULT];
	int lEyeCount[MAX_RESULT];
	// Initialization //
	int i, maxFaceW, maxFaceCount, best_face, offset_x, offset_y, min_eye_wnd;
	int ret, faceN, rEyeN, lEyeN, t, int_scale_face, int_scale_eye, detectH;	
	// xstep
	int xstep_face = 2;
	int xstep_eye = 1;
	// ystep
	int ystep_face = 2;
	int ystep_eye = 2;	
	// face nbr
	int min_face_nbr_h = 5;
	int min_face_nbr_l = 2;
	// eye nbr
	int min_eye_nbr = 1;
	// min face window
	int min_face_wnd = 50;
	// sacler max window
	int max_wnd = MIN(gray->width, gray->height);	
		
	Rect.x = 0;
	Rect.y = 0;
	Rect.width = gray->width;
	Rect.height = gray->height;
	
	int_scale_face = 72089; // 1.1
	int_scale_eye = 72089; // 1.1, 65535 = 1
	
	t = MIN(Rect.width, Rect.height);
	if(max_wnd>t) max_wnd = t;

	//--------------------
	//	Face Detection
	//--------------------
	ret = FaceDetect_Config(odWorkMem->obj_detect_WorkMem, odWorkMem->WorkMemSize, Rect.width, Rect.height, 1, MAX_RESULT, xstep_face, ystep_face);

	/* setting cascade type (face) */
	clfface.obj_type = OBJ_FACE;			
	FaceDetect_set_detect_obj(odWorkMem->obj_detect_WorkMem, &clfface);

	FaceDetect_set_ScalerFn(odWorkMem->obj_detect_WorkMem, scalerStart, scalerEnd, scalerClip);  
	ret = FaceDetect_SetScale(odWorkMem->obj_detect_WorkMem, int_scale_face, min_face_wnd, max_wnd, mode);

	faceN = FaceDetect(odWorkMem->obj_detect_WorkMem, gray, &Rect, MAX_RESULT, faceResult, faceCount);

	if (!faceN)
	{	
		return 0;
	}
	
	maxFaceW = 0;
	maxFaceCount = min_face_nbr_l;
	best_face = 0;
	i = faceN-1;
	
	do
	{
		if (faceCount[i] >= min_face_nbr_l)
		{			
			if ((maxFaceCount >= min_face_nbr_h) && (faceCount[i] >= min_face_nbr_h))
			{
				if (faceResult[i].width > maxFaceW)
				{
					maxFaceW = faceResult[i].width;
					maxFaceCount = faceCount[i];
					best_face = i;
				}
			}
			else
			{
				if (faceCount[i] >= maxFaceCount)
				{
					maxFaceW = faceResult[i].width;
					maxFaceCount = faceCount[i];
					best_face = i;
				}
			}
		}
	} while (i--);
		
	if(!maxFaceW)	
	{
		return 0;
	}
		
	best_faceROI->x = faceResult[best_face].x;
	best_faceROI->y = faceResult[best_face].y;
	best_faceROI->width = faceResult[best_face].width;	
	best_faceROI->height = faceResult[best_face].height;
	
	/* Face Position Determination */
	offset_x = faceResult[best_face].width/7;	
	offset_y = faceResult[best_face].height/7;
	
	faceROI->x = faceResult[best_face].x + offset_x;
	faceROI->y = faceResult[best_face].y + offset_y;
	faceROI->width = (short)(faceResult[best_face].width*0.68);	
	faceROI->height = (short)(faceResult[best_face].height - offset_y*1.5);
	
	//--------------------
	//	Eyes Detection
	//--------------------
	min_eye_wnd = MAX(faceROI->width/7, 24); // 24 is minimum
	
	//Eyes detect range
	detectH = (int)(faceResult[best_face].height*0.6);
	ret = FaceDetect_Config(odWorkMem->obj_detect_WorkMem, odWorkMem->WorkMemSize, (faceResult[best_face].width>>1), detectH, 1, MAX_RESULT, xstep_eye, ystep_eye);

	// setting cascade type (right eye) //	
	clfreye.obj_type = OBJ_REYE;			
	FaceDetect_set_detect_obj(odWorkMem->obj_detect_WorkMem, &clfreye);

	FaceDetect_set_ScalerFn(odWorkMem->obj_detect_WorkMem, scalerStart, scalerEnd, scalerClip); 
	ret = FaceDetect_SetScale(odWorkMem->obj_detect_WorkMem, int_scale_eye, min_eye_wnd, max_wnd, mode);

	rFace = GPRect_utils(faceResult[best_face].x + (faceResult[best_face].width>>1), faceResult[best_face].y, (faceResult[best_face].width>>1), detectH);
	rEyeN = FaceDetect(odWorkMem->obj_detect_WorkMem, gray, &rFace, MAX_RESULT, rEyeResult, rEyeCount);

	// setting cascade type (left eye) //
	clfleye.obj_type = OBJ_LEYE;			
	FaceDetect_set_detect_obj(odWorkMem->obj_detect_WorkMem, &clfleye);

	FaceDetect_set_ScalerFn(odWorkMem->obj_detect_WorkMem, scalerStart, scalerEnd, scalerClip); 
	ret = FaceDetect_SetScale(odWorkMem->obj_detect_WorkMem, int_scale_eye, min_eye_wnd, max_wnd, mode);

	lFace = GPRect_utils(faceResult[best_face].x, faceResult[best_face].y, (faceResult[best_face].width>>1), detectH);
	lEyeN = FaceDetect(odWorkMem->obj_detect_WorkMem, gray, &lFace, MAX_RESULT, lEyeResult, lEyeCount);

	if(!rEyeN)
	{
		rEyeROI->width = 0;
		return 0;
	}
	else
	{
		int minEyeDist = INT_MAX;
		int maxEyeCount = min_eye_nbr - 1;
		int most_possible_eye = 0;

		int i = rEyeN - 1;
		do
		{
			if (rEyeCount[i] > maxEyeCount)
			{				
				maxEyeCount = rEyeCount[i];
				most_possible_eye = i;
				minEyeDist = ABS(rEyeResult[i].x + rEyeResult[i].width/2 - faceROI->x - faceROI->width*3/4) + ABS(rEyeResult[i].y + rEyeResult[i].height/2 - faceROI->y - faceROI->height/4);
			}
			else if (rEyeCount[i] == maxEyeCount)
			{
				int evaluateDist
					= ABS(rEyeResult[i].x + rEyeResult[i].width/2 - faceROI->x - faceROI->width*3/4) + ABS(rEyeResult[i].y + rEyeResult[i].height/2 - faceROI->y - faceROI->height/4);

				if (evaluateDist < minEyeDist)
				{	
					minEyeDist = evaluateDist;
					most_possible_eye = i;
				}
			}
		} while (i--);

		// Face Position Determination //
		*rEyeROI = rEyeResult[most_possible_eye];
	}
				
	if(!lEyeN)
	{
		lEyeROI->width = 0;
		return 0;
	}
	else
	{
		int minEyeDist = INT_MAX;
		int maxEyeCount = min_eye_nbr - 1;
		int most_possible_eye = 0;

		int i = lEyeN - 1;
		do
		{
			if (lEyeCount[i] > maxEyeCount)			
			{				
				maxEyeCount = lEyeCount[i];
				most_possible_eye = i;
				minEyeDist = ABS(lEyeResult[i].x + lEyeResult[i].width/2 - faceROI->x - faceROI->width/4) + ABS(lEyeResult[i].y + lEyeResult[i].height/2 - faceROI->y - faceROI->height/4);
			}
			else if (lEyeCount[i] == maxEyeCount)
			{
				int evaluateDist
					= ABS(lEyeResult[i].x + lEyeResult[i].width/2 - faceROI->x - faceROI->width/4) + ABS(lEyeResult[i].y + lEyeResult[i].height/2 - faceROI->y - faceROI->height/4);

				if (evaluateDist < minEyeDist)
				{	
					minEyeDist = evaluateDist;
					most_possible_eye = i;
				}
			}
		} while (i--);


		// Face Position Determination //
		*lEyeROI = lEyeResult[most_possible_eye];
	}
	
	if(idWorkMem->id_mode)
	{	
	#if FD_DEBUG_MSG_EN == 1
		DBG_PRINT("\r\n****************************ROI******************************\r\n");
		DBG_PRINT("best_face.count = %d\r\n", faceCount[best_face]);
		DBG_PRINT("best_face.x = %d\r\n", faceResult[best_face].x);
		DBG_PRINT("best_face.y = %d\r\n", faceResult[best_face].y);
		DBG_PRINT("best_face.width = %d\r\n", faceResult[best_face].width);
		DBG_PRINT("best_face.height = %d\r\n", faceResult[best_face].height);	
		DBG_PRINT("faceROI.x = %d\r\n", faceROI->x);
		DBG_PRINT("faceROI.y = %d\r\n", faceROI->y);
		DBG_PRINT("faceROI.width = %d\r\n", faceROI->width);
		DBG_PRINT("faceROI.height = %d\r\n", faceROI->height);		
		DBG_PRINT("lEyeROI.x = %d\r\n", lEyeROI->x);
		DBG_PRINT("lEyeROI.y = %d\r\n", lEyeROI->y);
		DBG_PRINT("lEyeROI.width = %d\r\n", lEyeROI->width);
		DBG_PRINT("lEyeROI.height = %d\r\n", lEyeROI->height);
		DBG_PRINT("rEyeROI.x = %d\r\n", rEyeROI->x);
		DBG_PRINT("rEyeROI.y = %d\r\n", rEyeROI->y);
		DBG_PRINT("rEyeROI.width = %d\r\n", rEyeROI->width);
		DBG_PRINT("rEyeROI.height = %d\r\n", rEyeROI->height);		
		DBG_PRINT("*****************************************************************\r\n");			
	#endif	
	}	
	if(((lEyeROI->y + lEyeROI->height/2) < faceROI->y) || ((rEyeROI->y + rEyeROI->height/2) < faceROI->y))
		return 0;
	else
		return 1;	
}

void face_detect_proc(ObjDetect_t *odWorkMem)
{
	INT32S obj_cnt;	    

	if(faceRoiDetect(odWorkMem, (gpImage *)&odWorkMem->image, (gpRect *)&obj_result.rect[0],SCALE_NONLINEAR))
	{
		obj_cnt = 1;
		odWorkMem->no_face_cnt = 0;
	}
	else
	{
		obj_cnt = 0;
		odWorkMem->no_face_cnt++;
	}
	
	obj_result.is_best_face = obj_cnt; 
	obj_result.result_cnt = obj_cnt;
	
	// post process
	if(odWorkMem->post_proc.flag == POST_PROC_ENABLE && obj_result.is_best_face) {
		odWorkMem->post_proc.run(odWorkMem->post_proc.workmem, &odWorkMem->image, &obj_result);
	} else {
		obj_result.is_smile = 0;
		obj_result.is_get_eye= 0;	
	}
	
	#if FD_DEBUG_MSG_EN == 1
		obj_result.is_get_eye = obj_result.is_best_face;
		obj_result.eye[0].x = obj_result.rect[LEYE_ROI].x;
		obj_result.eye[0].y = obj_result.rect[LEYE_ROI].y;
		obj_result.eye[0].width = obj_result.rect[LEYE_ROI].width;	
		obj_result.eye[0].height = obj_result.rect[LEYE_ROI].height;
		obj_result.eye[1].x = obj_result.rect[REYE_ROI].x;
		obj_result.eye[1].y = obj_result.rect[REYE_ROI].y;
		obj_result.eye[1].width = obj_result.rect[REYE_ROI].width;	
		obj_result.eye[1].height = obj_result.rect[REYE_ROI].height;					
	#else
		obj_result.is_get_eye = 0;
		obj_result.rect[FACE_ROI].x = obj_result.rect[BESTFACE_ROI].x;
		obj_result.rect[FACE_ROI].y = obj_result.rect[BESTFACE_ROI].y;
		obj_result.rect[FACE_ROI].width = obj_result.rect[BESTFACE_ROI].width;	
		obj_result.rect[FACE_ROI].height = obj_result.rect[BESTFACE_ROI].height;
	#endif
	
	// set sensor ae
	if(obj_cnt > 0) {	
		sensor_control_ae();
	}

	if(odWorkMem->no_face_cnt >=16) {
		odWorkMem->no_face_cnt = 16;	
		sensor_control_ae();
	}
}
#endif
////////////////////////////////////////////////////////////////
// gesture detect
////////////////////////////////////////////////////////////////
#if GESTURE_DETECT_EN == 1
void gesture_detect_proc(ObjDetect_t *odWorkMem)
{	
	INT32S i, N, nCnt;	
	INT32S *p_count;					
	INT32S *p_min_width, *p_xstep, *p_scale;
	gpRect *p_range;
	gpRect *p_result;
	INT32S search_cnt, min_width, max_result_t;
	INT32S xstep, scale;
	INT32S best_face = -1;
	INT32S obj_cnt = 0;
	INT32U maxFaceW = 0;
	INT32U maxFaceCount = OBJDETECT_MIN_NBR;
	gpRect *p_obj_rect = obj_result.rect;
	gpRect *p_best = &obj_result.best_face;
	
	//get tracking info
	p_min_width = (int *)track_get_search_min_width(odWorkMem->obj_track_WorkMem);
	p_range = (gpRect *)track_get_search_range(odWorkMem->obj_track_WorkMem);
	search_cnt = 1;//track_get_search_cnt(odWorkMem->obj_track_WorkMem);
	min_width = track_get_min_wnd(odWorkMem->obj_track_WorkMem);			
	i = track_get_max_range(odWorkMem->obj_track_WorkMem);
	p_xstep = track_get_xstep(odWorkMem->obj_track_WorkMem);
	p_scale = track_get_scale(odWorkMem->obj_track_WorkMem);
	
	p_result = odWorkMem->result;
	max_result_t = OBJDETECT_MAX_RESULT;
	p_count = odWorkMem->count;
	nCnt = 0;
	i = search_cnt;
	do {
		INT32S cnt;
		INT32S min_width;
		
		min_width = *p_min_width++;
		scale = *p_scale++;
		FaceDetect_SetScale(odWorkMem->obj_detect_WorkMem, scale, min_width, odWorkMem->image.height,SCALE_NONLINEAR);

		xstep = *p_xstep++;
		FaceDetect_SetX(odWorkMem->obj_detect_WorkMem, xstep, 0);

		for(N=0; N<3; N++) {
			switch(odWorkMem->gesture_mode)
			{
			case 0: //N -> R -> L
				if(N == 0) {
					odWorkMem->clf.obj_type = OBJ_HAND;
					odWorkMem->gesture_mode = 0;
				} else if(N == 1) {
					odWorkMem->clf.obj_type = OBJ_HAND_RT45;
					odWorkMem->gesture_mode = 1;
				} else if(N == 2) {
					odWorkMem->clf.obj_type = OBJ_HAND_LT45;
					odWorkMem->gesture_mode = 2;
				}
				break;
				
			case 1: //R -> N -> L
				if(N == 0) {
					odWorkMem->clf.obj_type = OBJ_HAND_RT45;
					odWorkMem->gesture_mode = 1;
				} else if(N == 1) {
					odWorkMem->clf.obj_type = OBJ_HAND;
					odWorkMem->gesture_mode = 0;
				} else if(N == 2) {
					odWorkMem->clf.obj_type = OBJ_HAND_LT45;
					odWorkMem->gesture_mode = 2;
				}
				break;
				
			case 2: //L -> N -> R 
				if(N == 0) {
					odWorkMem->clf.obj_type = OBJ_HAND_LT45;
					odWorkMem->gesture_mode = 2;
				} else if(N == 1) {
					odWorkMem->clf.obj_type = OBJ_HAND;
					odWorkMem->gesture_mode = 0;
				} else if(N == 2) {
					odWorkMem->clf.obj_type = OBJ_HAND_RT45;
					odWorkMem->gesture_mode = 1;
				}
				break;
			}
			
			FaceDetect_set_detect_obj(odWorkMem->obj_detect_WorkMem, &odWorkMem->clf);
			cnt = FaceDetect(odWorkMem->obj_detect_WorkMem, &odWorkMem->image, p_range, max_result_t, p_result, p_count);
			if(p_count[0] >= OBJDETECT_MIN_NBR) {
				goto __Find;
			} 
		}	
		
__Find:		
		max_result_t -= cnt;
		p_result += cnt;
		p_count += cnt;
		nCnt += cnt;
		p_range++;
		i--;
	} while(i != 0);
	
	// get result
	for (i=0; i<nCnt; i++) {
		if(odWorkMem->count[i] >= OBJDETECT_MIN_NBR) {
			// set obj_result
			p_obj_rect->x = odWorkMem->result[i].x;
			p_obj_rect->y = odWorkMem->result[i].y;
			p_obj_rect->width = odWorkMem->result[i].width;
			p_obj_rect->height = odWorkMem->result[i].height;
			p_obj_rect++;
			obj_cnt++;
		
			// get best face
			if(odWorkMem->count[i] >= OBJDETECT_MIN_fist_NBR) {
				if(odWorkMem->result[i].width > maxFaceW) {
					maxFaceW = odWorkMem->result[i].width;
					maxFaceCount = odWorkMem->count[i];
					best_face = i;
				} else if(odWorkMem->count[i] > maxFaceCount) {
					maxFaceW = odWorkMem->result[i].width;
					maxFaceCount = odWorkMem->count[i];
					best_face = i;
				} 
			}
		}
	}
	
	obj_result.result_cnt = obj_cnt;	
	if(best_face >= 0) {
		p_best->x = odWorkMem->result[best_face].x;
		p_best->y = odWorkMem->result[best_face].y;
		p_best->width = odWorkMem->result[best_face].width;
		p_best->height = odWorkMem->result[best_face].height;
		obj_result.is_best_face = 1;
	} else {
		p_best->x = 0;
		p_best->y = 0;
		p_best->width = 0;
		p_best->height = 0;
		obj_result.is_best_face = 0;
	}
	
	//DEBUG_MSG("Face = %d, best_no = %d\r\n", obj_result.result_cnt, best_face);	
	// set tracking 
	if(obj_result.result_cnt > 0) {
		odWorkMem->no_face_cnt = 0;
		track_reset_face_location(odWorkMem->obj_track_WorkMem);
		for(i=0; i<obj_result.result_cnt; i++) {
			track_set_face_location(odWorkMem->obj_track_WorkMem, &obj_result.rect[i], i);
		}
	} else {
		odWorkMem->no_face_cnt++;
	}
	
	// tracking
	track_run(odWorkMem->obj_track_WorkMem, obj_result.result_cnt);
	
	// set sensor ae
	if(obj_cnt > 0) {
		obj_result.is_hand = 1;	
		sensor_control_ae();
	} else {
		obj_result.is_hand = 0;
	}

	if(odWorkMem->no_face_cnt >=16) {
		odWorkMem->no_face_cnt = 16;	
		sensor_control_ae();
	}
}
#endif

////////////////////////////////////////////////////////////////
// object detect
////////////////////////////////////////////////////////////////
INT32S obj_detect_init(ObjDetect_t *odWorkMem)
{
	INT32U mem_size;
	gpImage *image;
	
	image = &odWorkMem->image;
	mem_size = FaceDetect_Config(0, 0, image->width, image->height, 
								image->ch, OBJDETECT_MAX_RESULT, odWorkMem->xstep, 2);
								
	odWorkMem->obj_detect_WorkMem = gp_malloc_align(mem_size, 16);
	if(odWorkMem->obj_detect_WorkMem == 0) {
		DEBUG_MSG("Fail to allocate obj_detect_WorkMem\r\n");
		return -1;
	}
	gp_memset((INT8S*)odWorkMem->obj_detect_WorkMem, 0x00, mem_size);
	
	mem_size = FaceDetect_Config(odWorkMem->obj_detect_WorkMem, mem_size, image->width, image->height, 
								image->ch, OBJDETECT_MAX_RESULT, odWorkMem->xstep, 2);
	
	FaceDetect_set_ScalerFn(odWorkMem->obj_detect_WorkMem, scalerStart, scalerEnd, scalerClip); 
	
	// setting cascade type 	
	FaceDetect_set_detect_obj(odWorkMem->obj_detect_WorkMem, &odWorkMem->clf);	
	
	return 0;
}

INT32S obj_detect_alloc(INT32S obj_type, 
					INT32S post_flag, 
					void(*post_proc_reset)(void*para),
					INT32S (*post_proc_alloc)(void), 
					void (*post_proc_free)(void *para), 
					void (*post_proc_run)(void *para, gpImage *org_img, void *pre_result) , 
					void(*post_proc_get_result)(void *para, void *result))
{
	INT32S nRet;
	ObjDetect_t *odWorkMem;
	
	odWorkMem = gp_malloc_align(sizeof(ObjDetect_t), 16);
	if(odWorkMem == 0) {
		DEBUG_MSG("Fail to allocate odWorkMem\r\n");
		return 0;
	}
	DEBUG_MSG("odWorkMem = 0x%x\r\n", odWorkMem);
	gp_memset((INT8S*)odWorkMem, 0, sizeof(ObjDetect_t));
	
	// image info setting
	odWorkMem->image.width     = OBJDETECT_IMG_WIDTH;
	odWorkMem->image.height    = OBJDETECT_IMG_HEIGHT;
	odWorkMem->image.ch        = 1;
	odWorkMem->image.widthStep = OBJDETECT_IMG_WIDTH;
	odWorkMem->image.format    = IMG_FMT_GRAY;

	//post proc setting
	odWorkMem->post_proc.flag  = post_flag;
	odWorkMem->post_proc.run   = post_proc_run;
	odWorkMem->post_proc.alloc = post_proc_alloc;
	odWorkMem->post_proc.free  = post_proc_free;
	odWorkMem->post_proc.reset = post_proc_reset;
	odWorkMem->post_proc.get_result = post_proc_get_result;
	
	// setting cascade type 
	odWorkMem->clf.obj_type 		 = obj_type;
	odWorkMem->clf.data              = NULL;
	odWorkMem->clf.stage_of_classify = NULL;
	odWorkMem->clf.num_of_classify   = NULL;
	
	//scan step and scale size
	odWorkMem->xstep        		 = 2;  			//ystep always = 2
	odWorkMem->scale                 = (INT32S)(1.1*65536 + 0.5);
	
	// allocate memory & initial
	if(obj_detect_init(odWorkMem) < 0) {
		RETURN(STATUS_FAIL);
	}

	if(obj_track_init(odWorkMem) < 0) {
		RETURN(-1);
	}

	if(odWorkMem->post_proc.flag == POST_PROC_ENABLE) {
		odWorkMem->post_proc.workmem = (void*) odWorkMem->post_proc.alloc();
		if(odWorkMem->post_proc.workmem == 0) {
			RETURN(STATUS_FAIL);
		}
	}
	
	nRet = STATUS_OK;
Return:
	if(nRet < 0) {
		obj_detect_free((void *)odWorkMem);
		return 0;
	}
	
	return (INT32S)odWorkMem;
}

void obj_detect_free(ObjDetect_t *odWorkMem)
{
	// free memory
	if(odWorkMem) {
	    if(odWorkMem->obj_detect_WorkMem) {
			gp_free(odWorkMem->obj_detect_WorkMem);
			odWorkMem->obj_detect_WorkMem = 0;
		}
		
		if(odWorkMem->obj_track_WorkMem) {
			gp_free(odWorkMem->obj_track_WorkMem);
			odWorkMem->obj_track_WorkMem = 0;
		}

		if(odWorkMem->post_proc.flag == POST_PROC_ENABLE) {
			odWorkMem->post_proc.free(odWorkMem->post_proc.workmem);
		}

		gp_free(odWorkMem);
		odWorkMem = 0;
	}
}

void obj_detect_proc(ObjDetect_t *odWorkMem)
{	    
	INT32S i, nCnt;	
	INT32S *p_count;					
	INT32S *p_min_width, *p_xstep, *p_scale;
	gpRect *p_range;
	gpRect *p_result;
	INT32S search_cnt, min_width, max_result_t;
	INT32S xstep, scale;
	INT32S best_face = -1;
	INT32S obj_cnt = 0;
	INT32U maxFaceW = 0;
	INT32U maxFaceCount = OBJDETECT_MIN_NBR;
	gpRect *p_obj_rect = obj_result.rect;
	gpRect *p_best = &obj_result.best_face;

	//get tracking info
	p_min_width = (int *)track_get_search_min_width(odWorkMem->obj_track_WorkMem);
	p_range = (gpRect *)track_get_search_range(odWorkMem->obj_track_WorkMem);
	search_cnt = track_get_search_cnt(odWorkMem->obj_track_WorkMem);
	min_width = track_get_min_wnd(odWorkMem->obj_track_WorkMem);			
	i = track_get_max_range(odWorkMem->obj_track_WorkMem);
	p_xstep = track_get_xstep(odWorkMem->obj_track_WorkMem);
	p_scale = track_get_scale(odWorkMem->obj_track_WorkMem);
	
	//set face detect object type
	FaceDetect_set_detect_obj(odWorkMem->obj_detect_WorkMem, &odWorkMem->clf);

	p_result = odWorkMem->result;
	max_result_t = OBJDETECT_MAX_RESULT;
	p_count = odWorkMem->count;
	nCnt = 0;
	i = search_cnt;
	do {
		INT32S cnt;
		INT32S min_width;
		
		min_width = *p_min_width++;
		scale = *p_scale++;
		FaceDetect_SetScale(odWorkMem->obj_detect_WorkMem, scale, min_width, odWorkMem->image.height,SCALE_NONLINEAR);
			
		xstep = *p_xstep++;
		FaceDetect_SetX(odWorkMem->obj_detect_WorkMem, xstep, 0);

		//R_IOA8_O_DATA |= 1 << 8;
		cnt = FaceDetect(odWorkMem->obj_detect_WorkMem, &odWorkMem->image, p_range, max_result_t, p_result, p_count);	
		//R_IOA8_O_DATA &= ~(1 << 8);
		
		max_result_t -= cnt;
		p_result += cnt;
		p_count += cnt;
		nCnt += cnt;
		p_range++;
		i--;
	} while(i > 0);
	
	// get result
	for (i=0; i<nCnt; i++) {
		if(odWorkMem->count[i] >= OBJDETECT_MIN_NBR) {
			// set obj_result
			p_obj_rect->x = odWorkMem->result[i].x;
			p_obj_rect->y = odWorkMem->result[i].y;
			p_obj_rect->width = odWorkMem->result[i].width;
			p_obj_rect->height = odWorkMem->result[i].height;
			p_obj_rect++;
			obj_cnt++;
		
			// get best face
			if(odWorkMem->count[i] >= OBJDETECT_MIN_fist_NBR) {
				if(odWorkMem->result[i].width > maxFaceW) {
					maxFaceW = odWorkMem->result[i].width;
					maxFaceCount = odWorkMem->count[i];
					best_face = i;
				} else if(odWorkMem->count[i] > maxFaceCount) {
					maxFaceW = odWorkMem->result[i].width;
					maxFaceCount = odWorkMem->count[i];
					best_face = i;
				} 
			}
		}
	}
	
	obj_result.result_cnt = obj_cnt;	
	if(best_face >= 0) {
		p_best->x = odWorkMem->result[best_face].x;
		p_best->y = odWorkMem->result[best_face].y;
		p_best->width = odWorkMem->result[best_face].width;
		p_best->height = odWorkMem->result[best_face].height;
		obj_result.is_best_face = 1;
	} else {
		p_best->x = 0;
		p_best->y = 0;
		p_best->width = 0;
		p_best->height = 0;
		obj_result.is_best_face = 0;
	}
	
	//DEBUG_MSG("Face = %d, best_no = %d\r\n", obj_result.result_cnt, best_face);
	// set tracking 
	if(obj_result.result_cnt > 0) {
		odWorkMem->no_face_cnt = 0;
		track_reset_face_location(odWorkMem->obj_track_WorkMem);
		for (i = 0; i <obj_result.result_cnt; i++) {
			track_set_face_location(odWorkMem->obj_track_WorkMem, &obj_result.rect[i], i);
		}
	} else {
		odWorkMem->no_face_cnt++;
	}
	
	// tracking
	track_run(odWorkMem->obj_track_WorkMem, obj_result.result_cnt);

	// post process
	if(odWorkMem->post_proc.flag == POST_PROC_ENABLE) {
		odWorkMem->post_proc.run(odWorkMem->post_proc.workmem, &odWorkMem->image, &obj_result);
		// only for mouth detection
		if(odWorkMem->no_face_cnt >= 16) {
			odWorkMem->no_face_cnt = 16;
			obj_result.is_smile = 0;
			obj_result.is_get_eye = 0;
			odWorkMem->post_proc.reset(odWorkMem->post_proc.workmem);
		}
	} else {
		obj_result.is_smile = 0;
		obj_result.is_get_eye= 0;	
	}

	// set sensor ae
	if(obj_cnt > 0) {	
		sensor_control_ae();
	}

	if(odWorkMem->no_face_cnt >=16) {
		odWorkMem->no_face_cnt = 16;	
		sensor_control_ae();
	}
}

void obj_detect_get_result(void *od_work_mem, void *objresult)
{
	INT32S i;
	ObjDetResult_t *result = (ObjDetResult_t *)objresult;
	
	if(obj_result.result_cnt <= 0) {
		result->ResultCnt = 0;
		result->EyeFlag = 0;
		result->NoseFlag = 0;
		result->MouthFlag = 0;
		result->HandFlag = 0;
		result->SmileFlag = 0;
		return;
	}
	 
	result->ResultCnt = obj_result.result_cnt;
	for(i=0; i<result->ResultCnt; i++) {
		result->Result[i].x = obj_result.rect[i].x;
		result->Result[i].y = obj_result.rect[i].y;
		result->Result[i].width = obj_result.rect[i].width;
		result->Result[i].height = obj_result.rect[i].height;
	}
	
	result->EyeFlag = obj_result.is_get_eye;
	if(result->EyeFlag) {
		for(i=0; i<2; i++) {
			result->Eye[i].x = obj_result.eye[i].x;
			result->Eye[i].y = obj_result.eye[i].y;
			result->Eye[i].width = obj_result.eye[i].width;
			result->Eye[i].height = obj_result.eye[i].height;
			if(face_recognize_state == 0)
			{
				result->Eyebrow[i].x = obj_result.eyebrow[i].x;
				result->Eyebrow[i].y = obj_result.eyebrow[i].y;
				result->Eyebrow[i].width = obj_result.eyebrow[i].width;
				result->Eyebrow[i].height = obj_result.eyebrow[i].height;
			}	
		}
	}
	
	result->NoseFlag = obj_result.is_get_nose;
	if(result->NoseFlag) {
		for(i=0; i<2; i++) {
			result->Nose[i].x = obj_result.nose[i].x;
			result->Nose[i].y = obj_result.nose[i].y;
			result->Nose[i].width = obj_result.nose[i].width;
			result->Nose[i].height = obj_result.nose[i].height;
		}
	}
	
	result->MouthFlag = (INT8U)obj_result.is_get_mouth;
	if(result->MouthFlag) {
		for(i=0; i<4; i++) {
			result->Mouth[i].x = obj_result.mouth[i].x;
			result->Mouth[i].y = obj_result.mouth[i].y;
			result->Mouth[i].width = obj_result.mouth[i].width;
			result->Mouth[i].height = obj_result.mouth[i].height;
		}
	}
	
	result->SmileFlag = obj_result.is_smile;
	result->FaceTrainingFlag = obj_result.training_ok;
	result->FaceIdentifyFlag = obj_result.identify_ok;
	result->HandFlag = obj_result.is_hand;
	result->FistFlag = obj_result.is_fist;
	
	// clear flag
	gp_memset((INT8S *)&obj_result, 0x00, sizeof(obj_result));
}

void obj_detect_set_table(void *data, INT32S stage, INT32S num)
{
	ObjDetect_t *odWorkMem = obj_detect_get_workmem();
		
	odWorkMem->clf.data              = data;
	odWorkMem->clf.stage_of_classify = stage;
	odWorkMem->clf.num_of_classify   = num;
}

////////////////////////////////////////////////////////////////
// RFMS detect
////////////////////////////////////////////////////////////////
#if OBJECT_RECOGNIZE_EN == 1
typedef struct {
	INT32U ImageSaveNumber;
	INT32U CardsN;
	INT32U ObjCardsN;
	INT32U ImageSaveBufferBase;
	INT32U ObjectState;
	INT32U ObjWorkMem;
	INT32U ObjSmoothBuffer;
} ObjectIdentify_t;

#define TRAINING_IMAGE_SAVE_MAX_NUMBER        				15
#define RETRY_FIND_EXTRACT_POINT							5
#define TRAINING_IMAGE_SAVE_SIZE         					(RFMSDETECT_IMG_WIDTH*RFMSDETECT_IMG_HEIGHT)
static ObjectIdentify_t object_training_set;
static ObjectIdentify_t *object_training_set_ptr;
static INT32S obj_recognize_load_mode(ObjDetect_t *odWorkMem);

INT32U obj_recognize_get_state(INT32U mode)
{
	INT32U result;
	ObjDetect_t *odWorkMem = (ObjDetect_t *)object_training_set_ptr->ObjWorkMem;
	RfmsIdentify_t *idWorkMem = odWorkMem->rfms_detect_WorkMem;
	
	switch(mode)
	{
		case 1:
			result = object_training_set_ptr->ObjectState;
			break;
		
		case 2:
			result = object_training_set_ptr->ObjCardsN;
			break;
						
		default:
			if(object_training_set_ptr->ObjectState == 2)
				result = idWorkMem->object_training_cnt;
			else if(object_training_set_ptr->ObjectState == 3)
				result = idWorkMem->matchingCardNum;
			else
	        	result = object_training_set_ptr->ImageSaveNumber;
	        break;
	}
	
	return result;	
}

static INT32S object_training_buffer_malloc(void)
{
    object_training_set_ptr = &object_training_set;
    gp_memset((INT8S *)object_training_set_ptr,0,sizeof(ObjectIdentify_t));	
	
	object_training_set_ptr->ImageSaveBufferBase = (INT32U) gp_malloc_align(((TRAINING_IMAGE_SAVE_MAX_NUMBER+1)*TRAINING_IMAGE_SAVE_SIZE),32);	
	if(!object_training_set_ptr->ImageSaveBufferBase)
	 	return 0;
	else
	{
		gp_memset((INT8S *)object_training_set_ptr->ImageSaveBufferBase,0,(TRAINING_IMAGE_SAVE_MAX_NUMBER+1)*TRAINING_IMAGE_SAVE_SIZE);	
		DEBUG_MSG("bin_save_buffer_base = 0x%x\r\n", object_training_set_ptr->ImageSaveBufferBase); 
	}
	object_training_set_ptr->ObjSmoothBuffer = (object_training_set_ptr->ImageSaveBufferBase + (TRAINING_IMAGE_SAVE_MAX_NUMBER*TRAINING_IMAGE_SAVE_SIZE));
	gp_memset((INT8S *)object_training_set_ptr->ObjSmoothBuffer,0,(object_training_set_ptr->ImageSaveBufferBase + ((TRAINING_IMAGE_SAVE_MAX_NUMBER+1)*TRAINING_IMAGE_SAVE_SIZE)));	
	DEBUG_MSG("obj_smooth_buf = 0x%x\r\n", object_training_set_ptr->ObjSmoothBuffer); 
	
	return object_training_set_ptr->ImageSaveBufferBase;		
}

static INT32S object_training_buffer_free(void)
{
	if(object_training_set_ptr->ImageSaveBufferBase)
	{
		gp_free((void *)object_training_set_ptr->ImageSaveBufferBase);
		object_training_set_ptr->ImageSaveBufferBase = 0;
		DEBUG_MSG("object_training_buffer_free OK\r\n"); 	
	}

	return 0;		
}

static INT32S image_save_buffer_copy(INT32U inbuf,INT32U number)
{
	INT8S done1;
	INT32S result;
    DMA_STRUCT dma_struct;
	
	if((object_training_set_ptr->ImageSaveBufferBase > 0) && (number < TRAINING_IMAGE_SAVE_MAX_NUMBER))
	{
		result = 0;
		done1 = C_DMA_STATUS_WAITING;	
		dma_struct.s_addr = (INT32U)inbuf;
		dma_struct.t_addr = (INT32U)(object_training_set_ptr->ImageSaveBufferBase+(number*TRAINING_IMAGE_SAVE_SIZE));	
		DEBUG_MSG("t_addr = 0x%x\r\n",dma_struct.t_addr);
		dma_struct.width = DMA_DATA_WIDTH_1BYTE;
		dma_struct.count = TRAINING_IMAGE_SAVE_SIZE;
		dma_struct.notify = &done1;
		dma_struct.timeout = 0;
		result=dma_transfer_wait_ready(&dma_struct);

		return result;
	}
	else
		return -1;
}

static INT32S object_training_bin_2_sd(INT32U image,INT32U bin_number)
{
	char path[128];
	INT16S fd;
	INT32S temp;
		
	sprintf((char *)path, (const char *)"objtraining%02d.bin",bin_number);
    temp = 0;
    fd = open(path, O_CREAT|O_RDWR);
    if(fd>=0)
    {
		temp = write(fd, (INT32U)object_training_set_ptr->ImageSaveBufferBase, (image*TRAINING_IMAGE_SAVE_SIZE));
		if(temp < 0)
			DBG_PRINT("objtraining bin error\r\n");
		else
			DBG_PRINT("objtraining bin end\r\n");			
		close(fd);
	}
	else
		DBG_PRINT("bin file open error\r\n");
	
	sprintf((char *)path, (const char *)"image.bin");
    temp = 0;
    fd = open(path, O_CREAT|O_RDWR);
    if(fd>=0)
    {
		temp = write(fd, (INT32U)&object_training_set_ptr->CardsN, sizeof(INT32U));
		if(temp < 0)
			DBG_PRINT("image bin error\r\n");
		else
			DBG_PRINT("image bin end\r\n");			
		close(fd);
	}
	else
		DBG_PRINT("bin file open error\r\n");		

	return temp;
}

void obj_recognize_set_mode(INT32U mode)
{
	ObjDetect_t *odWorkMem = (ObjDetect_t *)object_training_set_ptr->ObjWorkMem;
	RfmsIdentify_t *idWorkMem = odWorkMem->rfms_detect_WorkMem;
	
	if(mode == 1) { 
		//training
		DBG_PRINT("obj save\r\n");
		idWorkMem->id_mode = 1; 
		idWorkMem->cardsN = 0;
		object_training_set_ptr->ObjectState = 1;
	} else if(mode == 2) { 
		//identify
		DBG_PRINT("obj training\r\n");
		idWorkMem->id_mode = 2;
		idWorkMem->object_training_cnt = 0;
		idWorkMem->object_match_cnt = 0;
		idWorkMem->cardsN = object_training_set_ptr->ImageSaveNumber;
		object_training_set_ptr->CardsN = object_training_set_ptr->ImageSaveNumber;
		object_training_set_ptr->ObjectState = 2;					
	} else if(mode == 3) { 
		//identify
		if(obj_recognize_load_mode(odWorkMem))
		{
			DBG_PRINT("obj idle\r\n");
			idWorkMem->id_mode = 0;
			object_training_set_ptr->ObjectState = 0;
		}
		else
		{
			DBG_PRINT("obj recognize\r\n");
			idWorkMem->id_mode = 3;	
			object_training_set_ptr->ObjectState = 3;
		}			 
	} else {
		//not do any thing
		DBG_PRINT("obj idle\r\n");
		idWorkMem->id_mode = 0;
		idWorkMem->object_training_cnt = object_training_set_ptr->ImageSaveNumber;
		object_training_set_ptr->ImageSaveNumber = 0;
		object_training_set_ptr->ObjectState = 0; 
	}
}

void obj_recognize_set_matchingThre_level(INT8U value)
{
	ObjDetect_t *odWorkMem = (ObjDetect_t *)object_training_set_ptr->ObjWorkMem;
	RfmsIdentify_t *idWorkMem = odWorkMem->rfms_detect_WorkMem;

	if(value)
		idWorkMem->matchingThre += 500;
	else
		idWorkMem->matchingThre -= 500;
	
	DBG_PRINT("matchingThre = %d \r\n",idWorkMem->matchingThre);
	 
	RFMS_ParamSet(idWorkMem->rfms_WorkMem,
				300, 
				idWorkMem->matchingThre, 
				idWorkMem->matchingThreRangeV, 
				idWorkMem->minExtractionThre, 
				idWorkMem->incExtractionThre, 
				idWorkMem->decExtractionThre, 
				idWorkMem->startMatchingPointN);			
}

static INT32U obj_recognize_load_data(INT32S *card_number)
{
	INT16S fd;
	INT32S nRet, size;
	INT32U data_buf = 0;

	//chdir("c:\\");
	fd = open("objDatabase.bin", O_RDONLY);
	if(fd < 0) {
		RETURN(STATUS_FAIL);
	}
	
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	
	data_buf = (INT32U)gp_malloc_align(size, 16);
	if(data_buf == 0) {
		RETURN(STATUS_FAIL);
	} 
	
	DEBUG_MSG("data_buf = 0x%x\r\n", data_buf); 
	nRet = read(fd, data_buf, size);
	close(fd);
	if(nRet != size) {
		RETURN(STATUS_FAIL);
	}
	if(object_training_set_ptr->CardsN)
		*card_number = object_training_set_ptr->CardsN;//6;
	else
	{
		fd = open("image.bin", O_RDONLY);
		if(fd < 0) {
			RETURN(STATUS_FAIL);
		}
		size = sizeof(INT32U);	
		nRet = read(fd, (INT32U)&object_training_set_ptr->CardsN, size);
		close(fd);	
		if(nRet != size) {
			RETURN(STATUS_FAIL);
		}		
		*card_number = object_training_set_ptr->CardsN;//6;
	}
	DEBUG_MSG("card_number = %d\r\n", *card_number); 	
Return:
	if(nRet < 0) {
		close(fd);
		if(data_buf) {
			gp_free((void *)data_buf);
		}
		return 0;
	}

	return data_buf;	
}

static INT32S obj_recognize_load_mode(ObjDetect_t *odWorkMem)
{
	//INT32U mem_size;
	RfmsIdentify_t *rfms;

	rfms = (RfmsIdentify_t *)odWorkMem->rfms_detect_WorkMem;
	
	// load data buffer
	if(rfms->obj_data_buf)
	{
		gp_free((void *)rfms->obj_data_buf);
		rfms->obj_data_buf = 0;		
	}
	
	rfms->obj_data_buf = (CHAR *)obj_recognize_load_data(&rfms->cardsN);
	if(rfms->obj_data_buf == 0) {
		return -1;
	}
    
	// rfms init
	RFMS_init(rfms->image.width, 
			rfms->image.height, 
			rfms->rfms_WorkMem, 
			rfms->image.width, 
			rfms->image.height, 
			rfms->cardsN, 
			rfms->obj_data_buf); 		 
		
	RFMS_ParamSet(rfms->rfms_WorkMem,
				300, 
				rfms->matchingThre, 
				rfms->matchingThreRangeV, 
				rfms->minExtractionThre, 
				rfms->incExtractionThre, 
				rfms->decExtractionThre, 
				rfms->startMatchingPointN);

	return 0;					
}

INT32S obj_recognize_init(ObjDetect_t *odWorkMem)
{
	INT32S nRet;
	INT32U mem_size;
	RfmsIdentify_t *rfms;
	
	// rmfs variable alloc
	mem_size = sizeof(RfmsIdentify_t);
	odWorkMem->rfms_detect_WorkMem = gp_malloc_align(mem_size, 16);
	if(odWorkMem->rfms_detect_WorkMem == 0) {
		DEBUG_MSG("Fail to allocate odWorkMem->rfms_detect_WorkMem\r\n");
		return -1;
	}
	DEBUG_MSG("rfms_detect_WorkMem = 0x%x\r\n", odWorkMem->rfms_detect_WorkMem); 
	rfms = (RfmsIdentify_t *)odWorkMem->rfms_detect_WorkMem;
	gp_memset((INT8S*)rfms, 0x00, mem_size);
	
	#if 0 //gp modification
		// load data buffer
		rfms->obj_data_buf = (CHAR *)obj_recognize_load_data(&rfms->cardsN);
		if(rfms->obj_data_buf == 0) {
			RETURN(STATUS_FAIL);
		}
	#endif
	
	// rmfs workmem alloc
	mem_size = RFMS_get_memory_size(odWorkMem->image.width, odWorkMem->image.height); 
	rfms->rfms_WorkMem = gp_malloc_align(mem_size, 16);
	if(rfms->rfms_WorkMem == 0) {
		DEBUG_MSG("Fail to allocate rfms->rfms_WorkMem\r\n");
		RETURN(STATUS_FAIL);
	}
	DEBUG_MSG("rfms_WorkMem = 0x%x\r\n", rfms->rfms_WorkMem);  
	gp_memset((INT8S*)rfms->rfms_WorkMem, 0x00, mem_size);
	
	rfms->image.width = odWorkMem->image.width;
	rfms->image.height = odWorkMem->image.height;
	rfms->image.widthStep = odWorkMem->image.widthStep;
	rfms->image.ch = odWorkMem->image.ch;
	rfms->image.format = odWorkMem->image.format;
	
	rfms->matchingThre = 23000;
	rfms->matchingThreRangeV = 2000;
	rfms->minExtractionThre = 100;
	rfms->incExtractionThre = 50;
	rfms->decExtractionThre = 100;
	rfms->startMatchingPointN = 70;

#if 0	
	// rfms init
	RFMS_init(rfms->image.width, 
			rfms->image.height, 
			rfms->rfms_WorkMem, 
			odWorkMem->image.width, 
			odWorkMem->image.height, 
			rfms->cardsN, 
			rfms->obj_data_buf);  
			
	RFMS_ParamSet(rfms->rfms_WorkMem,
				300, 
				rfms->matchingThre, 
				rfms->matchingThreRangeV, 
				rfms->minExtractionThre, 
				rfms->incExtractionThre, 
				rfms->decExtractionThre, 
				rfms->startMatchingPointN);
#endif	
	
	nRet = STATUS_OK;
Return:
	if(nRet < 0) {
		obj_recognize_free(odWorkMem);
	}	
	
	return nRet;
}

INT32S obj_recognize_alloc(void)
{
	INT32S nRet;
	ObjDetect_t *odWorkMem;
	
	nRet = object_training_buffer_malloc();
	if(nRet < 0)
	{
		DEBUG_MSG("Fail to allocate object_training_buffer\r\n");
		return 0;	
	}
	
	odWorkMem = gp_malloc_align(sizeof(ObjDetect_t), 16);
	if(odWorkMem == 0) {
		DEBUG_MSG("Fail to allocate odWorkMem\r\n");
		return 0;
	}
	
	DEBUG_MSG("odWorkMem = 0x%x\r\n", odWorkMem);
	gp_memset((INT8S*)odWorkMem, 0, sizeof(ObjDetect_t));
	object_training_set_ptr->ObjWorkMem = (INT32S)odWorkMem;
	
	// image info setting
	odWorkMem->image.width     = RFMSDETECT_IMG_WIDTH;
	odWorkMem->image.height    = RFMSDETECT_IMG_HEIGHT;
	odWorkMem->image.ch        = 1;
	odWorkMem->image.widthStep = RFMSDETECT_IMG_WIDTH;
	odWorkMem->image.format    = IMG_FMT_GRAY;
	
	//post proc setting
	odWorkMem->post_proc.flag  = POST_PROC_DISABLE;
	odWorkMem->post_proc.run   = 0;
	odWorkMem->post_proc.alloc = 0;
	odWorkMem->post_proc.free  = 0;
	odWorkMem->post_proc.reset = 0;
	odWorkMem->post_proc.get_result = 0;
	
	// setting cascade type 
	odWorkMem->clf.obj_type 		 = 0xFF;
	odWorkMem->clf.data              = NULL;
	odWorkMem->clf.stage_of_classify = NULL;
	odWorkMem->clf.num_of_classify   = NULL;
	
	if(obj_recognize_init(odWorkMem) < 0) {
		RETURN(-1);
	}
	
	nRet = STATUS_OK;
Return:
	if(nRet < 0) {
		obj_recognize_free((void *)odWorkMem);
		return 0;
	}
	
	return (INT32S)odWorkMem;
}

static void saveRFInformation(int* workmem, RFMSPoint_INT* keypoints, RFMSDescriptor* descriptors, unsigned short fpNum, int frameNum)
{
	char path[128];
	unsigned short RF_ptr[1];
	int size;
	char *objDataBuf;
	unsigned int tmpBytes;
	int i,j,temp = 0;
	globalData *gData_ptr = (globalData *)workmem;
	INT16S fd;
	INT32S num;	
	
	RF_ptr[0] = fpNum;
	if(frameNum == 1)
	{
		gData_ptr->cardsN = object_training_set_ptr->ImageSaveNumber;
		size = gData_ptr->cardsN*sizeof(unsigned short);
		size = checkINTElemt(size);
		size += gData_ptr->cardsN*DATABASE_RFMS_NUM_LIMIT*sizeof(RFMSPoint_INT);
		size = checkINTElemt(size);
		size += gData_ptr->cardsN*DATABASE_RFMS_NUM_LIMIT*sizeof(RFMSDescriptor);
		size = checkINTElemt(size); 
		objDataBuf = (char *) gp_malloc_align(size*2,16);

		if(!objDataBuf)
		    goto BUF_ERROR;
		else
			DEBUG_MSG("objDataBuf = 0x%x\r\n", objDataBuf);    
		gData_ptr->objData.cardsRFNArr = (unsigned int*)objDataBuf;
		tmpBytes = gData_ptr->cardsN*sizeof(unsigned short);
		tmpBytes = checkINTElemt(tmpBytes);
		objDataBuf += tmpBytes;

		gData_ptr->objData.objKeypoints = (RFMSPoint_INT*)objDataBuf;
		tmpBytes = DATABASE_RFMS_NUM_LIMIT*gData_ptr->cardsN*sizeof(RFMSPoint_INT);
		tmpBytes = checkINTElemt(tmpBytes);
		objDataBuf += tmpBytes;

		gData_ptr->objData.objDescriptors = (RFMSDescriptor*)objDataBuf;
	}

	if(!gData_ptr->objData.cardsRFNArr || !gData_ptr->objData.objKeypoints || !gData_ptr->objData.objDescriptors)
		goto BUF_ERROR;
	
	gData_ptr->objData.cardsRFNArr[(frameNum-1)] = fpNum;
	for(i = 0; i < fpNum; i++)
	{
		gData_ptr->objData.objKeypoints[i + (frameNum-1)*DATABASE_RFMS_NUM_LIMIT].dir_INT = keypoints[i].dir_INT;
		gData_ptr->objData.objKeypoints[i + (frameNum-1)*DATABASE_RFMS_NUM_LIMIT].laplacian = keypoints[i].laplacian;
		gData_ptr->objData.objKeypoints[i + (frameNum-1)*DATABASE_RFMS_NUM_LIMIT].pt.x = keypoints[i].pt.x;
		gData_ptr->objData.objKeypoints[i + (frameNum-1)*DATABASE_RFMS_NUM_LIMIT].pt.y = keypoints[i].pt.y;

		for(j = 0; j < 32; j++)
			gData_ptr->objData.objDescriptors[i + (frameNum-1)*DATABASE_RFMS_NUM_LIMIT].descriptor_short[j] = descriptors[i].descriptor_short[j];
	}

	if(!temp)	
		DEBUG_MSG("RF num = %d\r\n", fpNum);
	else
	{
	  BUF_ERROR:
	  	DEBUG_MSG("objDataBuf = NULL\r\n");
	}
		
	if(frameNum == object_training_set_ptr->CardsN)
	{
		if(gData_ptr->cardsN != object_training_set_ptr->CardsN)
		{
			gData_ptr->cardsN = object_training_set_ptr->CardsN;
			DEBUG_MSG("gData_ptr->cardsN = %d\r\n", object_training_set_ptr->CardsN);
		}
		sprintf((char *)path, (const char *)"objDatabase.bin");
	    fd = open(path, O_CREAT|O_RDWR);
	    if(fd>=0)
	    {
			num = write(fd, (INT32U)gData_ptr->objData.cardsRFNArr, (sizeof(unsigned int)*gData_ptr->cardsN));
			if(num < 0)
				DBG_PRINT("cardsRFNArr error\r\n");
		    num = write(fd, (INT32U)gData_ptr->objData.objKeypoints, (sizeof(RFMSPoint_INT)*(gData_ptr->cardsN*DATABASE_RFMS_NUM_LIMIT)));			
			if(num < 0)
				DBG_PRINT("objKeypoints error\r\n");			
			num = write(fd, (INT32U)gData_ptr->objData.objDescriptors, (sizeof(RFMSDescriptor)*(gData_ptr->cardsN*DATABASE_RFMS_NUM_LIMIT)));
			if(num < 0)
				DBG_PRINT("objDescriptors error\r\n");			
			close(fd);
			DEBUG_MSG("objDatabase write end\r\n");
		}
		else
			DBG_PRINT("file open error\r\n");
		
		if(gData_ptr->objData.cardsRFNArr)
		{
			DEBUG_MSG("free gData_ptr->objData = 0x%x \r\n",gData_ptr->objData.cardsRFNArr);
			gp_free((void *)gData_ptr->objData.cardsRFNArr);
			gData_ptr->objData.cardsRFNArr = 0;	
			gData_ptr->objData.objKeypoints = 0;
			gData_ptr->objData.objDescriptors = 0;		
		}				
	}
		
}

void obj_recognize_proc(ObjDetect_t *odWorkMem)
{
	INT32U i,Extraction_temp,objectTotal_temp;
	INT32U obj_smooth_buf = object_training_set_ptr->ObjSmoothBuffer;
	INT32S objectTotal;
	RfmsIdentify_t *rfms = (RfmsIdentify_t *)odWorkMem->rfms_detect_WorkMem;
	globalData *gData_ptr;

	if(rfms->id_mode == 1)
	{
		if(object_training_set_ptr->ImageSaveNumber < TRAINING_IMAGE_SAVE_MAX_NUMBER)
		{
			image_save_buffer_copy((INT32U)odWorkMem->image.ptr,object_training_set_ptr->ImageSaveNumber);
			object_training_set_ptr->ImageSaveNumber++;
			rfms->id_mode = 0;
			DBG_PRINT("save number = %d \r\n", object_training_set_ptr->ImageSaveNumber);
		}
		else
		{
			rfms->id_mode = 0;
			DBG_PRINT("save error \r\n");
		}	
	}
	else if(rfms->id_mode == 2)
	{
		if(rfms->object_training_cnt < object_training_set_ptr->ImageSaveNumber)
		{
			GPSmoothImage((unsigned char*)(object_training_set_ptr->ImageSaveBufferBase+(rfms->object_training_cnt*TRAINING_IMAGE_SAVE_SIZE)), (unsigned char*)obj_smooth_buf, odWorkMem->image.width, odWorkMem->image.height);
			odWorkMem->image.ptr = (unsigned char*)obj_smooth_buf;
			//image can not be mirror or flip
			i = 0;
			Extraction_temp = 0;
			objectTotal_temp = 0;					
		reExtraction:	
			objectTotal = RFMS_extractPoint(rfms->rfms_WorkMem, &odWorkMem->image);
			OSTimeDly(1);
			if(objectTotal < 130 && rfms->decExtractionThre > rfms->minExtractionThre)
			{
				DBG_PRINT("SURF feature point is not enough (feature NUM = %d)!! extract again!\r\n", objectTotal);
				DBG_PRINT("extractionThre = %d \r\n", rfms->decExtractionThre);
				rfms->decExtractionThre -= 10;
				i++;
				if(i >= RETRY_FIND_EXTRACT_POINT)
				{
					if(objectTotal_temp >= rfms->startMatchingPointN)
					{
						rfms->decExtractionThre = Extraction_temp;
						objectTotal = RFMS_extractPoint(rfms->rfms_WorkMem, &odWorkMem->image);						
						goto reExtraction_exit;
					}
					else
					{
						DBG_PRINT("Image feature point is not enough[%d]!\r\n",objectTotal);
						goto reExtraction_error;
					}	
				}	
				else
				{
					if(rfms->decExtractionThre > Extraction_temp)
					{
						Extraction_temp = rfms->decExtractionThre;
						objectTotal_temp = objectTotal;
					}
					goto reExtraction;
				}	
			}
			else
			{
				if(objectTotal < rfms->startMatchingPointN)
				{
					DBG_PRINT("Image feature point is not enough[%d]!\r\n",objectTotal);
					goto reExtraction_error;				
				}		
			}

			if(objectTotal > 150 )
			{
				DBG_PRINT("SURF feature point is too many (feature NUM = %d)!! extract again!\r\n", objectTotal);
				DBG_PRINT("extractionThre = %d \r\n", rfms->decExtractionThre);
				rfms->decExtractionThre += 10;
				i++;
				if(i >= RETRY_FIND_EXTRACT_POINT)
				{
					if(objectTotal_temp)
					{
						if(objectTotal_temp >= rfms->startMatchingPointN)
						{
							rfms->decExtractionThre = Extraction_temp;
							objectTotal = RFMS_extractPoint(rfms->rfms_WorkMem, &odWorkMem->image);
							goto reExtraction_exit;
						}
						else
						{
							DBG_PRINT("Image feature point is not enough[%d]!\r\n",objectTotal);
							goto reExtraction_error;												
						}	
					}
					else
					{
						DBG_PRINT("Image feature point is NULL!\r\n");
						goto reExtraction_error;
					}
				}	
				else							
					goto reExtraction;	
			}
		reExtraction_exit:	
			if((objectTotal > 130) && (objectTotal < 150))
				DBG_PRINT("Image recognize reliability is good!\r\n");
			else
				DBG_PRINT("Image recognize reliability is medium!\r\n");	
			gData_ptr = (globalData *)rfms->rfms_WorkMem;
			saveRFInformation((int *)rfms->rfms_WorkMem, &(gData_ptr->imgKeypoints[0]), &(gData_ptr->imgDescriptors[0]), objectTotal, (rfms->object_match_cnt+1));
		reExtraction_error:
			if(objectTotal < rfms->startMatchingPointN)
				object_training_set_ptr->CardsN--;	
			OSTimeDly(1);
		}
		
		if((rfms->object_training_cnt < object_training_set_ptr->ImageSaveNumber))
		{
			if(objectTotal > rfms->startMatchingPointN)
			{
				rfms->object_match_cnt++;
				DBG_PRINT("*****training image number = %d[OK]!*********\r\n", (rfms->object_training_cnt + 1));
			}
			else
				DBG_PRINT("*****training image number = %d[remove]!*****\r\n", (rfms->object_training_cnt + 1));			
			rfms->object_training_cnt++;
		}
		else
		{
			if(object_training_set_ptr->CardsN != rfms->object_match_cnt)
			{
				object_training_set_ptr->CardsN = rfms->object_match_cnt;
				DBG_PRINT("image_match_cnt != Object_CardsN\r\n");
				DBG_PRINT("Object_CardsN = %d\r\n",object_training_set_ptr->CardsN);
			}
			
			if(!rfms->object_match_cnt)
				obj_recognize_set_mode(0);
			else
				obj_recognize_set_mode(3);
		}	
	}
	else if(rfms->id_mode == 3)
	{
		//image can not be mirror or flip
		GPSmoothImage((unsigned char*)(odWorkMem->image.ptr), (unsigned char*)obj_smooth_buf, odWorkMem->image.width, odWorkMem->image.height);
		odWorkMem->image.ptr = (unsigned char*)obj_smooth_buf;		
		objectTotal = RFMS_extractPoint(rfms->rfms_WorkMem, &odWorkMem->image);
		OSTimeDly(1);
		rfms->matchingCardNum = RFMS_findPairs(rfms->rfms_WorkMem, objectTotal);		
		if(rfms->matchingCardNum == 0) {
			sensor_control_ae();
		}
	    OSTimeDly(1);
	}
	else
	{
		if(rfms->object_training_cnt)
		{
			object_training_bin_2_sd(rfms->object_training_cnt,0);
			rfms->object_training_cnt = 0;
		}
		rfms->matchingCardNum = 0;
	}
}

void obj_recognize_free(ObjDetect_t *odWorkMem)
{
	RfmsIdentify_t *rfms = (RfmsIdentify_t *)odWorkMem->rfms_detect_WorkMem;
	
	object_training_buffer_free();
	if(odWorkMem->rfms_detect_WorkMem) {
		if(rfms->rfms_WorkMem) {
			gp_free((void *)rfms->rfms_WorkMem);
			rfms->rfms_WorkMem = 0;
		}
		
		if(rfms->obj_data_buf) {
			gp_free((void *)rfms->obj_data_buf);
			rfms->obj_data_buf = 0;
		}
	
		gp_free((void *)odWorkMem->rfms_detect_WorkMem);
		odWorkMem->rfms_detect_WorkMem = 0;
	}
}

void obj_recognize_get_result(void *od_work_mem, void *objresult)
{
	ObjDetect_t *odWorkMem = (ObjDetect_t *)od_work_mem;
	ObjDetResult_t *result = (ObjDetResult_t *)objresult;
	RfmsIdentify_t *rfms = (RfmsIdentify_t *)odWorkMem->rfms_detect_WorkMem;
	
	if(rfms->matchingCardNum > 0) {
		result->ObjIdentifyFlag = 1;
		result->MatchingCardNum = rfms->matchingCardNum;
		DBG_PRINT("CardNumber = %d\r\n", result->MatchingCardNum); 
	} else {
		result->ObjIdentifyFlag = 0;
		result->MatchingCardNum = 0;
	}
}
#endif

////////////////////////////////////////////////////////////////
//		draw frame buffer for display
////////////////////////////////////////////////////////////////
void DrawRect(gpImage *Image, gpRect *Rect, const INT8U _color[4])
{
	INT32S i;
	INT8U *dst1;
	INT8U *dst2;
	INT8U *dst11;
	INT8U *dst21;
	INT8U *dst12;
	INT8U *dst22;		
	INT8U color[4];
	
	if(_color) {
		color[0] = _color[0];	color[1] = _color[1];
		color[2] = _color[2];	color[3] = _color[3];
	} else {
		color[0] = 255;	color[1] = 255;
		color[2] = 255;	color[3] = 255;
	}

	if(Image->format==IMG_FMT_GRAY) {
		dst1 = Image->ptr + Rect->x + Rect->y * Image->widthStep;
		dst2 = Image->ptr + Rect->x + (Rect->y + Rect->height - 1)* Image->widthStep;
		for(i=0; i<Rect->width; i++) {
			*dst1++ = color[0];
			*dst2++ = color[0];
		}
		
		dst1 = Image->ptr + Rect->x + Rect->y * Image->widthStep;
		dst2 = Image->ptr + Rect->x + Rect->width - 1 + Rect->y * Image->widthStep;
		for(i=0;i<Rect->height;i++) {
			*dst1 = color[0];
			*dst2 = color[0];
			dst1 += Image->widthStep;
			dst2 += Image->widthStep;
		}
	} else if(Image->format==IMG_FMT_YUYV || Image->format==IMG_FMT_YCbYCr || Image->format==IMG_FMT_UYVY || Image->format==IMG_FMT_CbYCrY) {
		dst1 = Image->ptr + Rect->x * 2 + Rect->y * Image->widthStep;
		dst2 = Image->ptr + Rect->x * 2 + (Rect->y + Rect->height - 1)* Image->widthStep;
		dst11 = Image->ptr + (Rect->x) * 2 + (Rect->y-1) * Image->widthStep;
		dst21 = Image->ptr + (Rect->x) * 2 + ((Rect->y+1) + Rect->height - 1)* Image->widthStep;
		dst12 = Image->ptr + (Rect->x) * 2 + (Rect->y-2) * Image->widthStep;
		dst22 = Image->ptr + (Rect->x) * 2 + ((Rect->y+2) + Rect->height - 1)* Image->widthStep;		
		
		for(i=0; i<Rect->width; i++) {
			*dst1++ = color[0];		*dst1++ = color[1];
			*dst2++ = color[0];		*dst2++ = color[1];
			*dst11++ = color[0];	*dst11++ = color[1];
			*dst21++ = color[0];	*dst21++ = color[1];
			*dst12++ = color[0];	*dst12++ = color[1];
			*dst22++ = color[0];	*dst22++ = color[1];			
		}
		
		dst1 = Image->ptr + Rect->x * 2 + Rect->y * Image->widthStep;
		dst2 = Image->ptr + (Rect->x + Rect->width - 1) * 2 + Rect->y * Image->widthStep;
		dst11 = Image->ptr + (Rect->x-1) * 2 + (Rect->y) * Image->widthStep;
		dst21 = Image->ptr + ((Rect->x+1) + Rect->width - 1) * 2 + (Rect->y) * Image->widthStep;
		dst12 = Image->ptr + (Rect->x-2) * 2 + (Rect->y) * Image->widthStep;
		dst22 = Image->ptr + ((Rect->x+2) + Rect->width - 1) * 2 + (Rect->y) * Image->widthStep;		
		
		for(i=0; i<Rect->height; i++) {
			dst1[0] = color[0];		dst1[1] = color[1];
			dst2[0] = color[0];		dst2[1] = color[1];
			dst11[0] = color[0];	dst11[1] = color[1];
			dst21[0] = color[0];	dst21[1] = color[1];
			dst12[0] = color[0];	dst12[1] = color[1];
			dst22[0] = color[0];	dst22[1] = color[1];			
			dst1 += Image->widthStep;
			dst2 += Image->widthStep;
			dst11 += Image->widthStep;
			dst21 += Image->widthStep;
			dst12 += Image->widthStep;
			dst22 += Image->widthStep;			
		}
	} else if(Image->format==IMG_FMT_RGB) {
		dst1 = Image->ptr + Rect->x * 3 + Rect->y * Image->widthStep;
		dst2 = Image->ptr + Rect->x * 3 + (Rect->y + Rect->height - 1)* Image->widthStep;
		for(i=0; i<Rect->width; i++) {
			*dst1++ = color[0];		*dst1++ = color[1];		*dst1++ = color[2];
			*dst2++ = color[0];		*dst2++ = color[1];		*dst2++ = color[2];
		}
		
		dst1 = Image->ptr + Rect->x * 3 + Rect->y * Image->widthStep;
		dst2 = Image->ptr + (Rect->x + Rect->width - 1) * 3 + Rect->y * Image->widthStep;
		for(i=0; i<Rect->height; i++) {
			dst1[0] = color[0];		dst1[1] = color[1];		dst1[2] = color[2];
			dst2[0] = color[0];		dst2[1] = color[1];		dst2[2] = color[2];
			dst1 += Image->widthStep;
			dst2 += Image->widthStep;
		}
	}
}

void draw_rect(gpImage *image, gpRect *r, INT32U color)
{
	INT32S i;
	INT32U *p_img;
	INT32S len;
	INT32S addr;
	INT32S x, y, w, h;
	INT16U *img = (INT16U *) image->ptr;
		
	w = r->width;
	h = r->height;
	
	if(w !=0 && h !=0) {
		x = r->x;
		y = r->y;
	
		// top line
		len = w >> 1;
		addr = (INT32U) (img + (y * image->width) + x);
		addr >>= 2;
		addr <<= 2;
		p_img = (INT32U *) addr;		
		
		for(i=0; i<len; i++) {
			*p_img++ = color;
		}
		
		addr = (INT32S) (img + ((y+1) * image->width) + x);
		addr >>= 2;
		addr <<= 2;
		p_img = (INT32U *) addr;
		for(i=0; i<len; i++) {
			*p_img++ = color;
		}
			
		addr = (INT32S) (img + ((y+2) * image->width) + x);
		addr >>= 2;
		addr <<= 2;
		p_img = (INT32U *) addr;
		for(i=0; i<len; i++) {
			*p_img++ = color;
		}
			
		addr = (INT32S) (img + ((y+3) * image->width) + x);
		addr >>= 2;
		addr <<= 2;
		p_img = (INT32U *) addr;
		for(i=0; i<len; i++) {
			*p_img++ = color;
		}
			
		// bottom line	
		addr = (INT32S) (img + ((y +h) * image->width) + x);
		addr >>= 2;
		addr <<= 2;
		p_img = (INT32U *) addr;
		for(i=0;i<len;i++) {
			*p_img++ = color;
		}
	
		addr = (INT32S) (img + ((y - 1 + h) * image->width) + x);
		addr >>= 2;
		addr <<= 2;
		p_img = (INT32U *) addr;
		for(i=0;i<len;i++) {
			*p_img++ = color;
		}
			
		addr = (INT32S) (img + ((y - 2 + h) * image->width) + x);
		addr >>= 2;
		addr <<= 2;
		p_img = (INT32U *) addr;
		for(i=0;i<len;i++) {
			*p_img++ = color;
		}	
			
		addr = (INT32S) (img + ((y - 3 + h) * image->width) + x);
		addr >>= 2;
		addr <<= 2;
		p_img = (INT32U *) addr;
		for(i=0;i<len;i++) {
			*p_img++ = color;
		}
	
		// left line
		addr = (INT32S) (img + ((y+4) * image->width) + x);
		addr >>= 2;
		addr <<= 2;
		p_img = (INT32U *) addr;
		len = h - 8;	
		for(i=0;i<len;i++) {
			*p_img++ = color;
			*p_img++ = color;
			p_img += (image->width - 4) >> 1;
		}
	
		// right line
		addr = (INT32S) (img + ((y+4) * image->width) + x + w - 2);
		addr >>= 2;
		addr <<= 2;
		p_img = (INT32U *) addr;
		for(i=0;i<len;i++) {
			*p_img++ = color;
			*p_img++ = color;
			p_img += (image->width - 4) >> 1;
		}	
	}
}

void draw_obj(INT32U img_adr, INT32U bufw, INT32U bufh, INT32S total_obj_cnt, gpRect *obj_result_d)
{
	INT32S i;	
	INT32U color;
	gpImage Image;
	
	Image.width = bufw;					// depends on display buffer width
	Image.height = bufh;				// depends on display buffer height
	Image.widthStep = bufw*2; 			// depends on display format
	Image.ch = 2; 						// depends on display format
	Image.format = IMG_FMT_UYVY;  		// depends on display format
	Image.ptr = (INT8U *)img_adr;

	//color = 0xff4c554c; // green
	//color = 0x4cff4c55; // blue
	color = 0xffffffff; // red
	for(i=0; i<total_obj_cnt; i++) {
		draw_rect(&Image, &obj_result_d[i], color);
	}
}

void obj_detect_show_result(INT32U img_adr, INT32U bufw, INT32U bufh, ObjDetResult_t *result)
{
	if(result->ResultCnt > 0) {
		draw_obj(img_adr, bufw, bufh, result->ResultCnt, result->Result);
	}
	
	if(result->EyeFlag) {
		draw_obj(img_adr, bufw, bufh, 2, result->Eye);
		draw_obj(img_adr, bufw, bufh, 2, result->Eyebrow);
	}
	
	if(result->NoseFlag) {
		draw_obj(img_adr, bufw, bufh, 2, result->Nose);
	}
	
	if(result->MouthFlag) {
		draw_obj(img_adr, bufw, bufh, 4, result->Mouth);
	}
	
	if(result->SmileFlag) {
		DEBUG_MSG("Smile = %d\r\n", result->SmileFlag);
	}
	
	if(result->FaceTrainingFlag) {
		DEBUG_MSG("Traning = %d\r\n", result->FaceTrainingFlag);
	}
	
	if(result->FaceIdentifyFlag) {
		DEBUG_MSG("Identify = %d\r\n", result->FaceIdentifyFlag);
	}
	
	if(result->HandFlag) {
		DEBUG_MSG("Hand = %d\r\n", result->HandFlag);
	}

	if(result->FistFlag) {
		DEBUG_MSG("Fist = %d\r\n", result->FistFlag);
	}
}
#endif