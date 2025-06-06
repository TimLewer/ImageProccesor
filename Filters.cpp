#include "Filters.h"
#include "structur.h"

FilterOwn::FilterOwn() = default;

FilterOwn::FilterOwn(ReadingChangegPicture* picture) {
    picture_ = picture;
    bitmap_header_ = picture_->GetBIT();
    dib_header_ = picture_->GetDIB();
    image_ = picture_->GetImage();
    bitmap_ = picture_->GetDibMap();
}

void FilterOwn::Synchronization() {
    LONG newlineize = ((dib_header_.biWidth * 3 + 3) / 4) * 4;
    for (LONG i = 0; i < dib_header_.biHeight; ++i) {
        BYTE* destrow = image_ + i * newlineize;
        LONG index = 0;
        for (LONG j = 0; j < dib_header_.biWidth; ++j) {
            destrow[index++] = bitmap_[i][j].blue;
            destrow[index++] = bitmap_[i][j].green;
            destrow[index++] = bitmap_[i][j].red;
        }
        while (index < newlineize) {
            destrow[index++] = 0;
        }
    }
    picture_->SetBIT(bitmap_header_);
    picture_->SetDIB(dib_header_);
    picture_->SetDibMap(bitmap_);
    picture_->SetImage(image_);
}

void GrayScale::Active() {
    for (LONG i = 0; i < dib_header_.biHeight; ++i) {
        for (LONG j = 0; j < dib_header_.biWidth; ++j) {
            BYTE med =
                static_cast<BYTE>(bitmap_[i][j].red * GRAYREDCOEFFICIENT + bitmap_[i][j].green * GRAYGREENCOEFFICIENT +
                                  GRAYBLUECOEFFICIENT * bitmap_[i][j].blue);
            bitmap_[i][j].red = med;
            bitmap_[i][j].blue = med;
            bitmap_[i][j].green = med;
        }
    }
    FilterOwn::Synchronization();
}

void Negative::Active() {
    for (LONG i = 0; i < dib_header_.biHeight; ++i) {
        for (LONG j = 0; j < dib_header_.biWidth; ++j) {
            bitmap_[i][j].blue = MAXCOLORVALUE - bitmap_[i][j].blue;
            bitmap_[i][j].green = MAXCOLORVALUE - bitmap_[i][j].green;
            bitmap_[i][j].red = MAXCOLORVALUE - bitmap_[i][j].red;
        }
    }
    FilterOwn::Synchronization();
}

void Crop::SetNum(LONG new_wight, LONG new_height) {
    new_height_ = new_height;
    new_wight_ = new_wight;
}

void Crop::Active() {
    if (new_height_ > dib_header_.biHeight) {
        new_height_ = dib_header_.biHeight;
    }
    if (new_wight_ > dib_header_.biWidth) {
        new_wight_ = dib_header_.biWidth;
    }

    LONG newlinesize = ((new_wight_ * 3 + 3) / 4) * 4;

    std::vector<std::vector<Pixel>> new_bitmap(new_height_, std::vector<Pixel>(new_wight_));

    for (LONG i = 0; i < new_height_; ++i) {
        for (LONG j = 0; j < new_wight_; ++j) {
            new_bitmap[i][j] = bitmap_[dib_header_.biHeight - new_height_ + i][j];
        }
    }

    dib_header_.biHeight = new_height_;
    dib_header_.biWidth = new_wight_;

    dib_header_.biSizeImage = newlinesize * new_height_;

    bitmap_ = new_bitmap;

    delete[] image_;
    image_ = new BYTE[dib_header_.biSizeImage];

    FilterOwn::Synchronization();
}

void MatrixFiltr::SetMatrix(std::array<std::array<int, 3>, 3> matrix_filtr) {
    matrix_sharp = matrix_filtr;
}

