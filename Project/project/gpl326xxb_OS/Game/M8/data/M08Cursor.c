#include	"sprite.h"
const INT16U ALIGN4   M08Cursor_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  4,	  0,	  0,
		0xffff,};
const INT32U  M08Cursor_Frame[] = {
	(const INT32U) M08Cursor_Frame00Cell,
};
const INT16U ALIGN4   M08Y_M_D_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  1,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Y_M_D_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  2,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Y_M_D_Frame02Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  3,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Y_M_D_Frame03Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  4,	  0,	  0,
	  5,	  16,	  0,
	  6,	  32,	  0,
		0xffff,};
const INT32U  M08Y_M_D_Frame[] = {
	(const INT32U) M08Y_M_D_Frame00Cell,
	(const INT32U) M08Y_M_D_Frame01Cell,
	(const INT32U) M08Y_M_D_Frame02Cell,
	(const INT32U) M08Y_M_D_Frame03Cell,
};
const INT16U ALIGN4   M08Delete_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
	  29,	  64,	  0,
	  29,	  128,	  0,
	  29,	  192,	  0,
	  29,	  256,	  0,
	  29,	  0,	  64,
	  76,	  64,	  64,
	  77,	  128,	  64,
	  78,	  192,	  64,
	  29,	  256,	  64,
	  29,	  0,	  128,
	  79,	  64,	  128,
	  80,	  128,	  128,
	  81,	  192,	  128,
	  29,	  256,	  128,
	  29,	  0,	  192,
	  29,	  64,	  192,
	  29,	  128,	  192,
	  29,	  192,	  192,
	  29,	  256,	  192,
		0xffff,};
const INT16U ALIGN4   M08Delete_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
	  29,	  64,	  0,
	  29,	  128,	  0,
	  29,	  192,	  0,
	  29,	  256,	  0,
	  29,	  0,	  64,
	  76,	  64,	  64,
	  77,	  128,	  64,
	  82,	  192,	  64,
	  29,	  256,	  64,
	  29,	  0,	  128,
	  79,	  64,	  128,
	  80,	  128,	  128,
	  83,	  192,	  128,
	  29,	  256,	  128,
	  29,	  0,	  192,
	  29,	  64,	  192,
	  29,	  128,	  192,
	  29,	  192,	  192,
	  29,	  256,	  192,
		0xffff,};
const INT16U ALIGN4   M08Delete_Frame02Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
	  29,	  64,	  0,
	  29,	  128,	  0,
	  29,	  192,	  0,
	  29,	  256,	  0,
	  29,	  0,	  64,
	  76,	  64,	  64,
	  77,	  128,	  64,
	  84,	  192,	  64,
	  29,	  256,	  64,
	  29,	  0,	  128,
	  79,	  64,	  128,
	  80,	  128,	  128,
	  85,	  192,	  128,
	  29,	  256,	  128,
	  29,	  0,	  192,
	  29,	  64,	  192,
	  29,	  128,	  192,
	  29,	  192,	  192,
	  29,	  256,	  192,
		0xffff,};
const INT16U ALIGN4   M08Delete_Frame03Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
	  29,	  64,	  0,
	  29,	  128,	  0,
	  29,	  192,	  0,
	  29,	  256,	  0,
	  29,	  0,	  64,
	  76,	  64,	  64,
	  77,	  128,	  64,
	  86,	  192,	  64,
	  29,	  256,	  64,
	  29,	  0,	  128,
	  79,	  64,	  128,
	  80,	  128,	  128,
	  87,	  192,	  128,
	  29,	  256,	  128,
	  29,	  0,	  192,
	  29,	  64,	  192,
	  29,	  128,	  192,
	  29,	  192,	  192,
	  29,	  256,	  192,
		0xffff,};
const INT32U  M08Delete_Frame[] = {
	(const INT32U) M08Delete_Frame00Cell,
	(const INT32U) M08Delete_Frame01Cell,
	(const INT32U) M08Delete_Frame02Cell,
	(const INT32U) M08Delete_Frame03Cell,
};
const INT16U ALIGN4   M08Emotion_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Emotion_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  30,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Emotion_Frame02Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  31,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Emotion_Frame03Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  32,	  0,	  0,
		0xffff,};
