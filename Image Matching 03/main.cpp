#include <iostream>
using namespace std;

#include "functions.h"

int main()
{
	cv::String xml_file_name = "TempletInformation-600px.xml";	// XML文件名
	vector< vector<cv::KeyPoint> > key_points_arr;				// 所有模板图片的特征点vector
	vector<cv::Mat> descriptor_arr;								// 所有图片的特征向量vector
	double t = (double)cv::getTickCount();
	if (!LoadXML(xml_file_name, key_points_arr, descriptor_arr))// 加载XML文件失败
	{
		cout << "无法加载XML文件" << xml_file_name << endl;	
		return -1;
	}
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	cout << "加载XML文档用时: " << t << "s" << endl;

	cv::VideoCapture camera;
	cv::Mat frame, img;
	camera.open(1);			// 尝试打开摄像头
	if (!camera.isOpened())	// 如果打开失败
	{
		cout << "打开摄像头失败!\n";
		return -1;
	}
	cout << "已成功打开摄像头!\n";
	cv::namedWindow("Camera");

	while (true)
	{
		while (true)
		{
			camera >> frame;				// 将摄像头的图像帧流至frame
			cv::imshow("Camera", frame);	// 在Camera窗口展示图像帧
			int k = cv::waitKey(1);			// 等待按下按键
			if ((char)k == ' ')				// 如果按下的按键时空格键
			{
				img = frame;				// 将当前帧数据赋值给img
				cv::destroyWindow("Camera");// 关闭Camera窗口
				break;
			}
			else if (k == 27)				// 如果按下的按键是Esc键
			{
				return 0;
			}
		}
		if (img.empty())		// 如果获取帧数据失败
		{
			cout << "获取视频帧失败！\n";
			continue;
		}

		double t_begin = (double)cv::getTickCount();	// 用于统计整个匹配过程的总用时

		cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(800);// 创建SURF特征检测器
		vector<cv::KeyPoint> key_points;				// 存储待匹配图片的特征点
		detector->detect(img, key_points);				// 对待匹配图片img进行特征检测并将结果存至key_points
		if (key_points.size() == 0)
		{
			cout << "没有在图片中检测到SURF特征点！\n";
			continue;
		}
		cv::Mat descriptor;		// 存储待匹配图片的特征向量
		detector->compute(img, key_points, descriptor);	// 计算特征向量并将结果存至descriptor

		int max_index = -1;		// 具有最多较好匹配点的模板图片的下标（模板下标从0开始）
		int max_num = 0;		// 最多较好匹配点的个数
		int num_of_good_matchs;	// 与其中一张图匹配时较好匹配点的个数

		cout << "   编号   匹配特征点个数     匹配用时" << endl;
		for (int i = 0; i < descriptor_arr.size(); i++)
		{
			double t = (double)cv::getTickCount();		// 用于计算其中一个模板的匹配用时
													// 拿其中一个模板与待匹配图进行匹配，并将匹配的较好匹配点个数赋值给num_of_good_matchs
			num_of_good_matchs = Match(key_points_arr[i], key_points, descriptor_arr[i], descriptor);
			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();	// 算出这次匹配的用时（单位s）
			cout << "   " << setw(2) << i << "            " << num_of_good_matchs
				<< "          " << t << "s" << endl;

			if (num_of_good_matchs > max_num)		// 如果此个模板的匹配点更多
			{
				max_num = num_of_good_matchs;		// 将此次的匹配点个数赋值给max_num
				max_index = i;	// 记录下下标
			}
		}

		// 算出整个过程的匹配用时（单位s）
		t_begin = ((double)cv::getTickCount() - t_begin) / cv::getTickFrequency();
		cout << "匹配用时: " << t_begin << "s" << endl;

		// 如果最好的匹配图的较好匹配点个数还不到3个，则认为没有匹配到
		if (max_num < 3)
		{
			cout << "\n没有匹配到！\n";
			continue;
		}

		cout << "\n匹配图编号: " << max_index << endl;
		ShowResult(max_index);
		cv::waitKey();
		cv::destroyAllWindows();
	}

	return 0;
}