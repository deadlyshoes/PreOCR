#include <iostream>

#include "ppm.h"

int main(int argc, char **argv) {
    std::string image_path;
    std::cout << "Caminho da imagem: ";
    std::cin >> image_path;

    std::cout << "Aplicando filtro da mediana 3x3..." << std::endl;

    PPM ppm(image_path);
    ppm.median(3);

    std::string median_image_path;
    std::cout << "Salvar como: ";
    std::cin >> median_image_path;
    ppm.write(median_image_path);

    return 0;
}
