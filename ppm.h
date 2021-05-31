#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

struct Header {
    int bpp; //bits per pixel
    int n_channels;
    bool in_binary;
    int width, height;
    int max_val;
};

class PPM {
private:
    void read_bpp(const int& n);

public:
    PPM(const std::string& file);
    PPM(Header header, std::vector<std::vector<std::vector<int>>> data);
    ~PPM();

    Header header;
    std::vector<std::vector<std::vector<int>>> data;

    std::vector<double> get_histogram();
    void write(const std::string& file);

    // operations
    std::vector<std::vector<std::vector<int>>> equal();
};
