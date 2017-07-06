#include "Shader.h"
#include <omp.h>

using namespace std;

Shader::Shader()
{
	//this->model = model;
	//light_position = Point3f(0.0f, 500.0f, 0.0f);
	//light_color = Color3f(0.6, 0.6, 0.6);
	light_color = Color3f(0.3, 0.3, 0.3);
	ambient_color = Color3f(0.3, 0.3, 0.3);

	//light_position = Point3f(400.0f, 600.0f, 0.0f);
	light_position = Point3f(400.0f, 600.0f, 500.0f);
	//根据光源位置计算模型各面元的颜色，放到子线程中进行
	//render(model);
}


//简单的点光源照射下的绘制
void Shader::run(Model& model)
{
	int face_num = model.faces.size();

	//此处尝试openmp
#pragma omp parallel for
	for (int i = 0; i < face_num; ++i)
	{
		Face& face = model.faces[i];
		int face_vertex_num = face.vertexIdx.size();
		for (int j = 0; j < face_vertex_num; ++j)
		{
			Vertex face_vertex = model.vertexes[face.vertexIdx[j]];
			Vec3f ray_direction = normalize(light_position - face_vertex.point);//光线入射反方向
			Vec3f normal = face.normalIdx[j] >= 0 ?
				model.normals[face.normalIdx[j]]:face.normal;
			float cosine = dot(ray_direction, normal);//求光线入射反方向与面法线/顶点法线的夹角cos
			if (cosine>0.0)face.color += kd*cosine*light_color;//点光源散射颜色
			face.color += ambient_color;//增加环境颜色
		}
		face.color /= face.vertexIdx.size();///多边形小块的颜色取顶点的平均颜色

		//控制颜色取值范围在0.0～1.0之间
		if (face.color.r > 1.0f)face.color.r = 1.0f;
		if (face.color.r < 0.0f)face.color.r = 0.0f;
		if (face.color.g > 1.0f)face.color.g = 1.0f;
		if (face.color.g < 0.0f)face.color.g = 0.0f;
		if (face.color.b > 1.0f)face.color.b = 1.0f;
		if (face.color.b < 0.0f)face.color.b = 0.0f;
	}
	
	
}






