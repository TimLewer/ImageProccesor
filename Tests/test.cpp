#include <vector>
#define CATCH_CONFIG_MAIN
#include "../catch/catch.hpp"

#include "../Controller.cpp"
#include "../Filters.cpp"
#include "../ReadingChangedPicture.cpp"

constexpr LONG TESTTHRESOLD = 8;
constexpr LONG TESTWIGHT = 8;
constexpr LONG TESTHEIGHT = 8;
constexpr LONG TESTPIXELART = 10;
constexpr double TESTSIGMA = 2.0;

// TEST_CASE("BMPFormatError is thrown for invalid BMP signature") {
//     BITMAPFILEHEADER invalid_header = {};
//     invalid_header.bfType = 0x0000;

//     std::string temp_file = "invalid_bmp.bmp";
//     {
//         std::ofstream invalid_file(temp_file, std::ios::binary);
//         invalid_file.write(reinterpret_cast<char*>(&invalid_header), sizeof(BITMAPFILEHEADER));
//     }

//     std::ifstream input_file(temp_file, std::ios::binary);
//     REQUIRE(input_file.is_open());

//     REQUIRE_THROWS_AS(ReadingChangegPicture(input_file), BMPFormatError);

//     REQUIRE_THROWS_WITH(ReadingChangegPicture(input_file), "Invalid BMP signature. Expected 'BM'.");

//     input_file.close();
//     std::remove(temp_file.c_str());
// }

// TEST_CASE("FilterError is thrown for invalid filter parameters") {
//     const char* argv[] = {
//         "./image_project",
//         "../../Tests_image/flag.bmp",
//         "../../Tests_image/flag2.bmp",
//         "-crop",
//         "-1",
//         "100"
//     };
//     int argc = sizeof(argv) / sizeof(argv[0]);

//     REQUIRE_THROWS_AS(ReadFromConsol(argc, argv), FilterError);
// }

// TEST_CASE("std::invalid_argument is thrown for invalid filter parameters") {
//     const char* argv[] = {
//         "./image_project",
//         "../../Tests_image/flag.bmp",
//         "../../Tests_image/flag2.bmp",
//         "-crop",
//         "abc",
//         "100"
//     };
//     int argc = sizeof(argv) / sizeof(argv[0]);

//     REQUIRE_THROWS_AS(ReadFromConsol(argc, argv), std::invalid_argument);
// }

// TEST_CASE("FilterError is thrown for incorrect number of parameters") {
//     const char* argv[] = {
//         "./image_project",
//         "../../Tests_image/flag.bmp",
//         "../../Tests_image/flag2.bmp",
//         "-crop",
//         "100"
//     };
//     int argc = sizeof(argv) / sizeof(argv[0]);

//     REQUIRE_THROWS_AS(ReadFromConsol(argc, argv), FilterError);
// }

// TEST_CASE("FileError") {
//     std::ifstream input_file("file.bmp", std::ios::binary);

//     REQUIRE_THROWS_AS(ReadingChangegPicture(input_file), FileError);
// }

TEST_CASE("Negative filter") {
    std::ifstream input_file("/home/timur/Documents/C++/ImageProceccer/Tests_image/flag.bmp", std::ios::binary);
    REQUIRE(input_file.is_open());

    ReadingChangegPicture img(input_file);
    img.SetPathOut("/home/timur/Documents/C++/ImageProceccer/Tests_image/test2.bmp");
    std::vector<std::vector<Pixel>> bitmap = img.GetDibMap();
    std::vector<std::vector<Pixel>> or_bitmap = bitmap;

    Negative neg_filter(&img);
    neg_filter.Active();

    bitmap = img.GetDibMap();

    for (size_t i = 0; i < bitmap.size(); ++i) {
        for (size_t j = 0; j < bitmap[i].size(); ++j) {
            REQUIRE((LONG)bitmap[i][j].red == MAXCOLORVALUE - or_bitmap[i][j].red);
            REQUIRE((LONG)bitmap[i][j].green == MAXCOLORVALUE - or_bitmap[i][j].green);
            REQUIRE((LONG)bitmap[i][j].blue == MAXCOLORVALUE - or_bitmap[i][j].blue);
        }
    }
}

TEST_CASE("Grayscale filter") {
    std::ifstream input_file("../../Tests_image/flag.bmp", std::ios::binary);
    REQUIRE(input_file.is_open());

    ReadingChangegPicture img(input_file);
    img.SetPathOut("../../Tests_image/test2.bmp");
    input_file.close();

    GrayScale gray_filter(&img);
    gray_filter.Active();

    auto bitmap = img.GetDibMap();
    for (const auto& row : bitmap) {
        for (const auto& pixel : row) {
            REQUIRE(pixel.red == pixel.green);
            REQUIRE(pixel.green == pixel.blue);
        }
    }
}

