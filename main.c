#include <stdio.h>
#include <stdlib.h>

//读取图像
void ReadBmpImg(
	char *path,                 //图像路径
	unsigned char *imgHeader,   //图像头信息
	unsigned char **dbImg,		//位图数据容器
	unsigned int *width,	    //图像宽（像素）
	unsigned int *height	    //图像高（像素）
) {
	FILE *stream=NULL;
	stream=fopen(path,"rb");
	if(stream!=NULL){
		printf("读取文件成功\n");
		
		//读取图像宽高
		fread(imgHeader,54,1,stream); 
		*width =*(unsigned int*)(&imgHeader[18]);
		*height =*(unsigned int*)(&imgHeader[22]);

		//分配位图数据空间
		int pixelSpace=(*width)*(*height)*3;
		*dbImg=(unsigned char*)malloc(pixelSpace);

		//读取位图数据
		fseek(stream,54,SEEK_SET);
		fread(*dbImg,pixelSpace,1,stream);
		
		fclose(stream);
		stream=NULL;
	} else {
		printf("读取图像文件失败\n");
	}
}

//彩色图像的高斯滤波
void GaussFilter(
	unsigned char **filterImg,  //滤波图像容器（输出图像）
	unsigned char *souImg,      //源图像位图数据（输入图像）
	unsigned int width,         //图像宽（像素）
	unsigned int height,        //图像高（像素）
	int *kernel                 //高斯核（目前仅支持3x3的）
){
	//判断kernel的形状和大小
	
	//分配滤波图像数据空间
	int pixelSpace=width*height*3;
	*filterImg=(unsigned char*)malloc(pixelSpace);

    //定义工作变量
    int x,y,Gb,Gg,Gr,
	
	//读取高斯核
	x1=kernel[0],x2=kernel[1],x3=kernel[2],
	x4=kernel[3],x5=kernel[4],x6=kernel[5],
	x7=kernel[6],x8=kernel[7],x9=kernel[8],
	
	//工作区（3x3）的9个像素点，每一个像素点又包括b,g,r三个分量
	bpx1,bpx2,bpx3,bpx4,bpx5,bpx6,bpx7,bpx8,bpx9,
	gpx1,gpx2,gpx3,gpx4,gpx5,gpx6,gpx7,gpx8,gpx9,
	rpx1,rpx2,rpx3,rpx4,rpx5,rpx6,rpx7,rpx8,rpx9;
	
	//从上往下，从左至右进行卷积运算
	for(y=1;y<height-1;y++){
		//工作区换行时需要重新设置px1,px4,px7; px2,px5,px8;
		bpx1=(int)(souImg[(y-1)*width*3]);
		bpx4=(int)(souImg[y*width*3]);
		bpx7=(int)(souImg[(y+1)*width*3]);
		bpx2=(int)(souImg[(y-1)*width*3+3]);
		bpx5=(int)(souImg[y*width*3+3]);
		bpx8=(int)(souImg[(y+1)*width*3+3]);
		
		gpx1=(int)(souImg[(y-1)*width*3+1]);
		gpx4=(int)(souImg[y*width*3+1]);
		gpx7=(int)(souImg[(y+1)*width*3+1]);
		gpx2=(int)(souImg[(y-1)*width*3+3+1]);
		gpx5=(int)(souImg[y*width*3+3+1]);
		gpx8=(int)(souImg[(y+1)*width*3+3+1]);
		
		rpx1=(int)(souImg[(y-1)*width*3+2]);
		rpx4=(int)(souImg[y*width*3+2]);
		rpx7=(int)(souImg[(y+1)*width*3+2]);
		rpx2=(int)(souImg[(y-1)*width*3+3+2]);
		rpx5=(int)(souImg[y*width*3+3+2]);
		rpx8=(int)(souImg[(y+1)*width*3+3+2]);
		
		//遍历一行
		for(x=3;x<width*3-3;x+=3){
			bpx3=(int)(souImg[(y-1)*width*3+x+3]);
			bpx6=(int)(souImg[y*width*3+x+3]);
			bpx9=(int)(souImg[(y+1)*width*3+x+3]);
			
			gpx3=(int)(souImg[(y-1)*width*3+x+3+1]);
			gpx6=(int)(souImg[y*width*3+x+3+1]);
			gpx9=(int)(souImg[(y+1)*width*3+x+3+1]);
			
			rpx3=(int)(souImg[(y-1)*width*3+x+3+2]);
			rpx6=(int)(souImg[y*width*3+x+3+2]);
			rpx9=(int)(souImg[(y+1)*width*3+x+3+2]);
			
			//做卷积运算
			Gb=x1*bpx1+x2*bpx2+x3*bpx3+x4*bpx4+x5*bpx5+x6*bpx6+x7*bpx7+x8*bpx8+x9*bpx9;
			Gb=Gb/16;
			Gg=x1*gpx1+x2*gpx2+x3*gpx3+x4*gpx4+x5*gpx5+x6*gpx6+x7*gpx7+x8*gpx8+x9*gpx9;
			Gg=Gg/16;
			Gr=x1*rpx1+x2*rpx2+x3*rpx3+x4*rpx4+x5*rpx5+x6*rpx6+x7*rpx7+x8*rpx8+x9*rpx9;
			Gr=Gr/16;
			
			//设置新图像对应像素点的像素值
			(*filterImg)[y*width*3+x]=Gb;  //B
			(*filterImg)[y*width*3+x+1]=Gg;  //G
			(*filterImg)[y*width*3+x+2]=Gr;  //R
			
			//工作区右移一个像素
			bpx1=bpx2; bpx4=bpx5; bpx7=bpx8;
			bpx2=bpx3; bpx5=bpx6; bpx8=bpx9;
			
			gpx1=gpx2; gpx4=gpx5; gpx7=gpx8;
			gpx2=gpx3; gpx5=gpx6; gpx8=gpx9;
			
			rpx1=rpx2; rpx4=rpx5; rpx7=rpx8;
			rpx2=rpx3; rpx5=rpx6; rpx8=rpx9;
		}
	}
}

