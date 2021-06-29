#ifndef PREOCR_H
#define PREOCR_H

#include <vector>

#include "pbm.h"

class PreOCR
{
public:
    PreOCR(PBM *out_pbm);
    ~PreOCR();

    PBM *pbm;

    struct Rectangle {
        int x1, x2;
        int y1, y2;
    };

    void dilatar_colunas();
    std::vector<Rectangle> varredura();

    void etapa1();
    void etapa2();

private:
    std::vector<std::vector<bool>> vis;
    const std::vector<int> dir{1, 0, -1};
};

#endif // PREOCR_H
