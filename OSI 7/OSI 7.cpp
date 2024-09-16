#include "iostream"
#include "windows.h"
#include "tchar.h"
#include "ShellAPI.h"
#include <fstream>
#include <string>
#include <stdio.h>
#include <chrono>//подключаем библиотеку для времени
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))//через это будем получать значения X
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))//через это будем получать значения Y
using namespace std;

TCHAR textString[] = _T("This is my message");
int colorRGB[3] = { 255,0,100 };//для цветов
int const Max = 15;//максимальный размер массива(игрового поля), но больше 10 не рекомендуется
int Massx[Max];//для записи разделения экрана по X
int Massy[Max];//для записи разделения экрана по Y
int(*MassXY)[Max];//создаем C массив
//int MassXY[Max][Max];//для яйчеек
int res[50];
int* step = 0;//количество ходов
int N = 3;//переменная для игрового поля
int N1 = 0;
int sizeX = 320;
int sizeY = 240;
int ch;
int r = 0, g = 0, b = 255;
TCHAR clsName[] = _T("Dota2(Всё ещё бета)");
COLORREF Grid = RGB(colorRGB[0], colorRGB[1], colorRGB[2]);
COLORREF RGB = RGB(0, 0, 255);
HBRUSH brush = CreateSolidBrush(RGB(r, g, b));
char list[50];
HANDLE hMapFile, hMapFile1, hMapFile2, hMapseml, hMapsemr, hMapStep, hMutex, hThread;//для отображения в память
int* instances = 0;//используем для того, чтобы подсчитывать кол-во экземпляров
int* sides;//используем как массив для сторон игроков
typedef int semaphore;//определяем семафор
semaphore* mleft = 0;//
semaphore* mright = 0;//
int side_player1, side_player2;//нужно для сторон игроков
int id_player = 0;//нужно для привязки игроков и зрителей
static int x, y;
bool ThreadP = false;
float _dtTime;
chrono::steady_clock::time_point _clock;
RECT rect;


int choice(int ch) {
	ifstream f;
	if (ch == 1) {
		f.open("OSI4.INI");
		if (f) {
			f >> sizeX;
			f >> sizeY;
			f >> colorRGB[0];
			f >> colorRGB[1];
			f >> colorRGB[2];
			f >> r;
			f >> g;
			f >> b;
			if (N1 == 1) {
				f >> N;
			}
			return sizeX, sizeY;
		}
		else {
			return 3;
		}
		f.close();
	}
	else {
		if (ch == 0) {
			return 0;
		}
	}
	if (ch == 2) {
		HANDLE hFile = CreateFile
		(
			TEXT("OSI4.INI"),                          //имя файла
			GENERIC_READ | GENERIC_WRITE,   //режим доступа, разрешены и чтение и запись
			FILE_SHARE_READ | FILE_SHARE_WRITE,  //совместный доступ
			NULL,                                         //дескриптор не может быть унаследован
			OPEN_ALWAYS,                            //открываем файл, либо создаем и открываем
			FILE_ATTRIBUTE_NORMAL,             //обычный файл
			NULL                                          //дескр.шаблона файла
		);
		HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, L"Testmap");
		LPVOID hViewMap = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);//соединяем память с файлом
		char* buff = (char*)hViewMap;
		if (buff != NULL) {//переводим из char в int
			char* v, * p = strtok_s(buff, " ", &v);
			for (int i = 0; p; ++i) {
				res[i] = atoi(p);
				p = strtok_s(0, " ", &v);
				cout << res[i] << endl;
			}
			sizeX = res[0];
			sizeY = res[1];
			colorRGB[0] = res[2];
			colorRGB[1] = res[3];
			colorRGB[2] = res[4];
			r = res[5];
			g = res[6];
			b = res[7];
			if (N1 == 1) {
				N = res[8];
			}
		}
		UnmapViewOfFile(hViewMap);
		CloseHandle(hFileMap);
		CloseHandle(hFile);
	}
	if (ch == 3) {
		HANDLE hFile = CreateFile
		(
			TEXT("OSI4.INI"),                          //имя файла
			GENERIC_READ | GENERIC_WRITE,   //режим доступа, разрешены и чтение и запись
			FILE_SHARE_READ | FILE_SHARE_WRITE,  //совместный доступ
			NULL,                                         //дескриптор не может быть унаследован
			OPEN_EXISTING,                            //открываем файл, либо создаем и открываем
			FILE_ATTRIBUTE_NORMAL,             //обычный файл
			NULL                                          //дескр.шаблона файла
		);
		if (hFile != INVALID_HANDLE_VALUE) {//проверяем открыт-ли файл
			const int BUFFER_SIZE = 1024;
			char buffer[1024] = { 0 };
			DWORD bytesRead;
			ReadFile(hFile, buffer, BUFFER_SIZE, &bytesRead, NULL);
			char* v, * p = strtok_s(buffer, " ", &v);//переводим из char в int
			for (int i = 0; p; ++i) {
				res[i] = atoi(p);
				p = strtok_s(0, " ", &v);
				cout << res[i] << endl;
			}
			sizeX = res[0];
			sizeY = res[1];
			colorRGB[0] = res[2];
			colorRGB[1] = res[3];
			colorRGB[2] = res[4];
			r = res[5];
			g = res[6];
			b = res[7];
			if (N1 == 1) {
				N = res[8];
			}
		}
		CloseHandle(hFile);
	}
	if (ch == 4) {
		FILE* ptrfile = fopen("OSI4.INI", "a+");
		if (ptrfile) {
			size_t readed = fread(list, sizeof(char), 50, ptrfile);
			if (readed > 0) {//проверяем на длину файла
				char* v, * p = strtok_s(list, " ", &v);//переводим из char в int
				for (int i = 0; p; ++i) {
					res[i] = atoi(p);
					p = strtok_s(0, " ", &v);
					cout << res[i] << endl;
				}
				sizeX = res[0];
				sizeY = res[1];
				colorRGB[0] = res[2];
				colorRGB[1] = res[3];
				colorRGB[2] = res[4];
				r = res[5];
				g = res[6];
				b = res[7];
				if (N1 == 1) {
					N = res[8];
				}
			}
		}
		fclose(ptrfile);
	}
}

