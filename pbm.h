#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <math.h>
#include <algorithm>

class PBM {
private:
    void read_bpp(const int& n);

public:
    PBM(const std::string& file);
    ~PBM();

    int width, height;
    std::vector<std::vector<int>> data;

    void write(const std::string& file);

    // operations
    void median(const int &n);
    void erosion();
};
