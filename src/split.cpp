#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include<iostream>
#include<list>
#include<vector>
#include<stdio.h>
#include<fstream>
#include <time.h>

using namespace cv;
using namespace std;


int main(int argc, char **argv)
{
    vector<Mat> img_all;          //总图片矩阵 一张图片为单位  
    Mat imgl,imgr;
    std::string param1(argv[1]);
    int img_num = atoi(param1.c_str());    //照片数量(照片以1对为单位)

    for(int i=1;i<=img_num;i++)  //读左图
    {
        char addr_buf[100];
        char img_buf[100];
        sprintf(addr_buf, "./data/");
        sprintf(img_buf, "%s%d.jpg", addr_buf,i);
        string img_addr = img_buf;
        cout<<img_addr<<endl;
        Mat src=imread(img_addr,CV_LOAD_IMAGE_COLOR);
        img_all.push_back(src);
    }

    for(int i=1;i<=img_num;i++)  //读左图
    {
        Mat tmp;
        tmp=img_all[i-1].clone();
        tmp.rowRange(0,480).colRange(0,640).copyTo(imgl);
        tmp.rowRange(0,480).colRange(640,1280).copyTo(imgr);

        vector<int> compression_params;
        compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
        compression_params.push_back(100);

        char addr_buf[100];
        char img_buf[100];
        sprintf(addr_buf, "./data/left/");
        sprintf(img_buf, "%s%d.jpg", addr_buf,i);
        string img_addr = img_buf;
        cout<<img_addr<<endl;
        imwrite(img_addr,imgl,compression_params);

        char addr_buf2[100];
        char img_buf2[100];
        sprintf(addr_buf2, "./data/right/");
        sprintf(img_buf2, "%s%d.jpg", addr_buf2,i);
        string img_addr2 = img_buf2;
        cout<<img_addr2<<endl;
        imwrite(img_addr2,imgr,compression_params);
    }

  return 0;
}
