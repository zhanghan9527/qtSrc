#include	"sprite.h"
const INT16U ALIGN4   pub_powsp_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0|  SP_VSIZE16|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK1,
	  1,	  0,	  0,
	  2,	  16,	  0,
		0xffff,};
const INT16U ALIGN4   pub_powsp_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0|  SP_VSIZE16|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK1,
	  3,	  0,	  0,
	  4,	  16,	  0,
		0xffff,};
const INT16U ALIGN4   pub_powsp_Frame02Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0|  SP_VSIZE16|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK1,
	  5,	  0,	  0,
	  6,	  16,	  0,
		0xffff,};
const INT16U ALIGN4   pub_powsp_Frame03Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0|  SP_VSIZE16|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK1,
	  7,	  0,	  0,
	  6,	  16,	  0,
		0xffff,};
const INT16U ALIGN4   pub_powsp_Frame04Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0|  SP_VSIZE16|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK1,
	  84,	  0,	  0,
	  2,	  16,	  0,
		0xffff,};
const INT32U  pub_powsp_Frame[] = {
	(const INT32U) pub_powsp_Frame00Cell,
	(const INT32U) pub_powsp_Frame01Cell,
	(const INT32U) pub_powsp_Frame02Cell,
	(const INT32U) pub_powsp_Frame03Cell,
	(const INT32U) pub_powsp_Frame04Cell,
};
const INT16U ALIGN4   pub_delSp_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0|  SP_VSIZE16|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK1,
	  8,	  0,	  0,
	  9,	  16,	  0,
	  10,	  32,	  0,
	  11,	  48,	  0,
	  12,	  64,	  0,
	  13,	  80,	  0,
	  14,	  96,	  0,
	  15,	  112,	  0,
	  16,	  0,	  16,
	  17,	  16,	  16,
	  18,	  32,	  16,
	  19,	  48,	  16,
	  20,	  64,	  16,
	  21,	  80,	  16,
	  22,	  96,	  16,
	  23,	  112,	  16,
	  24,	  0,	  32,
	  25,	  16,	  32,
	  26,	  32,	  32,
	  27,	  48,	  32,
	  28,	  64,	  32,
	  29,	  80,	  32,
	  30,	  96,	  32,
	  31,	  112,	  32,
	  32,	  0,	  48,
	  33,	  16,	  48,
	  34,	  32,	  48,
	  19,	  48,	  48,
	  35,	  64,	  48,
	  36,	  80,	  48,
	  37,	  96,	  48,
	  38,	  112,	  48,
	  39,	  0,	  64,
	  40,	  16,	  64,
	  41,	  32,	  64,
	  42,	  48,	  64,
	  43,	  64,	  64,
	  44,	  80,	  64,
	  45,	  96,	  64,
	  46,	  112,	  64,
	  47,	  0,	  80,
	  48,	  16,	  80,
	  49,	  32,	  80,
	  50,	  48,	  80,
	  51,	  64,	  80,
	  52,	  80,	  80,
	  53,	  96,	  80,
	  54,	  112,	  80,
		0xffff,};
const INT16U ALIGN4   pub_delSp_Frame01Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0|  SP_VSIZE16|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR256,
	SP_PBANK1,
	  8,	  0,	  0,
	  9,	  16,	  0,
	  55,	  32,	  0,
	  56,	  48,	  0,
	  57,	  64,	  0,
	  13,	  80,	  0,
	  14,	  96,	  0,
	  15,	  112,	  0,
	  58,	  0,	  16,
	  19,	  16,	  16,
	  59,	  32,	  16,
	  60,	  48,	  16,
	  61,	  64,	  16,
	  19,	  80,	  16,
	  62,	  96,	  16,
	  23,	  112,	  16,
	  63,	  0,	  32,
	  19,	  16,	  32,
	  64,	  32,	  32,
	  65,	  48,	  32,
	  66,	  64,	  32,
	  19,	  80,	  32,
	  67,	  96,	  32,
	  31,	  112,	  32,
	  68,	  0,	  48,
	  69,	  16,	  48,
	  70,	  32,	  48,
	  19,	  48,	  48,
	  71,	  64,	  48,
	  72,	  80,	  48,
	  73,	  96,	  48,
	  38,	  112,	  48,
	  74,	  0,	  64,
	  75,	  16,	  64,
	  76,	  32,	  64,
	  77,	  48,	  64,
	  78,	  64,	  64,
	  79,	  80,	  64,
	  80,	  96,	  64,
	  46,	  112,	  64,
	  47,	  0,	  80,
	  48,	  16,	  80,
	  81,	  32,	  80,
	  82,	  48,	  80,
	  83,	  64,	  80,
	  52,	  80,	  80,
	  53,	  96,	  80,
	  54,	  112,	  80,
		0xffff,};
const INT32U  pub_delSp_Frame[] = {
	(const INT32U) pub_delSp_Frame00Cell,
	(const INT32U) pub_delSp_Frame01Cell,
};