void MatrixFiltr::Active() {
    std::vector<std::vector<Pixel>> bitmap_temp = bitmap_;
    for (LONG i = 1; i < dib_header_.biHeight - 1; ++i) {
        for (LONG j = 1; j < dib_header_.biWidth - 1; ++j) {
            int new_blue = static_cast<int>(bitmap_[i][j - 1].blue) * matrix_sharp[1][0] +
                           static_cast<int>(bitmap_[i][j + 1].blue) * matrix_sharp[1][2] +
                           static_cast<int>(bitmap_[i - 1][j].blue) * matrix_sharp[0][1] +
                           static_cast<int>(bitmap_[i + 1][j].blue) * matrix_sharp[2][1] +
                           static_cast<int>(bitmap_[i][j].blue) * matrix_sharp[1][1];
            int new_green = static_cast<int>(bitmap_[i][j - 1].green) * matrix_sharp[1][0] +
                            static_cast<int>(bitmap_[i][j + 1].green) * matrix_sharp[1][2] +
                            static_cast<int>(bitmap_[i - 1][j].green) * matrix_sharp[0][1] +
                            static_cast<int>(bitmap_[i + 1][j].green) * matrix_sharp[2][1] +
                            static_cast<int>(bitmap_[i][j].green) * matrix_sharp[1][1];
            int new_red = static_cast<int>(bitmap_[i][j - 1].red) * matrix_sharp[1][0] +
                          static_cast<int>(bitmap_[i][j + 1].red) * matrix_sharp[1][2] +
                          static_cast<int>(bitmap_[i - 1][j].red) * matrix_sharp[0][1] +
                          static_cast<int>(bitmap_[i + 1][j].red) * matrix_sharp[2][1] +
                          static_cast<int>(bitmap_[i][j].red) * matrix_sharp[1][1];

            bitmap_temp[i][j].blue =
                static_cast<BYTE>(std::min(static_cast<LONG>(MAXCOLORVALUE), std::max(0, new_blue)));
            bitmap_temp[i][j].green =
                static_cast<BYTE>(std::min(static_cast<LONG>(MAXCOLORVALUE), std::max(0, new_green)));
            bitmap_temp[i][j].red = static_cast<BYTE>(std::min(static_cast<LONG>(MAXCOLORVALUE), std::max(0, new_red)));
        }
    }
    bitmap_ = bitmap_temp;
    FilterOwn::Synchronization();
}

void Sharpening::Active() {
    MatrixFiltr::SetMatrix(SHARPENMATRIX);
    MatrixFiltr::Active();
}

void EdgeDetection::SetThresod(LONG thresold) {
    thresold_ = thresold;
}

void EdgeDetection::BlackWhite() {
    LONG new_color = 0;
    for (LONG i = 0; i < dib_header_.biHeight; ++i) {
        for (LONG j = 0; j < dib_header_.biWidth; ++j) {
            if (std::max({bitmap_[i][j].blue, bitmap_[i][j].green, bitmap_[i][j].red}) > thresold_) {
                new_color = MAXCOLORVALUE;
            } else {
                new_color = 0;
            }
            bitmap_[i][j].blue = new_color;
            bitmap_[i][j].red = new_color;
            bitmap_[i][j].green = new_color;
        }
    }
}

void EdgeDetection::Active() {
    GrayScale::Active();
    SetMatrix(EDGEDETECTIONMATRIX);
    MatrixFiltr::Active();
    BlackWhite();
    FilterOwn::Synchronization();
}

void GaussianBlur::MakeCoreOne() {
    size_core_ = SIZECOREGAUSS * static_cast<int>(omega_) + 1;
    int center = size_core_ / 2;
    gauss_one_core_ = std::vector<double>(size_core_, 0.0);
    double sum = 0.0;
    for (int i = 0; i < size_core_; ++i) {
        int dx = i - center;
        double value = exp(-(dx * dx / (2 * omega_ * omega_)));
        gauss_one_core_[i] = value;
        sum += value;
    }

    for (int i = 0; i < size_core_; ++i) {
        gauss_one_core_[i] /= sum;
    }
}

