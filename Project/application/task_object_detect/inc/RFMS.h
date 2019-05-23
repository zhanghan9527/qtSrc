#ifndef _RFMS_H_
#define _RFMS_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "define.h"

	//#define MIN(a,b)  ((a) > (b) ? (b) : (a))
	//#define MAX(a,b)  ((a) < (b) ? (b) : (a))
	//#define vAbs(a)  ((a) > 0 ? (a) : (-a))


#define PATCH_SZ  20
#define PA1 24


	//©wÂI«Å§i
#define BasicOne  1
#define V0P81_INT20P12  3317




	/* Wavelet size at first layer of first octave. */ 
#define HAAR_SIZE0  9   

	/* Radius of the circle in which to sample gradients to assign an 
	orientation */
#define ORI_RADIUS  6 

#define max_ori_samples  (2*ORI_RADIUS+1)*(2*ORI_RADIUS+1)
	//static int nangle0 = 0;

	/* Wavelet size increment between layers. This should be an even number, 
	such that the wavelet sizes in an octave are either all even or all odd.
	This ensures that when looking for the neighbours of a sample, the layers
	above and below are aligned correctly. */
#define HAAR_SIZE_INC  6



#define DATABASE_RFMS_NUM_LIMIT 200
#define DETECT_RFMS_NUM_LIMIT 300
#define MATCHING_RFMS_NUM_LIMIT 200



	typedef struct DataArr
	{

		unsigned short width;
		unsigned short height;
		unsigned short step;
		int *i;

	} DataArr;


	typedef struct 
	{
		unsigned short x;
		unsigned short y;
	}ImgPoint2D;

	typedef struct 
	{
		short x;
		short y;
	}DataPoint2D;

	typedef struct 
	{
		ImgPoint2D pt;
		char laplacian;
		short size;
		int dir_INT;
		//float dir;  //float check
		//unsigned short  hessian;

	} RFMSPoint_INT;


	typedef struct 
	{
		//int descriptor_INT[64];
		int descriptor_short[32];
	} RFMSDescriptor;

	


	typedef struct 
	{
		unsigned short hessianThreshold;
		unsigned char nOctaves;
		unsigned char nOctaveLayers;
	}RFMSParams;


	typedef struct 
	{
		int p0, p1, p2, p3;
		unsigned int w_INT17P15;   //for dx, dy
		unsigned int w_INT20P12;   //for dxx, dyy, dxy
	}RFMSHF;

	typedef struct 
	{
		unsigned char T[100];
		unsigned char D[100];
		unsigned char L[100];
		unsigned char R[100];
		unsigned char TCnt;
		unsigned char DCnt;
		unsigned char LCnt;
		unsigned char RCnt;
	}CardFPStruct;

    typedef struct 
	{ 
		unsigned int *cardsRFNArr;  //record cards RF points number.
		RFMSPoint_INT *objKeypoints;
		RFMSDescriptor *objDescriptors;

	} objDatabase;

	typedef struct 
	{ 

		//unsigned short *cardsRFNArr;  //record cards RF points number.
		unsigned int *cardsMatchingCnt;  //cards Matching counter.

	}cardsInformation;



	typedef struct 
	{
		//extraction section
		DataArr sum;         //integral image
		DataArr detsResponse[2];   // det response
		DataPoint2D apt[169]; 
		//max 400 featrue point
		RFMSPoint_INT imgKeypoints[DETECT_RFMS_NUM_LIMIT];
		RFMSDescriptor imgDescriptors[DETECT_RFMS_NUM_LIMIT];
		objDatabase objData;
		/*RFMSPoint_INT objKeypoints[cardsN*DATABASE_RFMS_NUM_LIMIT];
		RFMSDescriptor objDescriptors[cardsN*DATABASE_RFMS_NUM_LIMIT];*/
		cardsInformation cardsData;
		int apt_w_INT2P30[169];
		RFMSParams params;
		RFMSHF Dxx_opt[16];
		RFMSHF Dyy_opt[16];
		RFMSHF Dxy[32];
		RFMSHF Dx[8];
		RFMSHF Dy[8];
		//RFMSDescriptor *objDes_ptr;
		int PATCH[144];   // 21 x 21, create 24 x 24
		int buff_INT_vec[64];
		int cardsN;



		//matching section
		DataPoint2D rotationMatchingP[MATCHING_RFMS_NUM_LIMIT];
		unsigned short recordMIT[MATCHING_RFMS_NUM_LIMIT];
		unsigned short patternWidth;
		unsigned short patternHeight;
		int RMCnt;
		int matchingCnt;
		unsigned int maxCnt;
		unsigned int matchingThre;
		unsigned int defultMatchingThre;
		unsigned int matchingThreRangeV;
		unsigned int extractionThre;
		unsigned int minExtractionThre;
		int incExtractionThre;
		int decExtractionThre;
		int startMatchingPointN;


		//unsigned char *win_bufs_arr_ptr;

		//find paire array
		unsigned short matchingIndexTable[MATCHING_RFMS_NUM_LIMIT];  
		unsigned short rematchedTable[360];
		unsigned short degreeHist[360];
		CardFPStruct obj_CFPS[100];
		CardFPStruct img_CFPS[100];
		char matchingTable_T[MATCHING_RFMS_NUM_LIMIT];
		char matchingTable_D[MATCHING_RFMS_NUM_LIMIT];
		char matchingTable_L[MATCHING_RFMS_NUM_LIMIT];
		char matchingTable_R[MATCHING_RFMS_NUM_LIMIT];

		unsigned char matchingDatabaseUI[100];

		//int matchingError[100];


	}globalData;


	typedef struct 
	{
		int width;
		int height;
	}DataSize;

	/*
	typedef struct
	{
	short width;
	short height;
	short widthStep;
	char  ch;
	char  format;
	unsigned char *ptr;
	} gpImage;*/


	RFMSParams Params(int threshold);
	int sin_INT(int x);
	int cos_INT(int x);
	int sqrtu32(unsigned int a);
	unsigned char exponent32(long in);
	unsigned int udiv32(unsigned int d, unsigned int n);
	int divFun(int a, int b);
	void GPSmoothImage(unsigned char *inputImg, unsigned char *outputImg, int RFMS_ImgW, int RFMS_ImgH);
	int calcHaarPatternN2_INT( const int* origin, const RFMSHF* f);
	int calcHaarPatternN3_INT( const int* origin, const RFMSHF* f);
	int calcHaarPatternN4_INT( const int* origin, const RFMSHF* f);
	unsigned int compareRFMS_INT( const RFMSDescriptor* d1, const RFMSDescriptor* d2, int best, int length );
	unsigned int * RFMS_cardsMatchingCnt_ptr(void* workmem);
	int checkINTElemt(unsigned int bytes);
	extern int gp_hw_checking(void);
	extern int gp_check_efuseID(int bit);
	const char *RFMS_GetVersion(void);






	//========================================================
	// Function Name :  RFMS_get_memory_size
	// Syntax : unsigned int RFMS_get_memory_size(int width, int height);
	// Purpose :  compute total working memory size
	// Parameters : int width: detect image width
	//              int height: detect image height
	// Return : working memory size (bytes)
	//========================================================
	unsigned int RFMS_get_memory_size(int width, int height);

	//========================================================
	// Function Name :  RFMS_init
	// Syntax : void RFMS_init(int MaxWidth, int MaxHeight, int *workmem, int cardsN, int *cardsData, unsigned short patternWidth, unsigned short patternHeight);
	// Purpose :  init system
	// Parameters : int MaxWidth:    detect image width
	//              int MaxHeight:   detect image height
	//              int *workmem:    allocated working memory pointer
	//              unsigned short patternWidth:     detect object model image width
	//              unsigned short patternHeight:    detect object model image height
	//              int cardsN:      database object number
	// Return : void
	//========================================================
	void RFMS_init(int MaxWidth, int MaxHeight, int *workmem, unsigned short patternWidth, unsigned short patternHeight, int cardsN, char *objdataBuf);


	//========================================================
	// Function Name :  RFMS_ParmSet
	// Syntax : void RFMS_ParamSet(int *workmem, unsigned int matchingThre, unsigned int matchingThreRangeV, unsigned int minExtractionThre, 
	//			                  int incExtractionThre, int decExtractionThre);
	// Purpose :  set system parm
	// Parameters : int *workmem:                        working memory pointer
	//              unsigned int extractionThre:         set defult feature point threshold
	//              unsigned int matchingThre:           set defult matching threshold value
	//              unsigned int matchingThreRangeV:     set defult matching threshold range
	//              unsigned int minExtractionThre:      set min extract point threshold value
	//              int incExtractionThre:               set incerease extract point threshold value
	//              int decExtractionThre:               set decerease extract point threshold value
	//              int startMatchingPointN:             set start matching's point number
	// Return : void
	//========================================================
	void RFMS_ParamSet(int *workmem, unsigned int extractionThre, unsigned int matchingThre, unsigned int matchingThreRangeV, unsigned int minExtractionThre, 
		int incExtractionThre, int decExtractionThre, int startMatchingPointN);



	//========================================================
	// Function Name :  RFMS_extractPoint
	// Syntax : int RFMS_extractPoint(void* workmem, gpImage *inputImage);
	// Purpose :  extraction RFMS feature point include point position and size
	// Parameters : 
	//              int *workmem:    allocated working memory pointer
	//              gpImage *inputImage:    inpute detect gray image
	// Return : RFMS point total number
	//========================================================
	int RFMS_extractPoint(void* workmem, gpImage *inputImage);

	//========================================================
	// Function Name :  RFMS_findPairs
	// Syntax : int RFMS_findPairs(void* workmem, int imageTotal, int cardsN);
	// Purpose :  matching feature point to recognize the object
	// Parameters : 
	//              int *workmem:    allocated working memory pointer
	//              int imageTotal:  detect inpute image's total feature number
	// Return : matching object result
	//========================================================
	int RFMS_findPairs(void* workmem, int imageTotal);

	int objDistanceDetect( unsigned char *bitOneCntTable, int imageWidth,  int imageHeight, int diffThreM , int diffThreLR);

    int ModEqualizeHist(gpImage *orgimg, gpImage *outimg, gpRect *roi);








#ifdef __cplusplus
}
#endif

#endif


