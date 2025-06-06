#pragma once

#include "ReadingChangedPicture.h"

class FilterOwn {
public:
    ReadingChangegPicture* picture_;
    BITMAPFILEHEADER bitmap_header_;
    BITMAPINFOHEADER dib_header_;
    BYTE* image_;
    std::vector<std::vector<Pixel>> bitmap_;

    FilterOwn();

    virtual void Active() = 0;
    virtual ~FilterOwn() = default;

    explicit FilterOwn(ReadingChangegPicture* picture);

    void Synchronization();
};

class GrayScale : virtual public FilterOwn {
public:
    using FilterOwn::FilterOwn;

    void Active() override;
};

class Negative : public FilterOwn {
public:
    using FilterOwn::FilterOwn;

    void Active() override;
};

class Crop : public FilterOwn {
private:
    LONG new_height_;
    LONG new_wight_;

public:
    using FilterOwn::FilterOwn;

    void SetNum(LONG new_wight, LONG new_height);

    void Active() override;
};

class MatrixFiltr : virtual public FilterOwn {
public:
    using FilterOwn::FilterOwn;

    std::array<std::array<int, 3>, 3> matrix_sharp;

    void SetMatrix(std::array<std::array<int, 3>, 3> matrix_filtr);

    void Active() override;
};

class Sharpening : virtual public FilterOwn, public MatrixFiltr {
public:
    using FilterOwn::FilterOwn;

    void Active() override;
};

class EdgeDetection : virtual public FilterOwn, public MatrixFiltr, virtual public GrayScale {
private:
    LONG thresold_;

public:
    using FilterOwn::FilterOwn;

    void SetThresod(LONG thresold);
    void BlackWhite();

    void Active() override;
};

class GaussianBlur : public FilterOwn {
private:
    double omega_;
    std::vector<double> gauss_one_core_;
    int size_core_;

    void MakeCoreOne();

public:
    using FilterOwn::FilterOwn;

    void SetOmega(double omega);

    void Active() override;
};

class Sepia : public FilterOwn {
public:
    using FilterOwn::FilterOwn;

    void Active() override;
};

class PixelArt : public FilterOwn {
private:
    int size_block_;

    void MakeBlock(LONG x_start, LONG y_start, LONG x_final, LONG y_final);

public:
    using FilterOwn::FilterOwn;

    void SetSize(int size);

    void Active() override;
};

class Kaleidoscope : public FilterOwn {
private:
    LONG count_os_;
    double centr_y_ = static_cast<double>(dib_header_.biHeight) / 2;
    double centr_x_ = static_cast<double>(dib_header_.biWidth) / 2;

public:
    using FilterOwn::FilterOwn;

    void SetCountOs(LONG count_os);

    void SetCentrCoord(LONG centr_x, LONG centr_y);

    void Active() override;
};