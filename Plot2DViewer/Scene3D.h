#ifndef SCENE_3D_H
#define SCENE_3D_H

#include <fstream>
#include <string>
#include "Camera3D.h"
#include "Model3D.h"

using namespace std;

class Scene3D : public Camera3D
{
public:

	Model3D model;
	void Render(HDC);
	void Apply(Matrix<>);
	void MoveTo(double, double, double);
	void LineTo(HDC, double, double, double);
	void Axes3D(HDC);
	void Update();
	void ResetModel();
	Point GetHouseCenter();
	Point GetGrowthVector();

	//Scene3D(): model(5, 6, 1,  "PyramidVertices.txt", "PyramidFaces.txt", "PyramidInvisibleEdges.txt")
	Scene3D(): model(10, 18, 8, "HouseVertices.txt", "HouseFaces.txt", "HouseInvisibleEdges.txt")
	{
		Apply(Identity());
	}
};

void Scene3D::Render(HDC dc)
{
	Scene3D::Axes3D(dc);
	Matrix <int> mEdg = model.GetEdges();
	int edgCount = mEdg.GetRows();
	Matrix<> ProjVer = model.GetProjectedVertices();
	for (int i = 1; i <= edgCount; i++)
	{
		int v1 = mEdg(i, 1), v2 = mEdg(i, 2);
		double
			x1 = model.GetProjVertX(v1),
			y1 = model.GetProjVertY(v1),
			x2 = model.GetProjVertX(v2),
			y2 = model.GetProjVertY(v2);
		Camera2D::MoveTo(x1, y1);
		Camera2D::LineTo(dc, x2, y2);
	}
}

void Scene3D::Apply(Matrix<> AT)
{
	model.Apply(AT);
	model.Project(WorldToProject);
}

void Scene3D::MoveTo(double X, double Y, double Z)
{
	// Перемещение графического курсора (posX, posY, posY) - мировые координаты!
	posX = X;
	posY = Y;
	posZ = Z;
}

void Scene3D::LineTo(HDC dc, double X, double Y, double Z)
{
	// Отрисовка линии из текущей позиции графического курсора в точку (X, Y, Z) и его перемещение в эту точку - мировые координаты!

	double coords[4] = { posX, posY, posZ, 1 };
	Matrix<> CursV(4, 1, coords);
	Model3D Cursor(CursV);
	Cursor.Apply(Identity());
	Cursor.Project(WorldToProject);
	double
		Xproj = Cursor.GetProjVertX(1),
		Yproj = Cursor.GetProjVertY(1);
	MoveToEx(dc, WorldToScreenX(Xproj), WorldToScreenY(Yproj), 0);
	this->MoveTo(X, Y, Z);
	double coords1[4] = { posX, posY, posZ, 1 };
	Matrix<> CursV1(4, 1, coords1);
	Model3D Cursor1(CursV1);
	Cursor1.Apply(Identity());
	Cursor1.Project(WorldToProject);
	Xproj = Cursor1.GetProjVertX(1);
	Yproj = Cursor1.GetProjVertY(1);
	::LineTo(dc, WorldToScreenX(Xproj), WorldToScreenY(Yproj));

}

void Scene3D::Axes3D(HDC dc)
{
	HGDIOBJ original = NULL;
	original = SelectObject(dc, GetStockObject(DC_PEN));
	SetDCPenColor(dc, RGB(130, 10, 130));
	this->MoveTo(10, 0, 0);
	this->LineTo(dc, 0, 0, 0);
	SetDCPenColor(dc, RGB(0, 0, 250));
	this->MoveTo(0, 10, 0);
	this->LineTo(dc, 0, 0, 0);
	SetDCPenColor(dc, RGB(0, 250, 0));
	this->MoveTo(0, 0, 10);
	this->LineTo(dc, 0, 0, 0);
	SelectObject(dc, original);
}

void Scene3D::ResetModel()
{
	model.Reset();
	model.Project(WorldToProject);
}
void Scene3D::Update()
{
	UpdateCamera();
	model.Project(WorldToProject);
}


Point Scene3D::GetHouseCenter()
{
	Point
		v1 = model.GetCoordinates(1),
		v3 = model.GetCoordinates(3);
	return Point({ (v1.x + v3.x) / 2, (v1.y + v3.y) / 2, (v1.z + v3.z) / 2 });
}

Point Scene3D::GetGrowthVector()
{
	Point
		v2 = model.GetCoordinates(2),
		v6 = model.GetCoordinates(6);
	return Point({ v6.x - v2.x, v6.y - v2.y, v6.z - v2.z });
}



#endif SCENE_3D_H