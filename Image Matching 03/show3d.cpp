#include "show3d.h"

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