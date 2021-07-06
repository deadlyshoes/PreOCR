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

    pbm->write(argv[2]);

    return 0;
}
