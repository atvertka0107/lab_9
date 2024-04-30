#include "pch.h"
#include <cmath>
#include <iostream>
#include <Windows.h>

double syrov_5(double eps, double x) {
	long double n0 = pow(x, 2), n1 = n0 * -(pow(x, 2) / 3), val = n0;
	for (long double n = 3; abs(n0 - n1) >= eps; n++) {
		n0 = n1;
		n1 *= (-4 * pow(x, 2)) / (4 * pow(n, 2) - 2 * n);
		val += n0;
	}
	return 1 - val;
}

double math_function(double x) {
	return pow(cos(x), 2);
}

extern "C" __declspec(dllexport) void myf_5(double x, double a, double eps, int info, void (*callback)(int info, ...)) { // Внешняя функция для расчета
	double f = syrov_5(eps, x);
	double F = math_function(x);
	double delta = sqrt(abs(pow(f, 2) - pow(F, 2)));

	// Получаем текущие атрибуты консоли
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	WORD originalAttributes = consoleInfo.wAttributes;

	callback(info, (info == 2) ? eps : x, f, F, delta);

	// Возвращаем цвет консоли обратно
	SetConsoleTextAttribute(hConsole, originalAttributes);
}

extern "C" __declspec(dllexport) char* FName() { // Функция для вывода названия функции
	return (char*)"cos(x)^2";
}

extern "C" __declspec(dllexport) char* LastName() { // Функция для вывода фамилии автора библиотеки
	return (char*)"Сыров";
}

extern "C" __declspec(dllexport) int* GetColors() {
	int* colors = new int[2];
	colors[0] = 37;
	colors[1] = 34;
	return colors;
}
