#include <iostream>
using namespace std;

#include "functions.h"

int main()
{
	// ShowVideo("Diesel Engine, How it  works .mp4");
	String xml_file_name = "TempletInformation-600px.xml";	// XML�ļ���
	vector< vector<KeyPoint> > key_points_arr;				// ����ģ��ͼƬ��������vector
	vector<Mat> descriptor_arr;								// ����ͼƬ����������vector
	double t = (double)getTickCount();
	LoadXML(xml_file_name, key_points_arr, descriptor_arr);	// ����XML�ļ�
	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "����XML�ĵ���ʱ: " << t << "s" << endl;

	VideoCapture camera;
	Mat frame, img;
	camera.open(1);			// ���Դ�����ͷ
	if (!camera.isOpened())	// �����ʧ��
	{
		cout << "������ͷʧ��!\n";
		return -1;
	}
	cout << "�ѳɹ�������ͷ!\n";
	namedWindow("Camera");

	while (true)
	{
		while (true)
		{
			camera >> frame;			// ������ͷ��ͼ��֡����frame
			imshow("Camera", frame);	// ��Camera����չʾͼ��֡
			int k = waitKey(1);			// �ȴ����°���
			if ((char)k == ' ')			// ������µİ���ʱ�ո��
			{
				img = frame;			// ����ǰ֡���ݸ�ֵ��img
				destroyWindow("Camera");// �ر�Camera����
				break;
			}
			else if (k == 27)			// ������µİ�����Esc��
			{
				return 0;
			}
		}
		if (img.empty())		// �����ȡ֡����ʧ��
		{
			cout << "��ȡ��Ƶ֡ʧ�ܣ�\n";
			continue;
		}

		double t_begin = (double)getTickCount();	// ����ͳ������ƥ����̵�����ʱ

		Ptr<SURF> detector = SURF::create(800);		// ����SURF���������
		vector<KeyPoint> key_points;				// �洢��ƥ��ͼƬ��������
		detector->detect(img, key_points);			// �Դ�ƥ��ͼƬimg����������Ⲣ���������key_points
		if (key_points.size() == 0)
		{
			cout << "û����ͼƬ�м�⵽SURF�����㣡\n";
			continue;
		}
		Mat descriptor;			// �洢��ƥ��ͼƬ����������
		detector->compute(img, key_points, descriptor);	// �����������������������descriptor

		int max_index = -1;		// �������Ϻ�ƥ����ģ��ͼƬ���±꣨ģ���±��0��ʼ��
		int max_num = 0;		// ���Ϻ�ƥ���ĸ���
		int num_of_good_matchs;	// ������һ��ͼƥ��ʱ�Ϻ�ƥ���ĸ���

		cout << "   ���   ƥ�����������     ƥ����ʱ" << endl;
		for (int i = 0; i < descriptor_arr.size(); i++)
		{
			double t = (double)getTickCount();		// ���ڼ�������һ��ģ���ƥ����ʱ
													// ������һ��ģ�����ƥ��ͼ����ƥ�䣬����ƥ��ĽϺ�ƥ��������ֵ��num_of_good_matchs
			num_of_good_matchs = Match(key_points_arr[i], key_points, descriptor_arr[i], descriptor);
			t = ((double)getTickCount() - t) / getTickFrequency();	// ������ƥ�����ʱ����λs��
			cout << "   " << setw(2) << i << "            " << num_of_good_matchs
				<< "          " << t << "s" << endl;

			if (num_of_good_matchs > max_num)		// ����˸�ģ���ƥ������
			{
				max_num = num_of_good_matchs;		// ���˴ε�ƥ��������ֵ��max_num
				max_index = i;	// ��¼���±�
			}
		}

		// ����������̵�ƥ����ʱ����λs��
		t_begin = ((double)getTickCount() - t_begin) / getTickFrequency();
		cout << "ƥ����ʱ: " << t_begin << "s" << endl;

		// �����õ�ƥ��ͼ�ĽϺ�ƥ������������3��������Ϊû��ƥ�䵽
		if (max_num < 3)
		{
			cout << "\nû��ƥ�䵽��\n";
			continue;
		}

		cout << "\nƥ��ͼ���: " << max_index << endl;
		ShowResult(max_index);
		waitKey();
		destroyAllWindows();
	}

	return 0;
}