#ifndef CAMERA_2D_H
#define CAMERA_2D_H

#include <windows.h>
#include "C_string_functions.h"
class Camera2D
{
protected:
	double X0, Y0;						// Экранные координаты начала мировой системы координат
	double px, py;						// Единицы масштаба мировой системы координат, выраженные в пикселях
	int W, H;							// Разрешение рабочей области окна
	bool isDragging;					// Перетаскивается ли сейчас график вместе со всей системой
	bool showingPoint;
	double
		deltaX,
		deltaY;
	int WorldToScreenX(double X)		// Переход от мировых координат к экранным (для абсциссы)
	{
		return X0 + px * X;
	}
	int WorldToScreenY(double Y)		// Переход от мировых координат к экранным (для ординаты)
	{
		return Y0 - py * Y;
	}
	double ScreenToWorldX(int X)		// Переход от экранных координат к мировым (для абсциссы)
	{
		return (X - X0 + 0.5) / px;
	}
	double ScreenToWorldY(int Y)		// Переход от экранных координат к мировым (для ординаты)
	{
		return -(Y - Y0 + 0.5) / py;
	}
	double L()							// Абсцисса левой границы рабочей области окна (в мировых координатах)
	{
		double ans = (double)(-X0 / px * 1.0);
		return ans;
	}
	double R()							// Абсцисса правой границы рабочей области окна (в мировых координатах)
	{
		double ans = (double)((W - X0) / px * 1.0);
		return ans;
	}
	double T()
	{
		double ans = (double)((Y0 - H) / py * 1.0);
		return ans;       // Ордината верхней границы рабочей области окна (в мировых координатах)
	}
	double B()
	{
		double ans = (double)(Y0 / py * 1.0);
		return ans;	// Ордината нижней границы рабочей области окна (в мировых координатах)
	}

protected:
	double posX, posY;		// Позиция графического курсора в мировых координатах (для функций MoveTo и LineTo)

public:
	Camera2D(): X0(200), Y0(200), px(20), py(20), posX(0), posY(0) {}
	Camera2D(double X0, double Y0, double px, double py) : X0(X0), Y0(Y0), px(px), py(py), posX(0), posY(0)
	{
	}
	void Clear(HDC dc)
	{
		// Очистка рабочей области окна
		RECT r;
		GetClientRect(WindowFromDC(dc), &r);
		Rectangle(dc, 0, 0, r.right, r.bottom);
		//MoveToEx(dc, 0, 0, nullptr);
	}
	void SetResolution(HDC dc)
	{
		// Данная процедура вызывается при изменении размеров окна
		// В ней задаются значения величин W, H, а также настраиваются значения параметров X0, Y0, px, py таким образом, чтобы обеспечить одинаковые масштабы по координатным осям
		RECT r;
		GetClientRect(WindowFromDC(dc), &r);
		if (W <= 0 && H <= 0)
		{
			W = r.right + 1;
			H = r.bottom + 1;

		}
		{
			int W_0 = W;
			int H_0 = H;
			W = r.right + 1;
			H = r.bottom + 1;
			double W_ratio = (double)(1.0 * W / W_0),
				H_ratio = (double)(1.0 * H / H_0),
				p_ratio = (double)(px / py);
			X0 = W_ratio * X0;
			Y0 = W_ratio * p_ratio * Y0 + double(1.0 * H / 2) * (H_ratio - W_ratio * p_ratio);
			px = W_ratio * px;
			py = px;

		}
	}
	void MoveTo(double X, double Y)
	{
		// Перемещение графического курсора (posX, posY)
		// Обратите внимание, что мы действуем в мировых координатах
		posX = X,
		posY = Y;
	}
	void LineTo(HDC dc, double X, double Y)
	{
		// Отрисовка линии из текущей позиции графического курсора в точку (X, Y) и его перемещение в эту точку
		// Обратите внимание, что мы действуем в мировых координатах
		// При отрисовке линии могут быть использованы WinApi функции
		// ::MoveToEx(dc, Xs, Ys, nullptr) и ::LineTo(dc, Xs, Ys)
		MoveToEx(dc, WorldToScreenX(posX), WorldToScreenY(posY), 0);
		this->MoveTo(X, Y);
		::LineTo(dc, WorldToScreenX(posX), WorldToScreenY(posY));

	}
	void Axes(HDC dc) // Отрисовка координатных осей
	{
		this->MoveTo(L(), 0);
		this->LineTo(dc, R(), 0);
		this->MoveTo(0, B());
		this->LineTo(dc, 0, T());
	}

