#ifndef __FIXED_H__
#define __FIXED_H__




#ifdef FIXED_PT


#define FIXED_TYPE	int



#define FACTOR_BITS	25
#define FACTOR_PRECISION	(1 << FACTOR_BITS)
#define FACTOR_CONST(A)		(((A) >= 0) ? ((int)((A)*(FACTOR_PRECISION)+0.5)) : ((int)((A)*(FACTOR_PRECISION)-0.5)))


#define FILTER_BITS 17
#define FILTER_PRECISION	(1 << FILTER_BITS)
#define FILTER_CONST(A)		(((A) >= 0) ? ((int)((A)*(FILTER_PRECISION)+0.5)) : ((int)((A)*(FILTER_PRECISION)-0.5)))


#define SINCOS_BITS 14
#define SINCOS_PRECISION	(1 << SINCOS_BITS)
#define SINCOS_CONST(A)		(((A) >= 0) ? ((int)((A)*(SINCOS_PRECISION)+0.5)) : ((int)((A)*(SINCOS_PRECISION)-0.5)))


#define FACIAL_COEF_BITS 18
#define FACIAL_COEF_PRECISION		(1 << FACIAL_COEF_BITS)
#define FACIAL_COEF_CONST(A)		(((A) >= 0) ? ((int)((A)*(FACIAL_COEF_PRECISION)+0.5)) : ((int)((A)*(FACIAL_COEF_PRECISION)-0.5)))



// 四捨五入
#define FACTOR_MUL_C(A,B)			(((__int64)(A)*(int)(B)+(1 << (FACTOR_BITS-1))) >> FACTOR_BITS)
#define SINCOS_MUL_C(A,B)			(((int)(A)*(int)(B)+(1 << (SINCOS_BITS-1))) >> SINCOS_BITS)
#define FACIAL_MUL_C(A,B)			(((int)(A)*(int)(B)+(1 << (FACIAL_COEF_BITS-1))) >> FACIAL_COEF_BITS)

// 無條件進位
//#define FACIAL_MUL_C_C(A,B)			(((((int)(A)*(int)(B)) & 0x3FFFF) == 0) ? (((int)(A)*(int)(B)) >> FACIAL_COEF_BITS) : ((((int)(A)*(int)(B)) >> FACIAL_COEF_BITS) + 1))
#define FACIAL_MUL_C_C(A,B)			(((((int)(A)*(int)(B)) & 0x3FFFF) < 0xCC00) ? (((int)(A)*(int)(B)) >> FACIAL_COEF_BITS) : ((((int)(A)*(int)(B)) >> FACIAL_COEF_BITS) + 1))
																	// ↑about 0.199

#define FACIAL_MUL_C_C3(A,B)		(((((int)(A)*(int)(B)) & 0x3FFFF) < 0x39999) ? (((int)(A)*(int)(B)) >> FACIAL_COEF_BITS) : ((((int)(A)*(int)(B)) >> FACIAL_COEF_BITS) + 1))
																	// ↑about 0.899

// 無條件捨去
#define FACIAL_MUL_C_D(A,B)			(((int)(A)*(int)(B)) >> FACIAL_COEF_BITS)



#else


#define FIXED_TYPE	double


#define FILTER_CONST(A)			(A)
#define FACIAL_COEF_CONST(A)	(A)


// 四捨五入
#define FACIAL_MUL_C(A,B)			((A)*(B))

#define FACIAL_MUL_C_D(A,B)			int((A)*(B))

#endif


#endif	// __FIXED_PT_H__
