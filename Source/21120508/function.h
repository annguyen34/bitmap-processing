#include "header.h"
#pragma pack(1)
// Đọc file BMP
void readBMP(const char* filepath, Picture& img, char*& data, char*& colortable) {
	ifstream fin;
	fin.open(filepath, ios::binary);
	if (!fin.is_open()) {
		cout << "Can't open the file!";
		exit(0);
	}
	else
	{
		fin.read((char*)&(img.header), 14);
		cout << "Signature : " << img.header.signature << endl;
		cout << "File size: " << img.header.size << endl;
		cout << "Data off set: " << img.header.dataoffset << endl;
		cout << endl;
		fin.read((char*)&(img.DIB), 40);
		cout << "DIB Size: " << img.DIB.DIB_size << endl;
		cout << "BPP: " << img.DIB.bpp << endl;
		cout << "Width: " << img.DIB.width << endl;
		cout << "Height: " << img.DIB.height << endl;
		cout << "Size: " << img.DIB.image_size << " bytes" << endl;
		cout << "Planes: " << img.DIB.planes << endl;
		cout << "Color Used: " << img.DIB.color_used << endl;
		cout << "Important Color: " << img.DIB.important_color << endl;
		cout << "Compression: " << img.DIB.compression << endl;

		// Set compression về 0
		if (img.DIB.compression != 0) {
			img.DIB.compression = 0;
		}

		// Đọc phần dư 
		if (img.DIB.DIB_size > 40) {
			int d = img.DIB.DIB_size - 40;
			img.temp = new char[d];
			fin.read((char*)(img.temp), d);
		}
		// Nếu là file 8 bit đọc color table
		if (img.DIB.bpp == 8) {
			fin.read((char*)colortable, 1024);
		}

		// Tính toán _padding và kích thước cho mảng 1 chiều data
		int _padding = (4 - (img.DIB.width * (img.DIB.bpp / 8)) % 4) % 4;
		int _size = img.DIB.width * img.DIB.height * (img.DIB.bpp / 8) + _padding * img.DIB.height;
		data = new char[_size];
		// Đọc dữ liệu điểm ảnh dưới dạng mảng 1 chiều
		fin.read((char*)data, _size);
		fin.close();
	}
}

// Chuyển dữ liệu điểm ảnh vào trong mảng 2 chiều
void convert_data(Picture& img, char* data) {
	char* temp = data;
	int _padding = img.DIB.width % 4;
	// Cấp phát mảng động 2 chiều data
	img.data = new Pix32 * [img.DIB.height];
	for (int i = 0; i < img.DIB.height; i++)
		img.data[i] = new Pix32[img.DIB.width];

	// Chuyển dữ liệu vào
	if (img.DIB.bpp == 32 || img.DIB.bpp == 24) {
		for (int i = 0; i < img.DIB.height; i++) {
			for (int j = 0; j < img.DIB.width; j++) {
				if (img.DIB.bpp == 32) {
					img.data[i][j].A = *(temp++);
				}
				img.data[i][j].B = *(temp++);
				img.data[i][j].G = *(temp++);
				img.data[i][j].R = *(temp++);
			}
			temp += _padding;
		}
	}
	else {
		// Dành cho file 8 bit
		for (int i = 0; i < img.DIB.height; i++) {
			for (int j = 0; j < img.DIB.width; j++) {
				img.data[i][j].R = img.data[i][j].G = img.data[i][j].B = *(temp++);
			}
			temp += _padding;
		}
	}
}

// Ghi file BMP 24 32
void writeBMP(Picture img, char* data, const char* output) {
	ofstream fout;
	fout.open(output, ios::binary);
	//Ghi header
	fout.write((char*)&(img.header), 14);
	//Ghi DIB
	fout.write((char*)&(img.DIB), 40);
	//Ghi phần dư
	int d = (img.DIB.DIB_size) - 40;
	for (int i = 0; i < d; i++)
		fout.write((char*)&(img.temp[i]), sizeof(img.temp[i]));
	// Ghi dữ liệu điểm ảnh
	int _padding = (4 - (img.DIB.width * (img.DIB.bpp / 8)) % 4) % 4;
	int _size = img.DIB.width * img.DIB.height * (img.DIB.bpp / 8) + _padding * img.DIB.height;
	for (int i = 0; i < _size; i++)
		fout.write((char*)&(data[i]), sizeof(char));
	fout.close();
}

// Ghi file 8 bit ( with colortable )
void write8bitBMP(const char* fileoutput, Picture img, char* colortable, char* data) {
	ofstream fout;
	fout.open(fileoutput, ios::binary);
	// Ghi header 
	fout.write((char*)&img.header, 14);
	// Ghi DIB
	fout.write((char*)&img.DIB, 40);
	// Ghi colortable
	for (int i = 0; i < 1024; i++)
		fout.write((char*)&colortable[i], 1);
	int _padding = (4 - (img.DIB.width * (img.DIB.bpp / 8)) % 4) % 4;
	int _size = img.DIB.width * img.DIB.height * (img.DIB.bpp / 8) + _padding * img.DIB.height;
	// Ghi dữ liệu điểm ảnh
	for (int i = 0; i < _size; i++)
		fout.write((char*)&data[i], 1);
	fout.close();
}


