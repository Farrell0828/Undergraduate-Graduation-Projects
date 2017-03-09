#include <iostream>  
#include <opencv2/opencv.hpp>    
#include "opencv2/xfeatures2d.hpp"

using namespace cv;
using namespace std;
using namespace xfeatures2d;

int main()
{
	Ptr<SURF> detector = SURF::create(800);		//创建SURF特征检测器
	Ptr<SURF> descriptor_extractor = SURF::create(800);//创建特征向量生成器

	Ptr<DescriptorMatcher> descriptor_matcher =
		DescriptorMatcher::create("BruteForce");//创建特征匹配器

	if (detector.empty() || descriptor_extractor.empty())
		cout << "fail to create detector!";

	Mat img1 = imread("piston0.jpg");		// 模板
	Mat img2 = imread("piston2.jpg");		// 常规样本
	//Mat img2 = imread("piston3.jpg");		// 负样本
	//Mat img2 = imread("piston8.jpg");		// 曲折样本
	//Mat img2 = imread("piston9.jpg");		// 旋转样本

	//特征点检测    
	double t = double(getTickCount());//当前滴答数    
	vector<KeyPoint> m_LeftKey, m_RightKey;
	detector->detect(img1, m_LeftKey);//检测img1中的特征点，存储到m_LeftKey中    
	detector->detect(img2, m_RightKey);
	cout << "图像1特征点个数:" << m_LeftKey.size() << endl;
	cout << "图像2特征点个数:" << m_RightKey.size() << endl;

	//根据特征点计算特征描述子矩阵，即特征向量矩阵
	Mat descriptors1, descriptors2;
	//detector->compute(img1, m_LeftKey, descriptors1);
	descriptor_extractor->compute(img1, m_LeftKey, descriptors1);
	descriptor_extractor->compute(img2, m_RightKey, descriptors2);
	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "SUFT算法用时：" << t << "秒" << endl;

	cout << "图像1特征描述矩阵大小：" << descriptors1.size()
		<< "，特征向量个数：" << descriptors1.rows << "，维数：" << descriptors1.cols << endl;
	cout << "图像2特征描述矩阵大小：" << descriptors2.size()
		<< "，特征向量个数：" << descriptors2.rows << "，维数：" << descriptors2.cols << endl;

	//画出特征点    
	Mat img_m_LeftKey, img_m_RightKey;
	drawKeypoints(img1, m_LeftKey, img_m_LeftKey, Scalar::all(-1), 0);
	drawKeypoints(img2, m_RightKey, img_m_RightKey, Scalar::all(-1), 0);
	imshow("Src1", img_m_LeftKey);
	imshow("Src2", img_m_RightKey);
	waitKey();

	//特征匹配    
	vector<DMatch> matches;//匹配结果    
	descriptor_matcher->match(descriptors1, descriptors2, matches);//匹配两个图像的特征矩阵    
	cout << "Match个数：" << matches.size() << endl;

	// 画出最初匹配的结果
	Mat img_matchs;
	drawMatches(img1, m_LeftKey, img2, m_RightKey, matches, img_matchs,
		Scalar::all(-1)/*CV_RGB(255,0,0)*/, CV_RGB(0, 255, 0), Mat(), 2);
	destroyAllWindows();
	imshow("MatchSURF", img_matchs);
	waitKey();

	//计算匹配结果中距离的最大和最小值    
	//距离是指两个特征向量间的欧式距离，表明两个特征的差异，值越小表明两个特征点越接近    
	double max_dist = 0;
	double min_dist = 100;
	for (int i = 0; i < matches.size(); i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	cout << "最大距离：" << max_dist << endl;
	cout << "最小距离：" << min_dist << endl;

	//通过欧式距离筛选出较好的匹配点    
	vector<DMatch> goodMatches;
	for (int i = 0; i < matches.size(); i++)
	{
		if (matches[i].distance < 0.2 * max_dist)
		{
			goodMatches.push_back(matches[i]);
		}
	}
	cout << "goodMatch个数：" << goodMatches.size() << endl;

	//画出欧式距离匹配结果    
	Mat img_good_matchs;
	//红色连接的是匹配的特征点对，绿色是未匹配的特征点    
	drawMatches(img1, m_LeftKey, img2, m_RightKey, goodMatches, img_good_matchs,
		Scalar::all(-1)/*CV_RGB(255,0,0)*/, CV_RGB(0, 255, 0), Mat(), 2);
	destroyAllWindows();
	imshow("GoodMatchSUFT", img_good_matchs);
	waitKey(0);
	if (goodMatches.size() == 0)	// 如果没有好的匹配点程序直接结束
		return -1;

	IplImage result = img_good_matchs;

	//RANSAC匹配过程  
	vector<DMatch> m_Matches = goodMatches;
	// 分配空间  
	int ptCount = (int)m_Matches.size();
	Mat p1(ptCount, 2, CV_32F);
	Mat p2(ptCount, 2, CV_32F);

	// 把Keypoint转换为Mat  
	Point2f pt;
	for (int i = 0; i<ptCount; i++)
	{
		pt = m_LeftKey[m_Matches[i].queryIdx].pt;
		p1.at<float>(i, 0) = pt.x;
		p1.at<float>(i, 1) = pt.y;

		pt = m_RightKey[m_Matches[i].trainIdx].pt;
		p2.at<float>(i, 0) = pt.x;
		p2.at<float>(i, 1) = pt.y;
	}

	// 用RANSAC方法计算F  
	Mat m_Fundamental;
	vector<uchar> m_RANSACStatus;       // 这个变量用于存储RANSAC后每个点的状态  
	findFundamentalMat(p1, p2, m_RANSACStatus, FM_RANSAC);

	// 计算野点个数  
	int OutlinerCount = 0;
	for (int i = 0; i<m_RANSACStatus.size(); i++) // 修改点 ptCount->m_RANSACStatus.size()
	{
		if (m_RANSACStatus[i] == 0)    // 状态为0表示野点  
		{
			OutlinerCount++;
		}
	}
	int InlinerCount = ptCount - OutlinerCount;   // 计算内点  
	cout << "内点数为：" << InlinerCount << endl;


	// 这三个变量用于保存内点和匹配关系  
	vector<Point2f> m_LeftInlier;
	vector<Point2f> m_RightInlier;
	vector<DMatch> m_InlierMatches;

	m_InlierMatches.resize(InlinerCount);
	m_LeftInlier.resize(InlinerCount);
	m_RightInlier.resize(InlinerCount);
	InlinerCount = 0;
	float inlier_minRx = float(img1.cols);        //用于存储内点中右图最小横坐标，以便后续融合  

	for (int i = 0; i<ptCount; i++)
	{
		if (m_RANSACStatus[i] != 0)
		{
			m_LeftInlier[InlinerCount].x = p1.at<float>(i, 0);
			m_LeftInlier[InlinerCount].y = p1.at<float>(i, 1);
			m_RightInlier[InlinerCount].x = p2.at<float>(i, 0);
			m_RightInlier[InlinerCount].y = p2.at<float>(i, 1);
			m_InlierMatches[InlinerCount].queryIdx = InlinerCount;
			m_InlierMatches[InlinerCount].trainIdx = InlinerCount;

			if (m_RightInlier[InlinerCount].x<inlier_minRx) inlier_minRx = m_RightInlier[InlinerCount].x;   //存储内点中右图最小横坐标  

			InlinerCount++;
		}
	}

	// 把内点转换为drawMatches可以使用的格式  
	vector<KeyPoint> key1(InlinerCount);
	vector<KeyPoint> key2(InlinerCount);
	KeyPoint::convert(m_LeftInlier, key1);
	KeyPoint::convert(m_RightInlier, key2);

	// 显示计算F过后的内点匹配  
	Mat OutImage;
	drawMatches(img1, key1, img2, key2, m_InlierMatches, OutImage);
	destroyAllWindows();
	cvNamedWindow("RANSAC Match Features", 1);
	cvShowImage("RANSAC Match Features", &IplImage(OutImage));
	waitKey(0);

	//矩阵H用以存储RANSAC得到的单应矩阵  
	Mat H = findHomography(m_LeftInlier, m_RightInlier, RANSAC);

	//存储左图四角，及其变换到右图位置  
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = Point(0, 0); obj_corners[1] = Point(img1.cols, 0);
	obj_corners[2] = Point(img1.cols, img1.rows); obj_corners[3] = Point(0, img1.rows);
	std::vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);

	//画出变换后图像位置  
	Point2f offset((float)img1.cols, 0);
	line(OutImage, scene_corners[0] + offset, scene_corners[1] + offset, Scalar(0, 255, 0), 2);
	line(OutImage, scene_corners[1] + offset, scene_corners[2] + offset, Scalar(0, 255, 0), 2);
	line(OutImage, scene_corners[2] + offset, scene_corners[3] + offset, Scalar(0, 255, 0), 2);
	line(OutImage, scene_corners[3] + offset, scene_corners[0] + offset, Scalar(0, 255, 0), 2);

	destroyAllWindows();
	imshow("Good Matches & Object detection", OutImage);
	waitKey(0);

	return 0;
}