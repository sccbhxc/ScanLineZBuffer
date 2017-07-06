#ifndef __MODEL_H
#define __MODEL_H

#include <iostream>
#include <vector>
#include <string>
#include "Vec.h"

class Vertex
{
public:
	Point3f point;
	Color3f color;
	Vec3f normal;//顶点法向量
};

class Face
{
public:
	std::vector<int> vertexIdx;
	std::vector<int> normalIdx;//obj文件提供的面法向量（顶点法向量）的索引
	Vec3f normal;//由3点确定的面法向量
	Color3f color;//为面元上各顶点的平均颜色
};

//用于加载obj文件的模型
class Model
{
public:
	std::vector<Vertex> vertexes;
	std::vector<Face> faces;
	std::vector<Vec3f> normals;//顶点法向量
	Point3f center_point;
	
	bool load(const std::string& path);
	Model(){};
	Model(const std::string& path);
	void rotate(float** RotateMat, std::vector<Vertex>& vertexes);
	
	//void transform(float theta);
};

#endif