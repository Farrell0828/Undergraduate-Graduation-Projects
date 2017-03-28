#ifndef SHOW3D_H_
#define SHOW3D_H_

#include <string>
#include <windows.h>
using namespace std;

// VTKͷ�ļ�
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtk3DSImporter.h>
#include <vtkOBJImporter.h>
#include <vtkSmartPointer.h>
#include <vtkRenderLargeImage.h>
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);

/*
@�������ܣ���ʾ3DS��ʽ��3Dģ��
@���� file_name����Ҫչʾ��3DSģ���ļ���·�����ļ�����������չ��.3ds��
*/
void Show3DS(const std::string &file_name);

/*
@�������ܣ���ʾOBJ��ʽ��3Dģ��
@���� file_name����Ҫչʾ��OBJģ���ļ���·�����ļ�������������չ��.obj,��Ӧ��.mtl�ļ����.obj�ļ�ͬ������ͬһĿ¼��
*/
void ShowOBJ(const std::string &file_name);

#endif // SHOW3D_H_