// Chuyển thành ảnh 8 bit
Picture grayscale(Picture img, char*& data8bit) {
	Picture img8bit;
	img8bit.header = img.header;
	img8bit.DIB = img.DIB; 
	// Sửa data
	img8bit.DIB.bpp = 8; // Sửa lại bit per pixel 
	img8bit.DIB.color_used = 256; // Color used của file 8 bit = 256 
	int padding = (4 - (img8bit.DIB.width % 4)) % 4;
	img8bit.DIB.image_size = img8bit.DIB.width * img8bit.DIB.height + padding * img8bit.DIB.height; // Lúc này 1 pixel = 1 byte
	// Lúc này địa chỉ lưu dữ liệu điểm ảnh bắt đầu ở byte 1078 ( 14 byte header + 40 byte DIB + 1024 byte colortable ) 
	img8bit.header.dataoffset = 1078; 
	img8bit.DIB.DIB_size = 40;
	img.header.size = img8bit.DIB.image_size + img.header.dataoffset;
	// Khởi tạo mảng để lưu dữ liệu điểm ảnh
	data8bit = new char[img8bit.DIB.image_size];
	char* temp = data8bit;
	for (int i = 0; i < img.DIB.height; i++) {
		for (int j = 0; j < img.DIB.width; j++) {
			int16_t val = (img.data[i][j].R + img.data[i][j].G + img.data[i][j].B) / 3;
			*(temp++) = val;
		}
		temp += padding;
	}
	return img8bit;

}

// Tính trung bình điểm ảnh 
Pix32 avg(Pix32* a, int n) {
	Pix32 result = { 0 };
	int suma = 0, sumb = 0, sumg = 0, sumr = 0;
	for (int i = 0; i < n; i++) {
		suma += a[i].A;
		sumb += a[i].B;
		sumr += a[i].R;
		sumg += a[i].G;
	}
	result.A = suma / n;
	result.B = sumb / n;
	result.G = sumg / n;
	result.R = sumr / n;
	return result;
}

// Hàm zoom theo tỉ lệ s cho trước 
Picture zoom(const Picture src, int s, char*& newdata) {
	// Sao chép lại dữ liệu phần header và dib của ảnh gốc
	Picture newpic;
	newpic.header = src.header;
	newpic.DIB = src.DIB; 
	newpic.temp = new char[sizeof(src.temp)];
	memcpy(newpic.temp, src.temp, sizeof(src.temp));
	newpic.data = new Pix32 * [src.DIB.height / s + 1];
	for (int i = 0; i < (src.DIB.height / s + 1); i++)
		newpic.data[i] = new Pix32[src.DIB.width / s + 1];

	newpic.DIB.height = 0;
	newpic.DIB.width = 0;
	// Generate
	int counting = 0;
	// Khởi tạo
	for (int i = 0; i < src.DIB.height; i += s)
	{
		for (int j = 0; j < src.DIB.width; j += s)
		{
			// Khởi tạo mảng pixel mới để lưu dữ liểu điểm ảnh để tính trung bình
			int d = s * s;
			Pix32* calc = new Pix32[d];
			counting = 0; // Biến counting để đếm số lượng phần tử trong mảng mới calc
			for (int k = 0; k < s; k++) {
				for (int o = 0; o < s; o++) {
					if ((i + k) < src.DIB.height && (j + o) < src.DIB.width) {
						// Lưu các giá trị điểm ảnh vào mảng mới để đi tính trung bình các điểm ảnh
						if (src.DIB.bpp == 32) {
							calc[counting].A = src.data[i + k][j + o].A;
						}
						calc[counting].R = src.data[i + k][j + o].R;
						calc[counting].G = src.data[i + k][j + o].G;
						calc[counting].B = src.data[i + k][j + o].B;
						counting++;
					}
				}
			}
			// Giá trị 1 pixel trong mảng mới  = trung bình giá trị điểm ảnh của phần s x s pixel trong ảnh cũ
			newpic.data[newpic.DIB.height][newpic.DIB.width] = avg(calc, counting);
			delete[] calc;
			newpic.DIB.width++;
		}
		newpic.DIB.height++;
		newpic.DIB.width = 0;
	}

	// Khởi tạo kích thước và lưu dữ liệu vào ảnh mới 
	newpic.DIB.width = src.DIB.width / s + 1;
	newpic.DIB.height = src.DIB.height / s + 1;
	int _padding = (4 - (newpic.DIB.width * (newpic.DIB.bpp / 8)) % 4) % 4;
	int size = newpic.DIB.height * newpic.DIB.width * (newpic.DIB.bpp / 8) + _padding * (newpic.DIB.height);
	newdata = new char[size];
	char* temp = newdata;
	// Lưu dữ liệu điểm ảnh mới 
	for (int i = 0; i < newpic.DIB.height; i++)
	{
		for (int j = 0; j < newpic.DIB.width; j++) {
			if (newpic.DIB.bpp == 32) {
				*(temp++) = newpic.data[i][j].A;
			}
			*(temp++) = newpic.data[i][j].B;
			*(temp++) = newpic.data[i][j].G;
			*(temp++) = newpic.data[i][j].R;
		}
		for (int g = 0; g < _padding; g++)
			*(temp++) = 0;
	}
	// Tính toán lại kích thước bức ảnh
	newpic.header.size = newpic.header.dataoffset + size;
	newpic.DIB.image_size = size;
	return newpic;
}
// Đọc colortable từ file colortable.txt
void readcolortable(char*& colortable) {
	ifstream in;
	in.open("colortable.txt", ios::binary);
	in.read((char*)colortable, 1024);
	in.close();
}

// Destructor
void destructor(Picture& img, char*& data)
{
	delete[] data;
	data = NULL;
	if (img.DIB.bpp != 8) {
		delete[] img.temp;
		for (int i = 0; i < img.DIB.height; i++)
			delete[] img.data[i];
		delete[]  img.data;
	}
	img.data = NULL;
}