	void Coordinates(HDC dc)
	{
		SetTextColor(dc, RGB(0, 0, 255));
		SetBkMode(dc, TRANSPARENT);
		RECT rect;
		GetClientRect(WindowFromDC(dc), &rect);
		double y = (B() < 0) ? B() : 0;
		y = (T() < 0) ? y : T() + 1;
		rect.top = WorldToScreenY(y);
		char buffer[20];
		for (long long x = L(); x < R(); x++)
		{
			rect.left = WorldToScreenX(x);
			_itoa_s(x, buffer, 20, 10);
			LPCSTR p = buffer;
			DrawText(dc, p, -1, &rect, DT_SINGLELINE | DT_NOCLIP);
		}
		double x = (L() > 0) ? L() : 0;
		x = (R() < 0) ? R() - 1 : x;
		rect.left = WorldToScreenX(x);
		for (long long y = B(); y > T(); y--)
		{
			rect.top = WorldToScreenY(y);
			_itoa_s(y, buffer, 20, 10);
			LPCSTR p = buffer;
			DrawText(dc, p, -1, &rect, DT_SINGLELINE | DT_NOCLIP);
		}
	}
	void Grid(HDC dc)
	{
		HGDIOBJ original = NULL;
		original = SelectObject(dc, GetStockObject(DC_PEN));
		SetDCPenColor(dc, RGB(0, 0, 255));
		for (long long x = L(); x < R(); x++)
		{
			this->MoveTo(x, B());
			this->LineTo(dc, x, T());
		}
		for (long long y = B(); y > T(); y--)
		{
			this->MoveTo(L(), y);
			this->LineTo(dc, R(), y);
		}
		SelectObject(dc, original);
	}
public:
	void StartDragging(int X, int Y)
	{
		isDragging = true;
		deltaX = X0 - X;//координаты начала движения относительно начала координат, то есть откуда начинаем движение
		deltaY = Y0 - Y;
	}
	void Drag(int X, int Y)
	{
		X0 = deltaX + X;//срабатывает при изменеии координат, то есть переносит начала координат в указанную точку, относительно того, где была зажата ЛКМ
		Y0 = deltaY + Y;
	}
	void MoveToSide(double delX, double delY)
	{
		X0 += delX;
		Y0 += delY;
	}
	void StopDragging()
	{
		isDragging = false;
	}
	bool IsDragging()
	{
		return isDragging;
	}

	void ChangeSize(double k, int X, int Y)
	{
		//if (!(k < 1 && px <= 30 && py <= 30))
		{
			X0 = X - k * (X - X0);
			Y0 = Y - k * (Y - Y0);
			px = k * px;
			py = k * py;
		}
	}

	void StartShowingPoint()
	{
		showingPoint = true;
	}

	void StopShowingPoint()
	{
		showingPoint = true;
	}

	void PointCoordinates(double X, double Y, char(&result)[20])
	{
		char xstr[20], ystr[20];
		DoubleToChar(X, xstr);
		DoubleToChar(Y, ystr);
		strcpy(result, "(x=");
		strcat(result, xstr);
		strcat(result, " y=");
		strcat(result, ystr);
		strcat(result, ")");
	}
	void ShowPoint(HDC dc, double X, double Y)
	{
		SetTextColor(dc, RGB(130, 10, 130));
		//SetBkMode(dc, TRANSPARENT);
		RECT rect;
		GetClientRect(WindowFromDC(dc), &rect);
		rect.top = Y - 20;
		rect.left = X - 40;
		char buffer[20];
		PointCoordinates(ScreenToWorldX(X), ScreenToWorldY(Y), buffer);
		LPCSTR p = buffer;
		DrawText(dc, p, -1, &rect, DT_SINGLELINE | DT_NOCLIP);
	}
};

#endif CAMERA_2D_H
