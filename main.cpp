#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <graphics.h>
#include <conio.h>
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

//题记：不是练习科研，而是学习调库

#define MAXFRAME 10000

#define WINDOW_W (1920-550)
#define WINDOW_H (1080-350)

#define MODE_OPENCV 1
#define MODE_EASYX 2

using namespace std;
using namespace cv;

vector<int> Diff(MAXFRAME, 0);
vector<int> Diff2;
vector<int> Diff3;
vector<int> Target(MAXFRAME, -1);
//int Diff[MAXFRAME] = { 0 };
//int Diff2[MAXFRAME] = { 0 };
//int Diff3[MAXFRAME] = { 0 };

char TimeString[50];//用于存储时间串
char ImageNameString[50];//用于存储图片名
char ImageNameN2[50];//-2
char ImageNameN1[50];//-1
char ImageName0[50];//0
char ImageNameP1[50];//+1
char ImageNameP2[50];//+2
int Iterator_i = 0;//i
int Iterator_j = 0;//j
int num = 0;//total
int flag_LoadInToMemory = 0;
int flag_CalculateDiff = 0;
int flag_AnalyseDiff = 0;
int flag_Auto = 0;
int flag_EasyXDebug = 0;

int FrameIterator = 0;
int FramePointer = 0;

int ImageMiddle0 = 0;
int ImagePositive1 = ImageMiddle0 + 1;
int ImagePositive2 = ImageMiddle0 + 2;
int ImageNegative1 = ImageMiddle0 - 1;
int ImageNegative2 = ImageMiddle0 - 2;

//TCHAR2char   
void TcharToChar(const TCHAR * tchar, char * _char)
{
	int iLength;
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);//获取字节长度 
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);//将tchar值赋给_char    
}

//char2TCHAR
void CharToTchar(const char * _char, TCHAR * tchar)
{
	int iLength;

	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
}

void SystemTime()
{
	time_t now = time(NULL);
	strftime(TimeString, 50, "%x %X", localtime(&now));
}

void NumToChar(int num, char *str)
{
	if (num >= 1000 && num <= 10000)
	{
		//四位数
		int a = num / 1000;
		int b = (num - a * 1000) / 100;
		int c = (num - a * 1000 - b * 100) / 10;
		int d = num % 10;
		str[0] = a + '0';
		str[1] = b + '0';
		str[2] = c + '0';
		str[3] = d + '0';
		str[4] = '\0';
	}
	else if (num >= 100 && num <= 10000)
	{
		//三位数
		int a = num / 100;
		int b = (num - a * 100) / 10;
		int c = num % 10;
		str[0] = a + '0';
		str[1] = b + '0';
		str[2] = c + '0';
		str[3] = '\0';
	}
	else if (num >= 10 && num <= 10000)
	{
		//两位数
		int a = num / 10;
		int b = num - a * 10;
		str[0] = a + '0';
		str[1] = b + '0';
		str[2] = '\0';
	}
	else if (num >= 1 && num <= 10000)
	{
		//一位数
		str[0] = num + '0';
		str[1] = '\0';
	}
	else
	{
		SystemTime();
		cout << TimeString;
		cout << " [INFO]见鬼了" << endl;
		//return 1;
	}
	//return 0;
}

void Refresh5Image(int Frame)
{
	ImageMiddle0 = Frame;
	ImagePositive1 = ImageMiddle0 + 1;
	ImagePositive2 = ImageMiddle0 + 2;
	ImageNegative1 = ImageMiddle0 - 1;
	ImageNegative2 = ImageMiddle0 - 2;
}

