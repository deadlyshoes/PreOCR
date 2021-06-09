#include "pbm.h"

PBM::PBM(const std::string& file) {
    std::ifstream input(file);

    std::string line;
    int info_count = 0;

    int col, row;
    col = row = 0;

    while (getline(input, line)) {
        if (line[0] == '#') //ignore comment
            continue;

        int n = 0;
        bool has_n = false;

        /* pixels */
        if (info_count == 3) {
            for (char c : line) {
                if (isdigit(c)) {
                    n = c - '0';

                    data[row][col] = n;
                    col++;
                    if (col == width) {
                        col = 0;
                        row++;
                    }
                }
            }

            continue;
        }

        /* info */
        for (char c : line) {
            if (isdigit(c)) {
                n = n * 10 + (c - '0');
                has_n = true;
            } else if (has_n) {
                if (info_count == 0) {
                    if (n != 1) {
                        std::cout << "Não é do tipo P1" << std::endl;
                        exit(1);
                    }
                }
                else if (info_count == 1) {
                    width = n;
                }
                else if (info_count == 2) {
                    height = n;

                    data = std::vector<std::vector<int>>(height, std::vector<int>(width));
                }

                n = 0;
                has_n = false;

                info_count++;
            }
        }

        if (has_n) {
            if (info_count == 0) {
                if (n != 1) {
                    std::cout << "Não é do tipo P1" << std::endl;
                    exit(1);
                }
            }
            else if (info_count == 1) {
                width = n;
            }
            else if (info_count == 2) {
                height = n;

                data = std::vector<std::vector<int>>(height, std::vector<int>(width));
            }

            n = 0;
            has_n = false;

            info_count++;
        }
    }

    input.close();
}

PBM::~PBM() {

}

void PBM::write(const std::string& file) {
    std::ofstream output(file);

    output << "P1" << std::endl;
    output << width << " " << height << std::endl;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            output << data[i][j];
        }
        output << std::endl;
    }

    output.close();
}

void PBM::median(const int &n) {
    // check
    if (n % 2 == 0) {
        std::cout << "n must be odd" << std::endl;
        return;
    }
    if (height < n || width < n) {
        std::cout << "image is too small" << std::endl;
        return;
    }

    std::vector<std::vector<int>> median_dp(height, std::vector<int>(width, 0));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (i)
                median_dp[i][j] += median_dp[i - 1][j];
            if (j)
                median_dp[i][j] += median_dp[i][j - 1];
            if (i && j)
                median_dp[i][j] -= median_dp[i - 1][j - 1];
            median_dp[i][j] += data[i][j];
        }
    }

    for (int i = n / 2; i < height - n / 2; i++) {
        for (int j = n / 2; j < width - n / 2; j++) {
            int count_1 = median_dp[i + n / 2][j + n / 2];
            if (i - n / 2)
                count_1 -= median_dp[i - n / 2 - 1][j + n / 2];
            if (j - n / 2)
                count_1 -= median_dp[i + n / 2][j - n / 2 - 1];
            if (i - n / 2 && j - n / 2)
                count_1 += median_dp[i - n / 2 - 1][j - n / 2 - 1];
            std::cout << count_1 << " ";

            if (count_1 > (n * n) / 2)
                data[i][j] = 1;
            else
                data[i][j] = 0;
        }
    }
}