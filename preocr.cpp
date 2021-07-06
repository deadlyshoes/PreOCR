#include "preocr.h"
#include <queue>

PreOCR::PreOCR(PBM *out_pbm) {
    pbm = out_pbm;
}

std::vector<PreOCR::Rectangle> PreOCR::varredura() {
    std::vector<Rectangle> rectangles;

    vis = std::vector<std::vector<bool>>(pbm->height, std::vector<bool>(pbm->width, false));

    for (int i = 0; i < pbm->height; i++) {
        for (int j = 0; j < pbm->width; j++) {
            // word?
            if (pbm->data[i][j] == 1 && !vis[i][j]) {
                Rectangle rect = {INT_MAX, INT_MIN, INT_MAX, INT_MIN};

                std::queue<std::pair<int, int>> q;
                q.push({i, j});
                vis[i][j] = true;
                while (!q.empty()) {
                    int x = q.front().first;
                    int y = q.front().second;
                    q.pop();

                    rect.x1 = std::min(rect.x1, x);
                    rect.x2 = std::max(rect.x2, x);
                    rect.y1 = std::min(rect.y1, y);
                    rect.y2 = std::max(rect.y2, y);

                    for (int k = 0; k < dir.size(); k++) {
                        for (int l = 0; l < dir.size(); l++) {
                            int x_dir = x + dir[k];
                            int y_dir = y + dir[l];
                            if (x_dir >= 0 && x_dir < pbm->height && y_dir >= 0 && y_dir < pbm->width &&
                                !vis[x_dir][y_dir] && pbm->data[x_dir][y_dir] == 1) {
                                q.push({x_dir, y_dir});
                                vis[x_dir][y_dir] = true;
                            }
                        }
                    }
                }

                rectangles.push_back(rect);
            }
        }
    }

    return rectangles;
}

void PreOCR::contar_linhas_colunas(int letter_height, int letter_width) {
    PBM *tmp = new PBM(*pbm);

    std::vector<std::vector<int>> se_line(1, std::vector<int>(letter_width, 1));
    pbm->dilation(se_line);

    const std::vector<std::vector<int>> default_se(std::max(1, letter_height / 5), std::vector<int>(1, 1));
    pbm->closing(default_se);

    std::vector<Rectangle> lines_rects = varredura();
    int n_lines = lines_rects.size();

    for (const Rectangle &rect : lines_rects) {
        for (int i = rect.x1; i <= rect.x2; i++) {
            pbm->data[i][rect.y1] = 1;
            pbm->data[i][rect.y2] = 1;
        }
        for (int i = rect.y1; i <= rect.y2; i++) {
            pbm->data[rect.x1][i] = 1;
            pbm->data[rect.x2][i] = 1;
        }
    }

    const std::vector<std::vector<int>> se_column(pbm->height, std::vector<int>(1, 1));
    pbm->dilation(se_column);

    std::vector<Rectangle> columns_rects = varredura();
    int n_columns = columns_rects.size();

    std::cout << n_lines << " linha(s)" << std::endl;
    std::cout << n_columns << " coluna(s)" << std::endl;

    pbm->data = tmp->data;
    delete tmp;
}

void PreOCR::etapa1() {
    pbm->median(3);

    /* remover bordas da imagem não tocadas pelo filtro da mediana */
    for (int j = 0; j < pbm->width; j++)
        pbm->data[0][j] = 0;
    for (int i = 1; i < pbm->height - 1; i++) {
        pbm->data[i][0] = 0;
        pbm->data[i][pbm->width - 1] = 0;
    }
    for (int j = 0; j < pbm->width; j++)
        pbm->data[pbm->height - 1][j] = 0;
}

void PreOCR::etapa2() {
    etapa1();

    PBM *tmp = new PBM(*pbm);

    /* letters */
    int letter_width = 0;
    int letter_height = 0;
    std::vector<Rectangle> letter_rects = varredura();

    std::cout << letter_rects.size() << " letras(s)" << std::endl;

    std::vector<std::vector<int>> letter_dp(pbm->height, std::vector<int>(pbm->width, 0));
    for (const Rectangle &rect : letter_rects) {
        letter_dp[rect.x2][rect.y1] += 1;
        letter_width = std::max(letter_width, rect.x2 - rect.x1);
        letter_height = std::max(letter_height, rect.y2 - rect.y1);
    }

    for (int i = 0; i < pbm->height; i++) {
        for (int j = 0; j < pbm->width; j++) {
            if (i)
                letter_dp[i][j] += letter_dp[i - 1][j];
            if (j)
                letter_dp[i][j] += letter_dp[i][j - 1];
            if (i && j)
                letter_dp[i][j] -= letter_dp[i - 1][j - 1];
        }
    }

    /* lines and columns */
    contar_linhas_colunas(letter_height, letter_width);

    /* words */
    const std::vector<std::vector<int>> default_se{{0, 1, 0}, {1, 1, 1}, {0, 1, 0}}; // 3x3 cross
    for (int i = 0; i < 10; i++) {
        pbm->dilation(default_se);
        std::vector<Rectangle> rects = varredura();
        int total_letters = 0;
        for (const Rectangle &rect : rects) {
            int rect_letters = letter_dp[rect.x2][rect.y2];
            if (rect.x1)
                rect_letters -= letter_dp[rect.x1 - 1][rect.y2];
            if (rect.y1)
                rect_letters -= letter_dp[rect.x2][rect.y1 - 1];
            if (rect.x1 && rect.y1)
                rect_letters += letter_dp[rect.x1 - 1][rect.y1 - 1];
            total_letters += rect_letters;
        }
        if (total_letters / rects.size() > 4)
            break;
    }

    std::vector<Rectangle> rects = varredura();
    pbm->data = tmp->data;
    delete tmp;

    for (const Rectangle &rect : rects) {
        for (int i = rect.x1; i <= rect.x2; i++) {
            pbm->data[i][rect.y1] = 1;
            pbm->data[i][rect.y2] = 1;
        }
        for (int i = rect.y1; i <= rect.y2; i++) {
            pbm->data[rect.x1][i] = 1;
            pbm->data[rect.x2][i] = 1;
        }
    }

    std::cout << rects.size() << " palavra(s)" << std::endl;
}
