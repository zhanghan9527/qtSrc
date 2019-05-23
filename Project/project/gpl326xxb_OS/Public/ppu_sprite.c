#include "sprite.h"
#include "driver_l1.h"
#include "drv_l1_sfr.h"
#include "data.h"
#include "math.h"


INT32U PPU_MemSet (INT32U *pDest, INT32U uValue, INT32S nBytes,INT16U mem_copy_8_16_32);
 // Spritebuf and SpriteRAM Memory management
 
INT8S	SP_Disble[Spritebuf_number];
extern SPRITE	Spritebuf[Spritebuf_number*sizeof(SPRITE)];
extern SpN_ptr Sprite_ptr[Sprite_ptr_number*sizeof(SpN_ptr)];
extern SpN_RAM	SpriteRAM[SpriteRAM_number*sizeof(SpN_RAM)];
extern FP32 HGridBuff[SP25D_MAX_CHAR_V+1][SP25D_MAX_CHAR_H+1];             //V3D Horizontal grid buffer
extern FP32 VGridBuff[SP25D_MAX_CHAR_V+1][SP25D_MAX_CHAR_H+1];             //V3D Vertical grid buffer
#if (MCU_VERSION >= GPL326XX)
extern SpN_EX_RAM SpriteExRAM[SpriteExRAM_number*sizeof(SpN_EX_RAM)];
#endif

// Gobel value
extern INT32U nV_Size,nH_Size;
extern INT32U ExSpritebuf,Sprite_ex_ptr,ExSpriteRAM,EXSP_NUM;

void	SetSpRotate(int nSpIdx, int nRotate);

void	SetSpZoom1(INT16U nSpIdx, INT16U nZoom,INT16U CenterX,INT16U CenterY);
void paint_ppu_spriteram_cell3(PPU_REGISTER_SETS *p_register_set,Sprite_Coordinate_Mode display_mode, Coordinate_Change_Mode coordinate_mode, INT16U sprite_number);
void SetSpriteBlend(INT32U sprite_number, INT32U nBlend,INT32U Value);
void set_sprite_posion(INT32U sprite_number,INT16S sprite_pos_x,INT16S sprite_pos_y);


void set_sprite_posion(INT32U sprite_number,INT16S sprite_pos_x,INT16S sprite_pos_y)
{
  Spritebuf[sprite_number].nPosX=sprite_pos_x;
  Spritebuf[sprite_number].nPosY=sprite_pos_y;
}

void set_sprite_x(INT32U sprite_number,INT32S sprite_pos_x)
{
	Spritebuf[sprite_number].nPosX=sprite_pos_x;
}

void set_sprite_y(INT32U sprite_number,INT32S sprite_pos_y)
{
	Spritebuf[sprite_number].nPosY=sprite_pos_y;
}

void set_sprite_posInc(INT32U sprite_number,INT16S inc_x,INT16S inc_y)
{
  Spritebuf[sprite_number].nPosX+=inc_x;
  Spritebuf[sprite_number].nPosY+=inc_y;
}

INT16S get_sprite_x(INT32U sprite_number)
{
	return Spritebuf[sprite_number].nPosX;
}
INT16S get_sprite_y(INT32U sprite_number)
{
	return Spritebuf[sprite_number].nPosY;
}
void set_sprite_nCharNum(INT32U sprite_number,INT32U idx)
{
	Spritebuf[sprite_number].nCharNum=idx;
}
INT32U get_sprite_spcell(INT32U sprite_number)
{
	return (INT32U)(Spritebuf[sprite_number].SpCell);
}
void set_sprite_spcell(INT32U sprite_number,INT32U sprite_ptr)
{
	Spritebuf[sprite_number].SpCell=(INT32U *)sprite_ptr;
	SP_Disble[sprite_number]=1;
}
void set_sprite_Size(INT32U sprite_number,int x,int y)
{
	Spritebuf[sprite_number].nSizeX=x;
	Spritebuf[sprite_number].nSizeY=y;
}

