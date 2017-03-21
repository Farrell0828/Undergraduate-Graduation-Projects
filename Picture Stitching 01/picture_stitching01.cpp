#include <iostream>
#include <opencv2/opencv.hpp>    
#include <opencv2/xfeatures2d.hpp>

using namespace cv;
using namespace std;
using namespace xfeatures2d;

// 距离筛选时指定小于最大距离多少比例时算较好的匹配点
const double DISTANCE_RATE_LIMIT = 0.15;

int main()
{
	Ptr<SURF> detector = SURF::create(800);				// 创建SURF特征检测器
	Ptr<SURF> descriptor_extractor = SURF::create(800);	// 创建特征向量生成器
	Ptr<DescriptorMatcher> descriptor_matcher = 
		DescriptorMatcher::create("BruteForce");		//创建特征匹配器    
	if (detector.empty() || descriptor_extractor.empty())
	{
		cout << "创建特征检测器失败！程序结束。\n";
		return -1;
	}

	// 读入图像    
	Mat img1 = imread("M2-1.jpg");
	Mat img2 = imread("M2-2.jpg");

	// 调整图像大小
	//resize(img1, img1, img1.size() / 2);
	//resize(img2, img2, img2.size() / 2);

	// 特征点检测
	double t = (double)getTickCount();	// 当前滴答数，用于统计算法用时   
	vector<KeyPoint> m_LeftKey, m_RightKey;
	detector->detect(img1, m_LeftKey);	// 检测img1中的SUFT特征点，存储到m_LeftKey中
	detector->detect(img2, m_RightKey);	// 检测img2中的SUFT特征点，存储到m_RightKey中
	cout << "图像1特征点个数:" << m_LeftKey.size() << endl;
	cout << "图像2特征点个数:" << m_RightKey.size() << endl;

	// 根据特征点计算特征描述子矩阵，即特征向量矩阵
	Mat descriptors1, descriptors2;
	descriptor_extractor->compute(img1, m_LeftKey, descriptors1);
	descriptor_extractor->compute(img2, m_RightKey, descriptors2);
	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "SUFT算法用时：" << t << "秒" << endl;

	cout << "图像1特征描述矩阵大小：" << descriptors1.size()
		<< "，特征向量个数：" << descriptors1.rows << "，维数：" << descriptors1.cols << endl;
	cout << "图像2特征描述矩阵大小：" << descriptors2.size()
		<< "，特征向量个数：" << descriptors2.rows << "，维数：" << descriptors2.cols << endl;

	// 画出特征点
	Mat img_m_LeftKey, img_m_RightKey;
	drawKeypoints(img1, m_LeftKey, img_m_LeftKey, Scalar::all(-1), 0);
	drawKeypoints(img2, m_RightKey, img_m_RightKey, Scalar::all(-1), 0);
	imshow("Img1", img_m_LeftKey);
	imshow("Img2", img_m_RightKey);

	// 特征匹配
	vector<DMatch> matches;		// 用于存储匹配结果
	descriptor_matcher->match(descriptors1, descriptors2, matches);	//匹配两个图像的特征矩阵    
	cout << "Match个数：" << matches.size() << endl;

	if (matches.size() <= 0)	// 如果没有匹配到特征点
	{
		cout << "没有匹配到特征点, 程序退出。\n";
		return -1;
	}

	// 画出最初的特征点匹配情况
	Mat img_first_matches;
	drawMatches(img1, m_LeftKey, img2, m_RightKey, matches, img_first_matches,
		Scalar::all(-1), CV_RGB(0, 255, 0), Mat(), 2);
	imshow("最初的特征点匹配情况", img_first_matches);
	waitKey(0);

	// 计算匹配结果中距离的最大和最小值
	// 距离是指两个特征向量间的欧式距离，表明两个特征的差异，值越小表明两个特征点越接近
	double max_dist = 0;
	double min_dist = 100;
	for (int i = 0; i<matches.size(); i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	cout << "最大距离：" << max_dist << endl;
	cout << "最小距离：" << min_dist << endl;

	// 通过距离筛选出较好的匹配点
	vector<DMatch> good_matches;
	for (int i = 0; i<matches.size(); i++)
	{
		if (matches[i].distance < DISTANCE_RATE_LIMIT * max_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}
	cout << "经过距离筛选后goodMatch个数：" << good_matches.size() << endl;

	if (good_matches.size() <= 0)
	{
		cout << "经过距离筛选后没有好的匹配点，程序退出。\n";
		return -1;
	}

	// 画出经过距离筛选的匹配结果
	Mat img_matches;
	drawMatches(img1, m_LeftKey, img2, m_RightKey, good_matches, img_matches,
		Scalar::all(-1), CV_RGB(0, 255, 0), Mat(), 2);
	imshow("经过距离筛选后的匹配情况", img_matches);
	waitKey(0);

	//RANSAC匹配过程  
	vector<DMatch> m_matches = good_matches;		// 采用距离筛选后的进行RANSAC
	//vector<DMatch> m_matches = matches;				// 采用最初的进行RANSAC
	// 分配空间  
	int ptCount = (int)m_matches.size();
	Mat p1(ptCount, 2, CV_32F);
	Mat p2(ptCount, 2, CV_32F);

	// 把Keypoint转换为Mat  
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

	// 用RANSAC方法计算F  
	Mat m_Fundamental;
	vector<uchar> m_RANSAC_status;       // 这个变量用于存储RANSAC后每个点的状态
	findFundamentalMat(p1, p2, m_RANSAC_status, FM_RANSAC);

	// 计算野点个数  
	int outlinerCount = 0;
	for (int i = 0; i<ptCount; i++)
	{
		if (m_RANSAC_status[i] == 0)    // 状态为0表示野点  
		{
			outlinerCount++;
		}
	}
	int inlinerCount = ptCount - outlinerCount;   // 总匹配点个数减去野点个数就是内点个数  
	cout << "内点数为：" << inlinerCount << endl;

	// 这三个变量用于保存内点和匹配关系
	vector<Point2f> m_LeftInlier;		// 保存左图的内点们的坐标
	vector<Point2f> m_RightInlier;		// 保存右图的内点们的坐标
	vector<DMatch> m_InlierMatches;		// 保存内点们的匹配关系

	// 将三者的大小设置为内点个数大小
	m_InlierMatches.resize(inlinerCount);
	m_LeftInlier.resize(inlinerCount);
	m_RightInlier.resize(inlinerCount);

	inlinerCount = 0;
	float inlier_minRx = img1.cols;		//用于存储内点中右图最小横坐标，以便后续融合  

	for (int i = 0; i<ptCount; i++)
	{
		if (m_RANSAC_status[i] != 0)	// 如果不是野点，即是内点
		{
			m_LeftInlier[inlinerCount].x = p1.at<float>(i, 0);
			m_LeftInlier[inlinerCount].y = p1.at<float>(i, 1);
			m_RightInlier[inlinerCount].x = p2.at<float>(i, 0);
			m_RightInlier[inlinerCount].y = p2.at<float>(i, 1);
			m_InlierMatches[inlinerCount].queryIdx = inlinerCount;
			m_InlierMatches[inlinerCount].trainIdx = inlinerCount;

			if (m_RightInlier[inlinerCount].x < inlier_minRx)	// 如果当前内点的右图横坐标比目前存的小
			{
				inlier_minRx = m_RightInlier[inlinerCount].x;   // 存储内点中右图最小横坐标
			}
			inlinerCount++;
		}
	}

	// 把内点转换为drawMatches可以使用的格式
	vector<KeyPoint> key1(inlinerCount);
	vector<KeyPoint> key2(inlinerCount);
	KeyPoint::convert(m_LeftInlier, key1);
	KeyPoint::convert(m_RightInlier, key2);

	// 显示计算F过后的内点匹配  
	Mat outImage;
	drawMatches(img1, key1, img2, key2, m_InlierMatches, outImage);
	imshow("RANSAC筛选后的匹配情况", outImage);
	waitKey(0);

	//矩阵H用以存储RANSAC得到的单应矩阵
	Mat H = findHomography(m_LeftInlier, m_RightInlier, RANSAC);
	if (H.empty())		// 如果H矩阵为空，即H矩阵不能被估计出来
	{
		cout << "无法算出H矩阵，程序结束。\n";
		return -1;
	}

	//存储左图四角，及其变换到右图位置
	vector<Point2f> obj_corners(4);
	obj_corners[0] = Point(0, 0); obj_corners[1] = Point(img1.cols, 0);
	obj_corners[2] = Point(img1.cols, img1.rows); obj_corners[3] = Point(0, img1.rows);
	vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);

	//画出变换后图像位置  
	Point2f offset((float)img1.cols, 0);
	line(outImage, scene_corners[0] + offset, scene_corners[1] + offset, Scalar(0, 255, 0), 4);
	line(outImage, scene_corners[1] + offset, scene_corners[2] + offset, Scalar(0, 255, 0), 4);
	line(outImage, scene_corners[2] + offset, scene_corners[3] + offset, Scalar(0, 255, 0), 4);
	line(outImage, scene_corners[3] + offset, scene_corners[0] + offset, Scalar(0, 255, 0), 4);
	imshow("变换后图像的位置", outImage);
	waitKey(0);

	int drift = scene_corners[1].x;		//储存偏移量

	//新建一个矩阵存储配准后四角的位置  
	int width = int(max(abs(scene_corners[1].x), abs(scene_corners[2].x)));
	int height = img1.rows;
	//或者：int height = int(max(abs(scene_corners[2].y), abs(scene_corners[3].y)));
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
	//可选：height-=int(origin_y);  
	Mat imageturn = Mat::zeros(width, height, img1.type());

	//获取新的变换矩阵，使图像完整显示  
	for (int i = 0; i<4; i++)
	{
		scene_corners[i].x -= origin_x;
	}
	//可选：scene_corners[i].y -= (float)origin_y; }  
	Mat H1 = getPerspectiveTransform(obj_corners, scene_corners);

	//进行图像变换，显示效果  
	warpPerspective(img1, imageturn, H1, Size(width, height));
	imshow("左图的变换结果", imageturn);
	waitKey(0);

	//图像融合  
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

	imshow("图像融合后的结果", img_result);
	waitKey(0);

	return 0;
}