#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <vector>
using namespace std;

// OpenCV头文件
#include <opencv2\opencv.hpp>
#include <opencv2\xfeatures2d.hpp>
// using namespace cv;
// using namespace xfeatures2d;

/*
@函数功能：对两张图片的特征点进行匹配，并返回经过距离和RANSAC筛选后较好的特征点的个数
@参数 key_points1：图片1的特征点vector
@参数 key_points2：图片2的特征点vector
@参数 descriptors1：图片1的特征矩阵
@参数 descriptors2：图片2的特征矩阵
*/
int Match(const vector<cv::KeyPoint> &key_points1, const vector<cv::KeyPoint> &key_points2,
	const cv::Mat &descriptors1, const cv::Mat &descriptors2);

/*
@函数功能：加载存储了图片序列的特征点和特征向量的XML文件至内存
@参数 xml_file_name：项目目录内的XML文件的文件名或XML文件的完整路径
@参数 key_points：内存中存储特征点vector们的vector
@参数 descriptors：内存中存储特征向量的vector
*/
bool LoadXML(const cv::String &xml_file_name, vector< vector<cv::KeyPoint> > &key_points,
	vector<cv::Mat> &descriptors);

/*
@函数功能：显示对应编号的标题
@参数 no：需要展示标题的编号
*/
void ShowResult(int no);

/*
@参数功能：创建一个窗口显示视频
@参数 file_name：需要展示的视频的路径或文件名
*/
void ShowVideo(cv::String file_name);

#endif // !FUNCTIONS_H_