int win() {
	//Проверка на вертикаль
	int count1 = 0, count2 = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (MassXY[i][j] == 1) {
				count1++;
				if (count1 == N) {
					return 1;
				}
			}
		}
		count1 = 0;
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (MassXY[i][j] == 2) {
				count2++;
				if (count2 == N) {
					return 2;
				}
			}
		}
		count2 = 0;
	}
	//Проверка для горизонтали
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (MassXY[j][i] == 1) {
				count1++;
				if (count1 == N) {
					return 1;
				}
			}
		}
		count1 = 0;
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (MassXY[j][i] == 2) {
				count2++;
				if (count2 == N) {
					return 2;
				}
			}
		}
		count2 = 0;
	}
	//Главная диагональ
	for (int i = 0; i < N; i++) {

		if (MassXY[i][i] == 1) {
			count1++;
			if (count1 == N) {
				return 1;
			}
		}
	}
	count1 = 0;

	for (int i = 0; i < N; i++) {
		if (MassXY[i][i] == 2) {
			count2++;
			if (count2 == N) {
				return 2;
			}
		}
	}
	count2 = 0;

	//Побочная диагональ
	for (int i = 0; i < N; i++) {

		if (MassXY[i][N - 1 - i] == 1) {
			count1++;
			if (count1 == N) {
				return 1;
			}
		}
	}
	count1 = 0;

	for (int i = 0; i < N; i++) {
		if (MassXY[i][N - 1 - i] == 2) {
			count2++;
			if (count2 == N) {
				return 2;
			}
		}
	}
	count2 = 0;

	if (*step == N * N) {
		return 3;
	}
	return 0;

}

void RGBWHEEL(WPARAM wParam) {//функция для плавного изменения цвета
	int wheel;
	if (int(wParam) < abs(int(wParam))) {
		wheel = -1;
	}
	else {
		wheel = 1;
	}
	int& r = colorRGB[0];
	int& g = colorRGB[1];
	int& b = colorRGB[2];
	if ((r == 255) && (g == 255) && (b == 0)) {
		r = 0;
		g = 0;
		b = 256;
	}
	if ((r == 0) && (g == 0) && (b == 255)) {
		r = 256;
		g = 256;
		b = 0;
	}
	if ((r >= 0) && (g >= 0) && (b >= 0)) {
		r += wheel * 5;
		g += wheel * 5;
		b -= wheel * 5;
	}
	if (colorRGB[0] < 0) {
		colorRGB[0] = 0;
	}
	if (colorRGB[1] < 0) {
		colorRGB[1] = 0;
	}
	if (colorRGB[2] < 0) {
		colorRGB[2] = 0;
	}
	if (colorRGB[0] > 255) {
		colorRGB[0] = 255;
	}
	if (colorRGB[1] > 255) {
		colorRGB[1] = 255;
	}
	if (colorRGB[2] > 255) {
		colorRGB[2] = 255;
	}
}

