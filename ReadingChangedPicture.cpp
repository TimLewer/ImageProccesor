#include "ReadingChangedPicture.h"
#include "structur.h"
#include <string>

ReadingChangegPicture::ReadingChangegPicture() : image_(nullptr) {
}

ReadingChangegPicture::ReadingChangegPicture(std::ifstream& input_file) : image_(nullptr) {
    input_file.read(reinterpret_cast<char*>(&bitmap_header_), sizeof(BITMAPFILEHEADER));
    input_file.read(reinterpret_cast<char*>(&dib_header_), sizeof(BITMAPINFOHEADER));
    Checking();

    int row_size = ((dib_header_.biWidth * 3 + 3) / 4) * 4;
    int data_size = row_size * dib_header_.biHeight;

    if (dib_header_.biSizeImage == 0) {
        dib_header_.biSizeImage = data_size;
    }

    image_ = new BYTE[dib_header_.biSizeImage];

    input_file.read(reinterpret_cast<char*>(image_), dib_header_.biSizeImage);

    bitmap_.resize(dib_header_.biHeight, std::vector<Pixel>(dib_header_.biWidth));
    for (LONG i = 0; i < dib_header_.biHeight; ++i) {
        for (LONG j = 0; j < dib_header_.biWidth; ++j) {
            LONG k = i * row_size + j * 3;
            bitmap_[i][j] = {image_[k], image_[k + 1], image_[k + 2]};
        }
    }
}

BITMAPFILEHEADER ReadingChangegPicture::GetBIT() {
    return bitmap_header_;
}

BITMAPINFOHEADER ReadingChangegPicture::GetDIB() {
    return dib_header_;
}

BYTE* ReadingChangegPicture::GetImage() {
    return image_;
}

std::vector<std::vector<Pixel>> ReadingChangegPicture::GetDibMap() {
    return bitmap_;
}

void ReadingChangegPicture::SetBIT(BITMAPFILEHEADER new_bitmap_header) {
    bitmap_header_ = new_bitmap_header;
}

void ReadingChangegPicture::SetDIB(BITMAPINFOHEADER new_dib_header) {
    dib_header_ = new_dib_header;
}

void ReadingChangegPicture::SetImage(BYTE* new_image) {
    image_ = new_image;
}

void ReadingChangegPicture::SetDibMap(std::vector<std::vector<Pixel>> new_bitmap) {
    bitmap_ = new_bitmap;
}

void ReadingChangegPicture::SetPathOut(std::string pathout) {
    pathtoout_ = pathout;
}

ReadingChangegPicture::~ReadingChangegPicture() {
    std::ofstream file2(pathtoout_, std::ios::binary);
    if (!file2.is_open()) {
        FileError("Failed to open output file: " + pathtoout_);
    }
    file2.write(reinterpret_cast<char*>(&bitmap_header_), sizeof(BITMAPFILEHEADER));
    file2.write(reinterpret_cast<char*>(&dib_header_), sizeof(BITMAPINFOHEADER));
    file2.write(reinterpret_cast<char*>(image_), dib_header_.biSizeImage);
    file2.close();
    delete[] image_;
}

void ReadingChangegPicture::Checking() const {
    if (bitmap_header_.bfType != BMPSIGNATURE) {
        throw BMPFormatError("Invalid BMP signature. Expected 'BM'.");
    }

    if (dib_header_.biBitCount != BMPBITSPERPIXEL) {
        throw BMPFormatError("Unsupported bit count. Only 24-bit BMP is supported.");
    }

    if (dib_header_.biCompression != BMPNOCOMPRESSION) {
        throw BMPFormatError("Unsupported compression. Only uncompressed BMP is supported.");
    }

    if (dib_header_.biWidth <= 0 || dib_header_.biHeight <= 0) {
        throw BMPFormatError("Invalid image dimensions. Width and height must be positive.");
    }

    if (bitmap_header_.bfOffBits < sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)) {
        throw BMPFormatError("Invalid data offset. Data starts before the end of headers.");
    }
}
