#ifndef SHOW3D_H_
#define SHOW3D_H_

#include <string>
#include <windows.h>
using namespace std;

// VTK头文件
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
@函数功能：显示3DS格式的3D模型
@参数 file_name：需要展示的3DS模型文件的路径或文件名（包含扩展名.3ds）
*/
void Show3DS(const std::string &file_name);

/*
@函数功能：显示OBJ格式的3D模型
@参数 file_name：需要展示的OBJ模型文件的路径或文件名（不包含扩展名.obj,相应的.mtl文件需和.obj文件同名且在同一目录）
*/
void ShowOBJ(const std::string &file_name);

#endif // SHOW3D_H_