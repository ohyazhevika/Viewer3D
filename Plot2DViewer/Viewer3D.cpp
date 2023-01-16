#define _USE_MATH_DEFINES
#include <windows.h>

#include <windowsx.h>
#include <cmath>
#include "Scene3D.h"

LRESULT _stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);						// прототип оконной процедуры
int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)		// основная процедура
{
	// Первая составляющая часть основной процедуры - создание окна: сначала описывается оконный класс wc, затем создаётся окно hWnd
	WNDCLASS wc;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;						// имя оконной процедуры, закрепленной за данным классом
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;						// идентификатор приложения, содержащий адрес начала выделенной ему области памяти
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(6);
	wc.lpszMenuName = 0;							// меню в оконном классе отсутствует
	wc.lpszClassName = (LPCSTR)"MainWindowClass";	// имя оконного класса, используемое при создании экземпляров окна
	RegisterClass(&wc);								// регистрация класса wc

	HWND hWnd = CreateWindow(						// hWnd - дескриптор, идентифицирующий окно; функция создания окна заполняет дескриптор hWnd ненулевым значением
		(LPCSTR)"MainWindowClass",					// имя оконного класса
		(LPCSTR)"3D Computer Graphics Viewer",					// заголовок окна
		WS_OVERLAPPEDWINDOW,						// стиль окна
		200, 200, 400, 400,							// координаты на экране левого верхнего угла окна, его ширина и высота
		nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Вторая составляющая часть основной процедуры - основной цикл обработки системных сообщений, который ожидает сообщения и рассылает их соответствующим окнам
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))				// функция GetMessage выбирает из очереди сообщение и заносит его в структуру msg
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);						// функция DispatchMessage оповещает систему о необходимости вызова оконной процедуры
	}

	return 0;
}

// В основном модуле объявляется только одна глобальная переменная - создаётся объект класса Scene3D
// Все дальнейшие действия осуществляются посредством обращения к методам, реализованным в этом классе
Scene3D scene;