TEST_CASE("Sepia filter") {
    std::ifstream input_file("../../Tests_image/flag.bmp", std::ios::binary);
    REQUIRE(input_file.is_open());

    ReadingChangegPicture img(input_file);
    img.SetPathOut("../../Tests_image/test2.bmp");
    input_file.close();
    std::vector<std::vector<Pixel>> original_bitmap = img.GetDibMap();

    Sepia sepia_filter(&img);
    sepia_filter.Active();

    std::vector<std::vector<Pixel>> bitmap = img.GetDibMap();
    for (size_t i = 0; i < bitmap.size(); ++i) {
        for (size_t j = 0; j < bitmap[i].size(); ++j) {
            BYTE expected_red = static_cast<BYTE>(original_bitmap[i][j].red * SEPIAREDRED +
                                                  original_bitmap[i][j].green * SEPIAREDGREEN +
                                                  original_bitmap[i][j].blue * SEPIAREDBLUE);
            BYTE expected_green = static_cast<BYTE>(original_bitmap[i][j].red * SEPIAGREENRED +
                                                    original_bitmap[i][j].green * SEPIAGREENGREEN +
                                                    original_bitmap[i][j].blue * SEPIAGREENBLUE);
            BYTE expected_blue = static_cast<BYTE>(original_bitmap[i][j].red * SEPIABLUERED +
                                                   original_bitmap[i][j].green * SEPIABLUEGREEN +
                                                   original_bitmap[i][j].blue * SEPIABLUEBLUE);

            REQUIRE(bitmap[i][j].red == std::min(expected_red, MAXCOLORVALUE));
            REQUIRE(bitmap[i][j].green == std::min(expected_green, MAXCOLORVALUE));
            REQUIRE(bitmap[i][j].blue == std::min(expected_blue, MAXCOLORVALUE));
        }
    }
}

TEST_CASE("Crop filter") {
    std::ifstream input_file("../../Tests_image/flag.bmp", std::ios::binary);
    REQUIRE(input_file.is_open());

    ReadingChangegPicture img(input_file);
    img.SetPathOut("../../Tests_image/test2.bmp");
    input_file.close();

    Crop crop_filter(&img);
    crop_filter.SetNum(TESTWIGHT, TESTHEIGHT);
    crop_filter.Active();

    REQUIRE(img.GetDIB().biWidth == TESTWIGHT);
    REQUIRE(img.GetDIB().biHeight == TESTHEIGHT);
}

TEST_CASE("PixelArt filter") {
    std::ifstream input_file("../../Tests_image/flag.bmp", std::ios::binary);
    REQUIRE(input_file.is_open());

    ReadingChangegPicture img(input_file);
    img.SetPathOut("../../Tests_image/test2.bmp");
    input_file.close();

    PixelArt pixel_filter(&img);
    LONG block = 2;
    pixel_filter.SetSize(block);
    pixel_filter.Active();
    LONG finish_h = 0;
    LONG finish_w = 0;

    std::vector<std::vector<Pixel>> pixelated_bitmap = img.GetDibMap();

    for (LONG i = 0; i < img.GetDIB().biHeight; i += block) {
        for (LONG j = 0; j < img.GetDIB().biWidth; j += block) {
            Pixel block_color = pixelated_bitmap[i][j];
            finish_h = i + block;
            finish_w = j + block;
            if (finish_h >= img.GetDIB().biHeight) {
                finish_h = img.GetDIB().biHeight - 1;
            }
            if (finish_w >= img.GetDIB().biWidth) {
                finish_w = img.GetDIB().biWidth - 1;
            }
            for (LONG x = i; x < finish_h; ++x) {
                for (LONG y = j; y < finish_w; ++y) {
                    REQUIRE((LONG)pixelated_bitmap[x][y].red == (LONG)block_color.red);
                    REQUIRE((LONG)pixelated_bitmap[x][y].green == (LONG)block_color.green);
                    REQUIRE((LONG)pixelated_bitmap[x][y].blue == (LONG)block_color.blue);
                }
            }
        }
    }
}

