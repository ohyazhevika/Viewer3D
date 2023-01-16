#ifndef CAMERA_3D_H
#define CAMERA_3D_H

#include "Vector3D.h"
#include "Matrix.h"
#include "AffineTransform3D.h"
#include "Camera2D.h"

class Camera3D : public Camera2D
{
protected:
	Vector3D Ov;
	Vector3D T;
	Vector3D N;
	double D;
	Matrix<> WorldToView;	 // матрица перехода от мировых координат к видовым
	Matrix<> ViewToProject;
	Matrix<> WorldToProject;

	double posZ; 

public:
	Camera3D() : posZ(0)
	{
		SetOv(Vector3D(0, 0, 0));
		SetT(Vector3D(0, 1, 0));
		SetN(Vector3D(1, 1, 1));
		SetD(16);
		UpdateCamera();
	}
	void SetOv(Vector3D _Ov) { Ov = _Ov; }
	void SetT(Vector3D _T) { T = _T;  }
	void SetN(Vector3D _N) { N = _N; }
	void SetD(double _D) { D = _D;  }

	Vector3D GetOv() { return Ov; }
	Vector3D GetT() { return T; }
	Vector3D GetN() { return N; }
	double GetD() { return D; }


	Matrix<> GetWorldToView() { return WorldToView; }
	Matrix<> GetViewToProject() { return ViewToProject; }
	Matrix<> GetWorldToProject() { return WorldToProject; }

	void UpdateCamera()
	{
		// обновляем матрицу WorldToView
		double N_norm = 1.0 / N.length();
		Vector3D k_v = N * N_norm;
		Vector3D i_v = T % N;
		i_v *= 1.0 / i_v.length();
		Vector3D j_v = k_v % i_v;
		double world_to_view [16] = {
			i_v.X(), i_v.Y(), i_v.Z(), -(i_v.X() * Ov.X() + i_v.Y() * Ov.Y() + i_v.Z() * Ov.Z()), 
			j_v.X(), j_v.Y(), j_v.Z(), -(j_v.X() * Ov.X() + j_v.Y() * Ov.Y() + j_v.Z() * Ov.Z()),
			k_v.X(), k_v.Y(), k_v.Z(), -(k_v.X() * Ov.X() + k_v.Y() * Ov.Y() + k_v.Z() * Ov.Z()), 
			0,		0,		0,		1
		};
		WorldToView = Matrix<>(4, 4, world_to_view) ;

		double view_to_project[12] = {
			1, 0, 0, 0, 
			0, 1, 0, 0, 
			0, 0, -1/D, 1
		};
		ViewToProject = Matrix<>(3, 4, view_to_project);

		WorldToProject = ViewToProject * WorldToView;
	}

	void Turn(double phi)	// поворот вектора N вокруг вектора Т (0,1,0) на угол phi (облетаем модель)
	{
		N = Vector3D(cos(phi) * N.X() + sin(phi) * N.Z(), N.Y(), -sin(phi) * N.X() + cos(phi) * N.Z());
	}

	void Yaw(double phi)	// "рыскание" - поворот вектора N вокруг вектора Т с пересчетом точки Ov так, что Oe - точка наблюдателя - остается на месте
		// поворот головы наблюдателя на угол phi в правую или левую сторону
	{
		Vector3D N_curr = N;
		N_curr = N_curr * (1 / N_curr.length());
		Vector3D Oe = Ov + (N_curr * D); // координаты наблюдателя
		N = Vector3D(cos(phi) * N_curr.X() + sin(phi) * N_curr.Z(), N_curr.Y(), -sin(phi) * N_curr.X() + cos(phi) * N_curr.Z());
		N_curr = N;
		N_curr = N_curr * (1 / N_curr.length());
		Ov = Oe - (N * D);
	}

	void Pitch(double phi)	// "тангаж" - поворот вектора N в плоскости векторов N и T(0, 1, 0) c пересчетом точки Ov так, что Oe- точка
		// наблюдателя - не меняется (поворот головы наблюдателя вверх и вниз)
	{
		Vector3D
			N_curr = N,
			T_curr = T,
			v = N_curr % T_curr;
		double V1 = v.X(), V2 = v.Y(), V3 = v.Z();
		double vlen = 1 / sqrt(V1 * V1 + V3 * V3),
			c_vx = V1 * vlen,
			s_vx = V3 * vlen;
		double Nx1 = N_curr.X(), Ny1 = N_curr.Y(), Nz1 = N_curr.Z();
		double ncoord[4] = { Nx1, Ny1, Nz1, 1 };
		Matrix<> Nmatr(4, 1, ncoord);
		Nmatr = RotationY(c_vx, s_vx) * RotationZ(phi) * RotationY(c_vx, -s_vx) * Nmatr;
		double N1 = Nmatr(1, 1) / Nmatr(4, 1), N2 = Nmatr(2, 1) / Nmatr(4, 1), N3 = Nmatr(3, 1) / Nmatr(4, 1);
		N_curr = N_curr * (1 / N_curr.length());
		Vector3D Oe = Ov + (N_curr * D); // координаты наблюдателя
		N = (Vector3D(Nx1, N2, Nz1));
		N_curr = N;
		N_curr = N_curr * (1 / N_curr.length());
		Ov = Oe - (N_curr * D);
	}


};




#endif CAMERA_3D_H