const INT32U  M08Emotion_Frame[] = {
	(const INT32U) M08Emotion_Frame00Cell,
	(const INT32U) M08Emotion_Frame01Cell,
	(const INT32U) M08Emotion_Frame02Cell,
	(const INT32U) M08Emotion_Frame03Cell,
};
const INT16U ALIGN4   M08Month_Num_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  37,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Month_Num_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  38,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Month_Num_Frame02Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  39,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Month_Num_Frame03Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  40,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Month_Num_Frame04Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  41,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Month_Num_Frame05Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  42,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Month_Num_Frame06Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  43,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Month_Num_Frame07Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  44,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Month_Num_Frame08Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  45,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Month_Num_Frame09Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  46,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Month_Num_Frame10Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  47,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Month_Num_Frame11Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  48,	  0,	  0,
		0xffff,};
const INT32U  M08Month_Num_Frame[] = {
	(const INT32U) M08Month_Num_Frame00Cell,
	(const INT32U) M08Month_Num_Frame01Cell,
	(const INT32U) M08Month_Num_Frame02Cell,
	(const INT32U) M08Month_Num_Frame03Cell,
	(const INT32U) M08Month_Num_Frame04Cell,
	(const INT32U) M08Month_Num_Frame05Cell,
	(const INT32U) M08Month_Num_Frame06Cell,
	(const INT32U) M08Month_Num_Frame07Cell,
	(const INT32U) M08Month_Num_Frame08Cell,
	(const INT32U) M08Month_Num_Frame09Cell,
	(const INT32U) M08Month_Num_Frame10Cell,
	(const INT32U) M08Month_Num_Frame11Cell,
};
const INT16U ALIGN4   M08Weather_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  25,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Weather_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  26,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Weather_Frame02Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  27,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Weather_Frame03Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  28,	  0,	  0,
		0xffff,};
const INT32U  M08Weather_Frame[] = {
	(const INT32U) M08Weather_Frame00Cell,
	(const INT32U) M08Weather_Frame01Cell,
	(const INT32U) M08Weather_Frame02Cell,
	(const INT32U) M08Weather_Frame03Cell,
};
const INT16U ALIGN4   M08Save_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
	  29,	  64,	  0,
	  29,	  128,	  0,
	  29,	  192,	  0,
	  29,	  256,	  0,
	  29,	  0,	  64,
	  51,	  64,	  64,
	  52,	  128,	  64,
	  53,	  192,	  64,
	  29,	  256,	  64,
	  29,	  0,	  128,
	  29,	  64,	  128,
	  54,	  128,	  128,
	  55,	  192,	  128,
	  29,	  256,	  128,
	  29,	  0,	  192,
	  29,	  64,	  192,
	  29,	  128,	  192,
	  29,	  192,	  192,
	  29,	  256,	  192,
		0xffff,};
const INT16U ALIGN4   M08Save_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
	  29,	  64,	  0,
	  29,	  128,	  0,
	  29,	  192,	  0,
	  29,	  256,	  0,
	  29,	  0,	  64,
	  51,	  64,	  64,
	  56,	  128,	  64,
	  53,	  192,	  64,
	  29,	  256,	  64,
	  29,	  0,	  128,
	  29,	  64,	  128,
	  57,	  128,	  128,
	  58,	  192,	  128,
	  29,	  256,	  128,
	  29,	  0,	  192,
	  29,	  64,	  192,
	  29,	  128,	  192,
	  29,	  192,	  192,
	  29,	  256,	  192,
		0xffff,};
const INT16U ALIGN4   M08Save_Frame02Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
	  29,	  64,	  0,
	  29,	  128,	  0,
	  29,	  192,	  0,
	  29,	  256,	  0,
	  29,	  0,	  64,
	  51,	  64,	  64,
	  59,	  128,	  64,
	  53,	  192,	  64,
	  29,	  256,	  64,
	  29,	  0,	  128,
	  29,	  64,	  128,
	  60,	  128,	  128,
	  61,	  192,	  128,
	  29,	  256,	  128,
	  29,	  0,	  192,
	  29,	  64,	  192,
	  29,	  128,	  192,
	  29,	  192,	  192,
	  29,	  256,	  192,
		0xffff,};
