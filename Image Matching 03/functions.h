#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <vector>
using namespace std;

// OpenCVͷ�ļ�
#include <opencv2\opencv.hpp>
#include <opencv2\xfeatures2d.hpp>
// using namespace cv;
// using namespace xfeatures2d;

/*
@�������ܣ�������ͼƬ�����������ƥ�䣬�����ؾ��������RANSACɸѡ��Ϻõ�������ĸ���
@���� key_points1��ͼƬ1��������vector
@���� key_points2��ͼƬ2��������vector
@���� descriptors1��ͼƬ1����������
@���� descriptors2��ͼƬ2����������
*/
int Match(const vector<cv::KeyPoint> &key_points1, const vector<cv::KeyPoint> &key_points2,
	const cv::Mat &descriptors1, const cv::Mat &descriptors2);

/*
@�������ܣ����ش洢��ͼƬ���е������������������XML�ļ����ڴ�
@���� xml_file_name����ĿĿ¼�ڵ�XML�ļ����ļ�����XML�ļ�������·��
@���� key_points���ڴ��д洢������vector�ǵ�vector
@���� descriptors���ڴ��д洢����������vector
*/
bool LoadXML(const cv::String &xml_file_name, vector< vector<cv::KeyPoint> > &key_points,
	vector<cv::Mat> &descriptors);

/*
@�������ܣ���ʾ��Ӧ��ŵı���
@���� no����Ҫչʾ����ı��
*/
void ShowResult(int no);

/*
@�������ܣ�����һ��������ʾ��Ƶ
@���� file_name����Ҫչʾ����Ƶ��·�����ļ���
*/
void ShowVideo(cv::String file_name);

#endif // !FUNCTIONS_H_
