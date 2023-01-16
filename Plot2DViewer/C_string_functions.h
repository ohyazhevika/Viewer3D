#include <windows.h>
#include <math.h>
void DoubleToChar(double x, char(&result)[20])
{
	char buffer[20], xx[20], * yy;
	int z = int(x);
	_itoa_s(z, result, 10);
	double del = fabs(x - z) * 10;
	int dell = del;
	_itoa_s(dell, xx, 10);
	strcat(result, ".");
	strcat(result, xx);
}