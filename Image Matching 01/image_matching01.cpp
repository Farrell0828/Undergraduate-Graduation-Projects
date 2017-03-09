#include <iostream>  
#include <opencv2/opencv.hpp>    
#include "opencv2/xfeatures2d.hpp"

using namespace cv;
using namespace std;
using namespace xfeatures2d;

int main()
{
	Ptr<SURF> detector = SURF::create(800);		//����SURF���������
	Ptr<SURF> descriptor_extractor = SURF::create(800);//������������������

	Ptr<DescriptorMatcher> descriptor_matcher =
		DescriptorMatcher::create("BruteForce");//��������ƥ����

	if (detector.empty() || descriptor_extractor.empty())
		cout << "fail to create detector!";

	Mat img1 = imread("piston0.jpg");		// ģ��
	Mat img2 = imread("piston2.jpg");		// ��������
	//Mat img2 = imread("piston3.jpg");		// ������
	//Mat img2 = imread("piston8.jpg");		// ��������
	//Mat img2 = imread("piston9.jpg");		// ��ת����

	//��������    
	double t = double(getTickCount());//��ǰ�δ���    
	vector<KeyPoint> m_LeftKey, m_RightKey;
	detector->detect(img1, m_LeftKey);//���img1�е������㣬�洢��m_LeftKey��    
	detector->detect(img2, m_RightKey);
	cout << "ͼ��1���������:" << m_LeftKey.size() << endl;
	cout << "ͼ��2���������:" << m_RightKey.size() << endl;

	//����������������������Ӿ��󣬼�������������
	Mat descriptors1, descriptors2;
	//detector->compute(img1, m_LeftKey, descriptors1);
	descriptor_extractor->compute(img1, m_LeftKey, descriptors1);
	descriptor_extractor->compute(img2, m_RightKey, descriptors2);
	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "SUFT�㷨��ʱ��" << t << "��" << endl;

	cout << "ͼ��1�������������С��" << descriptors1.size()
		<< "����������������" << descriptors1.rows << "��ά����" << descriptors1.cols << endl;
	cout << "ͼ��2�������������С��" << descriptors2.size()
		<< "����������������" << descriptors2.rows << "��ά����" << descriptors2.cols << endl;

	//����������    
	Mat img_m_LeftKey, img_m_RightKey;
	drawKeypoints(img1, m_LeftKey, img_m_LeftKey, Scalar::all(-1), 0);
	drawKeypoints(img2, m_RightKey, img_m_RightKey, Scalar::all(-1), 0);
	imshow("Src1", img_m_LeftKey);
	imshow("Src2", img_m_RightKey);
	waitKey();

	//����ƥ��    
	vector<DMatch> matches;//ƥ����    
	descriptor_matcher->match(descriptors1, descriptors2, matches);//ƥ������ͼ�����������    
	cout << "Match������" << matches.size() << endl;

	// �������ƥ��Ľ��
	Mat img_matchs;
	drawMatches(img1, m_LeftKey, img2, m_RightKey, matches, img_matchs,
		Scalar::all(-1)/*CV_RGB(255,0,0)*/, CV_RGB(0, 255, 0), Mat(), 2);
	destroyAllWindows();
	imshow("MatchSURF", img_matchs);
	waitKey();

	//����ƥ�����о����������Сֵ    
	//������ָ���������������ŷʽ���룬�������������Ĳ��죬ֵԽС��������������Խ�ӽ�    
	double max_dist = 0;
	double min_dist = 100;
	for (int i = 0; i < matches.size(); i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	cout << "�����룺" << max_dist << endl;
	cout << "��С���룺" << min_dist << endl;

	//ͨ��ŷʽ����ɸѡ���Ϻõ�ƥ���    
	vector<DMatch> goodMatches;
	for (int i = 0; i < matches.size(); i++)
	{
		if (matches[i].distance < 0.2 * max_dist)
		{
			goodMatches.push_back(matches[i]);
		}
	}
	cout << "goodMatch������" << goodMatches.size() << endl;

	//����ŷʽ����ƥ����    
	Mat img_good_matchs;
	//��ɫ���ӵ���ƥ���������ԣ���ɫ��δƥ���������    
	drawMatches(img1, m_LeftKey, img2, m_RightKey, goodMatches, img_good_matchs,
		Scalar::all(-1)/*CV_RGB(255,0,0)*/, CV_RGB(0, 255, 0), Mat(), 2);
	destroyAllWindows();
	imshow("GoodMatchSUFT", img_good_matchs);
	waitKey(0);
	if (goodMatches.size() == 0)	// ���û�кõ�ƥ������ֱ�ӽ���
		return -1;

	IplImage result = img_good_matchs;

	//RANSACƥ�����  
	vector<DMatch> m_Matches = goodMatches;
	// ����ռ�  
	int ptCount = (int)m_Matches.size();
	Mat p1(ptCount, 2, CV_32F);
	Mat p2(ptCount, 2, CV_32F);

	// ��Keypointת��ΪMat  
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

	// ��RANSAC��������F  
	Mat m_Fundamental;
	vector<uchar> m_RANSACStatus;       // ����������ڴ洢RANSAC��ÿ�����״̬  
	findFundamentalMat(p1, p2, m_RANSACStatus, FM_RANSAC);

	// ����Ұ�����  
	int OutlinerCount = 0;
	for (int i = 0; i<m_RANSACStatus.size(); i++) // �޸ĵ� ptCount->m_RANSACStatus.size()
	{
		if (m_RANSACStatus[i] == 0)    // ״̬Ϊ0��ʾҰ��  
		{
			OutlinerCount++;
		}
	}
	int InlinerCount = ptCount - OutlinerCount;   // �����ڵ�  
	cout << "�ڵ���Ϊ��" << InlinerCount << endl;


	// �������������ڱ����ڵ��ƥ���ϵ  
	vector<Point2f> m_LeftInlier;
	vector<Point2f> m_RightInlier;
	vector<DMatch> m_InlierMatches;

	m_InlierMatches.resize(InlinerCount);
	m_LeftInlier.resize(InlinerCount);
	m_RightInlier.resize(InlinerCount);
	InlinerCount = 0;
	float inlier_minRx = float(img1.cols);        //���ڴ洢�ڵ�����ͼ��С�����꣬�Ա�����ں�  

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

			if (m_RightInlier[InlinerCount].x<inlier_minRx) inlier_minRx = m_RightInlier[InlinerCount].x;   //�洢�ڵ�����ͼ��С������  

			InlinerCount++;
		}
	}

	// ���ڵ�ת��ΪdrawMatches����ʹ�õĸ�ʽ  
	vector<KeyPoint> key1(InlinerCount);
	vector<KeyPoint> key2(InlinerCount);
	KeyPoint::convert(m_LeftInlier, key1);
	KeyPoint::convert(m_RightInlier, key2);

	// ��ʾ����F������ڵ�ƥ��  
	Mat OutImage;
	drawMatches(img1, key1, img2, key2, m_InlierMatches, OutImage);
	destroyAllWindows();
	cvNamedWindow("RANSAC Match Features", 1);
	cvShowImage("RANSAC Match Features", &IplImage(OutImage));
	waitKey(0);

	//����H���Դ洢RANSAC�õ��ĵ�Ӧ����  
	Mat H = findHomography(m_LeftInlier, m_RightInlier, RANSAC);

	//�洢��ͼ�Ľǣ�����任����ͼλ��  
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = Point(0, 0); obj_corners[1] = Point(img1.cols, 0);
	obj_corners[2] = Point(img1.cols, img1.rows); obj_corners[3] = Point(0, img1.rows);
	std::vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);

	//�����任��ͼ��λ��  
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