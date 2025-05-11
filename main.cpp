#include "BMP_Reader.hpp"
#include <iostream>
#include <string>


int main() {
    BMP::BMP_Reader bmp;
    std::string input_path_file;


    std::cout << "Enter input BMP file name: ";
    std::cin >> input_path_file;

    try {

        bmp.read_bmp_file(input_path_file);

        
        bmp.display();

        std::cout << "\n\nEnter 2 coordinates to draw an X: ";
        uint32_t x1, y1, x2, y2;
        std::cin >> x1 >> y1 >> x2 >> y2;

        if (x1 >= bmp.get_width() || x2 >= bmp.get_width() || y1 >= bmp.get_height() || y2 >= bmp.get_height()) {
            std::cerr << "Error: Coordinates out of bounds!" << std::endl;
            return 1; 
        }

        BMP::Coordinate first(x1, y1);
        BMP::Coordinate second(x2, y2);

        bmp.modified_bmp_file(first, second);
        std::cout<<"\n\n";
        bmp.display();
        std::cout << "Enter path to save the modified file: ";
        std::string output_path_file;
        std::cin >> output_path_file;

        bmp.create_new_bmp_file(output_path_file);

        std::cout << "Image saved successfully!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;  
    }

    return 0;
}