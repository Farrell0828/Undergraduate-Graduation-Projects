#ifndef FUNCTIONS_H_

#include <vector>
#include <string>
#include <windows.h>
using namespace std;

// OpenCVͷ�ļ�
#include <opencv2\opencv.hpp>
#include <opencv2\xfeatures2d.hpp>
using namespace cv;
using namespace xfeatures2d;

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
@�������ܣ�������ͼƬ�����������ƥ�䣬�����ؾ��������RANSACɸѡ��Ϻõ�������ĸ���
@���� key_points1��ͼƬ1��������vector
@���� key_points2��ͼƬ2��������vector
@���� descriptors1��ͼƬ1����������
@���� descriptors2��ͼƬ2����������
*/
int Match(const vector<KeyPoint> &key_points1, const vector<KeyPoint> &key_points2,
	const Mat &descriptors1, const Mat &descriptors2);

/*
@�������ܣ����ش洢��ͼƬ���е������������������XML�ļ����ڴ�
@���� xml_file_name����ĿĿ¼�ڵ�XML�ļ����ļ�����XML�ļ�������·��
@���� key_points���ڴ��д洢������vector�ǵ�vector
@���� descriptors���ڴ��д洢����������vector
*/
void LoadXML(const String &xml_file_name, vector< vector<KeyPoint> > &key_points,
	vector<Mat> &descriptors);

/*
@�������ܣ���ʾ��Ӧ��ŵı���
@���� no����Ҫչʾ����ı��
*/
void ShowResult(int no);

/*
@�������ܣ�����һ��������ʾ��Ƶ
@���� file_name����Ҫչʾ����Ƶ��·�����ļ���
*/
void ShowVideo(String file_name);

/*
@�������ܣ���ʾ3DS��ʽ��3Dģ��
@���� file_name����Ҫչʾ��3DSģ���ļ���·�����ļ�����������չ��.3ds��
*/
void Show3DS(const string &file_name);

/*
@�������ܣ���ʾOBJ��ʽ��3Dģ��
@���� file_name����Ҫչʾ��OBJģ���ļ���·�����ļ�������������չ��.obj,��Ӧ��.mtl�ļ����.obj�ļ�ͬ������ͬһĿ¼��
*/
void ShowOBJ(const string &file_name);

#endif // !FUNCTIONS_H_