void Draw5Image(int Frame1L, int FrameU, int FrameEdgeX, int  FrameWide, int  FrameHigh)
{
	int resizemode = MODE_OPENCV;//这两个重采样，一个是void Resize，另一个是cv::resize
	//清空文字区
	setfillcolor(DARKGRAY);
	solidrectangle(0 + 25 + 1, FrameU + FrameHigh + 2, WINDOW_W - 25 - 1, 150 + 320 - 1);
	//生成命名序列
	sprintf(ImageNameN2, "%s%d%s", "Output_Image_", ImageNegative2, ".jpg"); 
	sprintf(ImageNameN1, "%s%d%s", "Output_Image_", ImageNegative1, ".jpg"); 
	sprintf(ImageName0, "%s%d%s", "Output_Image_", ImageMiddle0, ".jpg"); 
	sprintf(ImageNameP1, "%s%d%s", "Output_Image_", ImagePositive1, ".jpg"); 
	sprintf(ImageNameP2, "%s%d%s", "Output_Image_", ImagePositive2, ".jpg"); 
	if (resizemode == MODE_OPENCV)
	{
		string string_strN2(ImageNameN2);
		string string_strN1(ImageNameN1);
		string string_strM0(ImageName0);
		string string_strP1(ImageNameP1);
		string string_strP2(ImageNameP2);
	}
	//面向过程逐步处理名称与转换
	//后期有时间的话就封装一个SuperString类，这个类里面包含String，char，TCHAR，num多种不同的类型，能一样的都内容一样
	//然后四个flag表示哪些是有意义的，一个type表示用途
	TCHAR strN2[40];
	TCHAR strN1[40];
	TCHAR strM0[40];
	TCHAR strP1[40];
	TCHAR strP2[40];
	TCHAR TCHAR_numN2[40];
	TCHAR TCHAR_numN1[40];
	TCHAR TCHAR_numM0[40];
	TCHAR TCHAR_numP1[40];
	TCHAR TCHAR_numP2[40];
	char char_numN2[40];
	char char_numN1[40];
	char char_numM0[40];
	char char_numP1[40];
	char char_numP2[40];
	CharToTchar(ImageNameN2, strN2);
	CharToTchar(ImageNameN1, strN1);
	CharToTchar(ImageName0, strM0);
	CharToTchar(ImageNameP1, strP1);
	CharToTchar(ImageNameP2, strP2);
	NumToChar(ImageNegative2, char_numN2);
	NumToChar(ImageNegative1, char_numN1);
	NumToChar(ImageMiddle0, char_numM0);
	NumToChar(ImagePositive1, char_numP1);
	NumToChar(ImagePositive2, char_numP2);
	CharToTchar(char_numN2, TCHAR_numN2);
	CharToTchar(char_numN1, TCHAR_numN1);
	CharToTchar(char_numM0, TCHAR_numM0);
	CharToTchar(char_numP1, TCHAR_numP1);
	CharToTchar(char_numP2, TCHAR_numP2);
	//OpenCV取出前重采样
	if (resizemode == MODE_OPENCV)
	{
		//没有使用INTER_LINEAR双线性插值而是INTER_AREA像素面积相关重采样，因为为了避免缩小图像的时候产生波纹
		Mat imgN2 = imread(ImageNameN2);
		resize(imgN2, imgN2, cv::Size(FrameWide, FrameHigh), cv::INTER_AREA);
		imwrite(ImageNameN2, imgN2);
		Mat imgN1 = imread(ImageNameN1);
		resize(imgN1, imgN1, cv::Size(FrameWide, FrameHigh), cv::INTER_AREA);
		imwrite(ImageNameN1, imgN1);
		Mat imgM0 = imread(ImageName0);
		resize(imgM0, imgM0, cv::Size(FrameWide, FrameHigh), cv::INTER_AREA);
		imwrite(ImageName0, imgM0);
		Mat imgP1 = imread(ImageNameP1);
		resize(imgP1, imgP1, cv::Size(FrameWide, FrameHigh), cv::INTER_AREA);
		imwrite(ImageNameP1, imgP1);
		Mat imgP2 = imread(ImageNameP2);
		resize(imgP2, imgP2, cv::Size(FrameWide, FrameHigh), cv::INTER_AREA);
		imwrite(ImageNameP2, imgP2);
	}
	//取出图片
	IMAGE N2, N1, M0, P1, P2;
	loadimage(&N2, strN2);
	loadimage(&N1, strN1);
	loadimage(&M0, strM0);
	loadimage(&P1, strP1);
	loadimage(&P2, strP2);
	//Easyx取出后重采样
	if (resizemode == MODE_EASYX)
	{
		Resize(&N2, FrameWide, FrameHigh);
		Resize(&N1, FrameWide, FrameHigh);
		Resize(&M0, FrameWide, FrameHigh);
		Resize(&P1, FrameWide, FrameHigh);
		Resize(&P2, FrameWide, FrameHigh);
	}
	int FrameX = Frame1L;
	int FrameY = FrameU;
	int FrameInterval = FrameEdgeX + FrameWide;
	settextstyle(35, 0, _T("黑体"));
	putimage(FrameX+1, FrameY+1, &N2);
	outtextxy(FrameX + 120, FrameY + FrameHigh + 20, TCHAR_numN2);
	FrameX += FrameInterval;
	putimage(FrameX+1, FrameY+1, &N1);
	outtextxy(FrameX + 120, FrameY + FrameHigh + 20, TCHAR_numN1);
	FrameX += FrameInterval;
	putimage(FrameX+1, FrameY+1, &M0);
	outtextxy(FrameX + 120, FrameY + FrameHigh + 20, TCHAR_numM0);
	FrameX += FrameInterval;
	putimage(FrameX+1, FrameY+1, &P1);
	outtextxy(FrameX + 120, FrameY + FrameHigh + 20, TCHAR_numP1);
	FrameX += FrameInterval;
	putimage(FrameX+1, FrameY+1, &P2);
	outtextxy(FrameX + 120, FrameY + FrameHigh + 20, TCHAR_numP2);
	FrameX += FrameInterval;
}