//彩色图像的灰度化处理
void Img2Grey(
	unsigned char **greyImg,    //灰度图像容器（输出图像）
	unsigned char *souImg,	    //源图像位图数据（输入图像）
	unsigned int width,	        //图像宽（像素）
	unsigned int height,        //图像高（像素）
	int mode		            //灰度处理模式（1：算术平均、2：加权平均、3：最值平均）
) {
	int pixelSpace=width*height*3;
	*greyImg=(unsigned char*)malloc(pixelSpace);
	int x,y;
	switch(mode){
		case 1:
			for(y=0;y<height;y++){
				for(x=0;x<width*3;x+=3){
					int index=y*width*3+x;
					int avg=(souImg[index]+souImg[index+1]+souImg[index+2])/3;
					(*greyImg)[index]=avg;
					(*greyImg)[index+1]=avg;
					(*greyImg)[index+2]=avg;
				}
			}
			break;
		case 2:
			for(y=0;y<height;y++){
				for(x=0;x<width*3;x+=3){
					int index=y*width*3+x;
					double pixel=0.11*souImg[index]+0.59*souImg[index+1]+0.3*souImg[index+2];
					(*greyImg)[index]=pixel;
					(*greyImg)[index+1]=pixel;
					(*greyImg)[index+2]=pixel;
				}
			}
			break;
		case 3:
			for(y=0;y<height;y++){
				for(x=0;x<width*3;x+=3){
					int index=y*width*3+x;
					int max=souImg[index];
					if(souImg[index+1]>max)
						max=souImg[index+1];
					if(souImg[index+2]>max)
						max=souImg[index+2];
					(*greyImg)[index]=max;
					(*greyImg)[index+1]=max;
					(*greyImg)[index+2]=max;
				}
			}
			break;
		default:
			printf("the word 'mode' is not right!\n");
			break;
	}
}

//灰度图像的高斯滤波
void GaussFilterForGreyImg(
	unsigned char **filterImg,  //滤波图像容器（输出图像）
	unsigned char *souImg,      //灰度图像位图数据（输入图像）
	unsigned int width,         //图像宽（像素）
	unsigned int height,        //图像高（像素）
	int *kernel                 //高斯核（目前仅支持3x3的）
){
	//判断kernel的形状和大小
	
	//分配滤波图像数据空间
	int pixelSpace=width*height*3;
	*filterImg=(unsigned char*)malloc(pixelSpace);

	//定义工作变量
    int x,y,G,
	
	//读取高斯核
	x1=kernel[0],x2=kernel[1],x3=kernel[2],
	x4=kernel[3],x5=kernel[4],x6=kernel[5],
	x7=kernel[6],x8=kernel[7],x9=kernel[8],
	
	//工作区（3x3）的9个像素点
	px1,px2,px3,px4,px5,px6,px7,px8,px9;
	
	//从上往下，从左至右进行卷积运算
	for(y=1;y<height-1;y++){
		//工作区换行时需要重新设置px1,px4,px7; px2,px5,px8;（以下都取b分量）
		px1=(int)(souImg[(y-1)*width*3]);
		px4=(int)(souImg[    y*width*3]);
		px7=(int)(souImg[(y+1)*width*3]);
	
		px2=(int)(souImg[(y-1)*width*3+3]);
		px5=(int)(souImg[    y*width*3+3]);
		px8=(int)(souImg[(y+1)*width*3+3]);
		
		//遍历一行
		for(x=1*3;x<(width-1)*3;x+=3){      //1*3代表第一个而不是第0个
			px3=(int)(souImg[(y-1)*width*3+x+3]);
			px6=(int)(souImg[    y*width*3+x+3]);
			px9=(int)(souImg[(y+1)*width*3+x+3]);
			
			//做卷积运算
			G=(x1*px1+x2*px2+x3*px3+x4*px4+x5*px5+x6*px6+x7*px7+x8*px8+x9*px9)/16;
						
			//设置新图像对应像素点的像素值
			(*filterImg)[y*width*3+x]=G;  //B
			(*filterImg)[y*width*3+x+1]=G;  //G
			(*filterImg)[y*width*3+x+2]=G;  //R
			
            //工作区右移一个像素
			px1=px2; px4=px5; px7=px8;
			px2=px3; px5=px6; px8=px9;
		}
	}
}

