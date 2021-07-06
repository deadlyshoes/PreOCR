#include <iostream>
#include <algorithm>

#include "pbm.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "PreOCL [caminho da imagem] [caminho da imagem filtrada]" << std::endl;
        return 1;
    }

    std::cout << "Aplicando filtro da mediana 3x3..." << std::endl;

    PBM *pbm = NULL;
    pbm = new PBM(argv[1]);
    if (pbm == NULL) {
        std::cout << "Erro na leitura do PBM" << std::endl;
        return 1;
    }

    pbm->median(3);

    std::vector<std::vector<int>> default_se{{0, 1, 0}, {1, 1, 1}, {0, 1, 0}};
    for (int i = 0; i < 4; ++i)
    {
        pbm->dilation(default_se);
    }
    int l = pbm->count_lines();
    std::cout << l;
    pbm->write(argv[2]);

    return 0;
}
