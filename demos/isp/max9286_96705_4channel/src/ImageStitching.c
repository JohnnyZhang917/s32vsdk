#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include "ImageStitching.h"



#define BASE_ADJUSTMENT (128)
#define CAMERA_NUM (4)
int gains[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
/*******************************************************************************/





unsigned int sum_q1 = 256;
unsigned int sum_q2 = BACK_COEF/*256*/;
unsigned int sum_q3 = 256;
unsigned int sum_q4 = 256;


////int lumaPixelAvg = 0;
int lumaPixelCnt = 0;
int lumaPixelSum = 0;

//unsigned char SVGAMA[256]={2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 16, 18, 19, 21, 22, 24, 25, 27, 28, 29, 31, 32, 33, 35, 36, 38, 39, 40, 42, 43, 44, 46, 47, 48, 50, 51, 52, 53, 55, 56, 57, 58, 60, 61, 62, 63, 64, 66, 67, 68, 69, 70, 71, 73, 74, 75, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 97, 98, 99, 100, 101, 102, 103, 104, 105, 105, 106, 107, 108, 109, 109, 110, 111, 112, 112, 113, 114, 115, 115, 116, 117, 117, 118, 118, 119, 120, 120, 121, 121, 122, 122, 123, 123, 124, 124, 125, 125, 125, 126, 126, 126, 127, 127, 127, 127, 127, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};
//unsigned char SVGAMA[256]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};
unsigned char MYLUT[256]={1, 2, 4, 5, 6, 7, 9, 10, 11, 12, 14, 15, 16, 17, 18, 20, 21, 22, 
						23, 24, 26, 27, 28, 29, 30, 32, 33, 34, 35, 36, 37, 39, 
						40, 41, 42, 43, 44, 45, 47, 48, 49, 50, 51, 52, 53, 54, 56, 
						57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 
						73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 
						88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 100, 
						101, 102, 103, 104, 105, 106, 107, 108, 108, 109, 110, 
						111, 112, 113, 113, 114, 115, 116, 117, 117, 118, 119, 119,
						120, 121, 122, 122, 123, 124, 124, 125, 125, 126, 126, 127, 
						127, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 
						138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 
						150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161,
						162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 
						174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 
						186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 
						198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
						210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 
						224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 
						238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 
						252, 253, 254, 255};
int   size_lut_front = SVM_WIDTH*TOP_LEFT_HEIGHT * 4;
int   size_lut_back = SVM_WIDTH*BACK_RIGHT_HEIGHT * 4;
int   size_lut_left = SVM_HEIGHT*TOP_LEFT_WIDTH * 4;
int   size_lut_right = SVM_HEIGHT*BACK_RIGHT_WIDTH * 4;
void Resize_Interpolation_SingleView(unsigned short *resize_lut,unsigned int **remap_lut, unsigned char * dout,int Nwidth,int Nheight)
{
	unsigned short i_int_1, j_int_1,i_int_2, j_int_2;
	unsigned short a1a2_1,a3a4_1,a1a2_2,a3a4_2;
	unsigned char a1_1, a2_1, a3_1, a4_1,a1_2, a2_2, a3_2, a4_2;

	unsigned char *remap_ptr_y1;
	unsigned char *remap_ptr_uv1;
	unsigned char *remap_ptr_y2;
	unsigned char *remap_ptr_uv2;

	int i,j;
	int currentLine = 1;
	int nextLine = 1;
	int row = 0;
        for (i = 0; i<Nheight; i++)
	{
		for (j = 0; j<Nwidth; j=j+2)
		{
			j_int_1 = *(resize_lut + i*Nwidth*4 + j*4) ;
			i_int_1 = *(resize_lut + i*Nwidth*4 + j*4 + 1) ;
			a1a2_1    = *(resize_lut + i*Nwidth*4 + j*4 + 2) ;
			a3a4_1    = *(resize_lut + i*Nwidth*4 + j*4 + 3) ;

			a1_1 = a1a2_1 >> 8;
			a2_1 = a1a2_1 % 256;
			a3_1 = a3a4_1 >> 8;
			a4_1 = a3a4_1 % 256;


			row = j_int_1;
			if(((i^row)&1) == 1)
			{
				currentLine = 1;
				nextLine = 1;
			}
			else
			{
				currentLine = 0;
				nextLine = 2;
			}		
			remap_ptr_y1=(unsigned char *)remap_lut[row+currentLine]+1;
			remap_ptr_uv1=(unsigned char *)remap_lut[row+currentLine];
			remap_ptr_y2=(unsigned char *)remap_lut[row+nextLine]+1;
			remap_ptr_uv2=(unsigned char *)remap_lut[row+nextLine];
			
			if(i_int_1%2==0)
			{
				i_int_1=i_int_1 & 0xfffe;
				*dout++= (a1_1*(*(remap_ptr_uv1 + i_int_1*2)) + a2_1*(*(remap_ptr_uv1 + (i_int_1)*2)) +
						   a3_1*(*(remap_ptr_uv2 + i_int_1*2)) + a4_1*(*(remap_ptr_uv2 + (i_int_1)*2)))>>8;
			}
			else
			{
				i_int_1=i_int_1 | 0x01;
				*dout++= (a1_1*(*(remap_ptr_uv1 + i_int_1*2+2)) + a2_1*(*(remap_ptr_uv1 + (i_int_1)*2+2)) +
						   a3_1*(*(remap_ptr_uv2 + i_int_1*2+2)) + a4_1*(*(remap_ptr_uv2 + (i_int_1)*2+2)))>>8;
			}

			*dout++= (a1_1*(*(remap_ptr_y1 + i_int_1*2)) + a2_1*(*(remap_ptr_y1 + (i_int_1+1)*2)) +
					  a3_1*(*(remap_ptr_y2 + i_int_1*2)) + a4_1*(*(remap_ptr_y2 + (i_int_1+1)*2)))>>8;




			j_int_2 = *(resize_lut + i*Nwidth*4 + j*4 + 4) ;
			i_int_2 = *(resize_lut + i*Nwidth*4 + j*4 + 4+ 1) ;
			a1a2_2	  = *(resize_lut + i*Nwidth*4 + j*4 + 4 + 2) ;
			a3a4_2	  = *(resize_lut + i*Nwidth*4 + j*4 + 4 + 3) ;
			

			a1_2 = a1a2_2 >> 8;
			a2_2 = a1a2_2 % 256;
			a3_2 = a3a4_2 >> 8;
			a4_2 = a3a4_2 % 256;

			row = j_int_2;
			if(((i^row)&1) == 1)
			{
				currentLine = 1;
				nextLine = 1;
			}
			else
			{
				currentLine = 0;
				nextLine = 2;
			}
			
			remap_ptr_y1=(unsigned char *)remap_lut[row+currentLine]+1;
			remap_ptr_uv1=(unsigned char *)remap_lut[row+currentLine];
			remap_ptr_y2=(unsigned char *)remap_lut[row+nextLine]+1;
			remap_ptr_uv2=(unsigned char *)remap_lut[row+nextLine];
			
			if(i_int_2%2==1)
			{
				i_int_2=i_int_2 | 0x01;
				*dout++= (a1_2*(*(remap_ptr_uv1 + i_int_2*2)) + a2_2*(*(remap_ptr_uv1 + (i_int_2)*2)) +
						   a3_2*(*(remap_ptr_uv2 + i_int_2*2)) + a4_2*(*(remap_ptr_uv2 + (i_int_2)*2)))>>8;
			}
			else
			{
				i_int_2=i_int_2 & 0xfffe;
				*dout++= (a1_2*(*(remap_ptr_uv1 + i_int_2*2+2)) + a2_2*(*(remap_ptr_uv1 + (i_int_2)*2+2)) +
						   a3_2*(*(remap_ptr_uv2 + i_int_2*2+2)) + a4_2*(*(remap_ptr_uv2 + (i_int_2)*2+2)))>>8;
			}
			
			*dout++= (a1_2*(*(remap_ptr_y1 + i_int_2*2)) + a2_2*(*(remap_ptr_y1 + (i_int_2+1)*2)) +
					  a3_2*(*(remap_ptr_y2 + i_int_2*2)) + a4_2*(*(remap_ptr_y2 + (i_int_2+1)*2)))>>8;
			
		}
	}
	
}


void rotate270(unsigned char *src,unsigned char *dst,int width,int height)
{
	int copyBytes = 4;
	int bytesPerLine = width << 1;
	int step = height << 2;
	int offset = bytesPerLine - copyBytes;
	
	unsigned char *dest = dst;
	unsigned char *source = src;
	unsigned char *psrc = NULL;
	unsigned char *pdst[2] = {NULL,NULL};	
	int i,j,k;
	unsigned char temp;

	for(i=0;i<bytesPerLine;i+=copyBytes)
	{
		pdst[1] = dest;
		pdst[0] = dest+(height<<1);
		psrc = source + offset;

		for(j=0;j<height;++j)
		{
			k = j % 2;
			*((unsigned int *)pdst[k]) = *((unsigned int *)psrc);

			if(1==k)
			{
				temp = *(pdst[1]+1);
				*(pdst[1]+1) = *(pdst[0]-1);
				*(pdst[0]-1) = temp;
			}

			pdst[k] += copyBytes;
			psrc +=bytesPerLine;
		}
		
		dest += step;
		source -= copyBytes;

	
	}
}

void Rotate180(unsigned char *src,int width,int height)
{
	int copyBytes = 4;
	int totalBytes = width*height << 1;
	int end = totalBytes >> 1;
	unsigned char *dst = src + totalBytes - copyBytes;
	unsigned int tmp0;					
	unsigned char tmp1;
	int i;
	
	for(i = 0; i < end; i += copyBytes)
	{
		tmp0 = *((unsigned int *)dst);
		*((unsigned int *)dst) = *((unsigned int *)src);
		*((unsigned int *)src) = tmp0;
	
		//\u4ea4\u6362Y\u5206\u91cf
		tmp1 = dst[1];
		dst[1] = dst[3];
		dst[3] = tmp1;
	
		tmp1 = src[1];
		src[1] = src[3];
		src[3] = tmp1;
	
		dst -= copyBytes;
		src += copyBytes;
	
		//watchdog();
	}
}


void Rotate90(unsigned char *src,unsigned char *dst,int width,int height)
{
	int copyBytes = 4;
	int bytesPerLine = width << 1;
	int step = height << 2;
	int offset = (height-1)*bytesPerLine;	
	unsigned char *dest = dst;
	unsigned char *source = src;
	unsigned char *psrc = NULL;
	unsigned char *pdst[2] = {NULL,NULL};	
	int i,j,k;
	unsigned char temp;
	for(i=0;i<bytesPerLine;i+=copyBytes)
	{
		pdst[0] = dest;
		pdst[1] = dest+(height<<1);
		psrc = source + offset;
		for(j=0;j<height;++j)
		{
			k = j % 2;
			*((unsigned int *)pdst[k]) = *((unsigned int *)psrc);

			if(1==k)
			{
				temp = *(pdst[0]-1);
				*(pdst[0]-1) = *(pdst[1]+1);
				*(pdst[1]+1) = temp;
			}
			pdst[k] += copyBytes;
			psrc -=bytesPerLine;
		}		
		dest += step;
		source += copyBytes;
	}
}

#if 1
void ReadOriginalImage(void)
{
		FILE* fp_front = NULL;
		FILE* fp_back = NULL;
		FILE* fp_left = NULL;
		FILE* fp_right = NULL;
		printf("read 4 image begin\n");


		if((fp_front = fopen("/data/front.yuv", "rb")) == NULL)
		{
			printf("\nFail to fopen front.yuv\n");
		}
		else
		{		
			fread(front_p, sizeof(unsigned char), PIC_ORISIZE, fp_front);
			fflush(fp_front);
			fclose(fp_front);
		}
	
		if ((fp_back = fopen("/data/back.yuv", "rb")) == NULL)
		{
			printf("Fail to fopen back.yuv\n");
		}
		else
		{		
			fread(back_p, sizeof(unsigned char), PIC_ORISIZE, fp_back);
			fflush(fp_back);
			fclose(fp_back);
		}
			
		if ((fp_left = fopen("/data/left.yuv", "rb")) == NULL)
		{
			printf("Fail to fopen left.yuv\n");
		}
		else
		{		
			fread(left_p, sizeof(unsigned char), PIC_ORISIZE, fp_left);
			fflush(fp_left);
			fclose(fp_left);
		}

			//fopen("/home/henry/test/1280720/right.yuv", "rb")) 
	if ((fp_right = fopen("/data/right.yuv", "rb")) == NULL)
		{
			printf("Fail to fopen right.yuv\n");
		}
		else
		{		
			fread(right_p, sizeof(unsigned char), PIC_ORISIZE, fp_right);
			fflush(fp_right);
			fclose(fp_right);
		}
	printf("read 4 image end\n");	
}
#endif

void ReadLut(void)
{
///	int i;
///	FILE* filelut;
	FILE * fp_fsvlut_file;
///	char filename[200]= "";
///	char *str1;
///	char *filename1;
///	char * filename2;
///	memset(filename,0,100);
	//str1 = "/home/henry/test/wxlut/";
//	str1 = "/data/";
	
//	memcpy(filename,str1,23);
//	strcat(filename,"/data/Lut_Front_Single_View.bin");
 ///read single view resize of lut
	if((fp_fsvlut_file= fopen("/data/Lut_Front_Single_View.bin", "rb"))== NULL){
		printf("Lut_Front_Single_View.bin was not opened\n");		
	}

	else	{// front single view lut
		fread(Lut_Fsv_View,sizeof(unsigned int),LUT_FSV_VIEW,fp_fsvlut_file);    //SINGLEVIEW_SIZES 
		fflush(fp_fsvlut_file);
		fclose(fp_fsvlut_file);
		printf("Lut_Front_Single_View.bin success !\n");		
	}

 ///read front svm  of lut
	///memset(filename,0,100);
	///memcpy(filename,str1,strlen(str1));
	///strcat(filename,"Lut_Front.bin");
	///filename1 = "/data/Lut_Front.bin";

	if((fp_fsvlut_file= fopen("/data/Lut_Front.bin", "rb"))== NULL){
		printf("Lut_Front.bin was not opened\n");	
		}

	else	{
		fread(Lut_Front,sizeof(unsigned int),LUT_POS_FB,fp_fsvlut_file);    //SINGLEVIEW_SIZES 660608
		fflush(fp_fsvlut_file);
		fclose(fp_fsvlut_file);
		printf("Lut_Front.bin  success !\n");		
	}
	
 ///read back svm  of lut
	///memset(filename,0,100);
	///memcpy(filename,str1,strlen(str1));
	///strcat(filename,"Lut_Back.bin");
	if((fp_fsvlut_file= fopen("/data/Lut_Back.bin", "rb"))== NULL){
		printf("Lut_Back.bin was not opened\n");
		}

	else	{
		fread(Lut_Back,sizeof(unsigned int),LUT_POS_FB,fp_fsvlut_file);    //SINGLEVIEW_SIZES 658944
		fflush(fp_fsvlut_file);
		fclose(fp_fsvlut_file);
		printf("Lut_Back.bin  success !\n");		
	}
 ///read left svm  of lut
///	memset(filename,0,100);
///	memcpy(filename,str1,strlen(str1));
///	strcat(filename,"Lut_Left.bin");
	if((fp_fsvlut_file= fopen("/data/Lut_Left.bin", "rb"))== NULL){
		printf("Lut_Left.bin was not opened\n");	
		}

	else	{
		fread(Lut_Left,sizeof(unsigned int),LUT_POS_LR,fp_fsvlut_file);    //SINGLEVIEW_SIZES 722944
		fflush(fp_fsvlut_file);
		fclose(fp_fsvlut_file);
		printf("Lut_Left.bin success !\n");		
	}
 ///read right svm  of lut
///	memset(filename,0,100);
///	memcpy(filename,str1,strlen(str1));
///	strcat(filename,"Lut_Right.bin");
	if((fp_fsvlut_file= fopen("/data/Lut_Right.bin", "rb"))== NULL){
		printf("Lut_Right.bin was not opened\n");	
		}

	else	{
		fread(Lut_Right,sizeof(unsigned int),LUT_POS_LR,fp_fsvlut_file);    //SINGLEVIEW_SIZES 720896
		fflush(fp_fsvlut_file);
		fclose(fp_fsvlut_file);
		printf("Lut_Right.bin   success !\n");		
	}


	
 ///read wt front svm  of lut
///	memset(filename,0,100);
///	memcpy(filename,str1,strlen(str1));
///	strcat(filename,"Wt_Front.bin");
//	//filename2 = "/home/henry/test/wxlut/Wt_Front.bin";
//	filename2 = "/home/henry/test/wxlut/Wt_Front.bin";
	if((fp_fsvlut_file= fopen("/data/Wt_Front.bin", "rb"))== NULL){
		printf("Wt_Front.bin was not opened\n");	
		}

	else	{
		fread(Wt_Lut_Front,sizeof(unsigned int),LUT_WT_FB*2,fp_fsvlut_file);    //SINGLEVIEW_SIZES 
		fflush(fp_fsvlut_file);
		fclose(fp_fsvlut_file);
		printf("Wt_Front.bin   success !\n");		
	}
 ///read wt back svm  of lut
	///memset(filename,0,100);
	///memcpy(filename,str1,strlen(str1));
	///strcat(filename,"Wt_Back.bin");
	if((fp_fsvlut_file= fopen("/data/Wt_Back.bin", "rb"))== NULL){
		printf("Wt_Back.bin was not opened\n");	
		}

	else	{
		fread(Wt_Lut_Back,sizeof(UInt64_t),LUT_WT_FB,fp_fsvlut_file);    //SINGLEVIEW_SIZES 
		fflush(fp_fsvlut_file);
		fclose(fp_fsvlut_file);
		printf("Wt_Back.bin   success !\n");		
	}
 ///read wt left svm  of lut
///	memset(filename,0,100);
///	memcpy(filename,str1,strlen(str1));
///	strcat(filename,"Wt_Left.bin");
	if((fp_fsvlut_file= fopen("/data/Wt_Left.bin", "rb"))== NULL){
		printf("Wt_Left.bin was not opened\n");	
		}

	else	{
		fread(Wt_Lut_Left,sizeof(unsigned int),LUT_WT_LR*2,fp_fsvlut_file);    //SINGLEVIEW_SIZES 
		fflush(fp_fsvlut_file);
		fclose(fp_fsvlut_file);
		printf("Wt_Left.bin   success !\n");		
	}
 ///read wt right svm  of lut
///	memset(filename,0,100);
///	memcpy(filename,str1,strlen(str1));
///	strcat(filename,"Wt_Right.bin");
	if((fp_fsvlut_file= fopen("/data/Wt_Right.bin", "rb"))== NULL){
		printf("Wt_Right.bin was not opened\n");	
		}

	else	{
		fread(Wt_Lut_Right,sizeof(UInt64_t),LUT_WT_LR,fp_fsvlut_file);    //SINGLEVIEW_SIZES 
		fflush(fp_fsvlut_file);
		fclose(fp_fsvlut_file);
		printf("Wt_Right.bin   success !\n");		
	}
	
}

 int saveframe(char *str, void *p, int length, int is_oneframe)
{
	FILE *fd;

	if (is_oneframe) {
		fd = fopen(str, "wb");
	}
	else {
		fd = fopen(str, "a");
	}     

	if (!fd) {
		printf("Open file error\n");
		return -1;
	}
	if (fwrite(p, 1, length, fd)) {
		printf("Write file successfully\n");
		fclose(fd);
		return 0;
	}
	else {
		printf("Write file fail\n");
		fclose(fd);
		return -1;
	}
	
}
/// void Resize_Interpolation_SingleView(unsigned short *resize_lut,unsigned int **remap_lut, unsigned char * dout,int Nwidth,int Nheight)

int UndistortionSVProccess(unsigned int * p_lut,
	unsigned char* result_image,
	unsigned char* source_image,
	int result_height,
	int result_width,
	int source_height,
	int source_width)
{
       int ret = 0;

	///int half_result_height, half_result_width;
	///int half_source_height, half_source_width;
	unsigned int  item;
	int source_pos[2]; 
	int wt_up, wt_left;
	int wt_upleft, wt_upright, wt_downleft, wt_downright;
	int src_widthstep, result_widthstep;// result_nchannels; src_nchannels,
	int yuv[3];
///	unsigned char* p_dst = result_image;
	unsigned char* p_src = source_image;

	///half_source_height = source_height >> 1;
	////half_source_width = source_width >> 1;
	////half_result_height = result_height >> 1;
	////half_result_width = result_width >> 1;

	result_widthstep = result_width << 1;
	src_widthstep = source_width << 1;

	for (int i = 0; i < result_height; i++)
	{
		for (int j = 0; j < result_width; j++)
		{
			item = p_lut[i * result_width + j];

			source_pos[0] = item >> 21;
			source_pos[1] = (item >> 10) & 0x7FF;

			source_pos[0] = source_pos[0] < 0 ? 0 : (source_pos[0] > source_height - 1 ? source_height - 1 : source_pos[0]);
			source_pos[1] = source_pos[1] < 0 ? 0 : (source_pos[1] > source_width - 1 ? source_width - 1 : source_pos[1]);

			wt_up = (item >> 5) & 0x1F;
			wt_left = item & 0x1F;

			wt_upleft = (31 - wt_up) * (31 - wt_left);
			wt_upright = (31 - wt_up) * wt_left;
			wt_downleft = wt_up * (31 - wt_left);
			wt_downright = 31 * 31 - wt_upleft - wt_upright - wt_downleft;

			yuv[0] = (p_src[source_pos[0] * src_widthstep + source_pos[1] * 2 + 1] * wt_upleft
				+ p_src[source_pos[0] * src_widthstep + (source_pos[1] + 1) * 2 + 1] * wt_upright
				+ p_src[(source_pos[0] + 1) * src_widthstep + source_pos[1] * 2 + 1] * wt_downleft
				+ p_src[(source_pos[0] + 1) * src_widthstep + (source_pos[1] + 1) * 2 + 1] * wt_downright) >> 10;

			yuv[0] = yuv[0] < 0 ? 0 : (yuv[0] > 255 ? 255 : yuv[0]);

			result_image[i * result_widthstep + j*2+1] = yuv[0];///

			// process u & v channel
			if(((j^source_pos[1])&1)==1)
				
				source_pos[1]+=1;
				
			
			if (j % 2 == 0)
			{
				yuv[1] = p_src[source_pos[0] * src_widthstep + source_pos[1] * 2];
				yuv[1] = yuv[1] < 0 ? 0 : (yuv[1] > 255 ? 255 : yuv[1]);
				result_image[i * result_widthstep + j * 2] = yuv[1];
			}
			else
			{
				yuv[2] = p_src[source_pos[0] * src_widthstep + source_pos[1] * 2];
				yuv[2] = yuv[2] < 0 ? 0 : (yuv[2] > 255 ? 255 : yuv[2]);
				result_image[i * result_widthstep + j * 2] = yuv[2];
			}
		}
	}

	return ret;
}


Int32_t analysis_panorama_lut_uyvy(
	UInt32_t* p_lut_front_test,
	UInt32_t* p_lut_back_test,
	UInt32_t* p_lut_left_test,
	UInt32_t* p_lut_right_test,
	UInt64_t* p_wt_front_test,
	UInt64_t* p_wt_back_test,
	UInt64_t* p_wt_left_test,
	UInt64_t* p_wt_right_test,
	uchar* result_image,
	uchar* front_image_uyvy,
	uchar* back_image_uyvy,
	uchar* left_image_uyvy,
	uchar* right_image_uyvy,
	CvPoint1 car_up_left,
	CvPoint1 car_down_right,
	Int32_t frontback_fov_height,
	Int32_t leftright_fov_width,
	Int32_t result_height,
	Int32_t result_width,
	Int32_t source_height,
	Int32_t source_width)
{
	Int32_t ret = 0;

	
	uchar* p_src;
	UInt32_t* p_pos_lut;
	UInt64_t* p_wt_lut;
	Int32_t fov_height, fov_width;
	Int32_t dst_height, dst_width;
	Int32_t src_height, src_width;
	Int32_t row_start, col_start;
	uchar* p_dst = result_image;
	memset(p_dst, 0, SVM_WIDTH * SVM_HEIGHT * 2);

	src_height = source_height;
	src_width = source_width;
	dst_height = result_height;
	dst_width = result_width;

	UInt32_t loop_count = 0;
///	Float64_t total_time_1 = 0, total_time_2 = 0;
	while (loop_count < 1)
	{
#ifdef TIME_CONSUME
		time_t  start_time = clock();
#endif
		// process front fov
		p_src = front_image_uyvy;
		p_pos_lut = p_lut_front_test;
		p_wt_lut = p_wt_front_test;
		fov_height = frontback_fov_height;
		fov_width = result_width;
		row_start = 0;
		col_start = 0;
		analysis_single_fov_lut_uyvy(fov_height,
			fov_width,
			p_pos_lut,
			p_wt_lut,
			p_src,
			row_start,
			col_start,
			src_height,
			src_width,
			dst_height,
			dst_width,
			p_dst);
#ifdef TIME_CONSUME		
		time_t  time1_1 = clock();
		double time0_1 = (double)(clock() - start_time);
		//printf("image_result_test cost :%f\n", time0_1);
		total_time_1 += time0_1;
#endif
		loop_count++;
	}

	// process back fov
	p_src = back_image_uyvy;
	p_pos_lut = p_lut_back_test;
	p_wt_lut = p_wt_back_test;
	fov_height = frontback_fov_height;
	fov_width = result_width;
	row_start = car_down_right.y + 1;
	col_start = 0;
	analysis_single_fov_lut_uyvy(fov_height,
		fov_width,
		p_pos_lut,
		p_wt_lut,
		p_src,
		row_start,
		col_start,
		src_height,
		src_width,
		dst_height,
		dst_width,
		p_dst);
#if 1
	// process left fov
	p_src = left_image_uyvy;
	p_pos_lut = p_lut_left_test;
	p_wt_lut = p_wt_left_test;
	fov_height = result_height;
	fov_width = leftright_fov_width;
	row_start = 0;
	col_start = 0;
	analysis_single_fov_lut_uyvy(fov_height,
		fov_width,
		p_pos_lut,
		p_wt_lut,
		p_src,
		row_start,
		col_start,
		src_height,
		src_width,
		dst_height,
		dst_width,
		p_dst);

	// process right fov
	p_src = right_image_uyvy;
	p_pos_lut = p_lut_right_test;
	p_wt_lut = p_wt_right_test;
	fov_height = result_height;
	fov_width = leftright_fov_width;
	row_start = 0;
	col_start = car_down_right.x + 1;

	analysis_single_fov_lut_uyvy(fov_height,
		fov_width,
		p_pos_lut,
		p_wt_lut,
		p_src,
		row_start,
		col_start,
		src_height,
		src_width,
		dst_height,
		dst_width,
		p_dst);
#endif
	return ret;
}






Int32_t bev_process(
	uchar* result_image,
	uchar* front_image_uyvy,
	uchar* back_image_uyvy,
	uchar* left_image_uyvy,
	uchar* right_image_uyvy,
	CvPoint1 car_up_left,
	CvPoint1 car_down_right,
	Int32_t frontback_fov_height,
	Int32_t leftright_fov_width,
	Int32_t result_width,
	Int32_t source_width,
	TabBev **bev_Table[4])
{
	Int32_t ret = 0;
	uchar* p_src, *p_src1, *p_src2;
	memset(result_image, 0, DST_WIDTH * DST_HEIGHT* 2);
	
	Int32_t yuv[3];
	Int32_t result_widthstep, src_widthstep;
	CvRect1 region_roi;
	Int32_t camid, fusion_pic1, fusion_pic2;

	result_widthstep = result_width << 1;
	src_widthstep = source_width << 1;

	UInt32_t loop_count = 0;
//	Float64_t total_time_1 = 0, total_time_2 = 0;
	while (loop_count < 1)
	{
#ifdef TIME_CONSUME
	time_t  start_time = clock();
#endif
		//front left
		p_src1 = front_image_uyvy;
		p_src2 = left_image_uyvy;
		fusion_pic1 = 0;
		fusion_pic2 = 2;
		region_roi = cvRect1(0, 0, car_up_left.x, car_up_left.y);
		analysis_fusion_region_lut_uyvy(result_image, p_src1, p_src2, fusion_pic1, fusion_pic2, &region_roi, result_widthstep, src_widthstep, bev_Table);
#ifdef TIME_CONSUME		
		time_t  time1_1 = clock();
		double time0_1 = (double)(clock() - start_time);
		//printf("image_result_test cost :%f\n", time0_1);
		total_time_1 += time0_1;
#endif
		loop_count++;
	}
	

	///time_t time1_1 = clock();

	//front
	region_roi = cvRect1(car_up_left.x, 0, car_down_right.x - car_up_left.x + 1, frontback_fov_height);//-10
	camid = 0;
	p_src = front_image_uyvy;
	analysis_single_region_lut_uyvy(result_image, p_src, &region_roi, camid, result_widthstep, src_widthstep, bev_Table);


#ifdef TIME_CONSUME		
	time_t  time1_2 = clock();
	double time0_1 = (double)(clock() - time1_1);

///	time1_1 = clock();
#endif
	//front right
	p_src1 = front_image_uyvy;
	p_src2 = right_image_uyvy;
	fusion_pic1 = 0;
	fusion_pic2 = 3;
	region_roi = cvRect1(car_down_right.x + 1, 0, car_up_left.x, car_up_left.y);
	analysis_fusion_region_lut_uyvy(result_image, p_src1, p_src2, fusion_pic1, fusion_pic2, &region_roi, result_widthstep, src_widthstep, bev_Table);
#ifdef TIME_CONSUME		
	time1_2 = clock();
	time0_1 = (double)(clock() - time1_1);

	///time1_1 = clock();
#endif
	//left
	region_roi = cvRect1(0, car_up_left.y, car_up_left.x, car_down_right.y - car_up_left.y + 1);
//printf("y:%d ");
	analysis_single_region_lut_uyvy(result_image, left_image_uyvy, &region_roi, 2, result_widthstep, src_widthstep, bev_Table);

#ifdef TIME_CONSUME		
	time1_2 = clock();
	time0_1 = (double)(clock() - time1_1);

	///time1_1 = clock();
#endif
//right
	region_roi = cvRect1(car_down_right.x + 1, car_up_left.y, result_width - car_down_right.x - 1, car_down_right.y - car_up_left.y + 1);
	analysis_single_region_lut_uyvy(result_image, right_image_uyvy, &region_roi, 3, result_widthstep, src_widthstep, bev_Table);


#ifdef TIME_CONSUME		
	time1_2 = clock();
	time0_1 = (double)(clock() - time1_1);
	time1_1 = clock();
#endif	//back left
	p_src1 = left_image_uyvy;
	p_src2 = back_image_uyvy;
	fusion_pic1 = 2;
	fusion_pic2 = 1;
	region_roi = cvRect1(0, car_down_right.y + 1, car_up_left.x, frontback_fov_height);
	analysis_fusion_region_lut_uyvy(result_image, p_src1, p_src2, fusion_pic1, fusion_pic2, &region_roi, result_widthstep, src_widthstep, bev_Table);

#ifdef TIME_CONSUME		
	time1_2 = clock();
	time0_1 = (double)(clock() - time1_1);
#endif
	//back
	region_roi = cvRect1(car_up_left.x, car_down_right.y + 1, car_down_right.x - car_up_left.x + 1, frontback_fov_height);
	analysis_single_region_lut_uyvy(result_image, back_image_uyvy, &region_roi, 1, result_widthstep, src_widthstep, bev_Table);

///	time1_1 = clock();
	//back right

	p_src1 = right_image_uyvy;
	p_src2 = back_image_uyvy;
	fusion_pic1 = 3;
	fusion_pic2 = 1;
	region_roi = cvRect1(car_down_right.x + 1, car_down_right.y + 1, car_up_left.x, frontback_fov_height);

	analysis_fusion_region_lut_uyvy(result_image, p_src1, p_src2, fusion_pic1, fusion_pic2, &region_roi, result_widthstep, src_widthstep, bev_Table);

#ifdef TIME_CONSUME		
	time1_2 = clock();
	time0_1 = (double)(clock() - time1_1);
#endif	

	return ret;
}




void analysis_single_region_lut_uyvy(
	uchar* result_image, uchar* p_src,
	CvRect1* region_roi,
	Int32_t camid,
	Int32_t result_widthstep,
	Int32_t src_widthstep,
	TabBev **bev_Table[4])
{
	Int32_t yuv[2], x, y, row, col,i,j;
	if (camid == 0 || camid == 2) //0:front , 2:left
	{
		for ( i = region_roi->y; i < region_roi->height + region_roi->y; i++)
		{
			for ( j = region_roi->x; j < region_roi->width + region_roi->x; j++)
			{
				x = bev_Table[camid][i][j].point_pos.x;
				y = bev_Table[camid][i][j].point_pos.y;

				yuv[0] = ((p_src[y * src_widthstep + x * 2 + 1] * bev_Table[camid][i][j].wt_upleft
					+ p_src[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[camid][i][j].wt_upright
					+ p_src[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[camid][i][j].wt_downleft
					+ p_src[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[camid][i][j].wt_downright) >> 16)
					;

				yuv[1] = ((p_src[y * src_widthstep + x * 2]))
					;

				result_image[(i)* result_widthstep + (j)* 2 + 1] += yuv[0];
				result_image[(i)* result_widthstep + (j)* 2] += yuv[1];
			}
		}
	}
	else if (camid == 3)  //right
	{
		for ( i = region_roi->y; i < region_roi->height + region_roi->y; i++)
		{
			for ( j = region_roi->x; j < region_roi->width + region_roi->x; j++)
			{
				//row = i - region_roi->y;
				col = j - region_roi->x;
				x = bev_Table[camid][i][col].point_pos.x;
				y = bev_Table[camid][i][col].point_pos.y;

				yuv[0] = ((p_src[y * src_widthstep + x * 2 + 1] * bev_Table[camid][i][col].wt_upleft
					+ p_src[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[camid][i][col].wt_upright
					+ p_src[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[camid][i][col].wt_downleft
					+ p_src[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[camid][i][col].wt_downright) >> 16)
					;

				yuv[1] = ((p_src[y * src_widthstep + x * 2]))
					;

				result_image[(i)* result_widthstep + (j)* 2 + 1] += yuv[0];
				result_image[(i)* result_widthstep + (j)* 2] += yuv[1];
			}
		}
	}
	else    //back
	{
		for ( i = region_roi->y; i < region_roi->height + region_roi->y; i++)
		{
			for ( j = region_roi->x; j < region_roi->width + region_roi->x; j++)
			{
				row = i - region_roi->y;
				//col = j - region_roi->x;
				x = bev_Table[camid][row][j].point_pos.x;
				y = bev_Table[camid][row][j].point_pos.y;

				yuv[0] = ((p_src[y * src_widthstep + x * 2 + 1] * bev_Table[camid][row][j].wt_upleft
					+ p_src[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[camid][row][j].wt_upright
					+ p_src[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[camid][row][j].wt_downleft
					+ p_src[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[camid][row][j].wt_downright) >> 16)
					;

				yuv[1] = ((p_src[y * src_widthstep + x * 2]))
					;

				result_image[(i)* result_widthstep + (j)* 2 + 1] += yuv[0];
				result_image[(i)* result_widthstep + (j)* 2] += yuv[1];
			}
		}
	}
}

Int32_t analysis_single_fov_lut_uyvy(Int32_t fov_height,
	Int32_t fov_width,
	UInt32_t* p_pos_fusion_lut,
	UInt64_t* p_wt_lut,
	uchar* p_src,
	Int32_t row_start,
	Int32_t col_start,
	Int32_t source_height,
	Int32_t source_width,
	Int32_t result_height,
	Int32_t result_width,
	uchar* result_image)
{
	Int32_t ret = 0;
	Int32_t i,j, tmp;
	UInt32_t item;
	Int32_t yuv[3];
	Int32_t source_pos[2]; // y, x
	Int32_t wt_fusion;
	Int32_t wt_upleft, wt_upright, wt_downleft, wt_downright;
	Int32_t src_widthstep, src_nchannels, result_widthstep, result_nchannels;
	UInt64_t wt_item;

	result_widthstep = result_width << 1;
	src_widthstep = source_width << 1;	

	for ( i = 0; i < fov_height; i++)
	{
	tmp = i * fov_width ;
		for ( j = 0; j < fov_width; j++)
		{				
			item = p_pos_fusion_lut[tmp + j];
			wt_item = p_wt_lut[tmp+ j];

			source_pos[0] = item >> 21;
			source_pos[1] = (item >> 10) & 0x7FF;
			
			///source_pos[0] = source_pos[0] < 0 ? 0 : (source_pos[0] > source_height - 1 ? source_height - 1 : source_pos[0]);
			///source_pos[1] = source_pos[1] < 0 ? 0 : (source_pos[1] > source_width - 1 ? source_width - 1 : source_pos[1]);

			wt_fusion = item & 0x3FF;
			
			wt_upleft = (wt_item >> 48) ;
			wt_upright = (wt_item >> 32) & 0xFFFF;
			wt_downleft = (wt_item >> 16) & 0xFFFF;
			wt_downright = wt_item & 0xFFFF;

			yuv[0] = ((p_src[source_pos[0] * src_widthstep + source_pos[1] * 2 + 1] * wt_upleft
				+ p_src[source_pos[0] * src_widthstep + (source_pos[1] + 1) * 2 + 1] * wt_upright
				+ p_src[(source_pos[0] + 1) * src_widthstep + source_pos[1] * 2 + 1] * wt_downleft
				+ p_src[(source_pos[0] + 1) * src_widthstep + (source_pos[1] + 1) * 2 + 1] * wt_downright) >> 16)
				*wt_fusion >> 10;

		///	yuv[0] = yuv[0] < 0 ? 0 : (yuv[0] > 255 ? 255 : yuv[0]);

			result_image[(row_start + i) * result_widthstep + (col_start + j) * 2 + 1] += yuv[0];			
			// process u & v channel							
			yuv[1] = ((p_src[source_pos[0] * src_widthstep + source_pos[1] * 2] * (wt_upleft + wt_upright)
				+ p_src[(source_pos[0] + 1)* src_widthstep + source_pos[1] * 2] * (wt_downleft + wt_downright)) >> 16)
				* wt_fusion >> 10;
		///	yuv[1] = yuv[1] < 0 ? 0 : (yuv[1] > 255 ? 255 : yuv[1]);

			result_image[(row_start + i) * result_widthstep + (col_start + j) * 2] += yuv[1];			
		}
	}
	return 0;
}





void analysis_fusion_region_lut_uyvy(uchar* result_image,
	uchar* p_src1,
	uchar* p_src2,
	Int32_t fusion_pic1,
	Int32_t fusion_pic2,
	CvRect1* region_roi,
	Int32_t result_widthstep,
	Int32_t src_widthstep,
	TabBev **bev_Table[4])
{
	Int32_t yuv[2], x, y, row, col,i,j;
	if (fusion_pic1 == 0 && fusion_pic2 == 2) //front_left
	{
		for ( i = region_roi->y; i < region_roi->height + region_roi->y; i++)
		{
			for ( j = region_roi->x; j < region_roi->width + region_roi->x; j++)
			{
				x = bev_Table[fusion_pic1][i][j].point_pos.x;
				y = bev_Table[fusion_pic1][i][j].point_pos.y;
				col = j - region_roi->x;
				row = i - region_roi->y;

				if (bev_Table[fusion_pic1][i][j].wt_fusion == 1023)
				{
					yuv[0] = ((p_src1[y * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_upleft
						+ p_src1[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_upright
						+ p_src1[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_downleft
						+ p_src1[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_downright) >> 16)
						;

					yuv[1] = ((p_src1[y * src_widthstep + x * 2]))
						;
				}
				else if (bev_Table[fusion_pic2][row][col].wt_fusion == 1023)
				{
					x = bev_Table[fusion_pic2][row][col].point_pos.x;
					y = bev_Table[fusion_pic2][row][col].point_pos.y;
					yuv[0] = ((p_src2[y * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_upleft
						+ p_src2[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_upright
						+ p_src2[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_downleft
						+ p_src2[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_downright) >> 16)
						;

					yuv[1] = ((p_src2[y * src_widthstep + x * 2]))
						;
				}
				else  // fusion region
				{
					yuv[0] = ((p_src1[y * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_upleft
						+ p_src1[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_upright
						+ p_src1[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_downleft
						+ p_src1[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_downright) >> 16)
						* bev_Table[fusion_pic1][i][j].wt_fusion >> 10;

					yuv[1] = ((p_src1[y * src_widthstep + x * 2]))
						* bev_Table[fusion_pic1][i][j].wt_fusion >> 10;


					x = bev_Table[fusion_pic2][row][col].point_pos.x;
					y = bev_Table[fusion_pic2][row][col].point_pos.y;
					yuv[0] += ((p_src2[y * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_upleft
						+ p_src2[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_upright
						+ p_src2[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_downleft
						+ p_src2[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_downright) >> 16)
						* bev_Table[fusion_pic2][row][col].wt_fusion >> 10;
					yuv[1] += ((p_src2[y * src_widthstep + x * 2]))
						* bev_Table[fusion_pic2][row][col].wt_fusion >> 10;
				}

				result_image[(i)* result_widthstep + (j)* 2 + 1] += yuv[0];
				result_image[(i)* result_widthstep + (j)* 2] += yuv[1];
			}
		}
	}
	else if ((fusion_pic1 == 0 && fusion_pic2 == 3) || (fusion_pic1 == 2 && fusion_pic2 == 1)) //front_right  and  left_back
	{
		for ( i = region_roi->y; i < region_roi->height + region_roi->y; i++)
		{
			for ( j = region_roi->x; j < region_roi->width + region_roi->x; j++)
			{
				if (bev_Table[fusion_pic1][i][j].wt_fusion == 1023)
				{
					x = bev_Table[fusion_pic1][i][j].point_pos.x;
					y = bev_Table[fusion_pic1][i][j].point_pos.y;
					//col = j - region_roi->x;
					//row = i - region_roi->y;

					yuv[0] = ((p_src1[y * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_upleft
						+ p_src1[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_upright
						+ p_src1[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_downleft
						+ p_src1[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_downright) >> 16)
						;

					yuv[1] = ((p_src1[y * src_widthstep + x * 2]))
						;
				}
				else if (bev_Table[fusion_pic1][i][j].wt_fusion == 0)
				{
					col = j - region_roi->x;
					row = i - region_roi->y;
					x = bev_Table[fusion_pic2][row][col].point_pos.x;
					y = bev_Table[fusion_pic2][row][col].point_pos.y;

					yuv[0] = ((p_src2[y * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_upleft
						+ p_src2[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_upright
						+ p_src2[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_downleft
						+ p_src2[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_downright) >> 16)
						;

					yuv[1] = ((p_src2[y * src_widthstep + x * 2]))
						;
				}
				else
				{
					x = bev_Table[fusion_pic1][i][j].point_pos.x;
					y = bev_Table[fusion_pic1][i][j].point_pos.y;
					yuv[0] = ((p_src1[y * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_upleft
						+ p_src1[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_upright
						+ p_src1[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_downleft
						+ p_src1[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic1][i][j].wt_downright) >> 16)
						* bev_Table[fusion_pic1][i][j].wt_fusion >> 10;

					yuv[1] = ((p_src1[y * src_widthstep + x * 2]))
						* bev_Table[fusion_pic1][i][j].wt_fusion >> 10;

					col = j - region_roi->x;
					row = i - region_roi->y;
					x = bev_Table[fusion_pic2][row][col].point_pos.x;
					y = bev_Table[fusion_pic2][row][col].point_pos.y;
					yuv[0] += ((p_src2[y * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_upleft
						+ p_src2[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_upright
						+ p_src2[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_downleft
						+ p_src2[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic2][row][col].wt_downright) >> 16)
						* bev_Table[fusion_pic2][row][col].wt_fusion >> 10;
					yuv[1] += ((p_src2[y * src_widthstep + x * 2]))
						* bev_Table[fusion_pic2][row][col].wt_fusion >> 10;
				}

				result_image[(i)* result_widthstep + (j)* 2 + 1] += yuv[0];
				result_image[(i)* result_widthstep + (j)* 2] += yuv[1];
			}
		}
	}
	else   //right_back
	{
		for ( i = region_roi->y; i < region_roi->height + region_roi->y; i++)
		{
			for ( j = region_roi->x; j < region_roi->width + region_roi->x; j++)
			{
				if (bev_Table[fusion_pic1][i][col = j - region_roi->x].wt_fusion == 1023)
				{
					col = j - region_roi->x;
					//row = i - region_roi->y;
					x = bev_Table[fusion_pic1][i][col].point_pos.x;
					y = bev_Table[fusion_pic1][i][col].point_pos.y;

					yuv[0] = ((p_src1[y * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic1][i][col].wt_upleft
						+ p_src1[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic1][i][col].wt_upright
						+ p_src1[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic1][i][col].wt_downleft
						+ p_src1[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic1][i][col].wt_downright) >> 16)
						;

					yuv[1] = ((p_src1[y * src_widthstep + x * 2]))
						;
				}
				else if (bev_Table[fusion_pic1][i][col = j - region_roi->x].wt_fusion == 0)
				{
					//col = j - region_roi->x;
					row = i - region_roi->y;
					x = bev_Table[fusion_pic2][row][j].point_pos.x;
					y = bev_Table[fusion_pic2][row][j].point_pos.y;

					yuv[0] = ((p_src2[y * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic2][row][j].wt_upleft
						+ p_src2[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic2][row][j].wt_upright
						+ p_src2[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic2][row][j].wt_downleft
						+ p_src2[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic2][row][j].wt_downright) >> 16)
						;

					yuv[1] = ((p_src2[y * src_widthstep + x * 2]))
						;
				}
				else
				{
					col = j - region_roi->x;
					x = bev_Table[fusion_pic1][i][col].point_pos.x;
					y = bev_Table[fusion_pic1][i][col].point_pos.y;
					yuv[0] = ((p_src1[y * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic1][i][col].wt_upleft
						+ p_src1[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic1][i][col].wt_upright
						+ p_src1[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic1][i][col].wt_downleft
						+ p_src1[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic1][i][col].wt_downright) >> 16)
						* bev_Table[fusion_pic1][i][col].wt_fusion >> 10;

					yuv[1] = ((p_src1[y * src_widthstep + x * 2]))
						* bev_Table[fusion_pic1][i][col].wt_fusion >> 10;

					//col = j - region_roi->x;
					row = i - region_roi->y;
					x = bev_Table[fusion_pic2][row][j].point_pos.x;
					y = bev_Table[fusion_pic2][row][j].point_pos.y;
					yuv[0] += ((p_src2[y * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic2][row][j].wt_upleft
						+ p_src2[y * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic2][row][j].wt_upright
						+ p_src2[(y + 1) * src_widthstep + x * 2 + 1] * bev_Table[fusion_pic2][row][j].wt_downleft
						+ p_src2[(y + 1) * src_widthstep + (x + 1) * 2 + 1] * bev_Table[fusion_pic2][row][j].wt_downright) >> 16)
						* bev_Table[fusion_pic2][row][j].wt_fusion >> 10;
					yuv[1] += ((p_src2[y * src_widthstep + x * 2]))
						* bev_Table[fusion_pic2][row][j].wt_fusion >> 10;
				}

				result_image[(i)* result_widthstep + (j)* 2 + 1] += yuv[0];
				result_image[(i)* result_widthstep + (j)* 2] += yuv[1];
			}
		}
	}
}

CvRect1  cvRect1( int x, int y, int width, int height )
{
	CvRect1 r;

	r.x = x;
	r.y = y;
	r.width = width;
	r.height = height;

	return r;
}



