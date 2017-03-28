#include "functions.h"

int Match(const vector<KeyPoint> &key_points1, const vector<KeyPoint> &key_points2,
	const Mat &descriptors1, const Mat &descriptors2)
{
	Ptr<DescriptorMatcher> descriptor_matcher =
		DescriptorMatcher::create("BruteForce");//��������ƥ����

												//����ƥ��
	vector<DMatch> matches;//ƥ����
	descriptor_matcher->match(descriptors1, descriptors2, matches);//ƥ������ͼ�����������

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

	//ͨ������ɸѡ���Ϻõ�ƥ���    
	vector<DMatch> good_matches;		// �洢ɸѡ��Ϻõ�ƥ���
	double total_distance = 0.0;
	for (int i = 0; i < matches.size(); i++)
	{	// С���������0.2������Ϊ�Ϻõ�ƥ���
		if (matches[i].distance < 0.2 * max_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}
	//return good_matches.size();		// ���ؽϺ�ƥ���ĸ���

	//RANSACƥ�����  
	vector<DMatch> m_matches = good_matches;
	// ����ռ�  
	int pt_count = (int)m_matches.size();
	Mat p1(pt_count, 2, CV_32F);
	Mat p2(pt_count, 2, CV_32F);

	// ��Keypointת��ΪMat  
	Point2f pt;
	for (int i = 0; i < pt_count; i++)
	{
		pt = key_points1[m_matches[i].queryIdx].pt;
		p1.at<float>(i, 0) = pt.x;
		p1.at<float>(i, 1) = pt.y;

		pt = key_points2[m_matches[i].trainIdx].pt;
		p2.at<float>(i, 0) = pt.x;
		p2.at<float>(i, 1) = pt.y;
	}

	// ��RANSAC��������F  
	Mat m_fundamental;
	vector<uchar> m_RANSAC_status;       // ����������ڴ洢RANSAC��ÿ�����״̬  
	findFundamentalMat(p1, p2, m_RANSAC_status, FM_RANSAC);

	// ����Ұ�����  
	int outliner_count = 0;
	for (int i = 0; i < m_RANSAC_status.size(); i++)
	{
		if (m_RANSAC_status[i] == 0)    // ״̬Ϊ0��ʾҰ��  
		{
			outliner_count++;
		}
	}

	int inliner_count = pt_count - outliner_count;   // �����ڵ�

	return inliner_count;
}

void LoadXML(const String &xml_file_name, vector<vector<KeyPoint>> &key_points,
	vector<Mat> &descriptors)
{
	FileStorage fs(xml_file_name, FileStorage::READ);
	int num_of_pictures;			// ģ��ͼƬ������
	vector<KeyPoint> temp_k;		// �洢һ��ͼƬ��������vector����ʱ����
	Mat temp_d;						// �洢һ��ͼƬ��������������ʱ����
	String img_no;					// ͼƬ���
	String img_key_points_name;		// ͼƬ������vector����
	String img_descriptor_name;		// ͼƬ������������

	fs["NumOfPictures"] >> num_of_pictures;
	for (int i = 0; i < num_of_pictures; i++)
	{
		stringstream ss;
		ss << i;
		img_no = ss.str();

		img_key_points_name = "KeyPoints" + img_no;
		fs[img_key_points_name] >> temp_k;
		key_points.push_back(temp_k);

		img_descriptor_name = "Descriptor" + img_no;
		fs[img_descriptor_name] >> temp_d;
		descriptors.push_back(temp_d);
	}
}

void ShowResult(int no)
{
	if (no != -1)
	{
		stringstream ss;
		ss << no;
		String img_no = ss.str();
		String img_path = "ProcessedImage-600px\\" + img_no + ".jpg";
		Mat img = imread(img_path);
		imshow("Result", img);
	}

	switch (no)
	{
	case 0: cout << "ͼ3-1 S50MC-C�Ͳ��ͻ�"; break;
	case 1: cout << "ͼ3-8 ʮ��ͷʽ����"; break;
	case 2: cout << "ͼ3-9 ���������ȴ����"; break;
	case 3: cout << "ͼ3-10 Ͳ�λ���"; ShowVideo("Diesel Engine, How it  works .mp4"); break;
	case 4: cout << "ͼ3-18 ���������Ϻ�"; break;
	case 5: cout << "ͼ3-22 Ͳ�β��ͻ�������"; break;
	case 6: cout << "ͼ3-25 ѹ�����칤��"; break;
	case 7: cout << "ͼ3-26 WartsilaL32�Ͳ��ͻ����׸�"; break;
	case 8: cout << "ͼ3-32 ʮ��ͷ�Ĺ���"; break;
	case 9: cout << "ͼ3-36 Wartsila38�Ͳ��ͻ�����"; break;
	case 10: cout << "ͼ3-38 ΪWartsila46�Ͳ��ͻ�����"; break;
	case 11: cout << "ͼ3-39 S-MC-C���ͻ�����"; break;
	case 12: cout << "ͼ3-46 S-MC-C�Ͳ��ͻ�����"; break;
	case 13: cout << "ͼ3-50 Wartsila32�����ٲ��ͻ�����"; break;
	case 14: cout << "ͼ3-53 Wartsila���ͻ��ĵ���ʽ�����"; break;
	case 15: cout << "ͼ3-57 360���������"; break;
	case 16: cout << "ͼ3-58 ����������"; break;
	case 17: cout << "ͼ4-6 ���Ϳ�ʽ���ͱû����ṹͼ"; break;
	case 18: cout << "ͼ4-10 ���ͷ�ż��"; break;
	case 19: cout << "ͼ4-11 Wartsila32�Ͳ��ͻ����ͱ�"; break;
	case 20: cout << "ͼ4-12 MAN B&W S-MC-C�Ͳ��ͻ����ͱ�"; break;
	case 21: cout << "ͼ4-18 Wartsila38�Ͳ��ͻ�������"; break;
	case 22: cout << "ͼ6-3 MC�Ͳ��ͻ���������"; break;
	case 23: cout << "ͼ6-5 ����������������"; break;
	case 24: cout << "ͼ6-17 TCA��ѹ��������"; break;
	case 25: cout << "ͼ6-21 ��ѹ��ת��"; break;
	case 26: cout << "ͼ6-24 VTRϵ����ѹ����Ч�ʼ�ѹ�ȷ�Χ"; break;
	case 27: cout << "ͼ6-25 TPLϵ����ѹ��"; break;
	case 28: cout << "ͼ6-26 NASϵ����ѹ���Ľṹͼ"; break;
	case 29: cout << "ͼ7-9 �����ػ�ȫ�������ͷ��ͻ��ṹ"; break;
	case 30: cout << "ͼ7-10 ALFA-LAVAL Sϵ���ޱ��ػ��ͷ��ͻ��ṹ"; break;
	case 31: cout << "ͼ8-46 ��˹�ָ�������Ƭ������"; break;
	case 32: cout << "ͼ10-9 UG8����ʽ�������ṹԭ��ͼ"; break;
	case 33: cout << "ͼ10-10 PGA�������ṹԭ��ͼ"; break;
	case 34: cout << "ͼ11-4 ���͵�Һ���Ʒ�"; break;
	case 35: cout << "ͼ11-29 EG3PҺѹִ����"; break;
	case 36: cout << "ͼ12-3 ����·����ʽ�����𶯷�(S50MC-C)"; break;
	case 37: cout << "ͼ12-4 ˫��·����ʽ�����𶯷�"; break;
	case 38: cout << "ͼ12-6 MAN B&W S50MC-C���ͻ���תʽ����������"; break;
	case 39: cout << "ͼ12-7 RT-flex���ͻ��Ǿ���ʽ���𶯷�"; break;
	case 40: cout << "ͼ13-11 ME-GI���ͻ���ȼ���ҽṹ"; break;
	default: cout << "ƥ��ʧ�ܣ�"; break;
	}
	cout << endl;
}

void ShowVideo(String file_name)
{
	VideoCapture vid;
	vid.open(file_name);		// ����Ƶ�ļ�
	if (!vid.isOpened())		// �����ʧ��
	{
		cout << "����Ƶ��" << file_name << "��" << "ʧ�ܣ�" << endl;
		return;
	}

	long total_frame_num = vid.get(CV_CAP_PROP_FRAME_COUNT);// ��ȡ��Ƶ������֡��
	double rate = vid.get(CV_CAP_PROP_FPS);		// ��ȡ֡��
	bool need_stop = false;		// ��־�Ƿ���Ҫ��ǰ������Ƶ����
	Mat frame;					// ����ÿһ֡��ͼ��
	namedWindow(file_name);		// ��ʾ��Ƶ�Ĵ���
	int delay = 1000 / rate;	// ��֡��ļ��ʱ��
	long current_frame = 0;		// ��ǰ��Ҫ����֡�ı�� 

	while (!need_stop)			// ����whileѭ����ȡ֡ 
	{
		if (!vid.read(frame))	// �����ȡ֡ʧ��
		{
			cout << "��ȡ��Ƶʧ��" << endl;
			return;
		}
		imshow(file_name, frame);
		int c = waitKey(delay);

		// ����ESC���ߵ���ָ���Ľ���֡���˳���ȡ��Ƶ  
		if (c == 27 || current_frame > total_frame_num)
		{
			need_stop = true;
		}
		// ���¿ո�����ͣ���ڵ�ǰ֡���ȴ���һ�ΰ���  
		else if ((char)c == ' ')
		{
			waitKey(0);
		}
		current_frame++;
	}
	//�ر���Ƶ�ļ�  
	destroyWindow(file_name);
	vid.release();
}

void Show3DS(const string &file_name)
{
	vtkSmartPointer<vtkRenderWindowInteractor> iren =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();	// �������ڽ�����
	vtkSmartPointer<vtkRenderWindow> renWin =
		vtkSmartPointer<vtkRenderWindow>::New();			// ������Ⱦ����
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();				// ������Ⱦ��

	iren->SetRenderWindow(renWin);		// ���ý��������ƵĴ���ΪrenWin
	renWin->AddRenderer(renderer);		// ����Ⱦ��renderer��ӵ�����renWin����Ⱦ���б���

	renderer->SetBackground(0.1, 0.2, 0.4);			// ���ñ���ɫ
	renWin->SetSize(600, 600);			// ���ô��ڵ�Ĭ�ϴ�С

	vtkSmartPointer<vtk3DSImporter> importer =
		vtkSmartPointer<vtk3DSImporter>::New();		// ����3DS����������
	importer->SetFileName(file_name.c_str());		// ָ��Ҫ��ȡ��3DS�ļ����ļ�����·��
	importer->ComputeNormalsOn();		// ��������������
	importer->Read();					// ����3DSģ��

	renderer->GetActiveCamera()->SetPosition(100, 200, 0);	// ���������λ��(��������ϵ��)
	renderer->GetActiveCamera()->SetFocalPoint(100, 0, 50);	// ��������Ľ���(��������ϵ��)
	renderer->GetActiveCamera()->SetViewUp(0, 0, 1);		// ��������Ĺ۲���ͼ

															// ��ȡ���ص�3DSģ�����������ĸ���
	int numOfActors = importer->GetRenderer()->GetActors()->GetNumberOfItems();

	// ��3dsͼ�������ζ�ȡ����
	for (int i = 0; i < numOfActors; i++)
	{
		vtkActor *actor = (vtkActor *)importer->GetRenderer()->GetActors()->GetItemAsObject(i);
		renderer->AddActor(actor);		// �������ȡ��������ӵ���Ⱦ����
	}

	renWin->Render();			// ����Ⱦ����renWin�е�������Ⱦ����ʼ��Ⱦ
	iren->Start();				// ��ʼ�¼�ѭ��
}

void ShowOBJ(const string &file_name)
{
	vtkSmartPointer<vtkRenderWindowInteractor> iren =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();	// �������ڽ�����
	vtkSmartPointer<vtkRenderWindow> renWin =
		vtkSmartPointer<vtkRenderWindow>::New();			// ������Ⱦ����
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();				// ������Ⱦ��

	iren->SetRenderWindow(renWin);		// ���ý��������ƵĴ���ΪrenWin
	renWin->AddRenderer(renderer);		// ����Ⱦ��renderer��ӵ�����renWin����Ⱦ���б���

	renderer->SetBackground(0.1, 0.2, 0.4);			// ���ñ���ɫ
	renWin->SetSize(600, 600);			// ���ô��ڵ�Ĭ�ϴ�С
	renWin->SetWindowName(file_name.c_str());

	vtkSmartPointer<vtkOBJImporter> importer =
		vtkSmartPointer<vtkOBJImporter>::New();		// ����OBJ����������
	importer->SetFileName(std::string(file_name + ".obj").c_str());		// ָ��Ҫ��ȡ��OBJ�ļ�
	importer->SetFileNameMTL(std::string(file_name + ".mtl").c_str());	// ָ����Ӧ��MTL�ļ�
	importer->Read();					// ����OBJģ��

	renderer->GetActiveCamera()->SetPosition(100, 200, 0);	// ���������λ��(��������ϵ��)
	renderer->GetActiveCamera()->SetFocalPoint(100, 0, 50);	// ��������Ľ���(��������ϵ��)
	renderer->GetActiveCamera()->SetViewUp(0, 0, 1);		// ��������Ĺ۲���ͼ

															// ��ȡ���ص�3DSģ�����������ĸ���
	int numOfActors = importer->GetRenderer()->GetActors()->GetNumberOfItems();

	// ��OBJͼ�������ζ�ȡ����
	for (int i = 0; i < numOfActors; i++)
	{
		vtkActor *actor = (vtkActor *)importer->GetRenderer()->GetActors()->GetItemAsObject(i);
		renderer->AddActor(actor);		// �������ȡ��������ӵ���Ⱦ����
	}

	renWin->Render();			// ����Ⱦ����renWin�е�������Ⱦ����ʼ��Ⱦ
	iren->Start();				// ��ʼ�¼�ѭ��
}
