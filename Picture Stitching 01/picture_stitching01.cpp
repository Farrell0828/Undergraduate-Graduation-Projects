#include <iostream>
#include <opencv2/opencv.hpp>    
#include <opencv2/xfeatures2d.hpp>

using namespace cv;
using namespace std;
using namespace xfeatures2d;

// ����ɸѡʱָ��С����������ٱ���ʱ��Ϻõ�ƥ���
const double DISTANCE_RATE_LIMIT = 0.15;

int main()
{
	Ptr<SURF> detector = SURF::create(800);				// ����SURF���������
	Ptr<SURF> descriptor_extractor = SURF::create(800);	// ������������������
	Ptr<DescriptorMatcher> descriptor_matcher = 
		DescriptorMatcher::create("BruteForce");		//��������ƥ����    
	if (detector.empty() || descriptor_extractor.empty())
	{
		cout << "�������������ʧ�ܣ����������\n";
		return -1;
	}

	// ����ͼ��    
	Mat img1 = imread("M2-1.jpg");
	Mat img2 = imread("M2-2.jpg");

	// ����ͼ���С
	//resize(img1, img1, img1.size() / 2);
	//resize(img2, img2, img2.size() / 2);

	// ��������
	double t = (double)getTickCount();	// ��ǰ�δ���������ͳ���㷨��ʱ   
	vector<KeyPoint> m_LeftKey, m_RightKey;
	detector->detect(img1, m_LeftKey);	// ���img1�е�SUFT�����㣬�洢��m_LeftKey��
	detector->detect(img2, m_RightKey);	// ���img2�е�SUFT�����㣬�洢��m_RightKey��
	cout << "ͼ��1���������:" << m_LeftKey.size() << endl;
	cout << "ͼ��2���������:" << m_RightKey.size() << endl;

	// ����������������������Ӿ��󣬼�������������
	Mat descriptors1, descriptors2;
	descriptor_extractor->compute(img1, m_LeftKey, descriptors1);
	descriptor_extractor->compute(img2, m_RightKey, descriptors2);
	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "SUFT�㷨��ʱ��" << t << "��" << endl;

	cout << "ͼ��1�������������С��" << descriptors1.size()
		<< "����������������" << descriptors1.rows << "��ά����" << descriptors1.cols << endl;
	cout << "ͼ��2�������������С��" << descriptors2.size()
		<< "����������������" << descriptors2.rows << "��ά����" << descriptors2.cols << endl;

	// ����������
	Mat img_m_LeftKey, img_m_RightKey;
	drawKeypoints(img1, m_LeftKey, img_m_LeftKey, Scalar::all(-1), 0);
	drawKeypoints(img2, m_RightKey, img_m_RightKey, Scalar::all(-1), 0);
	imshow("Img1", img_m_LeftKey);
	imshow("Img2", img_m_RightKey);

	// ����ƥ��
	vector<DMatch> matches;		// ���ڴ洢ƥ����
	descriptor_matcher->match(descriptors1, descriptors2, matches);	//ƥ������ͼ�����������    
	cout << "Match������" << matches.size() << endl;

	if (matches.size() <= 0)	// ���û��ƥ�䵽������
	{
		cout << "û��ƥ�䵽������, �����˳���\n";
		return -1;
	}

	// ���������������ƥ�����
	Mat img_first_matches;
	drawMatches(img1, m_LeftKey, img2, m_RightKey, matches, img_first_matches,
		Scalar::all(-1), CV_RGB(0, 255, 0), Mat(), 2);
	imshow("�����������ƥ�����", img_first_matches);
	waitKey(0);

	// ����ƥ�����о����������Сֵ
	// ������ָ���������������ŷʽ���룬�������������Ĳ��죬ֵԽС��������������Խ�ӽ�
	double max_dist = 0;
	double min_dist = 100;
	for (int i = 0; i<matches.size(); i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	cout << "�����룺" << max_dist << endl;
	cout << "��С���룺" << min_dist << endl;

	// ͨ������ɸѡ���Ϻõ�ƥ���
	vector<DMatch> good_matches;
	for (int i = 0; i<matches.size(); i++)
	{
		if (matches[i].distance < DISTANCE_RATE_LIMIT * max_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}
	cout << "��������ɸѡ��goodMatch������" << good_matches.size() << endl;

	if (good_matches.size() <= 0)
	{
		cout << "��������ɸѡ��û�кõ�ƥ��㣬�����˳���\n";
		return -1;
	}

	// ������������ɸѡ��ƥ����
	Mat img_matches;
	drawMatches(img1, m_LeftKey, img2, m_RightKey, good_matches, img_matches,
		Scalar::all(-1), CV_RGB(0, 255, 0), Mat(), 2);
	imshow("��������ɸѡ���ƥ�����", img_matches);
	waitKey(0);

	//RANSACƥ�����  
	vector<DMatch> m_matches = good_matches;		// ���þ���ɸѡ��Ľ���RANSAC
	//vector<DMatch> m_matches = matches;				// ��������Ľ���RANSAC
	// ����ռ�  
	int ptCount = (int)m_matches.size();
	Mat p1(ptCount, 2, CV_32F);
	Mat p2(ptCount, 2, CV_32F);

	// ��Keypointת��ΪMat  
	Point2f pt;
	for (int i = 0; i < ptCount; i++)
	{
		pt = m_LeftKey[m_matches[i].queryIdx].pt;
		p1.at<float>(i, 0) = pt.x;
		p1.at<float>(i, 1) = pt.y;

		pt = m_RightKey[m_matches[i].trainIdx].pt;
		p2.at<float>(i, 0) = pt.x;
		p2.at<float>(i, 1) = pt.y;
	}

	// ��RANSAC��������F  
	Mat m_Fundamental;
	vector<uchar> m_RANSAC_status;       // ����������ڴ洢RANSAC��ÿ�����״̬
	findFundamentalMat(p1, p2, m_RANSAC_status, FM_RANSAC);

	// ����Ұ�����  
	int outlinerCount = 0;
	for (int i = 0; i<ptCount; i++)
	{
		if (m_RANSAC_status[i] == 0)    // ״̬Ϊ0��ʾҰ��  
		{
			outlinerCount++;
		}
	}
	int inlinerCount = ptCount - outlinerCount;   // ��ƥ��������ȥҰ����������ڵ����  
	cout << "�ڵ���Ϊ��" << inlinerCount << endl;

	// �������������ڱ����ڵ��ƥ���ϵ
	vector<Point2f> m_LeftInlier;		// ������ͼ���ڵ��ǵ�����
	vector<Point2f> m_RightInlier;		// ������ͼ���ڵ��ǵ�����
	vector<DMatch> m_InlierMatches;		// �����ڵ��ǵ�ƥ���ϵ

	// �����ߵĴ�С����Ϊ�ڵ������С
	m_InlierMatches.resize(inlinerCount);
	m_LeftInlier.resize(inlinerCount);
	m_RightInlier.resize(inlinerCount);

	inlinerCount = 0;
	float inlier_minRx = img1.cols;		//���ڴ洢�ڵ�����ͼ��С�����꣬�Ա�����ں�  

	for (int i = 0; i<ptCount; i++)
	{
		if (m_RANSAC_status[i] != 0)	// �������Ұ�㣬�����ڵ�
		{
			m_LeftInlier[inlinerCount].x = p1.at<float>(i, 0);
			m_LeftInlier[inlinerCount].y = p1.at<float>(i, 1);
			m_RightInlier[inlinerCount].x = p2.at<float>(i, 0);
			m_RightInlier[inlinerCount].y = p2.at<float>(i, 1);
			m_InlierMatches[inlinerCount].queryIdx = inlinerCount;
			m_InlierMatches[inlinerCount].trainIdx = inlinerCount;

			if (m_RightInlier[inlinerCount].x < inlier_minRx)	// �����ǰ�ڵ����ͼ�������Ŀǰ���С
			{
				inlier_minRx = m_RightInlier[inlinerCount].x;   // �洢�ڵ�����ͼ��С������
			}
			inlinerCount++;
		}
	}

	// ���ڵ�ת��ΪdrawMatches����ʹ�õĸ�ʽ
	vector<KeyPoint> key1(inlinerCount);
	vector<KeyPoint> key2(inlinerCount);
	KeyPoint::convert(m_LeftInlier, key1);
	KeyPoint::convert(m_RightInlier, key2);

	// ��ʾ����F������ڵ�ƥ��  
	Mat outImage;
	drawMatches(img1, key1, img2, key2, m_InlierMatches, outImage);
	imshow("RANSACɸѡ���ƥ�����", outImage);
	waitKey(0);

	//����H���Դ洢RANSAC�õ��ĵ�Ӧ����
	Mat H = findHomography(m_LeftInlier, m_RightInlier, RANSAC);
	if (H.empty())		// ���H����Ϊ�գ���H�����ܱ����Ƴ���
	{
		cout << "�޷����H���󣬳��������\n";
		return -1;
	}

	//�洢��ͼ�Ľǣ�����任����ͼλ��
	vector<Point2f> obj_corners(4);
	obj_corners[0] = Point(0, 0); obj_corners[1] = Point(img1.cols, 0);
	obj_corners[2] = Point(img1.cols, img1.rows); obj_corners[3] = Point(0, img1.rows);
	vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);

	//�����任��ͼ��λ��  
	Point2f offset((float)img1.cols, 0);
	line(outImage, scene_corners[0] + offset, scene_corners[1] + offset, Scalar(0, 255, 0), 4);
	line(outImage, scene_corners[1] + offset, scene_corners[2] + offset, Scalar(0, 255, 0), 4);
	line(outImage, scene_corners[2] + offset, scene_corners[3] + offset, Scalar(0, 255, 0), 4);
	line(outImage, scene_corners[3] + offset, scene_corners[0] + offset, Scalar(0, 255, 0), 4);
	imshow("�任��ͼ���λ��", outImage);
	waitKey(0);

	int drift = scene_corners[1].x;		//����ƫ����

	//�½�һ������洢��׼���Ľǵ�λ��  
	int width = int(max(abs(scene_corners[1].x), abs(scene_corners[2].x)));
	int height = img1.rows;
	//���ߣ�int height = int(max(abs(scene_corners[2].y), abs(scene_corners[3].y)));
	float origin_x = 0.0, origin_y = 0.0;
	if (scene_corners[0].x < 0)
	{
		if (scene_corners[3].x < 0)
		{
			origin_x += min(scene_corners[0].x, scene_corners[3].x);
		}
		else
		{
			origin_x += scene_corners[0].x;
		}
	}

	width -= int(origin_x);
	if (scene_corners[0].y<0)
	{
		if (scene_corners[1].y)
		{
			origin_y += min(scene_corners[0].y, scene_corners[1].y);
		}
		else
		{
			origin_y += scene_corners[0].y;
		}
	}
	//��ѡ��height-=int(origin_y);  
	Mat imageturn = Mat::zeros(width, height, img1.type());

	//��ȡ�µı任����ʹͼ��������ʾ  
	for (int i = 0; i<4; i++)
	{
		scene_corners[i].x -= origin_x;
	}
	//��ѡ��scene_corners[i].y -= (float)origin_y; }  
	Mat H1 = getPerspectiveTransform(obj_corners, scene_corners);

	//����ͼ��任����ʾЧ��  
	warpPerspective(img1, imageturn, H1, Size(width, height));
	imshow("��ͼ�ı任���", imageturn);
	waitKey(0);

	//ͼ���ں�  
	int width_ol = width - int(inlier_minRx - origin_x);
	int start_x = int(inlier_minRx - origin_x);
	cout << "width: " << width << endl;
	cout << "img1.width: " << img1.cols << endl;
	cout << "start_x: " << start_x << endl;
	cout << "width_ol: " << width_ol << endl;

	uchar* ptr = imageturn.data;
	double alpha = 0, beta = 1;
	for (int row = 0; row<height; row++)
	{
		ptr = imageturn.data + row*imageturn.step + (start_x)*imageturn.elemSize();
		for (int col = 0; col<width_ol; col++)
		{
			uchar* ptr_c1 = ptr + imageturn.elemSize1();
			uchar* ptr_c2 = ptr_c1 + imageturn.elemSize1();
			uchar* ptr2 = img2.data + row*img2.step + (col + int(inlier_minRx))*img2.elemSize();
			uchar* ptr2_c1 = ptr2 + img2.elemSize1();
			uchar* ptr2_c2 = ptr2_c1 + img2.elemSize1();

			alpha = double(col) / double(width_ol);
			beta = 1 - alpha;

			if (*ptr == 0 && *ptr_c1 == 0 && *ptr_c2 == 0)
			{
				*ptr = (*ptr2);
				*ptr_c1 = (*ptr2_c1);
				*ptr_c2 = (*ptr2_c2);
			}
			*ptr = (*ptr)*beta + (*ptr2)*alpha;
			*ptr_c1 = (*ptr_c1)*beta + (*ptr2_c1)*alpha;
			*ptr_c2 = (*ptr_c2)*beta + (*ptr2_c2)*alpha;
			ptr += imageturn.elemSize();
		}
	}

	Mat img_result = Mat::zeros(height, width + img2.cols - drift, img1.type());
	uchar* ptr_r = imageturn.data;

	for (int row = 0; row<height; row++)
	{
		ptr_r = img_result.data + row*img_result.step;
		for (int col = 0; col<imageturn.cols; col++)
		{
			uchar* ptr_rc1 = ptr_r + imageturn.elemSize1();
			uchar*  ptr_rc2 = ptr_rc1 + imageturn.elemSize1();
			uchar* ptr = imageturn.data + row*imageturn.step + col*imageturn.elemSize();
			uchar* ptr_c1 = ptr + imageturn.elemSize1();
			uchar*  ptr_c2 = ptr_c1 + imageturn.elemSize1();

			*ptr_r = *ptr;
			*ptr_rc1 = *ptr_c1;
			*ptr_rc2 = *ptr_c2;
			ptr_r += img_result.elemSize();
		}

		ptr_r = img_result.data + row*img_result.step + imageturn.cols*img_result.elemSize();
		for (int col = imageturn.cols; col<img_result.cols; col++)
		{
			uchar* ptr_rc1 = ptr_r + imageturn.elemSize1();
			uchar*  ptr_rc2 = ptr_rc1 + imageturn.elemSize1();
			uchar* ptr2 = img2.data + row*img2.step + (col - imageturn.cols + drift)*img2.elemSize();
			uchar* ptr2_c1 = ptr2 + img2.elemSize1();
			uchar* ptr2_c2 = ptr2_c1 + img2.elemSize1();

			*ptr_r = *ptr2;
			*ptr_rc1 = *ptr2_c1;
			*ptr_rc2 = *ptr2_c2;
			ptr_r += img_result.elemSize();
		}
	}

	imshow("ͼ���ںϺ�Ľ��", img_result);
	waitKey(0);

	return 0;
}