INT32U get_sprite_addr(INT16S no)
{
	return (INT32U)(&Spritebuf[no]);
}

//删除一个卡通，将后面的卡通向前移
void moveForward1Sprite(int nowsprite,int totalSpNo)
{
	int j;
	for(j=nowsprite;j<=totalSpNo;j++) Copy8(&Spritebuf[10+j],&Spritebuf[10+j+1],sizeof(SPRITE));
}

void save_stamp_spr(INT32U stampspr_buff)
{
	Copy32(stampspr_buff,&Spritebuf[10],sizeof(SPRITE)*100);	
}

void recover_stamp_spr(INT32U stampspr_buff)
{
	Copy32(&Spritebuf[10],stampspr_buff,sizeof(SPRITE)*100);	
}


								
void clear_sprite(int m)
{
	int i;
	for(i=0;i<m;i++){
		Spritebuf[i].nCharNum=0;
		Spritebuf[i].SpCell=0;
		Spritebuf[i].nSizeX=0;
		Spritebuf[i].nSizeY=0;
		Spritebuf[i].uAttr0=0;
		Spritebuf[i].uAttr1=0;	
			
	}
}
void ClearSomeSp(INT16S st,INT16S end)
{
	INT16S i;
	for(i=st;i<=end;i++){
		Spritebuf[i].nCharNum=0;
		Spritebuf[i].SpCell=0;
		Spritebuf[i].nSizeX=0;
		Spritebuf[i].nSizeY=0;
		Spritebuf[i].uAttr0=0;
		Spritebuf[i].uAttr1=0;	
	}
}

//  nBlend:0--63		Value:0:Disable,1:enable;	
void SetSpriteBlend(INT32U sprite_number, INT32U nBlend,INT32U Value)
{
	int k;
														
//	if( Spritebuf[sprite_number].SpCell != 0 )	// check sprite exist/ non-exist
	{
		k = Spritebuf[sprite_number].uAttr1 & 0xc0ff;	
		Spritebuf[sprite_number].uAttr1 = k | (nBlend << 8);
		if(Value==0) 
			Spritebuf[sprite_number].uAttr0 &=~0x4000;
		else 
			Spritebuf[sprite_number].uAttr0 |= (Value<<14);
	}	 	
}

void	SetSpDepth(int nSpIdx, int nDepth)	 //SP_DEPTH1
{
//	if( Spritebuf[nSpIdx].SpCell != 0 )	// check sprite exist/non-exist
	{
		Spritebuf[nSpIdx].uAttr0 &= ~0x3000;
		Spritebuf[nSpIdx].uAttr0 |= nDepth;
	}
}
void Set_Sprite_Sp_bank(INT16U *cell,int SP_PBANK)
{
	cell[1]=SP_PBANK;
}
void Set_Sprite_Group_bank(INT32U cell[],INT32U SP_PBANK,INT32U num)
{
	int i;
    
	for(i=num-1;i>=0;i--){
		Set_Sprite_Sp_bank((INT16U*)cell[i],SP_PBANK);
	}
			
}
void	SetSpPalette(int nSpIdx, int nPALETTE)	 //SP_PALETTE0
{
	Spritebuf[nSpIdx].uAttr0 &= ~0x0F00;
	Spritebuf[nSpIdx].uAttr0 |= nPALETTE;
}


void Set_Sprite_Idx_cpy(INT16U *cell,INT16U *res_cell)
{
	cell[2]=res_cell[2];
}

void Set_Sprite_Palette(INT16U *cell, int nPALETTE)
	 //SP_PALETTE0)
{
	cell[0]=(cell[0]&~0x0F00)|nPALETTE;
}

