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

void PreOCR::dilatar_colunas() {
}

void PreOCR::etapa1() {
    pbm->median(3);
}

void PreOCR::etapa2() {
    PBM *tmp = new PBM(*pbm);

    etapa1();

    std::vector<Rectangle> letter_rects = varredura();
    std::vector<std::vector<int>> letter_dp(pbm->height, std::vector<int>(pbm->width, 0));
    for (const Rectangle &rect : letter_rects)
        letter_dp[rect.x2][rect.y1] += 1;
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

    std::vector<std::vector<int>> default_se{{0, 1, 0}, {1, 1, 1}, {0, 1, 0}}; // 3x3 cross
    for (int i = 0; i < 3; i++)
        pbm->dilation(default_se);
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
}