//图像的边缘检测
void SobelEdgeImg(
	unsigned char **edgeImg,    //边缘图像容器（输出图像）
	unsigned char *souImg,      //输入图像的位图数据
	unsigned int width,         //图像宽（像素）
	unsigned int height,        //图像高（像素）
	int *opt,                   //卷积核（索贝尔算子3x3）
	int direct                  //输出图像的梯度方向（1：偏x方向、2：偏y方向、3：综合xy方向）
){
	int pixelSpace=width*height*3;
	*edgeImg=(unsigned char*)malloc(pixelSpace);

    //定义工作变量
	int x,y,G,Gx,Gy,
	
	//偏x方向的算子
	x1=opt[0],x2=opt[1],x3=opt[2],
	x4=opt[3],x5=opt[4],x6=opt[5],
	x7=opt[6],x8=opt[7],x9=opt[8],
	
	//偏y方向的算子
	y1=opt[0],y2=opt[3],y3=opt[6],
	y4=opt[1],y5=opt[4],y6=opt[7],
	y7=opt[2],y8=opt[5],y9=opt[8],
	
	//工作区（3x3）的有9个像素点
	px1,px2,px3,px4,px5,px6,px7,px8,px9;
	
	//从上往下，从左至右进行卷积运算
	for(y=1;y<height-1;y++){
		px1=(int)(souImg[(y-1)*width*3]);
		px4=(int)(souImg[y*width*3]);
		px7=(int)(souImg[(y+1)*width*3]);
		
		px2=(int)(souImg[(y-1)*width*3+3]);
		px5=(int)(souImg[y*width*3+3]);
		px8=(int)(souImg[(y+1)*width*3+3]);
		
		for(x=3;x<width*3-3;x+=3){
			px3=(int)(souImg[(y-1)*width*3+x+3]);
			px6=(int)(souImg[y*width*3+x+3]);
			px9=(int)(souImg[(y+1)*width*3+x+3]);
			
			//做卷积运算
			Gx=x1*px1+x2*px2+x3*px3+x4*px4+x5*px5+x6*px6+x7*px7+x8*px8+x9*px9;
			Gx=abs(Gx);
			Gy=y1*px1+y2*px2+y3*px3+y4*px4+y5*px5+y6*px6+y7*px7+y8*px8+y9*px9;
			Gy=abs(Gy);
			
			//导出梯度方向的数据
			if(direct==1) {
				G=Gx;
			} else if(direct==2) {
				G=Gy;
			} else if(direct==3) {
				G=(Gx+Gy)/2;
			} else {
                printf("请传入有效的梯度方向代码！1|2|3\n");
            }
			if(G>255){
				G=255;
			}

            /*
			//可选，设置x，y方向阈值Tx，Ty进行二值化增强
			if(direct==0) {
				if(Gx>Tx) G=255;
			} else if(direct==1) {
				if(Gy>Ty) G=255;
			} else {
				if(Gx>Tx||Gy>Ty) G=255;
			}
			*/
			
			//设置新图像对应像素点的像素值
			(*edgeImg)[y*width*3+x]=G;  //B
			(*edgeImg)[y*width*3+x+1]=G;  //G
			(*edgeImg)[y*width*3+x+2]=G;  //R
			
			//工作区右移一个像素
			px1=px2;px4=px5;px7=px8; 
			px2=px3;px5=px6;px8=px9;
			
			//存在问题：生成的图像丢失了四周的像素 
		}
	}
}

