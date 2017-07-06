#include "ScanLineZBuffer.h"

//#define NUM_THREADS 8

using namespace std;

ScanLineZBuffer::ScanLineZBuffer()
{
	//setSize(800, 600);
}

ScanLineZBuffer::~ScanLineZBuffer()
{
	release();
}

void ScanLineZBuffer::setSize(int width, int height)
{
	if (width == this->width&&height == this->height) return;

	release();

	this->width = width;
	this->height = height;
	needUpdate = true;
	zBuffer = new float[width];

	idBuffer = new int*[height];
	for (int i = 0; i < height; ++i)
	{
		idBuffer[i] = new int[width];
	}
}

void ScanLineZBuffer::getSize(int& width, int& height)
{
	width = this->width;
	height = this->height;
}

void ScanLineZBuffer::release()
{
	if (zBuffer != NULL)
	{
		delete[] zBuffer;
		zBuffer = NULL;
	}

	if (idBuffer != NULL)
	{
		for (int i = 0; i < height; ++i)
		{
			delete[] idBuffer[i];
			idBuffer[i] = NULL;
		}
	}
	delete[] idBuffer;
	idBuffer = NULL;
}

void ScanLineZBuffer::build(const Model& model)
{
	polygonTable.clear();
	edgeTable.clear();
	polygonTable.resize(height);
	edgeTable.resize(height);
	omp_lock_t mylock;
	omp_init_lock(&mylock);
	int faces_size = model.faces.size();
#pragma omp parallel for //num_threads(NUM_THREADS)
	for (int i = 0; i < faces_size; ++i)
	{
		float min_y = 0xfffffff, max_y = -0xfffffff;

		const Face& face = model.faces[i];
		Polygon polygon;
		polygon.id = i;
		//构建分类边表
		const vector<int>& vertexIdx = model.faces[i].vertexIdx;
		for (int j = 0, vsize = vertexIdx.size(); j < vsize; ++j)
		{
			//比较得到，边的上端点pt1
			Point3f pt1 = model.vertexes[vertexIdx[j]].point;
			Point3f pt2 = (j == vsize - 1 ?
				model.vertexes[vertexIdx[0]].point : 
				model.vertexes[vertexIdx[j + 1]].point);
			if (pt1.y < pt2.y)swap(pt1, pt2);

			assert(pt1.y >= 0 );//假设模型不超出窗口大小
			assert(round(pt1.y) < height);


			Edge edge;
			edge.dy = round(pt1.y) - round(pt2.y);
			if (edge.dy <= 0)continue;//避免不必要的计算，无法计算-1/k

			edge.x = pt1.x;
			edge.id = polygon.id;
			edge.dx = -(pt1.x - pt2.x) / (pt1.y - pt2.y);

//#pragma omp critical
			omp_set_lock(&mylock);
			edgeTable[round(pt1.y)].push_back(edge);
			omp_unset_lock(&mylock);

			min_y = min(pt2.y, min_y);
			max_y = max(pt1.y, max_y);

		}

		//构建分类多边形表
		polygon.dy = round(max_y) - round(min_y);
		if (polygon.dy > 0 && max_y > 0 && min_y < height)
		{
			Point3f v = model.vertexes[face.vertexIdx[0]].point;
			polygon.a = face.normal.x;
			polygon.b = face.normal.y;
			polygon.c = face.normal.z;
			polygon.d = -(polygon.a*v.x + polygon.b*v.y + polygon.c*v.z);
//#pragma omp critical
			omp_set_lock(&mylock);
			polygonTable[round(max_y)].push_back(polygon);
			omp_unset_lock(&mylock);
		}
	}
	omp_destroy_lock(&mylock);
	
}

//对活化多边形表排序的比较函数，递增
static bool edgeSortCmp(const ActiveEdge& lEdge, const ActiveEdge& rEdge)
{
	if (round(lEdge.x) < round(rEdge.x))return true;
	else if (round(lEdge.x) == round(rEdge.x))
	{
		if (round(lEdge.dx) < round(rEdge.dx))
			return true;
	}
	return false;
}

