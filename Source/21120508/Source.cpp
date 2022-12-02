#define _CRT_SECURE_NO_WARNINGS
#include "function.h"
#include "header.h"
#pragma pack (1)
int main(char argc, char* argv[]) {
	Picture img;
	Picture dest;
	bool check = false;
	char* data;
	char* newdata;
	char* colortable = new char[1024];
	readcolortable(colortable);
	readBMP(argv[1], img, data, colortable);
	convert_data(img, data);

	if (strcmp("-zoom", argv[2]) == 0) {
		int d = atoi(argv[4]);
		dest=zoom(img, d, newdata);
		writeBMP(dest, newdata,argv[3]);
		destructor(dest, newdata);
		check = true;
	}
	else
	if (strcmp("-conv", argv[2]) == 0)
	{
		dest = grayscale(img, newdata);
		write8bitBMP(argv[3], dest, colortable, newdata);
		destructor(dest, newdata);
		check = true;
	}
	if (!check) {
		cout << "Failed";
	}
	else
		cout << "Success";
	// Giải phóng bộ nhớ
	destructor(img, data);
	delete[] colortable;
}