//图像的二值化处理
void BinaryImg(
	unsigned char **binaryImg,  //二值图像容器（输出图像）
	unsigned char *souImg,      //输入图像的位图数据
	unsigned int width,         //图像宽（像素）
	unsigned int height,        //图像高（像素）
	int T                       //阈值
){
	int pixelSpace=width*height*3;
	*binaryImg=(unsigned char*)malloc(pixelSpace);

	//遍历所有的像素分量
	int i;
	for(i=0;i<pixelSpace;i++){
		if(souImg[i]<T){
			(*binaryImg)[i]=0;
		} else {
			(*binaryImg)[i]=255;
		}
	}
}

//保存图像
void SaveBmpImg(
	unsigned char *imgHeader,
	unsigned char *imgData,
	unsigned int width,
	unsigned int height,
	char *imgName
){
	/*
	//开始写入数据二进制文件，需要一片连续的空间
	typedef struct BmpData {
		unsigned char bmpDataHeader[54];  //头
		unsigned char bmpDbImg[width*height*3];  //位图数据
	} DefBmpData;
	
	//定义一个容器
	DefBmpData bmp1;
	
	//载入图像信息
	int i;
	for(i=0;i<54;i++) {
		bmp1.bmpDataHeader[i]=imgHeader[i];
		//printf("载入头信息%d\n",i+1,bmp1.bmpDataHeader[i]);
	}
	printf("载入头信息完成\n");

	//载入位图数据
	int j;
	for(j=0;j<width*height*3;j++){
		bmp1.bmpDbImg[j]=imgData[j];
		//printf("载入位图数据%d：%d\n",j+1,bmp1.bmpDbImg[j]);
	}
	printf("载入位图数据完成\n");
	*/

	//开始写入数据二进制文件（需要一片连续的空间）
	unsigned char *bmp;
	bmp=(unsigned char*)malloc(54+width*height*3);
	
    //载入图像的头信息
	int i;
	for(i=0;i<54;i++) {
		bmp[i]=imgHeader[i];
		//printf("载入头信息%d\n",i+1,bmp1.bmpDataHeader[i]);
	}
	
	//载入位图数据
	int j;
	for(j=0;j<width*height*3;j++){
		bmp[j+54]=imgData[j];
		//printf("载入位图数据%d：%d\n",j+1,bmp1.bmpDbImg[j]);
	}
	
	//导出图像所有数据到文件
	FILE *stream;
	stream=fopen(imgName,"wb");
	if(stream!=NULL){
		fwrite(bmp,54+width*height*3,1,stream);
		fclose(stream);
		stream=NULL;
	} else {
		printf("open error by wb!\n");
        return;
	}
	printf("图像文件导出完成\n");
}


int main(){
	//定义各量来加载读取的图像
	unsigned char header[54];
	unsigned char *dbImg;
	unsigned int width;
	unsigned int height;

	//读取图像
	ReadBmpImg("./input/color.bmp",header,&dbImg,&width,&height);
	
	//图像滤波处理
	unsigned char *filterImg;
	int kernel[9]={1,2,1,2,4,2,1,2,1};  //高斯核
	GaussFilter(&filterImg,dbImg,width,height,kernel);
	SaveBmpImg(header,filterImg,width,height,"./output/filter.bmp");
	
	//图像灰度处理
	unsigned char *greyImg;
	Img2Grey(&greyImg,filterImg,width,height,2);
	SaveBmpImg(header,greyImg,width,height,"./output/grey.bmp");
	
	//获得边缘图像
	unsigned char *edgeImg;
	int opt[9]={-1,0,1,-2,0,2,-1,0,1};  //索贝尔算子（偏x方向）
	SobelEdgeImg(&edgeImg,greyImg,width,height,opt,3);
	SaveBmpImg(header,edgeImg,width,height,"./output/sobelEdge-xy.bmp");
	unsigned char *edgeImgx,*edgeImgy;
	SobelEdgeImg(&edgeImgx,greyImg,width,height,opt,1);
	SobelEdgeImg(&edgeImgy,greyImg,width,height,opt,2);
	SaveBmpImg(header,edgeImgx,width,height,"./output/sobelEdge-x.bmp");
	SaveBmpImg(header,edgeImgy,width,height,"./output/sobelEdge-y.bmp");
	
	//灰度图像滤波处理
	unsigned char *filterGreyImg;
	GaussFilterForGreyImg(&filterGreyImg,greyImg,width,height,kernel);
	SaveBmpImg(header,filterGreyImg,width,height,"./output/filter-grey.bmp");
	
	//图像二值化
	unsigned char *binaryImg;
	BinaryImg(&binaryImg,greyImg,width,height,150);
	SaveBmpImg(header,binaryImg,width,height,"./output/grey2binary.bmp");
	
	printf("\nEND");
	return 0;
}
