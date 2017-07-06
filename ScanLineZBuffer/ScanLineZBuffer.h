#ifndef __SCANLINEZBUFFER_H
#define __SCANLINEZBUFFER_H

#include <vector>
#include <list>
#include "Model.h"
#include "utils.h"
#include "Rasterizer.h"
#include "Shader.h"
#include <omp.h>
#include <algorithm>
#include <cassert>
#include <ctime>



struct Edge
{
	float x;//边的上端点的x坐标
	float dx;//相邻两条扫描线交点的x坐标差,-1/k
	int dy;//边跨越的扫描线数目
	int id;//边所属多边形的编号
};

//这里的活化边表，与课件中的不同：
//活化边的交点，不分左边和右边，部分xl和xr
struct ActiveEdge
{
	float x; // 交点的x坐标
	float dx; // (交点边上)两相邻扫描线交点的x坐标之差.-1/k
	int dy; // 以和交点所在边相交的扫描线数为初值，以后向下每处理一条扫描线减1
	float z; // 交点处多边形所在平面的深度值
	float dzx; // 沿扫描线向右走过一个像素时，多边形所在平面的深度增量。对于平面方程，dzx = -a/c(c!=0)
	float dzy; // 沿y方向向下移过一根扫描线时，多边形所在平面的深度增量。对于平面方程，dzy = b/c(c!=0)
	int id; // 交点对所在的多边形的编号
};

//这里的多边形边表元素，不记录多边形颜色，通过id查询shader.model中face的color得到
//包括了活化边表
struct Polygon
{
	float a, b, c, d;//多边形所在平面的方程系数，a^2+b^2+c^2=1，法线向量(a,b,c)
	int id;//多边形编号
	int dy;//多边形跨跃的扫描线数目
	//Color3f color;
	std::vector< ActiveEdge> activeEdgeTable;
};

class ScanLineZBuffer
{
public:
	ScanLineZBuffer();
	~ScanLineZBuffer();
	void setSize(int width, int height);
	void ScanLineZBuffer::getSize(int& width, int& height);
	void run(Model& model);
	int** idBuffer;//记录多边形的id，用于查询model.faces[i].color
	bool needUpdate;//窗口大小改变时，需要进行更新，未考虑旋转的时候更新




private:
	int width, height;
	float* zBuffer;

	std::vector<std::list<Polygon>> polygonTable;
	std::vector<std::list<Edge>> edgeTable;
	std::vector<Polygon> activePolygonTable;//包括了活化边表

	void release();//释放zBuffer和idBuffer
	void build(const Model& model);//构建多边形表、边表
	void addEdge(int y, Polygon* active_polygon);//将相关边加到活化边表中

};



#endif