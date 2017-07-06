#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <glut.h>
#include "Model.h"
#include "ScanLineZBuffer.h"
#include <iostream>
#include <omp.h>
#include <iostream>
#include <cmath>
#include "utils.h"

class ScanLineZBuffer;

class Display
{
public:
	
	Display(Model* model, ScanLineZBuffer* slzBuffer);
	~Display();
	//void setSize(int w, int h);
	void run();

	static Model* model;
	//int width, height;
	static ScanLineZBuffer* engine;
	static Point3f point_1st, point_2rd;
	static float** RotateMat;
	static int idx1, idx2;

	//通过拖动,获取旋转矩阵，并旋转模型
	static void loop();
	static void reshape(int w, int h);
	static void drag_mouse(int button, int state, int x, int y);
	static void keyboard(int key, int x, int y);
	static void screen_menu(int value);

};


#endif