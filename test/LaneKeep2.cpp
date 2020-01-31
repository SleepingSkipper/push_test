//#include <cv.h>
//#include <highgui.h>
//�˂ނ�

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

int main(int argc, char** argv)	//**��d�|�C���^
{
	char buf[20];
    float angle = 0;
    int speed = 0;
    int set_v = 0;
    int set_t = 0;
    bool sign = false;�@/*bool�͐^���U���i�O�ȊO�͂��ׂ�false�j*/
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


	CvCapture *videoCapture = cvCreateCameraCapture(0);	//cvCreateCameraCapture(int index)�@�J��������̃r�f�I�L���v�`��������������֐��B

    IplImage *src;	//IplImage�͉摜���Ǘ�����\����
	IplImage *dst;
	IplImage *color_dst;
	CvMemStorage *storage;

	while(1)
    {
		src = cvQueryFrame(videoCapture);	//�J��������摜���L���v�`������. ���[�v�u���b�N���Ɋ֐� cvQueryFrame() ���Ăяo�����ƂŁC���ۂɃL���v�`�����s��
#if 0
        cvNamedWindow( "Source", 1 );	//�֐� cvNamedWindow �́C�摜�ƃg���b�N�o�[�̃v���[�X�z���_�Ƃ��ė��p�����E�B���h�E���쐬����D���̃E�B���h�E�́C���̖��O�ɂ���ĎQ�Ƃ����D�P�ȊO�̐������ƃE�B���h�E���������Ȃ� 
        cvShowImage( "Source", src );	//�E�B���h�E��src�̓��e��\��
#endif

        dst = cvCreateImage( cvGetSize(src), 8, 1 );	//�֐� cvCreateImage �̓w�b�_���쐬���C�f�[�^�̈���m�ۂ���D�֐�cvGetSize�́C���͍s��܂��͉摜�̍s���iCvSize::height�j�Ɨ񐔁iCvSize::width�j��Ԃ��D�摜�̏ꍇ�� ROI �̃T�C�Y���Ԃ����D
        color_dst = cvCreateImage( cvGetSize(src), 8, 3 );
        CvMemStorage* storage = cvCreateMemStorage(0);	//�������̊m��		�֐�cvCreateMemStorage�̓������X�g���[�W�𐶐����C���̃|�C���^��Ԃ��B0�̏ꍇ�C�f�t�H���g�l�i���݂�64K�j���g����D 
        CvSeq* lines = 0;								//�������̊J��
        int i;
        cvCanny( src, dst, 50, 200, 3 );		//�֐�cvCanny�́CCanny�A���S���Y�����g�p����,���͉摜src�Ɋ܂܂�Ă���G�b�W�����o���C������o�͉摜dst�ɕۑ�����D threshold1 �� threshold2 �̂����������ق����G�b�W���m��ڑ����邽�߂ɗp�����C�傫���ق��������G�b�W�̏������o�ɗp������.250 300�ɂ���Ɣ����̌��o�ɍœK�i�炵���j
        cvCvtColor( dst, color_dst, CV_GRAY2BGR );	//�֐� cvCvtColor �́C���͉摜dst�̐F��Ԃ�ϊ����� �o�͉摜��color_dst   �i�����œ�l���摜������Ă���ۂ��j
#if 0 
        lines = cvHoughLines2( dst, storage, CV_HOUGH_STANDARD, 1, CV_PI/180, 100, 0, 0 );	//Hough�ϊ���p����2�l�摜���璼�������o����(�����o�̂��߂�3��ނ̃n�t�ϊ������s����) threshold��臒l�p�����[�^�D�Ή����铊�[����threshold���傫���ꍇ�̂݁C���o���ꂽ�����Ԃ����D 

        for( i = 0; i < MIN(lines->total,30); i++ )					//�ő�\�������� 100
        {
            float* line = (float*)cvGetSeqElem(lines,i);
            float rho = line[0];
            float theta = line[1];
            CvPoint pt1, pt2;										//�������W�n�ɂ��2�����̓_
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));						//(x0, y0)�𒆐S�Ƃ��āC���ꂼ��1000pixel���ꂽ�����2�_pt1,pt2���v�Z����D
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            cvLine( color_dst, pt1, pt2, CV_RGB(255,0,0), 3, 4 );	//���ʕ\���p�̉摜�f�[�^color_dst��ɐԐF�C����3�Œ�����\������
        }
#endif

#if 1 
        lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 70, 70, 10 );	//�V�O�@�P�O�@�m���I�n�t�ϊ��ł�param1���ŏ��̐��̒����@param2���������ɑ��݂�������Ƃ��Ĉ�����̐����̍ő�̊Ԋu
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
        if(cvWaitKey(1) > 0)	//�L�[���������܂őҋ@����
			break;
    }
	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	cvReleaseImage(&color_dst);
	cvReleaseMemStorage(&storage);
}