TEST_CASE("Sharpening filter") {
    std::ifstream input_file("../../Tests_image/flag.bmp", std::ios::binary);
    REQUIRE(input_file.is_open());

    ReadingChangegPicture img(input_file);
    img.SetPathOut("../../Tests_image/flag2.bmp");
    input_file.close();

    auto original_bitmap = img.GetDibMap();
    LONG height = img.GetDIB().biHeight;
    LONG width = img.GetDIB().biWidth;

    Sharpening sharpen_filter(&img);
    sharpen_filter.Active();

    std::vector<std::vector<Pixel>> sharpened_bitmap = img.GetDibMap();

    bool pixel_changed = false;
    for (LONG i = 1; i < height - 1; ++i) {
        for (LONG j = 1; j < width - 1; ++j) {
            if (sharpened_bitmap[i][j].red != original_bitmap[i][j].red ||
                sharpened_bitmap[i][j].green != original_bitmap[i][j].green ||
                sharpened_bitmap[i][j].blue != original_bitmap[i][j].blue) {
                pixel_changed = true;
                break;
            }
        }
        if (pixel_changed) {
            break;
        }
    }
    REQUIRE(pixel_changed);

    for (LONG i = 0; i < height; ++i) {
        for (LONG j = 0; j < width; ++j) {
            REQUIRE(sharpened_bitmap[i][j].red >= 0);
            REQUIRE(sharpened_bitmap[i][j].red <= 255);
            REQUIRE(sharpened_bitmap[i][j].green >= 0);
            REQUIRE(sharpened_bitmap[i][j].green <= 255);
            REQUIRE(sharpened_bitmap[i][j].blue >= 0);
            REQUIRE(sharpened_bitmap[i][j].blue <= 255);
        }
    }
}

TEST_CASE("EdgeDetection filter") {
    std::ifstream input_file("../../Tests_image/flag.bmp", std::ios::binary);
    REQUIRE(input_file.is_open());

    ReadingChangegPicture img(input_file);
    img.SetPathOut("../../Tests_image/flag2.bmp");
    input_file.close();

    EdgeDetection edge_filter(&img);
    edge_filter.SetThresod(TESTTHRESOLD);
    edge_filter.Active();

    std::vector<std::vector<Pixel>> edge_bitmap = img.GetDibMap();

    bool has_white = false;
    bool has_black = false;
    for (const std::vector<Pixel>& row : edge_bitmap) {
        for (const Pixel& pixel : row) {
            if (pixel.red == MAXCOLORVALUE && pixel.green == MAXCOLORVALUE && pixel.blue == MAXCOLORVALUE) {
                has_white = true;
            }
            if (pixel.red == 0 && pixel.green == 0 && pixel.blue == 0) {
                has_black = true;
            }
        }
    }

    REQUIRE(has_white);
    REQUIRE(has_black);
}

TEST_CASE("GaussianBlur filter") {
    std::ifstream input_file("../../Tests_image/flag.bmp", std::ios::binary);
    REQUIRE(input_file.is_open());

    ReadingChangegPicture img(input_file);
    img.SetPathOut("../../Tests_image/flag2.bmp");
    input_file.close();

    auto original_bitmap = img.GetDibMap();
    LONG height = img.GetDIB().biHeight;
    LONG width = img.GetDIB().biWidth;

    GaussianBlur gaussian_filter(&img);
    gaussian_filter.SetOmega(TESTSIGMA);
    gaussian_filter.Active();

    auto blurred_bitmap = img.GetDibMap();

    bool pixel_changed = false;
    for (LONG i = 1; i < height - 1; ++i) {
        for (LONG j = 1; j < width - 1; ++j) {
            if (blurred_bitmap[i][j].red != original_bitmap[i][j].red ||
                blurred_bitmap[i][j].green != original_bitmap[i][j].green ||
                blurred_bitmap[i][j].blue != original_bitmap[i][j].blue) {
                pixel_changed = true;
                break;
            }
        }
    }
    REQUIRE(pixel_changed);

    LONG x = TESTPIXELART;
    LONG y = TESTPIXELART;
    if (x >= 1 && x < height - 1 && y >= 1 && y < width - 1) {
        Pixel blurred_pixel = blurred_bitmap[x][y];
        auto neighbors = {blurred_bitmap[x - 1][y], blurred_bitmap[x + 1][y], blurred_bitmap[x][y - 1],
                          blurred_bitmap[x][y + 1]};
        for (const auto& neighbor : neighbors) {
            REQUIRE(std::abs(blurred_pixel.red - neighbor.red) < 20);
            REQUIRE(std::abs(blurred_pixel.green - neighbor.green) < 20);
            REQUIRE(std::abs(blurred_pixel.blue - neighbor.blue) < 20);
        }
    }
}