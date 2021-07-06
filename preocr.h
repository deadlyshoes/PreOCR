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

    /* utilizado na varredura */
    struct Rectangle {
        int x1, x2;
        int y1, y2;
    };

    void contar_linhas_colunas(int letter_height, int letter_width);
    std::vector<Rectangle> varredura();

    void etapa1();
    void etapa2();

private:
    /* utilizado na varredura */
    std::vector<std::vector<bool>> vis; // lembrar os visitados
    const std::vector<int> dir{1, 0, -1}; // utilizado para navegar para pixels acesos vizinhos
};

#endif // PREOCR_H