//================================================================================
//	Description:	Set Sprite rotate level
//	Function:		SetSpRotate()
//	Syntax:			void	SetSpRotate(int nSpIdx, int nRotate);
//	Input Paramter:	int nSpIdx:	Sprite index (0~255)
//					int nRotate: Rotate level (0~63)
//	Return: 		none
//================================================================================
void	SetSpRotate(int nSpIdx, int nRotate)
{
	int k;

//	if( Spritebuf[nSpIdx].SpCell != 0 )	// check sprite exist/non-exist
	{
//		if ((Spritebuf[nSpIdx].nCharNum & 0x8000) == 0)
		{
			k = Spritebuf[nSpIdx].nSizeX & 0x03ff;
			Spritebuf[nSpIdx].nSizeX = k | (nRotate << 10);
		}
	}
}

//================================================================================
//	Description:	Set Sprite zoom level
//	Function:		SetSpZoom()
//	Syntax:			void	SetSpZoom(int nSpIdx, int nZoom);
//	Input Paramter:	int nSpIdx:	Sprite index (0~255)
//					int nZoom: Zoom level (0~63)
//	Return: 		none
//================================================================================
void	SetSpZoom(int nSpIdx, int nZoom)
{
	int k;

//	if( Spritebuf[nSpIdx].SpCell != 0 )	// check sprite exist/ non-exist
	{
//		if ((Spritebuf[nSpIdx].nCharNum & 0x8000) == 0)
		{
			k = Spritebuf[nSpIdx].nSizeY & 0x03ff;
			Spritebuf[nSpIdx].nSizeY = k | (nZoom << 10);
		}
	}
}

void	SetSpZoom_cell4(INT16U nSpIdx, INT16U nZoom,INT16U CenterX,INT16U CenterY)
{
	int k;

//	if( Spritebuf[nSpIdx].SpCell != 0 )	// check sprite exist/ non-exist
	{
//		if ((Spritebuf[nSpIdx].nCharNum & 0x8000) == 0)
		{
			k = Spritebuf[nSpIdx].nSizeX & (~0x03ff);
			Spritebuf[nSpIdx].nSizeX =k | (CenterX<<1);
			Spritebuf[nSpIdx].nSizeY =(nZoom << 10) | (CenterY<<1);
		}
	}
}
//nFlip: hflip_bit|vflip_bit;  
void SetSpriteFlip_cell4(INT32U nSpIdx, INT32U nFlip,INT16U CenterX,INT16U CenterY)
{
	int k;
	
	k = Spritebuf[nSpIdx].nSizeY & (~0x03ff);
    Spritebuf[nSpIdx].nSizeY =k | (CenterY<<1);
	k = Spritebuf[nSpIdx].nSizeX & (~0x03ff);    
	Spritebuf[nSpIdx].nSizeX =k | (CenterX<<1);
	k = Spritebuf[nSpIdx].uAttr0 & 0xfff3;	
	Spritebuf[nSpIdx].uAttr0 = k | (nFlip);	 	
}

//note: Size: must be 64x64 32x32
// CencerX,是相对卡通的左上角的中心坐标
void SpriteZoomXyadjust(INT16U sprite_number,INT16U nZoom,int Size,int CencerX,int CencerY)
{
	int k=0;
	int x,y,m=0,n=0;
	INT16U* ptr;
	if( Spritebuf[sprite_number].SpCell != 0 )	// check sprite exist/ non-exist
	{
		ptr=(INT16U*)Spritebuf[sprite_number].SpCell+2;
		if((nZoom!=0)&&(nZoom!=32)){
			if(nZoom<=32){
				m=nZoom;
				n=Size/2;
			}else{
				m=(nZoom-28)*8;
				n=Size/2;
			}
		}
		
		while(*ptr!=0xffff){
			if(m!=0){
				
				x=*(ptr+1);
			
				x=(x-CencerX+n)*m/32+CencerX-n;
				y=*(ptr+2);
		
				y=(y-CencerY+n)*m/32+CencerY-n;
				
				*(ptr+1)=x;
				*(ptr+2)=y;
				
			}
			ptr+=3;
		}
	}
}