LRESULT _stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)		// оконная процедура принимает и обрабатывает все сообщения, отправленные окну
{
	switch (msg)
	{
	case WM_PAINT:
	{
		HDC dc = GetDC(hWnd);
		scene.Clear(dc);				// Вызов реализованного в классе Camera2D метода, отвечающего за очистку рабочей области окна hWnd
		scene.Render(dc);		// Вызов реализованного в классе Scene3D метода, отвечающего за отрисовку проекции модели
		ReleaseDC(hWnd, dc);
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	case WM_SIZE:
	{
		HDC dc = GetDC(hWnd);
		scene.SetResolution(dc);
		ReleaseDC(hWnd, dc);
		InvalidateRect(hWnd, nullptr, false);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		scene.StartDragging(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	case WM_MOUSEMOVE:
	{
		if (scene.IsDragging())
		{
			scene.Drag(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			InvalidateRect(hWnd, nullptr, false);
		}
		return 0;
	}
	case WM_LBUTTONUP:
	{
		scene.StopDragging();
		return 0;
	}

	case WM_RBUTTONDOWN:
	{
		HDC dc = GetDC(hWnd);
		scene.StartShowingPoint();
		scene.ShowPoint(dc, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_RBUTTONUP:
	{
		HDC dc = GetDC(hWnd);
		scene.StopShowingPoint();
		ReleaseDC(hWnd, dc);
		InvalidateRect(hWnd, nullptr, false);
		return 0;
	}

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_LEFT:
		{
			if (GetAsyncKeyState(VK_SHIFT))
			{
				scene.Apply(Translation(0, 0, -0.5));
			}
			else
				scene.Apply(Translation(-0.5, 0, 0));
			break;
		}

		case VK_RIGHT:
		{
			if (GetAsyncKeyState(VK_SHIFT))
			{
				scene.Apply(Translation(0, 0, 0.5));
			}
			else
				scene.Apply(Translation(0.5, 0, 0));
			break;
		}

		case VK_UP:
		{
			if (GetAsyncKeyState(VK_CONTROL))
			{
				scene.Apply(RotationZ(-M_PI / 60));
				break;
			}
			if (GetAsyncKeyState(VK_SPACE))
			{
				scene.Apply(RotationX(-M_PI / 60));
				break;
			}
			else
				scene.Apply(Translation(0, 0.5, 0));
			break;
		}

		case VK_DOWN:
		{
			if (GetAsyncKeyState(VK_CONTROL))
			{
				scene.Apply(RotationZ(M_PI / 60));
				break;
			}
			if (GetAsyncKeyState(VK_OEM_MINUS))
			{
				/*Point P = scene.GetHouseCenter();
				scene.Apply(Scaling(0.95, P.x, P.y, P.z));*/
				break;
			}

			if (GetAsyncKeyState(VK_SPACE))
			{
				scene.Apply(RotationX(M_PI / 60));
				break;
			}
			else
				scene.Apply(Translation(0, -0.5, 0));
			break;
		}
		case VK_OEM_PLUS:
		{
			if (GetAsyncKeyState(VK_CONTROL))
			{
				POINT P;
				if (GetCursorPos(&P))
					scene.ChangeSize(1.05, P.x, P.y);
				break;
			}
			if (GetAsyncKeyState(VK_SPACE))
			{
				scene.SetD(scene.GetD() - 1);
				scene.Update();
				break;
			}
			else
			{
				scene.Apply(Scaling(2, 2, 2));
			}
			break;
		}
		case VK_OEM_MINUS:
		{
			if (GetAsyncKeyState(VK_CONTROL))
			{
				POINT P;
				if (GetCursorPos(&P))
					scene.ChangeSize(0.95, P.x, P.y);
				break;
			}
			if (GetAsyncKeyState(VK_SPACE))
			{
				scene.SetD(scene.GetD() + 1);
				scene.Update();
				break;
			}
			else
			{
				scene.Apply(Scaling(0.5, 0.5, 0.5));
			}
			break;

		}
		case VK_ESCAPE:
		{
			scene.ResetModel();
			break;
		}

		case VK_NUMPAD4:  // облет модели
		{
			scene.Turn(-M_PI / 60);
			scene.Update();
			break;
		}

		case VK_NUMPAD6:
		{
			scene.Turn(M_PI / 60);
			scene.Update();
			break;
		}

		case VK_NUMPAD1: // рысканье - поворот налево
		{
			scene.Yaw(M_PI / 60);
			scene.Update();
			break;
		}

		case VK_NUMPAD3: // рысканье - поворот направо (вокруг Т(0,1,0)!)
		{
			scene.Yaw(-M_PI / 60);
			scene.Update();
			break;
		}

		case VK_NUMPAD2: // тангаж - поворот вниз
		{
			scene.Pitch(M_PI / 60);
			scene.Update();
			break;
		}

		case VK_NUMPAD8: // тангаж - поворот вверх
		{
			scene.Pitch(-M_PI / 60);
			scene.Update();
			break;
		}

		case 0x44: // ключ D
		{
			if (GetAsyncKeyState(VK_SPACE))
			{
				Point
					P = scene.GetHouseCenter(),
					vect = scene.GetGrowthVector();
				scene.Apply(Scaling(0.95, vect.x, vect.y, vect.z, P.x, P.y, P.z));
				break;
			}
			else
			{
				Point P = scene.GetHouseCenter();
				scene.Apply(Scaling(0.95, P.x, P.y, P.z));
				break;
			}
		}

		case 0x55: // ключ U
		{
			if (GetAsyncKeyState(VK_SPACE))
			{
				Point
					P = scene.GetHouseCenter(),
					vect = scene.GetGrowthVector();
				scene.Apply(Scaling(1.05, vect.x, vect.y, vect.z, P.x, P.y, P.z));
				break;
			}
			else
			{
				Point P = scene.GetHouseCenter();
				scene.Apply(Scaling(1.05, P.x, P.y, P.z));
				break;
			}
		}
		}

		InvalidateRect(hWnd, nullptr, false);
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	default:
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	}
	return 0;
}
