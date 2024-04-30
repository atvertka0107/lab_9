#include <iostream>
#include <string>
#include <windows.h>
#include <iomanip>
#include <cstdarg>
#include <cmath>

const int N_VAR = 18; // Номер варианта
int n = 0, count_color = 0; // Количество вызовов callback
double eps, x0, x1, a, dx; // Данные для потсроения таблиц
int text_color, font_color;

// Установка цвета консоли
void setColor(int text, int background) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int colorCode = text + background * 16;
	SetConsoleTextAttribute(hConsole, colorCode);
}

// callback функция для отрисовки строк
void callbackFunction(int messageType, ...) {
	
	va_list args;
	va_start(args, messageType);
	// Изменяем цвет текста и фона
	setColor(text_color + count_color, font_color + count_color); // 37 - pink 34 - red
	double a = va_arg(args, double);
	double b = va_arg(args, double);
	double c = va_arg(args, double);
	double d = va_arg(args, double);

	switch (messageType) {

	case 3: // Ошибка при вычислении
		std::cout << std::setw(10) << "Error"
			<< std::setw(15) << "Error"
			<< std::setw(16) << "Error"
			<< std::setw(16) << "Error" << std::endl;
			break;
	case 1: case 2:
			std::cout << std::fixed << std::setw(11) << std::setprecision(7) << a
			<< std::setprecision(10) << std::setw(16) << b
			<< std::setw(20) << c
			<< std::setw(16) << d << std::endl;
			break;
	default:
			std::cerr << "Unexpected call-data...\n";
			break;
	}

	va_end(args);
	 
	n++;
	if (n % (N_VAR % 5) == 0)
		count_color++;
}

// функция обработки завешения работы
void exitProgram();

void getData(double& x, const char* str, int call) {
	std::cout << "Введите значение " << str << "...\n";
	std::cin >> x;
	switch (call) {
	case 1:
		if (std::cin.fail() || x <= 0) { // задаемм значение eps
			system("cls");
			std::cout << "Значение " << str << " задано неверно (Укажите числовое значение большее 0)\n";
			exitProgram();
			getData(x, str, call);
		}
		return;
	case 2:
		if (std::cin.fail()) { // задайм значение x0 и x_ideal
			system("cls");
			std::cout << "Значение " << str << " задано неверно (Укажите числовое значение)\n";
			exitProgram();
			getData(x, str, call);
		}
		return;
	case 3:
		if (std::cin.fail() || x0 > x) { // задаём значение x1
			system("cls");
			std::cout << "Значение " << str << " задано неверно (Укажите числовое значение больше " << x0 << ")\n";
			exitProgram();
			getData(x, str, call);
		}
		return;
	case 4:
		if (std::cin.fail() || x <= 0 || x >= x1 - x0) { // задаём значение dx (шага таблицы)
			system("cls");
			std::cout << "Значение " << str << " задано неверно (Укажите числовое значение больше нуля и меньше, чем разность x0 и x1)\n";
			exitProgram();
			getData(x, str, call);
		}
	}
}

int main() {
	setlocale(LC_ALL, "RU"); // переменнные параметры, вывод, проверить ввод
	// Подключение к библиотеке
	HINSTANCE mydll = LoadLibraryA("C:/Users/egor-/OneDrive/Desktop/прога/MyLibrary/x64/Debug/MyLibrary.dll");
	
	// Проверка библиотеки на "подмен"
	if (mydll == nullptr) {
		std::cerr << "Failed to load MyLibrary.dll" << std::endl;
		return 1;
	}

	typedef char* (*FName)();
	FName Name = (FName)GetProcAddress(mydll, "FName");

	if (Name == nullptr) {
		std::cerr << "Failed to import FName from DLL" << std::endl;
		return 1;
	}

	typedef int* (*color)();
	color get_color = (color)GetProcAddress(mydll, "GetColors");

	if (get_color == nullptr) {
		std::cerr << "Failed to import GetColors from DLL" << std::endl;
		return 1;
	}

	typedef void(*s)(double, double, double, int, void(*)(int, ...));
	s get_str = (s)GetProcAddress(mydll, "myf_5");

	if (get_str == nullptr) {
		std::cerr << "Failed to import myf_5 from DLL" << std::endl;
		return 1;
	}

	typedef char* (*LN)();
	LN LastName = (LN)GetProcAddress(mydll, "LastName");
	if (LastName == nullptr) {
		std::cerr << "Failed to import LastName from DLL" << std::endl;
		return 1;
	}

	getData(eps, "эпсилон", 1);
	getData(x0, "x0", 2);
	getData(x1, "x1", 3);
	getData(a, "а (Если не требуется - введите любое числовое значение)", 2);
	getData(dx, "dx", 4);
	text_color = get_color()[0];
	font_color = get_color()[1];

	std::cout << "Название функции: " << Name() << " Фамилия автора exe: Сыров. Фамилия автора DLL: " << LastName()
		<< std::endl << std::endl;

	std::cout << std::setw(6) << "x"
		<< std::setw(18) << "f(x)" << std::setw(19) << "F(x)"
		<< std::setw(18) << "delta" << std::endl;

	// Вычисляем значения строк таблицы
	for (double x = x0; x <= x1; x += dx)
		get_str(x, a, eps, 1, callbackFunction);

	std::cout << std::endl << std::endl;

	double x_ideal; getData(x_ideal, "x_ideal", 2);

	std::cout << std::setw(8) << "eps"
		<< std::setw(15) << 'f' << std::setw(20) << 'F'
		<< std::setw(17) << "delta" << std::endl;
	

	for (long double e = 0.1; e >= 0.0000001; e /= 10)
		get_str(x_ideal, a, e, 2, callbackFunction);


	// Освобожение рессурсов библиотеки
	FreeLibrary(mydll);

	return 0;
}

void exitProgram() {
	std::cin.clear();
	std::cin.ignore(1000, '\n');
	std::string ans;
	std::cout << "Введите - exit, для завершения работы или введите любую другую последовательность для продолжения...\n"; std::cin >> ans;
	if (ans == "exit") {
		system("cls");
		std::cout << "Завершение работы...\n";
		exit(0);
	}
	else {
		system("cls");
		return;
	}
}
