#include<iostream>
#include<list>
#include<vector>
#include<stdio.h>
#include<fstream>
#include<time.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/opencv.hpp>
#include "camera_calibrate.h"

using namespace std;
using namespace cv;

void combineK(list<int> &Lis,vector<int> &Vec,int nK,int nN,int k);
void sorterror(vector< vector<double> > &errfile, int &cols);
void saveerrfile(vector< vector<double> > &errfile, int cols);

vector<Mat> img_all;            //总图片矩阵 一张图片为单位
vector<Mat> img_com;            //组合图片矩阵
vector<Mat> img_cur;            //正在使用的图片矩阵
vector<Mat> img_opt;            //最优化图片矩阵

vector<int> dis_img;              //去除的图片
string filename;                  //写入文件名
vector< vector<double> > errfile; //统计文件
bool exitflag =false;             //停止循环
double exerr = 0;                 //预期误差

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cout<<" Usage: stereo_calibration   num of imgs    expected error."<<endl;
        return 1;
    }

    int64_t st = getTickCount();

    std::string param1(argv[1]);
    int img_num = atoi(param1.c_str());    //照片数量(照片以1对为单位)
//    std::string param2(argv[2]);
//    int img_cnum = atoi(param2.c_str());   //标定照片数量(照片以1对为单位)
    int img_cnum = img_num-1;
    std::string param2(argv[2]);           //设定预期误差值
    exerr = atof(param2.c_str());

    time_t curtime = time(0);
    char cttemp[64];
    strftime( cttemp, sizeof(cttemp), "%Y%m%d%X",localtime(&curtime) );
    char filenamebuf[30];
    sprintf(filenamebuf, "./output/mono_calibration_%s.txt", cttemp);
    filename=filenamebuf;

    list<int> img_list;

    for(int i=1;i<=img_num;i++)  //读图
    {
        char addr_buf[100];
        char img_buf[100];
        sprintf(addr_buf, "./data/");
        //sprintf(img_buf, "%s%d.png", addr_buf,i);
        sprintf(img_buf, "%s%d.jpg", addr_buf,i);
        string img_addr = img_buf;
        cout<<img_addr<<endl;
        Mat src=imread(img_addr,0);
        img_all.push_back(src);
        vector<Mat> tmp;
        tmp.push_back(src);
        double error;
        string camera_param = "./data/camera.yml";
        if(!camera_calibrate(11, 8, tmp, camera_param, error))
        {
            dis_img.push_back(i);
        }

    }

    cout<<"img_all size: "<<img_all.size()<<endl;

    for(int i=1;i<=img_num;i++)
    {
        img_list.push_back(i);
        for(int j=0; j<dis_img.size();j++)
        {
            if(i == dis_img[j])
            {
                 img_list.pop_back();
                 break;
            }

        }
    }

    cout<<"discarded image size: "<<dis_img.size()<<",  "<<"discarded image NO: ";
    for(int j=0; j<dis_img.size();j++)
    {
        cout<<dis_img[j]<<"  ";
    }
    cout<<endl;

    img_num = img_num -dis_img.size();
    img_cnum = img_cnum -dis_img.size();
    vector<int> img_cap(img_cnum);
    combineK(img_list,img_cap,img_cnum,img_num,0); //所有照片顺序列表，所求照片数量向量，所求照片数量，照片总数
    int cols= img_cnum+1;
    sorterror(errfile,cols);         //排序组合结果
    for(int i=0; i < errfile.size(); i++)
    {
        for(int j=0; j<cols; j++)
        {
            cout << errfile[i][j] <<" ";
        }
        cout << endl;
    }
    saveerrfile(errfile,cols);        //保存排序结果

    while(img_cnum>8&&exitflag == false)   //最少8张图
    {
        img_list.clear();
        for(int i=0;i<img_cnum;i++)
        {
            img_list.push_back(errfile[0][i]);
        }

        list<int>::iterator it;
        cout<< img_list.size()<<endl;
        for(it = img_list.begin();it != img_list.end();it++)
        {
            img_cur.push_back(img_all[*it-1]);
//            cout<<img_all[*it-1].size();
            cout<<*it<<" ";
//            cout<<endl;
        }
        cout<<endl;
        img_num = img_num-1;
        img_cnum = img_cnum -1;
        vector<int> img_cap2(img_cnum);
        errfile.clear();
        combineK(img_list,img_cap2,img_cnum,img_num,0); //所有照片顺序列表，所求照片数量向量，所求照片数量，照片总数

        cols= img_cnum+1;
        sorterror(errfile,cols);
        for(int i=0; i < errfile.size(); i++)
        {
            for(int j=0; j<cols; j++)
            {
                cout << errfile[i][j] <<" ";
            }
            cout << endl;
        }
        saveerrfile(errfile,cols);
        if(exitflag == true)
            break;
    }

    if(exitflag == true)
    {
        double error;
        string camera_param = "./data/camera.yml";
        camera_calibrate(11, 8, img_opt, camera_param, error);
    }
    else
    {
        cols= img_cnum;
        for(int i=0; i < cols; i++)
        {
            img_opt.push_back(img_all[errfile[0][i]-1]);
        }
        double error;
        string camera_param = "./data/camera.yml";
        camera_calibrate(11, 8, img_opt, camera_param, error);
    }

    int64_t et = getTickCount();
    cout<<"total time: "<<(et-st)*1000/getTickFrequency()/1000<<"s"<<endl;
    return 0;
}

void combineK(list<int> &Lis,vector<int> &Vec,int nK,int nN,int k)
{
    if(exitflag == false)
    {
        int nSize = Lis.size();
        list<int>::iterator iter;
        for(iter = Lis.begin();iter != Lis.end();iter++)
        {
            if(nN - nSize == 0)
                k = 0;
            Vec[k] = *iter;
            if( k + 1 == nK)
            {
                img_com.clear();
                for(int j=0;j<nK;j++)
                {
                    cout<<Vec[j]<<"  ";
                    img_com.push_back(img_all[Vec[j]-1]);
                }
                cout<<endl;

                double err;
                string camera_param_file = "./data/camera.yml";

                camera_calibrate(11, 8, img_com, camera_param_file, err);

                vector<double> plugin;
                for(int k=0;k<Vec.size();k++)
                {
                    plugin.push_back(Vec[k]);
                }
                plugin.push_back(err);
                errfile.push_back(plugin);
                if(err < exerr)
                {
                    exitflag = true;
                    for(int k=0 ;k<img_com.size(); k++)
                        img_opt.push_back(img_com[k]);
                }
            }
            else
            {
                list<int> Lisc( ++iter,Lis.end() );
                iter--;
                k++;
                combineK(Lisc,Vec,nK,nN,k);
                k--;
            }
    }

    }
}

void sorterror(vector< vector<double> > &errfile, int &cols)
{
    for(int i=0; i < errfile.size(); i++)
        for(int j=i+1; j<errfile.size(); j++)
        {
            if(errfile[j][cols-1]<errfile[i][cols-1])
            {
                for(int k=0; k< cols; k++)
                {
                    double temp;
                    temp=errfile[i][k];
                    errfile[i][k]=errfile[j][k];
                    errfile[j][k]=temp;
                }
            }
        }
}

void saveerrfile(vector< vector<double> > &errfile, int cols)
{
    ofstream f;
    f.open(filename.c_str(),ios::app);
    for(int i=0; i < errfile.size(); i++)
    {
        for(int j=0; j<cols; j++)
        {
            f << errfile[i][j] <<" ";
        }
        f << endl;
    }
    f << endl;
    f.close();
}
