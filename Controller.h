#include "Filters.h"

class ReadFromConsol {
private:
    std::string inputfile_;
    std::string outputfile_;
    ReadingChangegPicture* file1_;
    std::vector<std::string> filter_map_;

    void MakeMap();

    void DoFilter(std::string filter_name, std::vector<std::string> filter_par);

    void HelpComment();

    void MakeFiltersVector(int argc, const char* argv[]);

public:
    ReadFromConsol(int argc, const char* argv[]);

    ~ReadFromConsol();
};