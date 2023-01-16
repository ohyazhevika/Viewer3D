#define _USE_MATH_DEFINES
#include <windows.h>

#include <windowsx.h>
#include <cmath>
#include "Scene3D.h"

LRESULT _stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);						// �������� ������� ���������
int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)		// �������� ���������
{
	// ������ ������������ ����� �������� ��������� - �������� ����: ������� ����������� ������� ����� wc, ����� �������� ���� hWnd
	WNDCLASS wc;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;						// ��� ������� ���������, ������������ �� ������ �������
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;						// ������������� ����������, ���������� ����� ������ ���������� ��� ������� ������
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(6);
	wc.lpszMenuName = 0;							// ���� � ������� ������ �����������
	wc.lpszClassName = (LPCSTR)"MainWindowClass";	// ��� �������� ������, ������������ ��� �������� ����������� ����
	RegisterClass(&wc);								// ����������� ������ wc

	HWND hWnd = CreateWindow(						// hWnd - ����������, ���������������� ����; ������� �������� ���� ��������� ���������� hWnd ��������� ���������
		(LPCSTR)"MainWindowClass",					// ��� �������� ������
		(LPCSTR)"3D Computer Graphics Viewer",					// ��������� ����
		WS_OVERLAPPEDWINDOW,						// ����� ����
		200, 200, 400, 400,							// ���������� �� ������ ������ �������� ���� ����, ��� ������ � ������
		nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// ������ ������������ ����� �������� ��������� - �������� ���� ��������� ��������� ���������, ������� ������� ��������� � ��������� �� ��������������� �����
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))				// ������� GetMessage �������� �� ������� ��������� � ������� ��� � ��������� msg
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);						// ������� DispatchMessage ��������� ������� � ������������� ������ ������� ���������
	}

	return 0;
}

// � �������� ������ ����������� ������ ���� ���������� ���������� - �������� ������ ������ Scene3D
// ��� ���������� �������� �������������� ����������� ��������� � �������, ������������� � ���� ������
Scene3D scene;

LRESULT _stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)		// ������� ��������� ��������� � ������������ ��� ���������, ������������ ����
{
	switch (msg)
	{
	case WM_PAINT:
	{
		HDC dc = GetDC(hWnd);
		scene.Clear(dc);				// ����� �������������� � ������ Camera2D ������, ����������� �� ������� ������� ������� ���� hWnd
		scene.Render(dc);		// ����� �������������� � ������ Scene3D ������, ����������� �� ��������� �������� ������
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

		case VK_NUMPAD4:  // ����� ������
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

		case VK_NUMPAD1: // �������� - ������� ������
		{
			scene.Yaw(M_PI / 60);
			scene.Update();
			break;
		}

		case VK_NUMPAD3: // �������� - ������� ������� (������ �(0,1,0)!)
		{
			scene.Yaw(-M_PI / 60);
			scene.Update();
			break;
		}

		case VK_NUMPAD2: // ������ - ������� ����
		{
			scene.Pitch(M_PI / 60);
			scene.Update();
			break;
		}

		case VK_NUMPAD8: // ������ - ������� �����
		{
			scene.Pitch(-M_PI / 60);
			scene.Update();
			break;
		}

		case 0x44: // ���� D
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

		case 0x55: // ���� U
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