const INT16U ALIGN4   M08Save_Frame03Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
	  29,	  64,	  0,
	  29,	  128,	  0,
	  29,	  192,	  0,
	  29,	  256,	  0,
	  29,	  0,	  64,
	  51,	  64,	  64,
	  62,	  128,	  64,
	  53,	  192,	  64,
	  29,	  256,	  64,
	  29,	  0,	  128,
	  29,	  64,	  128,
	  63,	  128,	  128,
	  64,	  192,	  128,
	  29,	  256,	  128,
	  29,	  0,	  192,
	  29,	  64,	  192,
	  29,	  128,	  192,
	  29,	  192,	  192,
	  29,	  256,	  192,
		0xffff,};
const INT16U ALIGN4   M08Save_Frame04Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
	  29,	  64,	  0,
	  29,	  128,	  0,
	  29,	  192,	  0,
	  29,	  256,	  0,
	  29,	  0,	  64,
	  51,	  64,	  64,
	  65,	  128,	  64,
	  53,	  192,	  64,
	  29,	  256,	  64,
	  29,	  0,	  128,
	  29,	  64,	  128,
	  66,	  128,	  128,
	  67,	  192,	  128,
	  29,	  256,	  128,
	  29,	  0,	  192,
	  29,	  64,	  192,
	  29,	  128,	  192,
	  29,	  192,	  192,
	  29,	  256,	  192,
		0xffff,};
const INT16U ALIGN4   M08Save_Frame05Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
	  29,	  64,	  0,
	  29,	  128,	  0,
	  29,	  192,	  0,
	  29,	  256,	  0,
	  29,	  0,	  64,
	  51,	  64,	  64,
	  68,	  128,	  64,
	  53,	  192,	  64,
	  29,	  256,	  64,
	  29,	  0,	  128,
	  29,	  64,	  128,
	  69,	  128,	  128,
	  64,	  192,	  128,
	  29,	  256,	  128,
	  29,	  0,	  192,
	  29,	  64,	  192,
	  29,	  128,	  192,
	  29,	  192,	  192,
	  29,	  256,	  192,
		0xffff,};
const INT16U ALIGN4   M08Save_Frame06Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
	  29,	  64,	  0,
	  29,	  128,	  0,
	  29,	  192,	  0,
	  29,	  256,	  0,
	  29,	  0,	  64,
	  51,	  64,	  64,
	  68,	  128,	  64,
	  53,	  192,	  64,
	  29,	  256,	  64,
	  29,	  0,	  128,
	  29,	  64,	  128,
	  69,	  128,	  128,
	  64,	  192,	  128,
	  29,	  256,	  128,
	  29,	  0,	  192,
	  29,	  64,	  192,
	  29,	  128,	  192,
	  29,	  192,	  192,
	  29,	  256,	  192,
		0xffff,};
const INT16U ALIGN4   M08Save_Frame07Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
	  29,	  64,	  0,
	  29,	  128,	  0,
	  29,	  192,	  0,
	  29,	  256,	  0,
	  29,	  0,	  64,
	  70,	  64,	  64,
	  71,	  128,	  64,
	  72,	  192,	  64,
	  29,	  256,	  64,
	  29,	  0,	  128,
	  73,	  64,	  128,
	  74,	  128,	  128,
	  75,	  192,	  128,
	  29,	  256,	  128,
	  29,	  0,	  192,
	  29,	  64,	  192,
	  29,	  128,	  192,
	  29,	  192,	  192,
	  29,	  256,	  192,
		0xffff,};
const INT32U  M08Save_Frame[] = {
	(const INT32U) M08Save_Frame00Cell,
	(const INT32U) M08Save_Frame01Cell,
	(const INT32U) M08Save_Frame02Cell,
	(const INT32U) M08Save_Frame03Cell,
	(const INT32U) M08Save_Frame04Cell,
	(const INT32U) M08Save_Frame05Cell,
	(const INT32U) M08Save_Frame06Cell,
	(const INT32U) M08Save_Frame07Cell,
};
const INT16U ALIGN4   M08Arrow_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  23,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Arrow_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  24,	  0,	  0,
		0xffff,};
const INT32U  M08Arrow_Frame[] = {
	(const INT32U) M08Arrow_Frame00Cell,
	(const INT32U) M08Arrow_Frame01Cell,
};
const INT16U ALIGN4   M08Scroll_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  3,	  0,	  0,
	  4,	  0,	  64,
		0xffff,};
