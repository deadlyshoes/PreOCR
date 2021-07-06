#include "pbm.h"

PBM::PBM(const std::string& file) {
    std::ifstream input(file);
    if (input.fail())
        throw "Não foi possível ler o arquivo";

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
                    throw "Não é do tipo P1";
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

PBM::PBM(const PBM &other) {
    height = other.height;
    width = other.width;

    data = other.data;
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

    /* pré-processamento */
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

    /* aplicação do filtro */
    for (int i = n / 2; i < height - n / 2; i++) {
        for (int j = n / 2; j < width - n / 2; j++) {
            int count_1 = median_dp[i + n / 2][j + n / 2];
            if (i - n / 2)
                count_1 -= median_dp[i - n / 2 - 1][j + n / 2];
            if (j - n / 2)
                count_1 -= median_dp[i + n / 2][j - n / 2 - 1];
            if (i - n / 2 && j - n / 2)
                count_1 += median_dp[i - n / 2 - 1][j - n / 2 - 1];

            if (count_1 > (n * n) / 2)
                data[i][j] = 1;
            else
                data[i][j] = 0;
        }
    }
}

void PBM::erosion(std::vector<std::vector<int>> se) {
    std::vector<std::vector<int>>new_img(height, std::vector<int>(width, 0));

    int se_height = se.size();
    int se_width = se[0].size();

    for (int i = se_height / 2; i < height - se_height / 2; i++) {
        for (int j = se_width / 2; j < width - se_width / 2; j++) {
            bool all_common = true;
            for (int k = 0; k < se_height; k++) {
                for (int l = 0; l < se_width; l++) {
                    int off_i = i + (k - se_height / 2);
                    int off_j = j + (l - se_width / 2);
                    if (data[off_i][off_j] == 0 && se[k][l] == 1) {
                        all_common = false;
                        goto done;
                    }
                }
            }

            done:
                if (all_common)
                    new_img[i][j] = 1;
        }
    }

    data = new_img;
}

void PBM::dilation(std::vector<std::vector<int>> se) {
    int se_height = se.size();
    int se_width = se[0].size();

    std::vector<std::vector<int>> dilated_data(height, std::vector<int>(width, 0));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            /* pular os já calculados */
            if (dilated_data[i][j] == 1)
                continue;

            // check intersection with SE
            bool one_common = false;
            int last_k, last_l;
            for (int k = 0; k < se_height; k++) {
                for (int l = 0; l < se_width; l++) {
                    int off_i = i + (k - se_height / 2);
                    int off_j = j + (l - se_width / 2);
                    if ((off_i >= 0 && off_i < height) && (off_j >= 0 && off_j < width))
                        if (data[off_i][off_j] == 1 && se[k][l] == 1) {
                            one_common = true;
                            last_k = k;
                            last_l = l;
                            goto done;
                        }
                }
            }
            done:
                if (one_common) {
                    /* otimização descrita no relatório */
                    for (int k = last_k, v = i; k >= 0 && se[k][last_l] == 1 && v < height; k--, v++)
                        dilated_data[v][j] = 1;
                    for (int l = last_l, w = j; l >= 0 && se[last_k][l] == 1 && w < width; l--, w++)
                        dilated_data[i][w] = 1;
                }
        }
    }

    data = dilated_data;
}

void PBM::opening(std::vector<std::vector<int>> se) {
    erosion(se);
    dilation(se);
}

void PBM::closing(std::vector<std::vector<int>> se) {
    dilation(se);
    erosion(se);
}
