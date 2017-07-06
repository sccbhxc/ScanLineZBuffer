#ifndef __SHADER_H
#define __SHADER_H

#include "Vec.h"
#include "Model.h"

const float kd = 0.8;//albedo ,散射反照率

class Shader
{
public:
	//Model model;
	Point3f light_position;//点光源位置
	Color3f light_color;
	Color3f ambient_color;

	Shader();
	//Shader(Model model);

	void run(Model& model);

};


#endif