//center：中心到卡通左上角的距离
//xsize:是卡通每一小块的大小
//注释 对坐标的原始公式 center*(32-zoom)/32-(xsize/2)*(32-zoom)/32+(zoom*xsize/32)*x/xsize;
void SpriteZoomXyadjust1(INT16U sprite_number,INT16U nZoom,int Size,int CencerX,int CencerY)
{
	int k=0;
	int x,y,m=0,n=0;
	INT16U* ptr;
	if( Spritebuf[sprite_number].SpCell != 0 )	// check sprite exist/ non-exist
	{
		ptr=(INT16U*)Spritebuf[sprite_number].SpCell+2;
		if((nZoom!=0)&&(nZoom!=32)){
			if(nZoom<=32){
				m=nZoom;
				n=Size/2;
			}else{
				m=(nZoom-28)*8;
				n=Size/2;
			}
		}
		
		while(*ptr!=0xffff){
			if(m!=0){
				
				x=*(ptr+1);
			
				x=(CencerX-n)*(32-m)/32+m*x/32;
				y=*(ptr+2);
				y=(CencerY-n)*(32-m)/32+m*y/32;
				
				
				*(ptr+1)=x;
				*(ptr+2)=y;
				
			}
			ptr+=3;
		}
	}
}

 /******************************************************************************* 
Rotate:1--64;
zoom:0--63;
xCenter,yCenter:自定义的中心点坐标
*******************************************************************************/
void SetSprite_RZ_AdjustXy(INT16U sprite_number,int Rotate,int zoom,int xCenter,int yCenter)
{
	int	i;
	
	INT16U uAttribute0;
	INT16S *p_cell;
    INT16U k;
	double dx,dy,dr;
	int nH_Size,nV_Size;	//H,V, Center Size
	int newX,newY;
	double dCosA,dSinA,dCosB,dSinB,dCosC,dSinC;
	double dxtemp[30],dytemp[30];
	double dTgC[30];
	int ixtemp[30],iytemp[30];
		
	p_cell=(INT16S*)Spritebuf[sprite_number].SpCell;
	
	k = Spritebuf[sprite_number].nSizeX & 0x03ff;
	Spritebuf[sprite_number].nSizeX = k | (Rotate << 10);
				
	uAttribute0 = *p_cell;
	p_cell+=2;
	nH_Size = SPSIZE[(uAttribute0>>4)&0x03]/2;
	nV_Size = SPSIZE[(uAttribute0>>6)&0x03]/2;
	dCosA=cos(5.625*Rotate*(M_PI/180));
	dSinA=sin(5.625*Rotate*(M_PI/180));

	if(zoom>32)zoom=(zoom-28)*8;
	else if(zoom==32)zoom=0;
	
	for(i=0;*p_cell++!=0xffff;i++){				
		//RZ ---x-cell center x--相对旋转原点坐标--zoom---turn angle--new x
		newX=*p_cell++;
		newY=*p_cell++;
		newX=newX+nH_Size;
		newY=newY+nV_Size;
		dx=newX-xCenter;
		dy=newY-yCenter;
		//相对旋转原点 zoom
		if(zoom){
			dx=dx*(zoom/32.0);
			dy=dy*(zoom/32.0);
		}
		dr=sqrt(dx*dx+dy*dy);
		dCosB=dx/dr;		//原角度cos,sin
		dSinB=dy/dr;
		dCosC=dCosA*dCosB-dSinA*dSinB;	//旋转后角度cos,sin
		dSinC=dSinA*dCosB+dCosA*dSinB;

		dxtemp[i]=dr*dCosC;
		dytemp[i]=dr*dSinC;
		if(dCosC>-0.000001 && dCosC<0.000001) dTgC[i]=0;
		else dTgC[i]=dSinC/dCosC;
	}
	
	p_cell=(INT16S*)Spritebuf[sprite_number].SpCell+2;
	
	for(i=0;*p_cell!=0xffff;i++,p_cell+=3){
		if((dTgC[i]>-0.000001 && dTgC[i]<0.000001)||(dTgC[i]==1)){
			ixtemp[i]=(int)dxtemp[i];
			iytemp[i]=(int)dytemp[i];
		}else if(dTgC[i]>=1){
			ixtemp[i]=(int)dxtemp[i];
			iytemp[i]=(int)dytemp[i];
			if(iytemp[i]>1) iytemp[i]--;
			else if(iytemp[i]<-1) iytemp[i]++;
		}else{
			ixtemp[i]=(int)dxtemp[i];
			iytemp[i]=(int)dytemp[i];
			if(ixtemp[i]>1) ixtemp[i]--;
			else if(ixtemp[i]<-1) ixtemp[i]++;
		}
		if(ixtemp[i]>64) ixtemp[i]--;
		else if(ixtemp[i]<-64) ixtemp[i]++;
	}
	
	p_cell=(INT16S*)Spritebuf[sprite_number].SpCell+2;

	for(i=0;*p_cell!=0xffff;i++,p_cell+=3){
		//回复到原坐标系
		newX=ixtemp[i]+xCenter;
		newY=iytemp[i]+yCenter;
		newX=newX-nH_Size;
		newY=newY-nV_Size;
		
		*(p_cell+1)=newX;
		*(p_cell+2)=newY;
	}
	
}

