#pragma once
#ifndef MODEL_3D_H
#define MODEL_3D_H

#include <vector>
#include <string>
#include "Matrix.h"
#include "AffineTransform3D.h"

struct Point
{
	double x; double y; double z;
};

class Model3D
{
private:
	Matrix<> Vertices; // карта вершин модели
	Matrix<> InitialVertices; // карта исходных вершин модели
	Matrix<> ProjectedVertices;
	Matrix<int> Faces; // карта граней модели
	Matrix<int> Edges; // карта рёбер модели
	Matrix<int> InvisibleEdges; // список фиктивных ребер
	Matrix <> CumulativeAT; // накапливающее аффинное преобразование

public:
	Model3D() : Vertices(), InitialVertices(), ProjectedVertices(), Faces(), Edges(), CumulativeAT(Identity()), 
	InvisibleEdges(){} // конструктор по умолчанию

	Model3D(const Matrix<>& Vertices, const Matrix<int>& Faces) // конструктор модели по заданным картам вершин и граней
		: InitialVertices(Vertices), Vertices(Vertices), Faces(Faces), CumulativeAT(Identity())
	{
		SetEdges();
	}

	Model3D(const Matrix<>& Vertices) : InitialVertices(Vertices), Vertices(Vertices), CumulativeAT(Identity()) {}

	Model3D(int, int, string, string); // конструктор создания модели по именам
	// файлов, в которых лежат карта вершин и карта граней

	Model3D(int, int, int, string, string, string);// аналогичный, но со списком фиктивных рёбер

	Matrix<int> GetAdjacencyMatrix(); // возвращает матрицу смежности вершин модели, построенную по карте граней

	void SetEdges(); // создание карты рёбер по заданной карте граней

	Matrix<> GetVertices() { return Vertices; }
	Matrix<int> GetEdges() { return Edges; }
	Matrix<int> GetFaces() { return Faces; }
	Matrix<> GetProjectedVertices() { return ProjectedVertices; }

	double GetVertexX(int); // получение координат вершины модели с заданным номером
	double GetVertexY(int);
	double GetVertexZ(int);

	Point GetCoordinates(int v) { return Point({ GetVertexX(v), GetVertexY(v), GetVertexZ(v) }); }

	double GetProjVertX(int);
	double GetProjVertY(int);

	void Apply(Matrix<>); // применение к модели аффинного преобразования, заданного своей матрицей
	void Project(Matrix<>); // проецирование  модели

	void Reset()
	{
		CumulativeAT = Identity();
		Vertices = InitialVertices;

	}
};

Model3D::Model3D(int v_cnt, int f_cnt, string fv_name, string ff_name) : CumulativeAT(Identity())
{
	Vertices = Matrix<>(4, v_cnt);
	Faces = Matrix<int>(f_cnt, 3);
	ifstream
		fv(fv_name),
		fe(ff_name);
	if (fv.is_open())
	{
		fv >> Vertices;
	}
	fv.close();
	if (fe.is_open())
	{
		fe >> Faces;
	}
	fe.close();
	InitialVertices = Vertices;
	SetEdges();
}

Model3D::Model3D(int v_cnt, int f_cnt, int e_cnt, string fv_name, string ff_name, string fe_name): CumulativeAT(Identity())
{
	
	Vertices = Matrix<>(4, v_cnt);
	Faces = Matrix<int>(f_cnt, 3);
	InvisibleEdges = Matrix<int>(e_cnt, 2);
	ifstream
		fv(fv_name),
		ff(ff_name),
		fe(fe_name);
	if (fv.is_open())
	{
		fv >> Vertices;
	}
	fv.close();
	if (ff.is_open())
	{
		ff >> Faces;
	}
	ff.close();
	if (fe.is_open())
	{
		fe >> InvisibleEdges;
		ofstream fo("MatrixOutput.txt");
		fo << InvisibleEdges;
	}
	fe.close();
	InitialVertices = Vertices;
	SetEdges();
}
	

Matrix<int> Model3D::GetAdjacencyMatrix()
{
	int
		v_cnt = Vertices.GetColumns(),
		f_cnt = Faces.GetRows();
	Matrix<int> AdjMatrix(v_cnt, v_cnt);
	for (int i = 1; i <= f_cnt; i++)
	{
		int
			v1 = Faces(i, 1),
			v2 = Faces(i, 2),
			v3 = Faces(i, 3);
		AdjMatrix(v1, v2) = AdjMatrix(v2, v1)
			= AdjMatrix(v1, v3) = AdjMatrix(v3, v1)
			= AdjMatrix(v2, v3) = AdjMatrix(v3, v2) = 1;
	}

	int e_cnt = InvisibleEdges.GetRows();
	for (int i = 1; i <= e_cnt; i++)
	{
		int
			v1 = InvisibleEdges(i, 1),
			v2 = InvisibleEdges(i, 2);
		AdjMatrix(v1, v2) = AdjMatrix(v2, v1) = 0;
	}
	return AdjMatrix;
}

void Model3D::SetEdges()
{
	int
		max_edges_cnt = Faces.GetRows() * 3 * 2,
		edges_cnt = 0;
	int* edges = new int[max_edges_cnt];
	int v_cnt = Vertices.GetColumns();
	Matrix<int> AdjMatrix = GetAdjacencyMatrix();
	for (int i = 1; i <= v_cnt; i++)
		for (int j = i + 1; j <= v_cnt; j++)
		{
			if (AdjMatrix(i, j) == 1)
			{
				edges[edges_cnt++] = i;
				edges[edges_cnt++] = j;
			}
		}
	Edges = Matrix<int> (edges_cnt / 2, 2, edges);
	delete[] edges;
}

double Model3D::GetVertexX(int v)
{
	return Vertices(1, v) / Vertices(4, v);
}

double Model3D::GetVertexY(int v)
{
	return Vertices(2, v) / Vertices(4, v);
}

double Model3D::GetVertexZ(int v)
{
	return Vertices(3, v) / Vertices(4, v);
}

double Model3D::GetProjVertX(int v)
{
	return ProjectedVertices(1, v) / ProjectedVertices(3, v);
}

double Model3D::GetProjVertY(int v)
{
	return ProjectedVertices(2, v) / ProjectedVertices(3, v);
}

void Model3D::Apply(Matrix<> A)
{
	CumulativeAT = A * CumulativeAT;
	Vertices = CumulativeAT * InitialVertices;
}

void Model3D::Project(Matrix<> P)
{
	ProjectedVertices = P * Vertices;
}

#endif MODEL_3D_H