const INT16U ALIGN4   M08Scroll_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  33,	  0,	  0,
	  34,	  0,	  64,
	  35,	  0,	  128,
	  36,	  0,	  192,
		0xffff,};
const INT32U  M08Scroll_Frame[] = {
	(const INT32U) M08Scroll_Frame00Cell,
	(const INT32U) M08Scroll_Frame01Cell,
};
const INT16U ALIGN4   M08Button_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  51,	  0,	  0,
	  52,	  64,	  0,
		0xffff,};
const INT16U ALIGN4   M08Button_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  49,	  0,	  0,
	  50,	  64,	  0,
		0xffff,};
const INT16U ALIGN4   M08Button_Frame02Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  53,	  0,	  0,
	  54,	  64,	  0,
		0xffff,};
const INT16U ALIGN4   M08Button_Frame03Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  55,	  0,	  0,
	  56,	  64,	  0,
		0xffff,};
const INT32U  M08Button_Frame[] = {
	(const INT32U) M08Button_Frame00Cell,
	(const INT32U) M08Button_Frame01Cell,
	(const INT32U) M08Button_Frame02Cell,
	(const INT32U) M08Button_Frame03Cell,
};
const INT16U ALIGN4   M8DateFrame_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  21,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M8DateFrame_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  45,	  0,	  0,
	  46,	  64,	  0,
	  47,	  128,	  0,
	  48,	  0,	  64,
	  49,	  64,	  64,
	  50,	  128,	  64,
		0xffff,};
const INT32U  M8DateFrame_Frame[] = {
	(const INT32U) M8DateFrame_Frame00Cell,
	(const INT32U) M8DateFrame_Frame01Cell,
};
const INT16U ALIGN4   M8Heart_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  22,	  0,	  0,
		0xffff,};
const INT32U  M8Heart_Frame[] = {
	(const INT32U) M8Heart_Frame00Cell,
};
const INT16U ALIGN4   M08AlbumFrame_Frame_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE64|  SP_HSIZE64|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  30,	  0,	  0,
	  31,	  64,	  0,
	  32,	  128,	  0,
	  31,	  192,	  0,
	  33,	  256,	  0,
	  34,	  0,	  64,
	  35,	  64,	  64,
	  36,	  128,	  64,
	  35,	  192,	  64,
	  37,	  256,	  64,
	  38,	  0,	  128,
	  29,	  64,	  128,
	  39,	  128,	  128,
	  29,	  192,	  128,
	  40,	  256,	  128,
	  41,	  0,	  192,
	  42,	  64,	  192,
	  43,	  128,	  192,
	  42,	  192,	  192,
	  44,	  256,	  192,
		0xffff,};
const INT16U ALIGN4   M08AlbumFrame_Frame_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  33,	  0,	  0,
	  33,	  32,	  0,
	  33,	  64,	  0,
	  33,	  96,	  0,
	  33,	  128,	  0,
	  33,	  160,	  0,
	  33,	  192,	  0,
	  33,	  224,	  0,
	  33,	  256,	  0,
	  33,	  288,	  0,
	  33,	  0,	  32,
	  33,	  32,	  32,
	  33,	  64,	  32,
	  33,	  96,	  32,
	  33,	  128,	  32,
	  33,	  160,	  32,
	  33,	  192,	  32,
	  33,	  224,	  32,
	  33,	  256,	  32,
	  33,	  288,	  32,
	  34,	  0,	  64,
	  35,	  32,	  64,
	  36,	  64,	  64,
	  37,	  96,	  64,
	  38,	  128,	  64,
	  39,	  160,	  64,
	  40,	  192,	  64,
	  41,	  224,	  64,
	  42,	  256,	  64,
	  43,	  288,	  64,
	  44,	  0,	  96,
	  45,	  32,	  96,
	  46,	  64,	  96,
	  47,	  96,	  96,
	  48,	  128,	  96,
	  49,	  160,	  96,
	  50,	  192,	  96,
	  51,	  224,	  96,
	  52,	  256,	  96,
	  53,	  288,	  96,
	  54,	  0,	  128,
	  55,	  32,	  128,
	  56,	  64,	  128,
	  57,	  96,	  128,
	  58,	  128,	  128,
	  59,	  160,	  128,
	  60,	  192,	  128,
	  61,	  224,	  128,
	  62,	  256,	  128,
	  63,	  288,	  128,
	  33,	  0,	  160,
	  33,	  32,	  160,
	  33,	  64,	  160,
	  33,	  96,	  160,
	  33,	  128,	  160,
	  33,	  160,	  160,
	  33,	  192,	  160,
	  33,	  224,	  160,
	  33,	  256,	  160,
	  33,	  288,	  160,
	  33,	  0,	  192,
	  33,	  32,	  192,
	  33,	  64,	  192,
	  33,	  96,	  192,
	  33,	  128,	  192,
	  33,	  160,	  192,
	  33,	  192,	  192,
	  33,	  224,	  192,
	  33,	  256,	  192,
	  33,	  288,	  192,
	  64,	  0,	  224,
	  64,	  32,	  224,
	  64,	  64,	  224,
	  64,	  96,	  224,
	  64,	  128,	  224,
	  64,	  160,	  224,
	  64,	  192,	  224,
	  64,	  224,	  224,
	  64,	  256,	  224,
	  64,	  288,	  224,
		0xffff,};