const INT32U SPSIZE[]={
8,16,32,64,128,256,
};

void paint_ppu_spriteram_cell4(PPU_REGISTER_SETS *p_register_set,Sprite_Coordinate_Mode display_mode, Coordinate_Change_Mode coordinate_mode, INT16U sprite_number)
{
   INT16S temp_x,temp_y,cx,cy;
   INT16U nZoom,m,Rotate;
   INT16U CencerX,CencerY;
   INT16U uAttribute0,uAttribute1,uAttribute2;
   INT16U *p_cell;
   INT16U Numidx,SPBank;
   
   INT32U i,j,sprite_large_size_enable;
   INT32S nPosXN, nPosYN;
   INT32S nScrWidth, nScrHeight;
   PSPRITE nSprite; 
   PSpN_RAM sprite_attr;

//   PSpN16_CellIdx pSpCell;
   PSpN_ptr  sp_ptr;

   switch(display_mode)
   {
       case Sprite_Coordinate_320X240:
            nScrWidth = 320;
		    nScrHeight = 240; //+ 14;
            break;
       case Sprite_Coordinate_640X480:
       		nScrWidth = 640;
		    nScrHeight = 480 + 14;
            break;         
       case Sprite_Coordinate_480X234:
       		nScrWidth = 480;
		    nScrHeight = 234;       
            break;
       case Sprite_Coordinate_480X272:
            nScrWidth = 480;
		    nScrHeight = 272;  
            break;
       case Sprite_Coordinate_720X480:
       		nScrWidth = 720;
		    nScrHeight = 480;         
            break;     
       case Sprite_Coordinate_800X480:
       		nScrWidth = 800;
		    nScrHeight = 480;         
            break;         
       case Sprite_Coordinate_800X600:
       		nScrWidth = 800;
		    nScrHeight = 600;         
            break;
       case Sprite_Coordinate_1024X768:
       		nScrWidth = 1024;
		    nScrHeight = 768;         
            break;
       case Sprite_Coordinate_128X128:
       		nScrWidth = 128;
		    nScrHeight = 128;         
            break;
       case Sprite_Coordinate_240X320:
       		nScrWidth = 240;
		    nScrHeight = 320;         
            break;
       default:
            nScrWidth = 640;
		    nScrHeight = 480 + 14;
            break;      
    }                      
    sprite_large_size_enable=0;
    sprite_attr=(PSpN_RAM)SpriteRAM;
    fill_mem((INT32U)sprite_attr,SpriteRAM_number*sizeof(SpN_RAM),0);
    //PPU_MemSet ((INT32U *)sprite_attr,0,SpriteRAM_number*sizeof(SpN_RAM),32);
    nSprite=(PSPRITE)Spritebuf;
    sp_ptr=(PSpN_ptr)Sprite_ptr;
    //PPU_MemSet ((INT32U *)sp_ptr,0,Sprite_ptr_number*sizeof(PSpN_ptr),32);
    fill_mem((INT32U)sp_ptr,Sprite_ptr_number*sizeof(PSpN_ptr),0);

    for(i=0;i<sprite_number;i++) {// Check sprite number?
	   if(nSprite->SpCell!= 0) { // Check sprite if exist?			  		
		  //sprite ram start address
		  p_cell = (INT16U *)nSprite->SpCell;
		  uAttribute0 = *p_cell | nSprite->uAttr0;
		  uAttribute1 = nSprite->uAttr1;
		  uAttribute2 = 0;
		  SPBank=*(++p_cell);
		  nZoom = (INT16U)(nSprite->nSizeY & ~0x03ff)>>10;
		  Rotate = (INT16U)(nSprite->nSizeX & ~0x03ff)>>10;	 
		  if(nZoom && Rotate==0){
			  CencerX = (INT16U)(nSprite->nSizeX & 0x03ff)>>1;
			  CencerY = (INT16U)(nSprite->nSizeY & 0x03ff)>>1;
			  if(nZoom>32){
	  			m=(nZoom-28)*8;
	  		  }else m=nZoom;
  		  }
  		  
		  sp_ptr->nSPNum_ptr=(INT32U)sprite_attr;

		  // MAX Sprite Characters Combination Mode	
//		  for(j=0; ; j++)
		  while(1)
		  {
//			  pSpCell = (PSpN16_CellIdx) nSprite->SpCell+j;
              Numidx=*(++p_cell);
			  if( Numidx == data_end_word )
			  {
			        // Check Sprite Characters Combination Flag
//					sp_ptr->nSP_CharNum=j;
//					sp_ptr->nSP_Hsize=nH_Size;
//			        sp_ptr->nSP_Vsize=nV_Size;
					break;
			  }
			  // Sprite RAM - Character Number
			  sprite_attr->nCharNumLo_16 = Numidx +nSprite->nCharNum ;//pSpCell->nCharNumLo;
			  // SpN Rotate
			  sprite_attr->uPosX_16 = Rotate<<10;//pSpCell->nRotate;
			  // SpN Zoom
			  sprite_attr->uPosY_16 = nZoom<<10;//pSpCell->nZoom;
			  // Sprite RAM - Attribute 0
			  if((p_register_set->ppu_palette_control & p1024_enable) == p1024_enable)
			  {
			    if(SPBank == SP_PBANK3){
			       uAttribute0 |= (1<<pal_control_1);
			       uAttribute1 |= (1<<pal_control_0);
			    }else if(SPBank == SP_PBANK2){
			       uAttribute0 |= (1<<pal_control_1);
			       uAttribute1 |= (0<<pal_control_0);
			    }else if(SPBank == SP_PBANK1){
		           uAttribute0 |= (0<<pal_control_1);
			       uAttribute1 |= (1<<pal_control_0);
		        }else {
		           uAttribute0 |= (0<<pal_control_1);
			       uAttribute1 |= (0<<pal_control_0);
		        }
		      }else{
			    if(SPBank == SP_PBANK3)
			       uAttribute0 |= (1<<pal_control_1);
		        else
		           uAttribute0 |= (0<<pal_control_1);
		      }
 
			  sprite_attr->attr0 = uAttribute0;
		      // Sprite RAM - Attribute 1
//		      uAttribute1 |= pSpCell->nCharNumHi;
//		      uAttribute1 |= pSpCell->nMosaic;
//		      uAttribute1 |= pSpCell->nBldLvl;
		      sprite_attr->attr1 = uAttribute1;

              if(sprite_large_size_enable == 0)
              {			  
		          //Get sprite H size
		          nH_Size = SPSIZE[(uAttribute0>>4)&0x03];
		          nV_Size = SPSIZE[(uAttribute0>>6)&0x03];
			  }
			  temp_x=*(++p_cell);
			  temp_y=*(++p_cell);
			    
			  if(nZoom && Rotate==0){
			  	temp_x=(temp_x-CencerX+(nV_Size>>1))*m/32+CencerX-(nV_Size>>1);
			  	temp_y=(temp_y-CencerY+(nV_Size>>1))*m/32+CencerY-(nV_Size>>1);
			  }
			  
			  if((uAttribute0 & (hflip_bit|vflip_bit)))
			  {
			     cx = (nSprite->nSizeX & spirte_pos_max_bit) - nH_Size;  // get maximum x position
                 cy = nV_Size - (nSprite->nSizeY & spirte_pos_max_bit);  // get minimum y position
                 if((uAttribute0 & hflip_bit) != 0)          // HFlip 						
                    temp_x = cx - temp_x;					
                 if((uAttribute0 & vflip_bit) != 0)          // VFlip				
                    temp_y = cy - temp_y;
              }    
			  // coordinates change  			     
			  if (coordinate_mode == Center2LeftTop_coordinate) 
			  {
				 // Center2LeftTop_coordinate
				 nPosXN = (nScrWidth>>1) + (nH_Size>>1) + 1 + nSprite->nPosX + temp_x;
				 //nPosXN = (nScrWidth>>1) + temp_x;
				 nPosYN = (nScrHeight>>1) - 1 - nSprite->nPosY + temp_y - (nV_Size>>1);	                               
                 //nPosYN = (nScrHeight>>1) - temp_y; 				            
			  } 
			  else if (coordinate_mode == LeftTop2Center_coordinate)
			  {
				 // LeftTop2Center_coordinate
				 nPosXN = nSprite->nPosX + temp_x - (nScrWidth>>1) + (nH_Size>>1) + 0;
				#if TV_TFT_MODE== TFT_128x128_MODE
					nPosYN = (nScrHeight>>1) - 1 - nSprite->nPosY - temp_y - (nV_Size>>1)+64;//640*480:+9-8; 480*272:+9-17;
				#elif TV_TFT_MODE== TFT_320x240_MODE
					nPosYN = (nScrHeight>>1) - 1 - nSprite->nPosY - temp_y - (nV_Size>>1)+8;//96;
				#elif TV_TFT_MODE== TFT_480x272_MODE
					nPosYN = (nScrHeight>>1) - 1 - nSprite->nPosY - temp_y - (nV_Size>>1)-8;//96;
				#endif
			  }
			  else
			  {
			  	 // PPU_hardware_coordinate
			  	 // fix x, y the origin of the coordinates by hardware
				 nPosXN = nSprite->nPosX + temp_x;
				 nPosYN = nSprite->nPosY + temp_y;
			  } 			    
			  // Sprite RAM -  Position X
			  sprite_attr->uPosX_16 |= (nPosXN & spirte_pos_max_bit);
			  // Sprite RAM -  Position Y
			  sprite_attr->uPosY_16 |= (nPosYN & spirte_pos_max_bit);
			  sprite_attr++;
			  if((p_register_set->sprite_control & cdm_enable) == cdm_enable)
			    sprite_attr++;
	      }                  
	   }
	 nSprite++;
//	 sp_ptr++;	  
   }
   p_register_set->update_register_flag |= C_UPDATE_REG_SET_SPRITE_ATTRIBUTE;
#if (MCU_VERSION >= GPL326XX)
//   p_register_set->update_register_flag |= C_UPDATE_REG_SET_SPRITE_EX_ATTRIBUTE;
#endif    	        
}



