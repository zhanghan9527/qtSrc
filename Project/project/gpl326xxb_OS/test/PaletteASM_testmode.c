#include	"sprite.h"
const INT16U ALIGN4 _test_Palette1[]={

	0x801F, 0x7FFF, 0x7C00,	0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318, 0x6318,
};

const INT16U ALIGN4   test_ASCII_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0|  SP_VSIZE32|  SP_HSIZE16|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR4,
	SP_PBANK1,
	  1,	  0,	  0,
		0xffff,};
const INT32U  test_ASCII_Frame[] = {
	(const INT32U) test_ASCII_Frame00Cell,
};

const INT16U ALIGN4   RedDot_Frame00Cell[] = {
	SPBLEND_DISABLE|  SP_DEPTH1|  SP_PALETTE0|  SP_VSIZE32|  SP_HSIZE32|  SPVFLIP_DISABLE|  SPHFLIP_DISABLE|	   SP_COLOR4,
	SP_PBANK1,
	  48,	  0,	  0,
		0xffff,};
const INT32U  RedDot_Frame[1] = {
	(const INT32U) RedDot_Frame00Cell,
};




