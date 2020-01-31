//#include <cv.h>
//#include <highgui.h>
//ねむい

#include "opencv2/opencv.hpp" 
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include "RcControl.h"

using namespace zmp::zrc;

int main(int argc, char** argv)	//**二重ポインタ
{
	char buf[20];
    float angle = 0;
    int speed = 0;
    int set_v = 0;
    int set_t = 0;
    bool sign = false;　/*boolは真か偽か（０以外はすべてfalse）*/
    float set_a = 0;
    bool period = false;
    bool loop = true;
    RcControl  _RcControl;
    _RcControl.init();
    _RcControl.Start();
 
    _RcControl.SetReportFlagReq(0x0f);
    _RcControl.SetServoEnable(1);
    _RcControl.SetMotorEnableReq(1);
    _RcControl.SetDriveSpeed(0);
    _RcControl.SetSteerAngle(0);


	CvCapture *videoCapture = cvCreateCameraCapture(0);	//cvCreateCameraCapture(int index)　カメラからのビデオキャプチャを初期化する関数。

    IplImage *src;	//IplImageは画像を管理する構造体
	IplImage *dst;
	IplImage *color_dst;
	CvMemStorage *storage;

	while(1)
    {
		src = cvQueryFrame(videoCapture);	//カメラから画像をキャプチャする. ループブロック中に関数 cvQueryFrame() を呼び出すことで，実際にキャプチャを行う
#if 0
        cvNamedWindow( "Source", 1 );	//関数 cvNamedWindow は，画像とトラックバーのプレースホルダとして利用されるウィンドウを作成する．このウィンドウは，その名前によって参照される．１以外の数字だとウィンドウが小さくなる 
        cvShowImage( "Source", src );	//ウィンドウにsrcの内容を表示
#endif

        dst = cvCreateImage( cvGetSize(src), 8, 1 );	//関数 cvCreateImage はヘッダを作成し，データ領域を確保する．関数cvGetSizeは，入力行列または画像の行数（CvSize::height）と列数（CvSize::width）を返す．画像の場合は ROI のサイズが返される．
        color_dst = cvCreateImage( cvGetSize(src), 8, 3 );
        CvMemStorage* storage = cvCreateMemStorage(0);	//メモリの確保		関数cvCreateMemStorageはメモリストレージを生成し，そのポインタを返す。0の場合，デフォルト値（現在は64K）が使われる． 
        CvSeq* lines = 0;								//メモリの開放
        int i;
        cvCanny( src, dst, 50, 200, 3 );		//関数cvCannyは，Cannyアルゴリズムを使用して,入力画像srcに含まれているエッジを検出し，それを出力画像dstに保存する． threshold1 と threshold2 のうち小さいほうがエッジ同士を接続するために用いられ，大きいほうが強いエッジの初期検出に用いられる.250 300にすると白黒の検出に最適（らしい）
        cvCvtColor( dst, color_dst, CV_GRAY2BGR );	//関数 cvCvtColor は，入力画像dstの色空間を変換する 出力画像はcolor_dst   （ここで二値化画像が作られてるっぽい）
#if 0 
        lines = cvHoughLines2( dst, storage, CV_HOUGH_STANDARD, 1, CV_PI/180, 100, 0, 0 );	//Hough変換を用いて2値画像から直線を検出する(線検出のための3種類のハフ変換を実行する) thresholdは閾値パラメータ．対応する投票数がthresholdより大きい場合のみ，抽出された線が返される． 

        for( i = 0; i < MIN(lines->total,30); i++ )					//最大表示直線数 100
        {
            float* line = (float*)cvGetSeqElem(lines,i);
            float rho = line[0];
            float theta = line[1];
            CvPoint pt1, pt2;										//整数座標系による2次元の点
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));						//(x0, y0)を中心として，それぞれ1000pixel離れた線上の2点pt1,pt2を計算する．
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            cvLine( color_dst, pt1, pt2, CV_RGB(255,0,0), 3, 4 );	//結果表示用の画像データcolor_dst上に赤色，線幅3で直線を表示する
        }
#endif

#if 1 
        lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 70, 70, 10 );	//７０　１０　確率的ハフ変換ではparam1が最小の線の長さ　param2が同一線上に存在する線分として扱う二つの線分の最大の間隔
        for( i = 0; i < lines->total; i++ )
//        for( i = 0; i < 2; i++ )
        {
            CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
#if 0
			printf("i=%d x0=%d y0=%d\n", i, line[0].x, line[0].y);
			printf("i=%d x1=%d y1=%d\n", i, line[1].x, line[1].y);
#endif
			if(line[0].y > 240) {
				if(line[0].x < 320){
					if(line[0].y > line[1].y){ 
	            		cvLine( color_dst, line[0], line[1], CV_RGB(255,0,0), 3, 8 );
					}
				} 
			}
			if(line[2].y > 240) {
				if(line[2].x > 320){
					if(line[2].y > line[3].y){ 
	            		cvLine( color_dst, line[2], line[3], CV_RGB(255,0,0), 3, 8 );
					}
				}
			}

			#define P1X line[0].x
			#define P1Y line[0].y
			#define P3X line[1].x
			#define P3Y line[1].y
			#define P2X line[2].x
			#define P2Y line[2].y
			#define P4X line[3].x
			#define P4Y line[3].y
 
                double s1, s2;
                s1 = ((P4X - P2X) * (P1Y - P2Y) - (P4Y - P2Y) * (P1X - P2X))/2;
                s2 = ((P4X - P2X) * (P2Y - P3Y) - (P4Y - P2Y) * (P2X - P3X))/2;
 
                CvPoint v;
                v.x = P1X + (P3X - P1X) * s1 / (s1 + s2);
                v.y = P1Y + (P3Y - P1Y) * s1 / (s1 + s2);
 
				printf("%d,%d\n",v.x,v.y);

        }
        cvNamedWindow( "Hough", 1 );
        cvShowImage( "Hough", color_dst );

		if(v.x >= 310 && v.x <= 330){
		 _RcControl.SetSteerAngle(0);
		}else if(v.x > 330){
		 _RcControl.SetSteerAngle(30);
		}else if(v.x < 310){
		 _RcControl.SetSteerAngle(-30);
		}
	
#endif


//        cvWaitKey(0);
        if(cvWaitKey(1) > 0)	//キーが押されるまで待機する
			break;
    }
	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	cvReleaseImage(&color_dst);
	cvReleaseMemStorage(&storage);
}








