#include "ppm.h"

PPM::PPM(const std::string& file) {
    std::ifstream input(file);

    std::string line;
    int info_count = 0;

    int col, row, channel;
    col = row = channel = 0;

    while (getline(input, line)) {
        if (line[0] == '#') //ignore comment
            continue;

        int n = 0;
        bool has_n = false;

        for (char c : line) {
            if (isdigit(c)) {
                n = n * 10 + (c - '0');
                has_n = true;
            }
            else if (has_n) {

                if (info_count > 3) {
                    data[row][col][channel] = n;
                    channel++;
                    if (channel == header.n_channels) {
                        channel = 0;
                        col++;
                        if (col == header.width) {
                            col = 0;
                            row++;
                        }
                    }
                }
                else if (info_count == 0) {
                    read_bpp(n);
                }
                else if (info_count == 1) {
                    header.width = n;
                }
                else if (info_count == 2) {
                    header.height = n;

                    data = std::vector<std::vector<std::vector<int>>>(header.height, std::vector<std::vector<int>>(header.width, std::vector<int>(header.n_channels)));
                }
                else if (info_count == 3) {
                    header.max_val = n;
                }

                n = 0;
                has_n = false;

                info_count++;
            }
        }

        if (has_n) {
            if (info_count > 3) {
                data[row][col][channel] = n;
                channel++;
                if (channel == header.n_channels) {
                    channel = 0;
                    col++;
                    if (col == header.width) {
                        col = 0;
                        row++;
                    }
                }
            }
            else if (info_count == 0) {
                read_bpp(n);
            }
            else if (info_count == 1) {
                header.width = n;
            }
            else if (info_count == 2) {
                header.height = n;

                data = std::vector<std::vector<std::vector<int>>>(header.height, std::vector<std::vector<int>>(header.width, std::vector<int>(header.n_channels)));
            }
            else if (info_count == 3) {
                header.max_val = n;
            }

            info_count++;
        }
    }

    input.close();
}

PPM::PPM(Header header, std::vector<std::vector<std::vector<int>>> data) {
    this->header = header;
    this->data = data;
}

PPM::~PPM() {

}

void PPM::read_bpp(const int& n) {
    header.in_binary = n > 3;

    const std::vector<int> bpp_values = { 1, 8, 24 };
    header.bpp = bpp_values[(n - 1) % 3];

    header.n_channels = (header.bpp == 24) ? 3 : 1;
}

void PPM::write(const std::string& file) {
    std::ofstream output(file);

    int p_bpp;
    if (header.bpp == 1)
        p_bpp = 1;
    else if (header.bpp == 8)
        p_bpp = 2;
    else
        p_bpp = 3;
    p_bpp += 3 * header.in_binary;

    output << "P" << p_bpp << std::endl;
    output << header.width << " " << header.height << std::endl;
    output << header.max_val << std::endl;
    for (int i = 0; i < header.height; i++) {
        for (int j = 0; j < header.width; j++) {
            for (int k = 0; k < header.n_channels; k++) {
                output << data[i][j][k];
                if (k < header.n_channels - 1)
                    output << " ";
            }
            output << std::endl;
        }
    }

    output.close();
}

// works only for one channel ppms
std::vector<double> PPM::get_histogram() {
    std::vector<int> histogram(header.max_val + 1, 0);
    for (int i = 0; i < header.height; i++) {
        for (int j = 0; j < header.width; j++) {
            for (int k = 0; k < header.n_channels; k++) {
                histogram[data[i][j][k]]++;
            }
        }
    }

    double n = header.width * header.height;
    std::vector<double> p(header.max_val + 1, 0);
    for (int i = 0; i <= header.max_val; i++) {
        p[i] = histogram[i] / n;
        // std::cout << p[i] << " " << histogram[i] << std::endl;
    }

    std::vector<double> t(header.max_val + 1, 0);
    for (int i = 0; i <= header.max_val; i++) {
        if (i > 0)
            t[i] = t[i - 1];
        t[i] += p[i];
        std::cout << t[i] << " ";
    }

    return t;
}

std::vector<std::vector<std::vector<int>>> PPM::equal() {
    std::vector<double> t = get_histogram();

    std::vector<int> g(header.max_val + 1, 0);
    for (int i = 0; i <= header.max_val; i++) {
        g[i] = round(t[i] * (header.max_val));
        if (g[i] > header.max_val)
            g[i] = header.max_val;
    }

    std::vector<std::vector<std::vector<int>>> equal_data = data;
    for (int i = 0; i < header.height; i++) {
        for (int j = 0; j < header.width; j++) {
            for (int k = 0; k < header.n_channels; k++) {
                equal_data[i][j][k] = g[data[i][j][k]];
                // std::cout << g[data[i][j][k]] << std::endl;
            }
        }
    }

    return equal_data;
}
