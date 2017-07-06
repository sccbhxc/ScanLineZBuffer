#include <iostream>
#include "Vec.h"
#include "utils.h"
#include "Model.h"
#include "Shader.h"
#include "ScanLineZBuffer.h"
#include "Display.h"
#include <ctime>


using namespace std;

int main()
{
	
	Model model("../models/al.obj");
	//窗口大小
	int width = 800, height = 600;
	//int width = 400, height = 300;

	//扫描线zbuffer计算得到需要进行绘制的屏幕坐标对应的面元id
	ScanLineZBuffer slzBuffer;
	slzBuffer.setSize(width, height);
	slzBuffer.run(model);

	Display display(&model, &slzBuffer);
	display.run();
	
	//system("pause");
	return 0;
}