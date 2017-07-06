#ifndef __RASTERISATION_H
#define __RASTERISATION_H

#include "Model.h"

class Rasterizer
{
public:
	//显示窗口的大小
	int width;
	int height;

	Rasterizer();
	//Rasterisation(Model& model);
	void setSize(int width, int height);//设置窗口大小
	void getSize(int& width, int& height);

	void run(Model& model);//引用model，使得rasterize()外的model改变
};


#endif