void ScanLineZBuffer::addEdge(int y, Polygon* active_polygon)
{
	//把该多边形在oxy平面上的投影和扫描线相交的边加入到活化边表中
	for (list<Edge>::iterator it = edgeTable[y].begin(), end = edgeTable[y].end();
		it != end;)
	{
		if (it->id != active_polygon->id)
		{
			++it;
			continue;
		}
		ActiveEdge active_edge;
		active_edge.x = it->x;
		active_edge.dx = it->dx;
		active_edge.dy = it->dy;

		if (isEqualf(active_polygon->c,0))
		{
			active_edge.z = 0;
			active_edge.dzx = 0;
			active_edge.dzy = 0;
		}
		else
		{
			active_edge.z = -(active_polygon->d + 
				active_polygon->a*it->x + 
				active_polygon->b*y) / active_polygon->c;
			active_edge.dzx = -(active_polygon->a / active_polygon->c);
			active_edge.dzy = active_polygon->b / active_polygon->c;
		}

		active_polygon->activeEdgeTable.push_back(active_edge);
		it->id = -1;
	}

	// 对当前活化多边形的活化边表按照x递增排序(x相等时，使用dxl)
	sort(active_polygon->activeEdgeTable.begin(), active_polygon->activeEdgeTable.end(), edgeSortCmp);
}

void ScanLineZBuffer::run(Model& model)
{
	clock_t t = clock();
	//if (!needUpdate)return idBuffer;
	if (!needUpdate)return;
	//根据显示大小，对顶点坐标进行光栅化
	Rasterizer raster;
	raster.setSize(width, height);
	raster.run(model);

	//点光源对面元进行着色
	Shader shader;
	shader.run(model);

	build(model);
	activePolygonTable.clear();

	//扫描线从上往下进行扫描

	for (int y = height - 1; y >= 0; --y)
	{
		memset(idBuffer[y], -1, sizeof(int)*width);
		fill(zBuffer, zBuffer + width, -0xfffffff);

		
		//检查分类的多边形表，如果有新的多边形涉及该扫描线，则把它放入活化的多边形表中
		for (list<Polygon>::iterator it = polygonTable[y].begin(),
			end = polygonTable[y].end(); it != end; ++it)
			activePolygonTable.push_back(*it);
		
		int apsize = activePolygonTable.size();
#pragma omp parallel for //num_threads(NUM_THREADS)
		for (int i = 0; i < apsize; ++i)
		{
			Polygon& active_polygon = activePolygonTable[i];
			addEdge(y, &active_polygon);
			
			vector<ActiveEdge>& active_edge_table = active_polygon.activeEdgeTable;
			assert(active_edge_table.size() % 2 == 0);//假设都是凸多边形

			int aesize = active_edge_table.size();
			//for (int j = 0; j < aesize; ++j)
			for (vector<ActiveEdge>::iterator ae_it = active_edge_table.begin(), end = active_edge_table.end();
				ae_it != end; ++ae_it)
			{
				//取出活化边对
				ActiveEdge& edge1 = *ae_it;
				ActiveEdge& edge2 = *(++ae_it);

				float zx = edge1.z;
				//更新z-buffer和id缓冲器
				for (int x = round(edge1.x), end = round(edge2.x); x < end; ++x)
				{
					if (zx > zBuffer[x])
					{
						zBuffer[x] = zx;
						idBuffer[y][x] = active_polygon.id;
					}
					zx += edge1.dzx;
				}
				//进入下一条扫描线，更新活化边参数
				--edge1.dy;
				--edge2.dy;
				edge1.x += edge1.dx;
				edge2.x += edge2.dx;
				edge1.z += edge1.dzx*edge1.dx + edge1.dzy;
				edge2.z += edge2.dzx*edge2.dx + edge2.dzy;
			}
			
			//移除已经结束的活化边
			int last = 0;
			aesize = active_edge_table.size();
			for (int j = 0; j < aesize; ++j, ++last)
			{
				while (active_edge_table[j].dy <= 0)
				{
					++j;
					if (j >= aesize)break;
				}
				if (j >= aesize)break;
				active_edge_table[last] = active_edge_table[j];
			}
			active_edge_table.resize(last);

			--active_polygon.dy;//活化多边形扫描线向下移动
		}

		//移除已经完成的活化多边形
		int last = 0;
		for (int i = 0, len = activePolygonTable.size(); i < len; ++i, ++last)
		{
			while (activePolygonTable[i].dy < 0)
			{
				++i;
				if (i >= len) break;
			}
			if (i >= len) break;

			activePolygonTable[last] = activePolygonTable[i];
		}
		activePolygonTable.resize(last);
	}

	needUpdate = false;
	cout << "ScanLineZbuffer耗时:" << float((clock() - t)) << "ms" << endl;
	//return idBuffer;
}