void DrawVisualMoe(int FrameIterator, int Frame1L, int  FrameU, int  FrameEdgeX, int  FrameWide, int  FrameHigh)
{
	FramePointer = Target[FrameIterator]+1;
	if (flag_EasyXDebug == 1)
	{
		FramePointer = 6;
	}
	Refresh5Image(FramePointer);
	Draw5Image(Frame1L, FrameU, FrameEdgeX, FrameWide, FrameHigh);
}

int main()
{
	//启动日志服务
	ofstream Output_Log;
	Output_Log.open("Output_Log.txt");
	ofstream Output_Data;
	Output_Data.open("Output_Data.txt");

	SystemTime();
	cout << TimeString ;
	cout << " [INFO]Start LogSystem" << endl;
	Output_Log << TimeString ;
	Output_Log << " [INFO]Start LogSystem" << endl;

	// 视频绝对路径读取
	
	VideoCapture capture("Input_Video.mp4");
	Mat frame[MAXFRAME];
	//Mat dstimg;
	int mode = -1;
	while (1)
	{
		cout << "=====请输入您接下来要执行的操作=====" << endl;
		cout << "【0】自动执行" << endl;
		cout << "【1】载入内存" << endl;
		cout << "【2】计算差异" << endl;
		cout << "【3】分析差异" << endl;
		cout << "【4】可视展示" << endl;
		cout << "【5】退出程序" << endl;
		if (mode == -1 ||( (mode == 1 || mode == 2 || mode == 3 || mode == 4)&&flag_Auto==0))
		{
			cin >> mode;
		}
		if (mode == 0 || flag_Auto == 1)
		{
			flag_Auto = 1;
			if (flag_LoadInToMemory == 0 && flag_CalculateDiff == 0 && flag_AnalyseDiff == 0)
			{
				mode = 1;
			}
			if (flag_LoadInToMemory == 1 && flag_CalculateDiff == 0 && flag_AnalyseDiff == 0)
			{
				mode = 2;
			}
			if (flag_LoadInToMemory == 1 && flag_CalculateDiff == 1 && flag_AnalyseDiff == 0)
			{
				mode = 3;
			}
			if (flag_LoadInToMemory == 1 && flag_CalculateDiff == 1 && flag_AnalyseDiff == 1)
			{
				mode = 4;
			}
		}
		if (mode == 1)
		{
			SystemTime();
			cout << TimeString;
			cout << " [INFO]Start Opening" << endl;
			Output_Log << TimeString;
			Output_Log << " [INFO]Start Opening" << endl;
			int time1 = time(NULL);
			int time2 = 0;

			//将视频加载入内存
			while (capture.isOpened() != 0)
			{
				//总帧数溢出警告
				if (Iterator_i > MAXFRAME)
				{
					MessageBox(NULL, TEXT("当前帧数超过10000，将可能引发致命错误"), TEXT("警告"), 0);
				}

				//判断视频是否读取结束并载入新视频
				capture >> frame[Iterator_i];
				if (frame[Iterator_i].empty() != 0)
				{
					break;
				}

				// 修改图片的尺寸为800*450，降低后期计算负担
				resize(frame[Iterator_i], frame[Iterator_i], cv::Size(800, 450), cv::INTER_LINEAR);

				//转换格式
				sprintf(ImageNameString, "%s%d%s", "Output_Image_", Iterator_i+1, ".jpg"); //为图片命名
				std::string str(ImageNameString);

				//展示当前帧
				imshow("Output_Image", frame[Iterator_i]);

				// 将图片保存在选择的路径中
				cv::imwrite(str, frame[Iterator_i]);

				waitKey(1);

				SystemTime();
				cout << TimeString;
				cout << " [INFO]Take the " << Iterator_i+1 << " picture" << endl;
				Output_Log << TimeString;
				Output_Log << " [INFO]Take the " << Iterator_i+1 << " picture" << endl;

				Iterator_i++;
				num++;
			}

			flag_LoadInToMemory = 1;
			cv::destroyWindow("Output_Image");
			time2 = time(NULL);
			SystemTime();
			cout << TimeString;
			cout << " [INFO]End Open" << endl;
			Output_Log << TimeString;
			Output_Log << " [INFO]End Open" << endl;
			Output_Data << "Total Frame is " << num << endl;
			Output_Log << TimeString;
			Output_Log << " [INFO]Opening use time " << time2 - time1 << " seconds" << endl;
			Output_Data << "Opening use time " << time2 - time1 << " seconds" << endl;
		}
		else if (mode == 2)
		{
			if (flag_LoadInToMemory == 1)
			{
				//
				//计算Diff2
				//
				Output_Data << "==========Diff2==========" << endl;
				int time1 = time(NULL);
				int time2 = 0;
				Diff2.push_back(-1);
				//Diff2[0] = -1;
				for (Iterator_i = 1; Iterator_i < num; ++Iterator_i)
				{
					cv::Mat matSrc1, matSrc2;
					CV_Assert(frame[Iterator_i].channels() == 3);
					cv::resize(frame[Iterator_i], matSrc1, cv::Size(357, 419), 0, 0, cv::INTER_NEAREST);
					cv::resize(frame[Iterator_i - 1], matSrc2, cv::Size(2177, 3233), 0, 0, cv::INTER_LANCZOS4);
					cv::Mat matDst1, matDst2;
					cv::resize(matSrc1, matDst1, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
					cv::resize(matSrc2, matDst2, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
					cv::Mat temp1 = matDst1;
					cv::Mat temp2 = matDst2;
					cv::cvtColor(temp1, matDst1, CV_BGR2GRAY);
					cv::cvtColor(temp2, matDst2, CV_BGR2GRAY);
					int iAvg1 = 0, iAvg2 = 0;
					int arr1[64], arr2[64];
					for (int i = 0; i < 8; i++)
					{
						uchar* data1 = matDst1.ptr<uchar>(i);
						uchar* data2 = matDst2.ptr<uchar>(i);
						int tmp = i * 8;
						for (int j = 0; j < 8; j++)
						{
							int tmp1 = tmp + j;
							arr1[tmp1] = data1[j] / 4 * 4;
							arr2[tmp1] = data2[j] / 4 * 4;
							iAvg1 += arr1[tmp1];
							iAvg2 += arr2[tmp1];
						}
					}
					iAvg1 /= 64;
					iAvg2 /= 64;
					for (int i = 0; i < 64; i++)
					{
						arr1[i] = (arr1[i] >= iAvg1) ? 1 : 0;
						arr2[i] = (arr2[i] >= iAvg2) ? 1 : 0;
					}
					int iDiffNum = 0;
					for (int i = 0; i < 64; i++)
						if (arr1[i] != arr2[i])
							++iDiffNum;
					if (iDiffNum <= 5)
					{
						SystemTime();
						cout << TimeString;
						cout << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Similar!" << endl;
						Output_Log << TimeString;
						Output_Log << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Similar!" << endl;
						Output_Data << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Similar!" << endl;
					}
					else if (iDiffNum > 5 && iDiffNum <= 10)
					{
						SystemTime();
						cout << TimeString;
						cout << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Doubt!" << endl;
						Output_Log << TimeString;
						Output_Log << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Doubt!" << endl;
						Output_Data << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Doubt!" << endl;
					}
					else if (iDiffNum > 10)
					{
						SystemTime();
						cout << TimeString;
						cout << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Different!" << endl;
						Output_Log << TimeString;
						Output_Log << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Different!" << endl;
						Output_Data << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Different!" << endl;
					}
					Diff2.push_back(iDiffNum);
					//Diff2[i] = iDiffNum;
				}
				time2 = time(NULL);
				Output_Log << TimeString;
				Output_Log << " [INFO]Compute Diff2 use time " << time2 - time1 << " seconds" << endl;
				Output_Data << "Compute Diff2 use time " << time2 - time1 << " seconds" << endl;
				//
				//计算Diff3
				//
				Output_Data << "==========Diff3==========" << endl;
				int time3 = time(NULL);
				int time4 = 0;
				Diff3.push_back(-1);
				Diff3.push_back(-1);
				//Diff3[0] = -1;
				//Diff3[1] = -1;
				for (Iterator_i = 2; Iterator_i < num; ++Iterator_i)
				{
					cv::Mat matSrc1, matSrc2;
					CV_Assert(frame[Iterator_i].channels() == 3);
					cv::resize(frame[Iterator_i], matSrc1, cv::Size(357, 419), 0, 0, cv::INTER_NEAREST);
					cv::resize(frame[Iterator_i - 2], matSrc2, cv::Size(2177, 3233), 0, 0, cv::INTER_LANCZOS4);
					cv::Mat matDst1, matDst2;
					cv::resize(matSrc1, matDst1, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
					cv::resize(matSrc2, matDst2, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
					cv::Mat temp1 = matDst1;
					cv::Mat temp2 = matDst2;
					cv::cvtColor(temp1, matDst1, CV_BGR2GRAY);
					cv::cvtColor(temp2, matDst2, CV_BGR2GRAY);
					int iAvg1 = 0, iAvg2 = 0;
					int arr1[64], arr2[64];
					for (int i = 0; i < 8; i++)
					{
						uchar* data1 = matDst1.ptr<uchar>(i);
						uchar* data2 = matDst2.ptr<uchar>(i);
						int tmp = i * 8;
						for (int j = 0; j < 8; j++)
						{
							int tmp1 = tmp + j;
							arr1[tmp1] = data1[j] / 4 * 4;
							arr2[tmp1] = data2[j] / 4 * 4;
							iAvg1 += arr1[tmp1];
							iAvg2 += arr2[tmp1];
						}
					}
					iAvg1 /= 64;
					iAvg2 /= 64;
					for (int i = 0; i < 64; i++)
					{
						arr1[i] = (arr1[i] >= iAvg1) ? 1 : 0;
						arr2[i] = (arr2[i] >= iAvg2) ? 1 : 0;
					}
					int iDiffNum = 0;
					for (int i = 0; i < 64; i++)
						if (arr1[i] != arr2[i])
							++iDiffNum;
					if (iDiffNum <= 5)
					{
						SystemTime();
						cout << TimeString;
						cout << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Similar!" << endl;
						Output_Log << TimeString;
						Output_Log << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Similar!" << endl;
						Output_Data << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Similar!" << endl;
					}
					else if (iDiffNum > 5 && iDiffNum <= 10)
					{
						SystemTime();
						cout << TimeString;
						cout << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Doubt!" << endl;
						Output_Log << TimeString;
						Output_Log << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Doubt!" << endl;
						Output_Data << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Doubt!" << endl;
					}
					else if (iDiffNum > 10)
					{
						SystemTime();
						cout << TimeString;
						cout << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Different!" << endl;
						Output_Log << TimeString;
						Output_Log << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Different!" << endl;
						Output_Data << " [iDiffNum = " << iDiffNum << "] between[" << Iterator_i - 1 << "]and[" << Iterator_i << "] Different!" << endl;
					}
					Diff3.push_back(iDiffNum);
					//Diff3[i] = iDiffNum;
				}
				time4 = time(NULL);
				Output_Log << TimeString;
				Output_Log << " [INFO]Compute Diff3 use time " << time4 - time3 << " seconds" << endl;
				Output_Data << "Compute Diff3 use time " << time4 - time3 << " seconds" << endl;
				flag_CalculateDiff = 1;
			}
			else
			{
				SystemTime();
				cout << TimeString;
				cout << " [INFO]您尚未载入内存" << endl;
				Output_Log << TimeString;
				Output_Log << " [INFO]您尚未载入内存" << endl;
			}
		}
		else if (mode == 3)
		{
			if (flag_LoadInToMemory == 1 && flag_CalculateDiff == 1)
			{
				SystemTime();
				cout << TimeString;
				cout << " [INFO]开始分析数据" << endl;
				Output_Data << "==========开始分析数据==========" << endl;
				for (Iterator_i = 0; Iterator_i < num; ++Iterator_i)
				{
					if (Iterator_i != 0 && Iterator_i != 1 && Iterator_i != (num - 1) && Iterator_i != (num - 2))
					{
						//compute
						Diff[Iterator_i] = Diff3[Iterator_i] + Diff2[Iterator_i] + Diff2[Iterator_i + 1] + Diff3[Iterator_i + 2];
						//Diff.push_back(Diff3[i] + Diff2[i] + Diff2[i + 1] + Diff3[i + 2]);
					}
					else
					{
						Diff[Iterator_i] = -1;
						//Diff.push_back(-1);
					}
				}
				for (Iterator_i = 0; Iterator_i < num; ++Iterator_i)
				{
					SystemTime();
					cout << TimeString;
					cout << " [INFO]Diff of Frame " << Iterator_i + 1 << " is " << Diff[Iterator_i] << endl;
					Output_Data << "Diff of Frame " << Iterator_i + 1 << " is " << Diff[Iterator_i] << endl;
				}
				for (Iterator_i = 0; Iterator_i < num; ++Iterator_i)
				{
					if (Diff[Iterator_i] >= 50)
					{
						SystemTime();
						cout << TimeString;
						cout << " [INFO]Frame " << Iterator_i + 1 << " may be even!" << endl;
						Output_Data << "Diff of Frame " << Iterator_i + 1 << " may be even!" << endl;
						Target[Iterator_j] = Iterator_i;
						Iterator_j += 1;
					}
				}
				flag_AnalyseDiff = 1;
				//Debug
				for (Iterator_i = 0; Target[Iterator_i] != -1; ++Iterator_i)
				{
					cout << "Target[ " << Iterator_i << " ]= " << Target[Iterator_i] << endl;
				}
			}
			else if (flag_LoadInToMemory == 1 && flag_CalculateDiff != 1)
			{
				SystemTime();
				cout << TimeString;
				cout << " [INFO]您载入内存但未计算" << endl;
				Output_Log << TimeString;
				Output_Log << " [INFO]您载入内存但未计算" << endl;
			}
			else if (flag_LoadInToMemory != 1)
			{
				SystemTime();
				cout << TimeString;
				cout << " [INFO]您尚未载入内存" << endl;
				Output_Log << TimeString;
				Output_Log << " [INFO]您尚未载入内存" << endl;
			}
		}
		else if (mode == 4)
		{
			if (flag_LoadInToMemory != 1)
			{
				SystemTime();
				cout << TimeString;
				cout << " [INFO]您尚未载入内存，您至少需要载入内存" << endl;
				Output_Log << TimeString;
				Output_Log << " [INFO]您尚未载入内存，您至少需要载入内存" << endl;
				continue;
			}
			else if (flag_LoadInToMemory == 1 && (flag_CalculateDiff != 1 || flag_AnalyseDiff != 1))
			{
				SystemTime();
				cout << TimeString;
				cout << " [INFO]您载入内存但未计算并分析结果，将显示默认示范序列" << endl;
				Output_Log << TimeString;
				Output_Log << " [INFO]您载入内存但未计算并分析结果，将显示默认示范序列" << endl;
				flag_EasyXDebug = 1;
			}
			else
			{
				SystemTime();
				cout << TimeString;
				cout << " [INFO]启动Easy X" << endl;
				Output_Log << TimeString;
				Output_Log << " [INFO]启动Easy X" << endl;
			}
			initgraph(WINDOW_W, WINDOW_H, SHOWCONSOLE);
			setfillcolor(DARKGRAY);
			solidrectangle(0, 0, WINDOW_W, WINDOW_H);
			TCHAR HELLOWORLD[] = { TEXT("以下为检测到的疑似诡异帧") };
			setbkmode(TRANSPARENT);
			settextstyle(50, 0, _T("黑体"));
			outtextxy(390,55,HELLOWORLD);

			int ImageWindowEdge = 25;
			int ImageWindowHigh = 320;
			int ImageWindowL = 0 + ImageWindowEdge;
			int ImageWindowU = 150;
			int ImageWindowR = WINDOW_W - ImageWindowEdge;
			int ImageWindowD = ImageWindowU + ImageWindowHigh;
			int ImageWindowWide = ImageWindowR - ImageWindowL;
			cout << "ImageWindowWide= " << ImageWindowR - ImageWindowL << endl;
			cout << "ImageWindowHigh= " << ImageWindowHigh << endl;
			rectangle(ImageWindowL, ImageWindowU, ImageWindowR, ImageWindowD);//ImageWindow是总的框框

			//Magic Number 备注：9/16==0.5625

			int FrameEdgeX = 5;//帧之间和帧与ImageWindow的横向宽窄
			int FrameEdgeY = (int)(ImageWindowHigh - (((ImageWindowWide - 6 * FrameEdgeX) / 5)*(0.5625))) / 2;//帧与ImageWindow的纵向间距
			cout << "FrameEdgeX= " << FrameEdgeX << endl;
			cout << "FrameEdgeY= " << FrameEdgeY << endl;

			int FrameWide = (int)((ImageWindowWide - 6 * FrameEdgeX) / 5);//帧宽度
			int FrameHigh =(FrameWide*(0.5625));//帧高度
			cout << "FrameWide= " << FrameWide << endl;
			cout << "FrameHigh= " << FrameHigh << endl;

			int FrameU = ImageWindowU + FrameEdgeY;
			int FrameD = ImageWindowD - FrameEdgeY;
			
			int Frame1L = ImageWindowL + FrameEdgeX;
			int Frame1R = Frame1L + FrameWide;
			int Frame2L = Frame1L + FrameWide + FrameEdgeX;
			int Frame2R = Frame2L + FrameWide;
			int Frame3L = Frame2L + FrameWide + FrameEdgeX;
			int Frame3R = Frame3L + FrameWide;
			int Frame4L = Frame3L + FrameWide + FrameEdgeX;
			int Frame4R = Frame4L + FrameWide;
			int Frame5L = Frame4L + FrameWide + FrameEdgeX;
			int Frame5R = Frame5L + FrameWide;
			rectangle(Frame1L, FrameU, Frame1R+1, FrameD+1);
			rectangle(Frame2L, FrameU, Frame2R+1, FrameD+1);
			rectangle(Frame3L, FrameU, Frame3R+1, FrameD+1);
			rectangle(Frame4L, FrameU, Frame4R+1, FrameD+1);
			rectangle(Frame5L, FrameU, Frame5R+1, FrameD+1);

			Iterator_j = 0;

			int ButtonWide = 180;
			int ButtonHigh = 60;
			int ButtonEdge = 350;
			int ButtonU = 550;
			int ButtonD = ButtonU + ButtonHigh;
			int ButtonInnerEdge = (WINDOW_W - ButtonEdge * 2 - ButtonWide * 3) / 2;
			cout << "ButtonInnerEdge= " << ButtonInnerEdge << endl;
			
			int ButtonBeforeL = ButtonEdge;
			int ButtonBeforeU = ButtonU;
			int ButtonBeforeR = ButtonBeforeL + ButtonWide;
			int ButtonBeforeD = ButtonD;

			int ButtonSaveL = ButtonEdge + ButtonWide + ButtonInnerEdge;
			int ButtonSaveU = ButtonU;
			int ButtonSaveR = ButtonSaveL + ButtonWide;
			int ButtonSaveD = ButtonD;

			int ButtonAfterL = ButtonEdge + 2 * ButtonWide + 2 * ButtonInnerEdge;
			int ButtonAfterU = ButtonU;
			int ButtonAfterR = ButtonAfterL + ButtonWide;
			int ButtonAfterD = ButtonD;

			rectangle(ButtonBeforeL, ButtonU, ButtonBeforeR, ButtonD);
			rectangle(ButtonSaveL, ButtonU, ButtonSaveR, ButtonD);
			rectangle(ButtonAfterL, ButtonU, ButtonAfterR, ButtonD);

			setfillcolor(BROWN);
			solidrectangle(ButtonBeforeL + 1, ButtonU + 1, ButtonBeforeR - 1, ButtonD - 1);
			solidrectangle(ButtonSaveL + 1, ButtonU + 1, ButtonSaveR - 1, ButtonD - 1);
			solidrectangle(ButtonAfterL + 1, ButtonU + 1, ButtonAfterR - 1, ButtonD - 1);

			TCHAR BUTTON_BEFORE[] = { TEXT("上一个") };
			TCHAR BUTTON_SAVE[] = { TEXT("保存") };
			TCHAR BUTTON_AFTER[] = { TEXT("下一个") };
			settextstyle(20, 0, _T("黑体"));
			outtextxy(ButtonBeforeL + 63, ButtonU + 23, BUTTON_BEFORE);
			outtextxy(ButtonSaveL + 70, ButtonU + 23, BUTTON_SAVE);
			outtextxy(ButtonAfterL + 62, ButtonU + 23, BUTTON_AFTER);

			FrameIterator = 0;
			DrawVisualMoe(FrameIterator, Frame1L, FrameU, FrameEdgeX, FrameWide, FrameHigh);

			MOUSEMSG VisualMoeV2;
			while (1)
			{
				VisualMoeV2 = GetMouseMsg();
				switch (VisualMoeV2.uMsg)
				{
					case WM_LBUTTONDOWN:
					{
						if (VisualMoeV2.x > ButtonSaveL&&VisualMoeV2.x< ButtonSaveR&&VisualMoeV2.y>ButtonSaveU&&VisualMoeV2.y < ButtonSaveD)
						{
							//保存按钮
							SystemTime();
							cout << TimeString;
							cout << " [INFO]保存按钮" << endl;
							Output_Log << TimeString;
							Output_Log << " [INFO]保存按钮" << endl;
							TCHAR TempImageName_TCHAR[100];
							char TempImageName_char[100];
							time_t now = time(NULL);
							strftime(TimeString, 100, "%Y%m%d-%H%M%S", localtime(&now));
							sprintf(TempImageName_char, "%s%s%d%s", TimeString, "-", Target[FrameIterator] + 1, ".jpg"); //为图片命名
							CharToTchar(TempImageName_char, TempImageName_TCHAR);
							saveimage(TempImageName_TCHAR);
						}
						else if (VisualMoeV2.x > ButtonBeforeL&&VisualMoeV2.x< ButtonBeforeR&&VisualMoeV2.y>ButtonBeforeU&&VisualMoeV2.y < ButtonBeforeD)
						{
							//上一个按钮
							SystemTime();
							cout << TimeString;
							cout << " [INFO]上一个按钮" << endl;
							Output_Log << TimeString;
							Output_Log << " [INFO]上一个按钮" << endl;
							if (FrameIterator - 1 >= 0)
							{
								FrameIterator -= 1;
								DrawVisualMoe(FrameIterator, Frame1L, FrameU, FrameEdgeX, FrameWide, FrameHigh);
							}
							else
							{
								SystemTime();
								cout << TimeString;
								cout << " [INFO]已经是第一个了" << endl;
								Output_Log << TimeString;
								Output_Log << " [INFO]已经是第一个了" << endl;
							}
						}
						else if (VisualMoeV2.x > ButtonAfterL&&VisualMoeV2.x< ButtonAfterR&&VisualMoeV2.y>ButtonAfterU&&VisualMoeV2.y < ButtonAfterD)
						{
							//下一个按钮
							SystemTime();
							cout << TimeString;
							cout << " [INFO]下一个按钮" << endl;
							Output_Log << TimeString;
							Output_Log << " [INFO]下一个按钮" << endl;
							//下一个
							if (Target[FrameIterator+1] != -1)
							{
								FrameIterator += 1;
								DrawVisualMoe(FrameIterator, Frame1L, FrameU, FrameEdgeX, FrameWide, FrameHigh);
							}
							else
							{
								SystemTime();
								cout << TimeString;
								cout << " [INFO]已经是最后一个了" << endl;
								Output_Log << TimeString;
								Output_Log << " [INFO]已经是最后一个了" << endl;
							}
						}
					}
					//case WM_RBUTTONDOWN:
					//{
					//	closegraph();
					//}
				}
			}
			system("pause");
		}
		else if (mode == 5)
		{
		    exit(0);
        }
	}
	return 0;
}