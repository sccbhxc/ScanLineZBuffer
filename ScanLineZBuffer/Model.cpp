#include "Model.h"
#include <fstream>
#include <iostream>
#include <string>
#include <ctime>

using namespace std;

bool Model::load(const string& path)
{
	clock_t t = clock();
	ifstream file(path);
	if (!file.is_open()) return false;
	string type;


	//加载顶点坐标
	while (file >> type)
	{
		if (type == "v")
		{
			Vertex vt;
			file >> vt.point.x >> vt.point.y >> vt.point.z;
			vertexes.push_back(vt);
		}
		//加载面-顶点索引/纹理索引/法线索引
		else if (type == "f")
		{
			Face face;
			int vIndex, tIndex, nIndex;
			int faceIndex = faces.size();

			while (true)
			{
				char ch = file.get();
				if (ch == ' ') continue;
				else if (ch == '\n' || ch == EOF) break;
				else file.putback(ch);

				file >> vIndex;//顶点的索引

				char splitter = file.get();
				nIndex = 0;

				if (splitter == '/')
				{
					splitter = file.get();
					if (splitter == '/')
					{
						file >> nIndex;//面法向量的index
					}
					else
					{
						file.putback(splitter);
						file >> tIndex;//纹理(texture)索引
						splitter = file.get();
						if (splitter == '/')
						{
							file >> nIndex;
						}
						else file.putback(splitter);
					}
				}
				else file.putback(splitter);

				face.vertexIdx.push_back(vIndex - 1);
				face.normalIdx.push_back(nIndex - 1);
			}
			//计算面元中3点确定的法向量，在obj文件不提供面法向量时使用
			if (face.vertexIdx.size() > 2)
			{
				Point3f &a = vertexes[face.vertexIdx[0]].point,
					&b = vertexes[face.vertexIdx[1]].point, &c = vertexes[face.vertexIdx[2]].point;
				Vec3f& normal = normalize(cross(b - a, c - b));//求法线，并单位化

				face.normal = normal;
				faces.push_back(face);
			}
		}
		//加载obj文件提供的法向量，如果有的话
		else if (type == "vn")
		{
			Vec3f vn;
			file >> vn.x >> vn.y >> vn.z;
			normals.push_back(vn);
		}
	}
	file.close();
	cout << "模型加载耗时：" << float(clock() - t) << "ms。" << endl;
	return true;
}

Model::Model(const string& path)
{
	//center_point = Point3f(0.0, 0.0, 0.0);
	if (load(path))
	{
		string face_type;
		cout << "模型"+path+"加载成功！" << endl;
		cout << "面元数：" << faces.size() << 
			"，顶点数："<< vertexes.size() << endl;
	}
	else
	{
		cout << "无法打开obj文件：" + path << endl;
	}
}

void Model::rotate(float** RotateMat, vector<Vertex>& vertexes)
{
	int vertex_num = vertexes.size();
#pragma omp parallel for
	for (int i = 0; i < vertex_num; ++i)
	{
		//Point3f* point = &vertexes[i].point;
		Point3f point;
		Point3f tmp_point = vertexes[i].point - center_point;
		vertexes[i].point.x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
		vertexes[i].point.y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
		vertexes[i].point.z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;
		vertexes[i].point += center_point;
	}

	int normal_num = normals.size();
#pragma omp parallel for
	for (int i = 0; i < normal_num; ++i)
	{
		Point3f tmp_point = normals[i];
		normals[i].x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
		normals[i].y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
		normals[i].z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;

	}

	int face_num = faces.size();
#pragma omp parallel for
	for (int i = 0; i < face_num; ++i)
	{
		Point3f tmp_point = faces[i].normal;
		faces[i].normal.x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
		faces[i].normal.y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
		faces[i].normal.z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;
	}
}