BOOL Line(HDC hdc, int x1, int y1, int x2, int y2)//функция для создания линий, используем для рисования сетки и крестиков
{
	MoveToEx(hdc, x1, y1, NULL);
	return LineTo(hdc, x2, y2);
}

DWORD WINAPI Paint(void* hWnd)//функция для отрисовки
{
	float timer = 0;//таймер обновления экрана
	_clock = chrono::steady_clock::now();//запоминаем время
	HWND hwnd = (HWND)hWnd;
	while (true) {
		if (ThreadP == true) {
			WaitForSingleObject(hMutex, INFINITE);//ждем пока разблокируется мьютекс, чтобы поток не закрывался
		}
		GetClientRect(hwnd, &rect);//извлекаем координаты
		x = rect.right - rect.left;
		y = rect.bottom - rect.top;
		HDC hdc1 = GetDC(hwnd);//используем для рисования
		_dtTime = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - _clock).count() / 1000.f;//время между итерациями цикла
		_clock = chrono::steady_clock::now();//запоминаем время
		if (timer < 2.f) {
			timer += _dtTime;//записываем время в таймер
		}
		else {//если прошло две секунды, то меняем цвет
			RGB = RGB(rand() % 255, rand() % 255, rand() % 255);
			brush = CreateSolidBrush(RGB);//меняем цвет фона на случайный
			SelectObject(hdc1, brush);
			Rectangle(hdc1, rect.left, rect.top, rect.right, rect.bottom);//отрисовываем цвет
			DeleteObject(brush);
			timer = 0;//обнуляем таймер
		}
		COLORREF Grid = RGB(colorRGB[0], colorRGB[1], colorRGB[2]);//выбираем цвет фона
		HPEN Pen1 = CreatePen(PS_SOLID, (x * 1. + y * 1.) / 100, Grid);//кисть для сетки
		SelectObject(hdc1, Pen1);//выбираем кисть
		for (int i = 1; i < N; i++) {//рисуем сетку
			int LineX = x * i / N;
			int LineY = y * i / N;
			Massx[i - 1] = LineX;//записываем разделение экрана по X
			Massy[i - 1] = LineY;//записываем разделение экрана по Y
			if (i == N - 1) {
				Massx[i] = x;
				Massy[i] = y;
			}
			Line(hdc1, LineX, 0, LineX, y);//рисуем с разделением по X
			Line(hdc1, 0, LineY, x, LineY);//рисуем с разделением по Y

		}
		for (int m = 0; m < N; m++)//проходим по ячейкам поля
		{
			for (int n = 0; n < N; n++)
			{
				if (MassXY[m][n] == 1)//рисуем круг
				{
					int cenX = m * Massx[0] + Massx[0] / 2; //находим середину нужной клетки
					int cenY = n * Massy[0] + Massy[0] / 2;
					int rad = min(Massx[0], Massy[0]) / 2; //высчитываем радиус для нолика
					HPEN circlePen = CreatePen(PS_SOLID, (x * 1.0 + y * 1.0) / 200, RGB(0, 255, 0)); //кисть для нолика
					SelectObject(hdc1, circlePen);//создаю круг
					HBRUSH brush = CreateSolidBrush(RGB);
					SelectObject(hdc1, brush);//закрашиваю внутри него фон
					Ellipse(hdc1, cenX - rad, cenY - rad, cenX + rad, cenY + rad);//рисуем нолик
					DeleteObject(brush);
					DeleteObject(circlePen);
				}
				else if (MassXY[m][n] == 2)//рисуем крестик
				{
					int x1 = m * Massx[0];//начальные координаты
					int y1 = n * Massy[0];
					int x2 = (m + 1) * Massx[0];//конечные координаты
					int y2 = (n + 1) * Massy[0];
					HPEN crossPen = CreatePen(PS_SOLID, (x * 1.0 + y * 1.0) / 300, RGB(255, 127, 80));//кисть крестика
					SelectObject(hdc1, crossPen);
					Line(hdc1, x1, y1, x2, y2);//первая линия
					Line(hdc1, x1, y2, x2, y1);//вторая линия
					DeleteObject(crossPen);
				}
			}
		}
		Sleep(20);
		DeleteObject(Pen1);
		DeleteObject(brush);
		ReleaseDC(hwnd, hdc1);
		ReleaseMutex(hMutex);//освобождаем от мьютекса
	}
	ExitThread(0);//закрываем поток
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

	case WM_DESTROY:
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);//нужна для получения окна
		int x1 = rect.right - rect.left;//размер окна по x
		int y1 = rect.bottom - rect.top;//размер окна по y
		if ((ch == 0) or (ch == 1)) {
			ofstream f;
			f.open("OSI4.INI");
			if (f) {
				f << x1 << " ";
				f << y1 << " ";
				f << colorRGB[0] << " ";
				f << colorRGB[1] << " ";
				f << colorRGB[2] << " ";
				f << r << " ";
				f << g << " ";
				f << b << " ";
				f << N;
			}
		}
		if (ch == 2) {
			PVOID pvFile;
			char* buff;
			HANDLE hfile = CreateFile(TEXT("OSI4.INI"), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);//создаю область памяти

			HANDLE hfilemap = CreateFileMapping(hfile, NULL, PAGE_READWRITE, 0, 104857600, NULL);

			//for(DWORD i = 0; i<104857600; i+=1024)
			//{
			pvFile = MapViewOfFile(hfilemap, FILE_MAP_WRITE, 0, 0, 1024);//связываю область памяти с программой

			string str0 = to_string(x1);
			string str1 = to_string(y1);
			string str2 = to_string(colorRGB[0]);
			string str3 = to_string(colorRGB[1]);
			string str4 = to_string(colorRGB[2]);
			string str5 = to_string(r);
			string str6 = to_string(g);
			string str7 = to_string(b);
			string str8 = to_string(N);
			string str = str0 + ' ' + str1 + ' ' + str2 + ' ' + str3 + ' ' + str4 + ' ' + str5 + ' ' + str6 + ' ' + str7 + ' ' + str8;//объединяю все данные

			buff = (char*)pvFile;
			strcpy(buff, str.c_str());//вношу все данные в файл

			//}
			UnmapViewOfFile(pvFile);
			CloseHandle(hfilemap);
			CloseHandle(hfile);
		}
		if (ch == 3) {
			HANDLE heFile = CreateFile
			(
				TEXT("OSI4.INI"),                          //имя файла
				GENERIC_READ | GENERIC_WRITE,   //режим доступа, разрешены и чтение и запись
				FILE_SHARE_READ | FILE_SHARE_WRITE,  //совместный доступ
				NULL,                                         //дескриптор не может быть унаследован
				OPEN_ALWAYS,                            //открываем файл, либо создаем и открываем
				FILE_ATTRIBUTE_NORMAL,             //обычный файл
				NULL
			);
			string str0 = to_string(x1);
			string str1 = to_string(y1);
			string str2 = to_string(colorRGB[0]);
			string str3 = to_string(colorRGB[1]);
			string str4 = to_string(colorRGB[2]);
			string str5 = to_string(r);
			string str6 = to_string(g);
			string str7 = to_string(b);
			string str8 = to_string(N);
			string str = str0 + ' ' + str1 + ' ' + str2 + ' ' + str3 + ' ' + str4 + ' ' + str5 + ' ' + str6 + ' ' + str7 + ' ' + str8;//объединяю все данные
			DWORD bytesWritten;
			WriteFile(heFile, str.c_str(), str.length(), &bytesWritten, NULL);//вношу все данные в файл
		}
		if (ch == 4) {
			FILE* ptrfile = fopen("OSI4.INI", "w");
			string str0 = to_string(x1);
			string str1 = to_string(y1);
			string str2 = to_string(colorRGB[0]);
			string str3 = to_string(colorRGB[1]);
			string str4 = to_string(colorRGB[2]);
			string str5 = to_string(r);
			string str6 = to_string(g);
			string str7 = to_string(b);
			string str8 = to_string(N);
			string str = str0 + ' ' + str1 + ' ' + str2 + ' ' + str3 + ' ' + str4 + ' ' + str5 + ' ' + str6 + ' ' + str7 + ' ' + str8;//объединяю все данные
			if (ptrfile) {
				fwrite(str.data(), sizeof(char), str.size(), ptrfile);//вношу все данные в файл
			}
		}
		PostQuitMessage(0);
		return 0;
	}

	case WM_KEYDOWN:
	{
		if (wParam == VK_RETURN) {
			r = rand() % 256;
			g = rand() % 256;
			b = rand() % 256;
			brush = CreateSolidBrush(RGB(r, g, b));//меняем цвет фона на случайный
			ULONG_PTR HB = SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);
			InvalidateRect(hWnd, NULL, TRUE);//перерисовываем окно
			DeleteObject(HGDIOBJ(HB));
			return 0;

		}
		else {
			if (wParam == VK_ESCAPE) {
				PostQuitMessage(0);
				break;
			}
			else {
				if (GetKeyState('Q') && wParam == VK_CONTROL) {
					PostQuitMessage(0);
					break;
				}
				else {
					if (wParam == VK_SHIFT && GetKeyState('C')) {
						ShellExecuteA(hWnd, NULL, "notepad.exe", NULL, NULL, SW_RESTORE);
					}
					else {
						if (GetKeyState(VK_SPACE))//при нажатии пробела
						{
							ThreadP = !ThreadP;//изменяем булевую функцию
							if (ThreadP != false) {
								WaitForSingleObject(hMutex, INFINITE);//задаем мьютекс
							}
							else {
								ReleaseMutex(hMutex);//освобождаем от мьютекса
							}

						}
						else {
							if (wParam == 0x31)
							{
								SetThreadPriority(hThread, THREAD_PRIORITY_IDLE);//низший
							}
							if (wParam == 0x32)
							{
								SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);//средний
							}
							if (wParam == 0x33)
							{
								SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);//самый высокий
							}
							cout << GetThreadPriority(hThread);
						}
					}
				}
			}
		}
		return 0;
	}

	//case WM_PAINT:
	//{
	//	PAINTSTRUCT ps;
	//	HDC hdc = BeginPaint(hWnd, &ps);
	//	COLORREF Grid = RGB(colorRGB[0], colorRGB[1], colorRGB[2]);
	//	HPEN Pen = CreatePen(PS_SOLID, (x * 1. + y * 1.) / 100, Grid);//кисть для сетки
	//	SelectObject(hdc, Pen);
	//	for (int i = 1; i < N; i++) {
	//		int LineX = x * i / N;
	//		int LineY = y * i / N;
	//		Massx[i - 1] = LineX;//записываем разделение экрана по X
	//		Massy[i - 1] = LineY;//записываем разделение экрана по Y
	//		if (i == N - 1) {
	//			Massx[i] = x;
	//			Massy[i] = y;
	//		}
	//		Line(hdc, LineX, 0, LineX, y);//рисуем с разделением по X
	//		Line(hdc, 0, LineY, x, LineY);//рисуем с разделением по Y

	//	}
	//	DeleteObject(Pen);

	//	int GridX = x / 3;
	//	int GridY = y / 3;

		//for (int m = 0; m < N; m++)//проходим по ячейкам поля
		//{
		//	for (int n = 0; n < N; n++)
		//	{
		//		if (MassXY[m][n] == 1)//рисуем круг
		//		{
		//			int cenX = m * Massx[0] + Massx[0] / 2; //находим середину нужной клетки
		//			int cenY = n * Massy[0] + Massy[0] / 2;
		//			int rad = min(Massx[0], Massy[0]) / 2; //высчитываем радиус для нолика
		//			HPEN circlePen = CreatePen(PS_SOLID, (x * 1.0 + y * 1.0) / 200, RGB(0, 255, 0)); //кисть для нолика
		//			SelectObject(hdc, circlePen);//создаю круг
		//			HBRUSH brush = CreateSolidBrush(RGB(r, g, b));
		//			SelectObject(hdc, brush);//закрашиваю внутри него фон
		//			Ellipse(hdc, cenX - rad, cenY - rad, cenX + rad, cenY + rad);//рисуем нолик
		//			DeleteObject(circlePen);
		//		}
		//		else if (MassXY[m][n] == 2)//рисуем крестик
		//		{
		//			int x1 = m * Massx[0];//начальные координаты
		//			int y1 = n * Massy[0];
		//			int x2 = (m + 1) * Massx[0];//конечные координаты
		//			int y2 = (n + 1) * Massy[0];
		//			HPEN crossPen = CreatePen(PS_SOLID, (x * 1.0 + y * 1.0) / 300, RGB(255, 127, 80));//кисть крестика
		//			SelectObject(hdc, crossPen);
		//			Line(hdc, x1, y1, x2, y2);//первая линия
		//			Line(hdc, x1, y2, x2, y1);//вторая линия
		//			DeleteObject(crossPen);
		//		}
		//	}
	//	}
	//	DeleteObject(Pen);
	//	EndPaint(hWnd, &ps);
	//	return 0;
	//}

	case WM_SIZE:
	{
		HDC hdc2 = GetDC(hWnd);//используем для рисования
		GetClientRect(hWnd, &rect);//получаем данные
		x = rect.right - rect.left;
		y = rect.bottom - rect.top;
		HBRUSH Brush = CreateSolidBrush(RGB);//задаем кисть
		SelectObject(hdc2, Brush);//выбираем кисть
		Rectangle(hdc2, rect.left, rect.top, rect.right, rect.bottom);//красим
		InvalidateRect(hWnd, NULL, TRUE);//перерисовываем окно при изменении размеров
		ReleaseDC(hWnd, hdc2);//освобождаем память
		DeleteObject(Brush);//удаляем объект
		return 0;
	}

	case WM_CREATE:
	{
		//if (choice(ch) == 5) {
			//MessageBox(hWnd, _T("Сохранений нет.Применены стандартные настройки игры"), _T("Предупреждение"), MB_OK);
		//}

		//отображение в память
		if (*instances < 5) {//если меньше 5 людей
			hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, Max * Max, clsName);
			//отображаем в памяти двумерный C массив
			MassXY = (int(*)[Max])MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, Max * Max);
			(*instances) += 1;
		}
		else {//если больше
			MessageBox(hWnd, _T("Вы не можете войти "), _T("Максимум зрителей"), MB_OK);
			SendMessage(hWnd, WM_DESTROY, 0, 0);//закрываем окно
		}
		//HBRUSH brush = CreateSolidBrush(RGB(r, g, b));
		//SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, LONG(brush));
		return 0;
	}

	case WM_LBUTTONDOWN: {
		if (id_player <= 2) {//если id игрока меньше 2, то это игрок
			if ((id_player == 1 and sides[0] == 1 and *mleft == 0) or (id_player == 2 and sides[1] == 1 and *mleft == 0)) {//проверяем, чтобы ходил тот игрок
				//or (*counter % 2 ==1 and side_2p == 1) or (*counter % 2 == 1 and side_2p == 2)
				POINT pos;
				int xx = 0;
				int yy = 0;
				int xPos = GET_X_LPARAM(lParam);//получаем значения курсора по X
				int yPos = GET_Y_LPARAM(lParam);//получаем значения курсора по Y
				for (int i = 0; i < N; i++) {
					if (xPos > 0 && xPos < Massx[0]) {//если позиция курсора до 1 линии по X
						xx = 0;
					}
					else {
						if (i > 0) {
							if ((xPos > Massx[i - 1] && xPos < Massx[i])) {//если позиция курсора после 1 линии по X
								xx = i;
							}
						}
					}
				}

				for (int j = 0; j < N; j++) {
					if (yPos > 0 && yPos < Massy[0]) {//если позиция курсора до 1 линии по Y
						yy = 0;
					}
					else {
						if (j > 0) {
							if ((yPos > Massy[j - 1] && yPos < Massy[j])) {//если позиция курсора после 1 линии по Y
								yy = j;
							}
						}
					}
				}
				if (MassXY[xx][yy] == 0) {
					MassXY[xx][yy] = 1;
					*mleft += 1;
					*mright -= 1;
					InvalidateRect(NULL, NULL, TRUE);//задаем значения NULL для отрисовки во всех окнах
					*step += 1;
					int endgame = win();
					if (endgame == 1) {
						system("cls");
						cout << "Game over" << endl << "Победил нолик";
						HWND wind = FindWindow(clsName, L"Dota2(Всё ещё бета)");//получаем информацию о всех окнах
						SendMessage(wind, WM_CLOSE, 0, 0);//закрываем все окна
					}
					else {
						if (endgame == 3) {
							system("cls");
							cout << "Game over" << endl << "Ничья";
							HWND wind = FindWindow(clsName, L"Dota2(Всё ещё бета)");//получаем информацию о всех окнах
							SendMessage(wind, WM_CLOSE, 0, 0);//закрываем все окна
						}
					}
				}
			}
			else if ((*mleft == 1 and id_player == 1 and sides[0] == 2) or (*mleft == 1 and id_player == 2 and sides[1] == 2)) {//проверяем ходит-ли он за свою сторону
				MessageBox(hWnd, _T("Вы пытаетесь играть не за свою сторону"), _T("Вы играете за крестики"), MB_OK);
				break;
			}
			else {
				MessageBox(hWnd, _T("Не ваш ход"), _T("Сейчас выбирает другой пользователь"), MB_OK);//если пытается сходить не в свой ход
			}
		}
		else {
			MessageBox(hWnd, _T("Вы зритель"), _T("Вы не можете играть"), MB_OK);//ошибка для зрителей
		}
		return 0;
	}

	case WM_RBUTTONDOWN: {
		if (id_player <= 2) {//если id меньше 2, то это игрок
			if ((id_player == 1 and sides[0] == 2 and *mright == 0) or (id_player == 2 and sides[1] == 2 and *mright == 0)) {//проверяем, чтобы ходил тот игрок
				//count_player - игрок играет
				//sides - сторона 0 - для первого 1 - для второго
				//if (*mleft == 1) {
				POINT pos;
				int xx = 0;
				int yy = 0;
				int xPos = GET_X_LPARAM(lParam);
				int yPos = GET_Y_LPARAM(lParam);
				for (int i = 0; i < N; i++) {
					if (xPos > 0 && xPos < Massx[0]) {
						xx = 0;
					}
					else {
						if (i > 0) {
							if ((xPos > Massx[i - 1] && xPos < Massx[i])) {
								xx = i;
							}
						}
					}
				}

				for (int j = 0; j < N; j++) {
					if (yPos > 0 && yPos < Massy[0]) {
						yy = 0;
					}
					else {
						if (j > 0) {
							if ((yPos > Massy[j - 1] && yPos < Massy[j])) {
								yy = j;
							}
						}
					}
				}
				if (MassXY[xx][yy] == 0) {
					MassXY[xx][yy] = 2;
					*mleft -= 1;
					*mright += 1;
					InvalidateRect(NULL, NULL, TRUE);//задаем значения NULL для отрисовки во всех окнах
					*step += 1;
					int endgame = win();
					if (endgame == 2) {
						system("cls");
						cout << "Game over" << endl << "Победил крестик";
						HWND wind = FindWindow(clsName, L"Dota2(Всё ещё бета)");//получаем информацию о всех окнах
						SendMessage(wind, WM_CLOSE, 0, 0);//закрываем все окна
					}
					else {
						if (endgame == 3) {
							system("cls");
							cout << "Game over" << endl << "Ничья";
							HWND wind = FindWindow(clsName, L"Dota2(Всё ещё бета)");//получаем информацию о всех окнах
							SendMessage(wind, WM_CLOSE, 0, 0);//закрываем все окна
						}
					}
				}
			}
			else if ((*mright == 1 and id_player == 1 and sides[0] == 1) or (*mright == 1 and id_player == 2 and sides[1] == 1)) {
				MessageBox(hWnd, _T("Вы пытаетесь играть не за свою сторону"), _T("Вы играете за нолики"), MB_OK);
				break;
			}
			else {
				MessageBox(hWnd, _T("Не ваш ход"), _T("Сейчас выбирает другой пользователь"), MB_OK);
			}
		}
		else {
			MessageBox(hWnd, _T("Вы зритель"), _T("Вы не можете играть"), MB_OK);
		}
		return 0;
	}

	case WM_MOUSEWHEEL:
	{
		RGBWHEEL(wParam);
		cout << colorRGB[0] << " " << colorRGB[1] << " " << colorRGB[2] << endl;
		Grid = RGB(colorRGB[0], colorRGB[1], colorRGB[2]);//меняем цвет
		InvalidateRect(hWnd, NULL, TRUE);//перерисовываем окно с новым цветом
		return 0;
	}

	case WM_MOUSEMOVE: {
		int Mx = lParam & 0xFFFF;
		int My = (lParam >> 16) & 0xFFFF;
		//cout << wParam << " " << Mx << " " << My << endl;
		return 0;
	}

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int main()
{
	hMutex = CreateMutex(NULL, NULL, NULL);
	hMapFile1 = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), _T("Hello"));//отображение в память для подсчета экземпляров
	hMapFile2 = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4, _T("Hi"));//отображение в память для сторон
	hMapseml = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(semaphore), L"sem");//отображение в память для левой кнопки мышки
	hMapsemr = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(semaphore), L"rig");//отображение в память для правой кнопки мышки
	hMapStep = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), L"st");//отображение в память ходов
	step = (int*)MapViewOfFile(hMapStep, FILE_MAP_ALL_ACCESS, 0, 0, 2);//закидываем в память шаги
	sides = (int*)MapViewOfFile(hMapFile2, FILE_MAP_ALL_ACCESS, 0, 0, 2);//закидываем в память стороны
	mleft = (semaphore*)MapViewOfFile(hMapseml, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(semaphore));//закидываем в память семафор для левой кнопки 
	mright = (semaphore*)MapViewOfFile(hMapsemr, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(semaphore));//для правой
	//задаём отображение в память
	instances = (int*)MapViewOfFile(hMapFile1, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(int));//закидываем в память счетчик экземпляров
	setlocale(LC_ALL, "Russian");
	cout << "Продолжить игру или начать заново:" << endl << "0 - Начать игру заново" << endl << "1 - fstream" << endl << "2 - отображение файлов на память" << endl << "3 - Файловые функции WinAPI" << endl << "4 - Потоки ввода/вывода" << endl;
	cin >> ch;
	while (ch < 0 or ch>4) {
		cout << "Введите снова(1-4):";
		cin >> ch;
	}
	if (*instances == 0) {
		if (ch != 0) {
			cout << "Задать новый размер поля?" << endl << "Если сохраненного размера нет, то по умолчанию размер будет 3x3" << endl << "1 - Не задавать новый размер" << endl << "2 - Задать новый размер" << endl;
			cin >> N1;
		}
	}
	choice(ch);
	if (N1 == 2) {
		cout << "Введите размер поля(больше 10 не рекомендуется):";
		cin >> N;
	}
	if (*instances < 2) {//если игроки, то раздаем им то, какими сторонами они играют
		if (*instances == 0) {
			cout << "Выберите сторону - Нолики ходят первыми (1 - нолик, 2 - крестик): ";
			cin >> side_player1;
			if (side_player1 == 1) {
				side_player2 = 2;
			}
			else {
				side_player2 = 1;
			}
			sides[0] = side_player1;//присываиваем нолики или крестики для первого игрока
			sides[1] = side_player2;//для второго присваиваем то, что остается
			id_player = 1;//задаем первому игроку айди
			*mright += 1;//блокируем нажатие крестиков
		}
		else {
			id_player = 2;//задаем второму игроку айди
		}
	}
	else {//если не игрок, то зритель
		if (*instances == 2) {//задаем зрителю айди
			id_player = 3;
		}
		if (*instances == 3) {
			id_player = 4;
		}
		if (*instances == 4) {
			id_player = 5;
		}
	}
	if (sides[0] == 1) {
		cout << "Первый игрок играет за нолики, второй за крестики";
	}
	else {
		cout << "Первый игрок играет за нолики,второй игрок играет за нолики";
	}
	HINSTANCE hThis = GetModuleHandle(NULL);
	WNDCLASS winCl = { 0 };
	winCl.lpszClassName = clsName;
	winCl.hInstance = hThis;
	winCl.lpfnWndProc = WndProc;
	if (!RegisterClass(&winCl))
	{
		cout << "RegisterClass failed! Code: " << GetLastError() << endl;
		return 1;
	}
	HWND hWnd = CreateWindow(
		clsName,
		_T("Dota2(Всё ещё бета)"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		sizeX, sizeY,
		HWND_DESKTOP,
		NULL,
		hThis,
		NULL
	);
	ShowWindow(hWnd, SW_SHOW);
	hThread = CreateThread(NULL, NULL, Paint, hWnd, NULL, NULL);//создаём поток для рисования
	BOOL bOk;
	MSG msg;
	while ((bOk = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bOk < 0)
		{
			cout << "GetMassage failed! Code: " << GetLastError() << endl;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	CloseHandle(hThread);//закрываем поток
	UnmapViewOfFile(MassXY);//закрываем отображение массива на память
	CloseHandle(hMapFile);//закрываем отображение на память
	UnmapViewOfFile(mleft);
	UnmapViewOfFile(mright);
	CloseHandle(hMapseml);
	CloseHandle(hMapsemr);
	UnmapViewOfFile(sides);
	UnmapViewOfFile(instances);
	CloseHandle(hMapFile1);
	CloseHandle(hMapFile2);
	UnmapViewOfFile(step);
	CloseHandle(hMapStep);
	if (id_player <= 2) {
		HWND wind = FindWindow(clsName, L"Dota2(Всё ещё бета)");//получаем информацию о всех окнах
		SendMessage(wind, WM_CLOSE, 0, 0);//закрываем все окна
	}
	DestroyWindow(hWnd);

	UnregisterClass(clsName, hThis);
}