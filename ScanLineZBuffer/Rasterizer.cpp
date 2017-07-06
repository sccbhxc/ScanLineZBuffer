#include "Rasterizer.h"
#include <algorithm>
#include <omp.h>

//#define NUM_THREADS 8

using namespace std;

Rasterizer::Rasterizer()
{
	/*width = 800;
	height = 600;*/

}

void Rasterizer::setSize(int width, int height)
{
	this->width = width;
	this->height = height;
}

void Rasterizer::getSize(int& width, int& height)
{
	width = this->width;
	height = this->height;
}

/********************************************************************************************/
/*  ？？？？疑问：无法通过修改rasterize()中的成员变量model使得rasterize()外的model改变        */
/*                  改为函数引用rasterize(Model& model)                                      */
/********************************************************************************************/
//光栅化到指定窗口大小（width，height）
void Rasterizer::run(Model& model)
{
	Point3f min_xyz(0xfffffff, 0xfffffff, 0xfffffff),
		max_xyz(-0xfffffff, -0xfffffff, -0xfffffff);//7个f
	Point3f center_xyz(0.0, 0.0, 0.0);
	int vertex_num = model.vertexes.size();
	for (int i = 0; i < vertex_num; ++i)
	{
		const Point3f& vertex = model.vertexes[i].point;
		min_xyz.x = min(min_xyz.x, vertex.x);
		min_xyz.y = min(min_xyz.y, vertex.y);
		min_xyz.z = min(min_xyz.z, vertex.z);
		max_xyz.x = max(max_xyz.x, vertex.x);
		max_xyz.y = max(max_xyz.y, vertex.y);
		max_xyz.z = max(max_xyz.z, vertex.z);
		//center_xyz += vertex;
	}
	//center_xyz /= vertex_num;
	center_xyz = (min_xyz + max_xyz) / 2;
	//Vec3f range(max_xyz.x - min_xyz.x, max_xyz.y - min_xyz.y, 0);
	//float max_model_len = length(range);
	float model_width = max_xyz.x - min_xyz.x;
	float model_height = max_xyz.y - min_xyz.y;
	float max_model_len = max(model_width, model_height);
	//将所有顶点放置到大小为(width,height)的窗口中

	//取小比例尺，防止模型长（max(width,height)）的部分没在窗口中
	float scale = min(width, height) / max_model_len;
	scale = 0.8*scale;
	//scale = 0.4*scale;



#pragma omp parallel for //num_threads(NUM_THREADS)
	for (int i = 0; i < vertex_num; ++i)
	{
		Point3f& vertex_point = model.vertexes[i].point;
		//放置到窗口中间
		vertex_point.x = (vertex_point.x - center_xyz.x)*scale + width / 2;
		vertex_point.y = (vertex_point.y - center_xyz.y)*scale + height / 2;
		vertex_point.z = (vertex_point.z - center_xyz.z)*scale;

	}
	model.center_point = Point3f(width / 2, height / 2, 0);
}