const INT32U  M08AlbumFrame_Frame_Frame[] = {
	(const INT32U) M08AlbumFrame_Frame_Frame00Cell,
	(const INT32U) M08AlbumFrame_Frame_Frame01Cell,
};
const INT16U ALIGN4   M08Date_Num_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  10,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  11,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame02Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  12,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame03Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  13,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame04Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  14,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame05Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  15,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame06Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  16,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame07Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  17,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame08Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  18,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame09Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  19,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame10Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  20,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame11Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  21,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame12Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  22,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame13Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  23,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame14Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  24,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame15Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  25,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame16Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  26,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame17Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  27,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame18Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  28,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame19Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  29,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame20Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  30,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame21Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  31,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame22Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  32,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame23Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  33,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame24Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  34,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame25Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  35,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame26Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  36,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame27Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  37,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame28Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  38,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame29Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  39,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame30Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  40,	  0,	  0,
		0xffff,};
const INT16U ALIGN4   M08Date_Num_Frame31Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0 |  SP_VSIZE16|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK3,
	  41,	  0,	  0,
		0xffff,};
const INT32U  M08Date_Num_Frame[] = {
	(const INT32U) M08Date_Num_Frame00Cell,
	(const INT32U) M08Date_Num_Frame01Cell,
	(const INT32U) M08Date_Num_Frame02Cell,
	(const INT32U) M08Date_Num_Frame03Cell,
	(const INT32U) M08Date_Num_Frame04Cell,
	(const INT32U) M08Date_Num_Frame05Cell,
	(const INT32U) M08Date_Num_Frame06Cell,
	(const INT32U) M08Date_Num_Frame07Cell,
	(const INT32U) M08Date_Num_Frame08Cell,
	(const INT32U) M08Date_Num_Frame09Cell,
	(const INT32U) M08Date_Num_Frame10Cell,
	(const INT32U) M08Date_Num_Frame11Cell,
	(const INT32U) M08Date_Num_Frame12Cell,
	(const INT32U) M08Date_Num_Frame13Cell,
	(const INT32U) M08Date_Num_Frame14Cell,
	(const INT32U) M08Date_Num_Frame15Cell,
	(const INT32U) M08Date_Num_Frame16Cell,
	(const INT32U) M08Date_Num_Frame17Cell,
	(const INT32U) M08Date_Num_Frame18Cell,
	(const INT32U) M08Date_Num_Frame19Cell,
	(const INT32U) M08Date_Num_Frame20Cell,
	(const INT32U) M08Date_Num_Frame21Cell,
	(const INT32U) M08Date_Num_Frame22Cell,
	(const INT32U) M08Date_Num_Frame23Cell,
	(const INT32U) M08Date_Num_Frame24Cell,
	(const INT32U) M08Date_Num_Frame25Cell,
	(const INT32U) M08Date_Num_Frame26Cell,
	(const INT32U) M08Date_Num_Frame27Cell,
	(const INT32U) M08Date_Num_Frame28Cell,
	(const INT32U) M08Date_Num_Frame29Cell,
	(const INT32U) M08Date_Num_Frame30Cell,
	(const INT32U) M08Date_Num_Frame31Cell,
};
