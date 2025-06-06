#pragma once

#include "structur.h"
#include "Exception.h"

class ReadingChangegPicture {
private:
    BITMAPFILEHEADER bitmap_header_;
    BITMAPINFOHEADER dib_header_;
    BYTE* image_;
    std::vector<std::vector<Pixel>> bitmap_;
    std::string pathtoout_;
    std::string pathtoimage_;

public:
    ReadingChangegPicture();

    explicit ReadingChangegPicture(std::ifstream& input_file);

    BITMAPFILEHEADER GetBIT();

    BITMAPINFOHEADER GetDIB();

    BYTE* GetImage();

    std::vector<std::vector<Pixel>> GetDibMap();

    void SetBIT(BITMAPFILEHEADER new_bitmap_header);

    void SetDIB(BITMAPINFOHEADER new_dib_header);

    void SetImage(BYTE* new_image);

    void SetDibMap(std::vector<std::vector<Pixel>> new_bitmap);

    void SetPathOut(std::string pathout);

    void SetPathImage(std::string pathimage);

    ~ReadingChangegPicture();

    void Checking() const;
};
