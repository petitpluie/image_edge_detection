# image_edge_detection：图像边缘检测

### 介绍

本项目是一个使用C语言实现的基于索贝尔算子的图像边缘检测程序。程序输入24位bmp格式的真彩图像，输出各梯度图像（包括x方向和y方向）和边缘图像。

### 使用说明

1. clone 本项目

2. 准备一张待检测的彩色bmp格式的图像放到input目录下，并命名为color.bmp

3. 编译并运行main.c

4. 在output目录下生成目标图像：

   >经高斯滤波处理的平滑图像：filter.bmp
   >
   >经加权平均处理的灰度图像：grey.bmp
   >
   >竖直方向梯度图像：sobelEdge-x.bmp
   >
   >水平方向梯度图像：sobelEdge-y.bmp
   >
   >综合梯度(边缘)图像：sobelEdge-xy.bmp
   >
   >经滤波处理的灰度图像：filter-grey.bmp
   >
   >二值化的图像：grey2binary.bmp

### 检测效果





<div style="width:530px;">
    <div style="display:inline-block; width:254px;">
         <div>平滑图像</div>
    	<img src="https://s1.ax1x.com/2023/02/28/ppCoGM6.png">
    </div>
   <div style="display:inline-block; width:254px;">
         <div>灰度图像</div>
    	<img src="https://s1.ax1x.com/2023/02/28/ppCo3xx.png">
    </div>
    <div style="display:inline-block; width:254px;">
         <div>边缘图像</div>
    	<img src="https://s1.ax1x.com/2023/02/28/ppCo1R1.png">
    </div>
    <div style="display:inline-block; width:254px;">
         <div>二值图像</div>
    	<img src="https://s1.ax1x.com/2023/02/28/ppColGR.png">
    </div>
</div>

