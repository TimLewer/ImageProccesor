#include "Controller.h"
#include <string>

void ReadFromConsol::MakeMap() {
    filter_map_ = {"-crop", "-neg", "-gs", "-sp", "-pa", "-kd", "-sharp", "-edge", "-blur"};
}

void ReadFromConsol::DoFilter(std::string filter_name, std::vector<std::string> filter_par) {
    try {
        if (filter_name == "-crop") {
            if (filter_par.size() != 2) {
                FilterError("Crop filter requires exactly 2 parameters: width and height.");
            }
            if (std::min(std::stoi(filter_par[0]), std::stoi(filter_par[1])) <= 0) {
                FilterError("Width and height must be positive.");
            }

            Crop filter(file1_);
            filter.SetNum(std::stoi(filter_par[0]), std::stoi(filter_par[1]));
            filter.Active();
        } else if (filter_name == "-neg") {
            Negative filter(file1_);
            filter.Active();
        } else if (filter_name == "-gs") {
            GrayScale filter(file1_);
            filter.Active();
        } else if (filter_name == "-sp") {
            Sepia filter(file1_);
            filter.Active();
        } else if (filter_name == "-pa") {
            if (filter_par.size() != 1) {
                FilterError("PixelArt filter requires exactly 1 parameter: block_size.");
            }
            if (std::stoi(filter_par[0]) <= 0) {
                FilterError("Block size must be positive.");
            }
            PixelArt filter(file1_);
            filter.SetSize(std::stoi(filter_par[0]));
            filter.Active();
        } else if (filter_name == "-kd") {
            if (filter_par.size() != 1) {
                FilterError("Kaleidoscope filter requires exactly 1 parameter: count_axis.");
            }
            if (std::stoi(filter_par[0]) <= 0) {
                FilterError("Count Axis must be positive.");
            }
            Kaleidoscope filter(file1_);
            filter.SetCountOs(std::stoi(filter_par[0]));
            filter.Active();
        } else if (filter_name == "-sharp") {
            Sharpening filter(file1_);
            filter.Active();
        } else if (filter_name == "-edge") {
            if (filter_par.size() != 1) {
                FilterError("Edge Detection filter requires exactly 1 parameter: threshold.");
            }
            if (std::stoi(filter_par[0]) <= 0) {
                FilterError("Thresold must be positive.");
            }
            EdgeDetection filter(file1_);
            filter.SetThresod(std::stoi(filter_par[0]));
            filter.Active();
        } else if (filter_name == "-blur") {
            if (filter_par.size() != 1) {
                FilterError("Gaussian Blur filter requires exactly 1 parameter: sigma.");
            }
            if (std::stoi(filter_par[0]) <= 0) {
                FilterError("Sigma must be positive.");
            }
            GaussianBlur filter(file1_);
            filter.SetOmega(std::stoi(filter_par[0]));
            filter.Active();
        } else {
            FilterError("Unknown filter: " + filter_name);
        }
    } catch (const std::invalid_argument& e) {
        FilterError("Incorrect data type");
    }
}

void ReadFromConsol::HelpComment() {
    std::cout << "Input format:\n";
    std::cout << "{Path to picture input} {Path to picture output} [{filtr1} [par1] [par2]...] [{filtr2} [par1] "
                 "[par2]...]...\n";
    std::cout << "You can use this filters:\n";
    std::cout << "Crop(-crop), parameters: width, height\n";
    std::cout << "Grayscale(-gs)\n";
    std::cout << "Sepia(-sp)\n";
    std::cout << "Negative(-neg)\n";
    std::cout << "PixelArt(-pa), parameter: block_size\n";
    std::cout << "Kaleidoscope(-kd), parameter: count_axis\n";
    std::cout << "Sharpening(-sharp)\n";
    std::cout << "Edge Detection(-edge), parameters: threshold\n";
    std::cout << "Gaussian Blur(-blur), parameters: sigma\n";
}

void ReadFromConsol::MakeFiltersVector(int argc, const char* argv[]) {
    std::string filter_name;
    std::vector<std::string> filter_par;
    for (int i = 3; i < argc;) {
        filter_name = argv[i];
        ++i;
        filter_par.clear();
        while (i < argc && std::find(filter_map_.begin(), filter_map_.end(), argv[i]) == filter_map_.end()) {
            filter_par.push_back(argv[i]);
            ++i;
        }
        DoFilter(filter_name, filter_par);
    }
}

ReadFromConsol::ReadFromConsol(int argc, const char* argv[]) {
    try {
        if (argc < 3) {
            HelpComment();
        } else {
            inputfile_ = argv[1];
            outputfile_ = argv[2];

            std::ifstream file(inputfile_, std::ios::binary);

            file1_ = new ReadingChangegPicture(file);
            file1_->SetPathOut(outputfile_);

            MakeMap();
            MakeFiltersVector(argc, argv);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " <<  std::endl;
        if (file1_) {
            delete file1_;
        }
        exit(1);
    }
}

ReadFromConsol::~ReadFromConsol() {
    delete file1_;
}