void GaussianBlur::SetOmega(double omega) {
    omega_ = omega;
    MakeCoreOne();
}

void GaussianBlur::Active() {
    double new_blue = 0.0;
    double new_green = 0.0;
    double new_red = 0.0;
    int center = size_core_ / 2;
    std::vector<std::vector<Pixel>> bitmap_temp = bitmap_;
    for (LONG i = 0; i < dib_header_.biHeight; ++i) {
        for (LONG j = 0; j < dib_header_.biWidth; ++j) {
            new_blue = 0.0;
            new_green = 0.0;
            new_red = 0.0;
            for (int ki = 0; ki < size_core_; ++ki) {
                int new_i =
                    std::clamp(static_cast<int>(i) + (ki - center), 0, static_cast<int>(dib_header_.biHeight) - 1);
                new_blue += bitmap_[new_i][j].blue * gauss_one_core_[ki];
                new_green += bitmap_[new_i][j].green * gauss_one_core_[ki];
                new_red += bitmap_[new_i][j].red * gauss_one_core_[ki];
            }
            bitmap_temp[i][j].blue = static_cast<BYTE>(new_blue);
            bitmap_temp[i][j].green = static_cast<BYTE>(new_green);
            bitmap_temp[i][j].red = static_cast<BYTE>(new_red);
        }
    }
    for (LONG i = 0; i < dib_header_.biHeight; ++i) {
        for (LONG j = 0; j < dib_header_.biWidth; ++j) {
            new_blue = 0.0;
            new_green = 0.0;
            new_red = 0.0;
            for (int ki = 0; ki < size_core_; ++ki) {
                int new_j =
                    std::clamp(static_cast<int>(j) + (ki - center), 0, static_cast<int>(dib_header_.biWidth) - 1);
                new_blue += bitmap_temp[i][new_j].blue * gauss_one_core_[ki];
                new_green += bitmap_temp[i][new_j].green * gauss_one_core_[ki];
                new_red += bitmap_temp[i][new_j].red * gauss_one_core_[ki];
            }

            bitmap_[i][j].blue =
                static_cast<BYTE>(std::min(static_cast<LONG>(MAXCOLORVALUE), std::max(0, static_cast<LONG>(new_blue))));
            bitmap_[i][j].green = static_cast<BYTE>(
                std::min(static_cast<LONG>(MAXCOLORVALUE), std::max(0, static_cast<LONG>(new_green))));
            bitmap_[i][j].red =
                static_cast<BYTE>(std::min(static_cast<LONG>(MAXCOLORVALUE), std::max(0, static_cast<LONG>(new_red))));
        }
    }
    FilterOwn::Synchronization();
}

void Sepia::Active() {
    LONG new_blue = 0;
    LONG new_green = 0;
    LONG new_red = 0;
    for (LONG i = 0; i < dib_header_.biHeight; ++i) {
        for (LONG j = 0; j < dib_header_.biWidth; ++j) {
            new_red = static_cast<LONG>(bitmap_[i][j].red * SEPIAREDRED + bitmap_[i][j].green * SEPIAREDGREEN +
                                        SEPIAREDBLUE * bitmap_[i][j].blue);
            new_green = static_cast<LONG>(bitmap_[i][j].red * SEPIAGREENRED + bitmap_[i][j].green * SEPIAGREENGREEN +
                                          SEPIAGREENBLUE * bitmap_[i][j].blue);
            new_blue = static_cast<LONG>(bitmap_[i][j].red * SEPIABLUERED + bitmap_[i][j].green * SEPIABLUEGREEN +
                                         SEPIABLUEBLUE * bitmap_[i][j].blue);

            bitmap_[i][j].blue =
                static_cast<BYTE>(std::min(static_cast<LONG>(MAXCOLORVALUE), std::max(0, static_cast<LONG>(new_blue))));
            bitmap_[i][j].green = static_cast<BYTE>(
                std::min(static_cast<LONG>(MAXCOLORVALUE), std::max(0, static_cast<LONG>(new_green))));
            bitmap_[i][j].red =
                static_cast<BYTE>(std::min(static_cast<LONG>(MAXCOLORVALUE), std::max(0, static_cast<LONG>(new_red))));
        }
    }
    FilterOwn::Synchronization();
}

