#pragma once

#define NOMINMAX
#include <Windows.h>
#include <opencv2\opencv.hpp>
#include <random>
#include <math.h>

#include<iostream> //入出力ライブラリ
#include<fstream> //iostreamのファイル入出力をサポート

#define DOTNUM 100
#define DOTDIST 30
#define DOTSIZE 10 //直径
#define PROJECTOR_WIDTH 1280
#define PROJECTOR_HEIGHT 800

#define USEPINHOLE 1 //ピンホールするかどうか
#define PINHOLERAD 200 //ピンホールの直径の最大

#define HOLESIZE 50 //真ん中部分の半径
#define BANDSIZE 50 //フェードアウトしてるところの幅 (HOLESIZE + BANDSIZE) * 2 = PINHOLERAD

#define OFFSET_X 10
#define OFFSET_Y 10

//ドット
std::vector<cv::Point> dots;

int getRandomNumber(int min, int max);
double getDistance(cv::Point p1, cv::Point p2);
void saveDots(std::string filename);

//ランダムなドット柄を生成する
int main(int argc, char** argv)
{
	//黒色画像生成
	cv::Mat dstImg(cv::Size(PROJECTOR_WIDTH, PROJECTOR_HEIGHT), CV_8UC4, cv::Scalar(0, 0, 0, 255));

	//Pinholeの中心
	cv::Point center(PROJECTOR_WIDTH / 2, PROJECTOR_HEIGHT / 2);

	//生成範囲
	int minX = DOTSIZE/2 + OFFSET_X;
	int maxX = PROJECTOR_WIDTH - DOTSIZE/2 - OFFSET_X;
	int minY = DOTSIZE/2 + OFFSET_Y;
	int maxY = PROJECTOR_HEIGHT - DOTSIZE/2 - OFFSET_Y;

	while(dots.size() < DOTNUM)
	{
		int x = getRandomNumber(minX, maxX);
		int y = getRandomNumber(minY, maxY);
		cv::Point p(x, y);

		bool valid = true;

		if(USEPINHOLE)
		{
			if(getDistance(center, p) < (PINHOLERAD/2 + DOTSIZE/2))
			{
				continue;
			}
		}

		for(int i = 0; i < dots.size(); i++)
		{
			if(getDistance(p, dots[i]) < DOTDIST + DOTSIZE)
			{
				valid = false;
				break;
			}
		}

		if(!valid) continue;
		else dots.push_back(p);
	}

	if(USEPINHOLE)
	{
		//アルファにブラーもどきをかける
		for(int y = 0 ; y < PROJECTOR_HEIGHT; y++)
		{
			for(int x = 0; x < PROJECTOR_WIDTH; x++)
			{
				cv::Point p(x, y);
				int d = (int)(getDistance(center, p) + 0.5);
				if(d <= HOLESIZE)
				{
					dstImg.data[y * dstImg.step + x * dstImg.elemSize() + 3] = 0;
				}
				else if(d > HOLESIZE && d < (HOLESIZE + BANDSIZE) )
				{
					dstImg.data[y * dstImg.step + x * dstImg.elemSize() + 3] =  255 * (d-HOLESIZE) / BANDSIZE;
				}
			}
		}
	}

	//ドットの描画
	for(int i = 0; i < dots.size(); i++)
	{
		cv::circle(dstImg, dots[i], DOTSIZE/2, cv::Scalar(255, 255, 255, 255), -1, CV_AA);
	}

	//Debug
	//cv::circle(dstImg, center, PINHOLERAD , cv::Scalar(0, 0, 255, 255), 1);

	//pngで保存
	cv::imwrite("dots.png", dstImg);
	//dotsの座標書き出し
	saveDots("dots.csv");

	//表示
	cv::imshow("dots", dstImg);
	cv::waitKey(0);

}

//min~max-1までの乱数（int）を返す
int getRandomNumber(int min, int max)
{
	std::random_device rnd;
	return ((rnd() % (max - min)) + min);
}

//点と点の距離を返す
double getDistance(cv::Point p1, cv::Point p2)
{
	return abs(sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) )); 
}

//座標の保存
void saveDots(std::string filename)
{
	std::ofstream ofs(filename);
	
	for(int i = 0; i < dots.size(); i++)
	{
		ofs << dots[i].x << "," << dots[i].y << std::endl;
	}
}