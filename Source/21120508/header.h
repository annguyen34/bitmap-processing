#pragma pack(1)
#pragma once

#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

//bitmap header: 14bytes 2+4+2+2+4
struct bitmap_header {
	int16_t signature; // Định dạng file là bitmap, 2 kí tự BM
	int32_t size; // Kích thước file
	int16_t reserved1;
	int16_t reserved2;
	int32_t dataoffset; // Vị trí bắt đầu của phần lưu dữ liệu điểm ảnh
};

// bitmap information: 40 bytes
struct bitmap_DIB {
	int32_t DIB_size; // Kích thước phần DIB
	int32_t width;  // Số pixel theo chiều rộng
	int32_t height;  // Số pixel theo chiều dài
	int16_t planes;    // Số lớp màu
	int16_t bpp;  // bit per pixel
	 /*Loại nén 4bytes, kích thước phần dữ liệu điểm ảnh 4bytes
	, độ phần giải phương dọc 4bytes, phương ngang 4bytes*/
	int32_t compression;
	int32_t image_size;
	int32_t x;
	int32_t y;
	int32_t color_used;  //  Số màu trong bảng màu
	int32_t important_color;  // Số màu quan trọng
};

struct Pix32 {
	unsigned char A;
	unsigned char G;
	unsigned char B;
	unsigned char R;
};

// Image Picture
struct Picture {
	bitmap_header header;
	bitmap_DIB DIB;
	char* temp;
	Pix32** data;
};