void PixelArt::MakeBlock(LONG x_start, LONG y_start, LONG x_final, LONG y_final) {
    LONG sum_blue = 0;
    LONG sum_green = 0;
    LONG sum_red = 0;

    LONG count_pixel = 0;

    for (LONG x = x_start; x < x_final; ++x) {
        for (LONG y = y_start; y < y_final; ++y) {
            sum_blue += bitmap_[x][y].blue;
            sum_green += bitmap_[x][y].green;
            sum_red += bitmap_[x][y].red;
            ++count_pixel;
        }
    }

    double middle_blue = static_cast<double>(sum_blue) / static_cast<double>(count_pixel);
    double middle_green = static_cast<double>(sum_green) / static_cast<double>(count_pixel);
    double middle_red = static_cast<double>(sum_red) / static_cast<double>(count_pixel);

    for (LONG x = x_start; x < x_final; ++x) {
        for (LONG y = y_start; y < y_final; ++y) {
            bitmap_[x][y].blue = static_cast<BYTE>(middle_blue);
            bitmap_[x][y].green = static_cast<BYTE>(middle_green);
            bitmap_[x][y].red = static_cast<BYTE>(middle_red);
        }
    }
}

void PixelArt::SetSize(int size) {
    size_block_ = size;
}

void PixelArt::Active() {
    LONG finish_h = 0;
    LONG finish_w = 0;

    for (LONG i = 0; i < dib_header_.biHeight; i += size_block_) {
        for (LONG j = 0; j < dib_header_.biWidth; j += size_block_) {
            finish_h = i + size_block_;
            finish_w = j + size_block_;
            if (finish_h >= dib_header_.biHeight) {
                finish_h = dib_header_.biHeight - 1;
            }
            if (finish_w >= dib_header_.biWidth) {
                finish_w = dib_header_.biWidth - 1;
            }
            MakeBlock(i, j, finish_h, finish_w);
        }
    }
    FilterOwn::Synchronization();
}

void Kaleidoscope::SetCountOs(LONG count_os) {
    count_os_ = count_os;
}

void Kaleidoscope::SetCentrCoord(LONG centr_x, LONG centr_y) {
    centr_x_ = static_cast<double>(centr_x);
    centr_y_ = static_cast<double>(centr_y);
}

void Kaleidoscope::Active() {
    std::vector<std::vector<Pixel>> bitmap_temp = bitmap_;
    double step_rad = 2 * M_PI / count_os_;
    for (LONG i = 0; i < dib_header_.biHeight; ++i) {
        for (LONG j = 0; j < dib_header_.biWidth; ++j) {
            LONG rel_i = static_cast<LONG>(i - centr_y_);
            LONG rel_j = static_cast<LONG>(j - centr_x_);

            for (LONG in_os = 0; in_os < count_os_; ++in_os) {
                double angle = in_os * step_rad;
                double cosinus = cos(angle);
                double sinus = sin(angle);

                double rotated_i = (rel_i * cosinus - rel_j * sinus);
                double rotated_j = (rel_i * sinus + rel_j * cosinus);

                LONG new_i = static_cast<LONG>(std::round(rotated_i + centr_y_));
                LONG new_j = static_cast<LONG>(std::round(rotated_j + centr_x_));

                if (new_i >= 0 && new_j >= 0 && new_i < dib_header_.biHeight && new_j < dib_header_.biWidth) {
                    bitmap_temp[new_i][new_j].blue = bitmap_[i][j].blue;
                    bitmap_temp[new_i][new_j].red = bitmap_[i][j].red;
                    bitmap_temp[new_i][new_j].green = bitmap_[i][j].green;
                }
            }
        }
    }
    bitmap_ = bitmap_temp;
    FilterOwn::Synchronization();
}
