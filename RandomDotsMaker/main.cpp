#pragma once

#define NOMINMAX
#include <Windows.h>
#include <opencv2\opencv.hpp>
#include <random>
#include <math.h>

#include<iostream> //���o�̓��C�u����
#include<fstream> //iostream�̃t�@�C�����o�͂��T�|�[�g

#define DOTNUM 80
#define DOTDIST 30
#define DOTSIZE 10 //���a
#define PROJECTOR_WIDTH 1280
#define PROJECTOR_HEIGHT 800

#define USEPINHOLE 1 //�s���z�[�����邩�ǂ���
#define PINHOLERAD 200 //�s���z�[���̒��a�̍ő�

#define OFFSET_X 10
#define OFFSET_Y 10

//�h�b�g
std::vector<cv::Point> dots;

int getRandomNumber(int min, int max);
double getDistance(cv::Point p1, cv::Point p2);
void saveDots(std::string filename);

//�����_���ȃh�b�g���𐶐�����
int main(int argc, char** argv)
{
	//���F�摜����
	cv::Mat dstImg(cv::Size(PROJECTOR_WIDTH, PROJECTOR_HEIGHT), CV_8UC4, cv::Scalar(0, 0, 0, 255));

	//Pinhole�̒��S
	cv::Point center(PROJECTOR_WIDTH / 2, PROJECTOR_HEIGHT / 2);

	//�����͈�
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
			if(getDistance(center, p) < (PINHOLERAD + DOTSIZE/2))
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
		//�A���t�@�Ƀu���[���ǂ���������
		for(int y = 0 ; y < PROJECTOR_HEIGHT; y++)
		{
			for(int x = 0; x < PROJECTOR_WIDTH; x++)
			{
				cv::Point p(x, y);
				double d = getDistance(center, p);
				if(d < PINHOLERAD)
				{
					dstImg.data[y * dstImg.step + x * dstImg.elemSize() + 3] = 255 * d / PINHOLERAD;
				}
			}
		}
	}

	//�h�b�g�̕`��
	for(int i = 0; i < dots.size(); i++)
	{
		cv::circle(dstImg, dots[i], DOTSIZE/2, cv::Scalar(255, 255, 255, 255), -1, CV_AA);
	}

	//Debug
	//cv::circle(dstImg, center, PINHOLERAD , cv::Scalar(0, 0, 255, 255), 1);

	//png�ŕۑ�
	cv::imwrite("dots.png", dstImg);
	//dots�̍��W�����o��
	saveDots("dots.csv");

	//�\��
	cv::imshow("dots", dstImg);
	cv::waitKey(0);

}

//min~max-1�܂ł̗����iint�j��Ԃ�
int getRandomNumber(int min, int max)
{
	std::random_device rnd;
	return ((rnd() % (max - min)) + min);
}

//�_�Ɠ_�̋�����Ԃ�
double getDistance(cv::Point p1, cv::Point p2)
{
	return abs(sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) )); 
}

//���W�̕ۑ�
void saveDots(std::string filename)
{
	std::ofstream ofs(filename);
	
	for(int i = 0; i < dots.size(); i++)
	{
		ofs << dots[i].x << "," << dots[i].y << std::endl;
	}
}