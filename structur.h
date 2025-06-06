#pragma once

#include <cstddef>
#include <iostream>
#include <functional>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <array>  // Для std::array

using WORD = uint16_t;
using DWORD = uint32_t;
using LONG = int32_t;
using BYTE = unsigned char;

// Константы BMP
constexpr WORD BMPSIGNATURE = 0x4D42;
constexpr WORD BMPBITSPERPIXEL = 24;
constexpr DWORD BMPNOCOMPRESSION = 0;

// Константы для фильтров
constexpr BYTE MAXCOLORVALUE = 255;
constexpr double GRAYREDCOEFFICIENT = 0.299;
constexpr double GRAYGREENCOEFFICIENT = 0.587;
constexpr double GRAYBLUECOEFFICIENT = 0.114;

constexpr double SEPIAREDRED = 0.393;
constexpr double SEPIAREDGREEN = 0.769;
constexpr double SEPIAREDBLUE = 0.189;
constexpr double SEPIAGREENRED = 0.349;
constexpr double SEPIAGREENGREEN = 0.686;
constexpr double SEPIAGREENBLUE = 0.168;
constexpr double SEPIABLUERED = 0.272;
constexpr double SEPIABLUEGREEN = 0.534;
constexpr double SEPIABLUEBLUE = 0.131;

// Константные матрицы для фильтров
constexpr std::array<std::array<int, 3>, 3> SHARPENMATRIX = {{{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}}};

constexpr std::array<std::array<int, 3>, 3> EDGEDETECTIONMATRIX = {{{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}}};

constexpr int SIZECOREGAUSS = 6;

#pragma pack(push, 1)
struct BITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} __attribute__((packed));

struct BITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} __attribute__((packed));
#pragma pack(pop)

struct Pixel {
    BYTE blue = 0;
    BYTE green = 0;
    BYTE red = 0;
};