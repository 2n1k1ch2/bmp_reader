#pragma once
#include "inc/BMP_Reader.hpp"
#include <vector>
#include <fstream>
#include <memory>
#include "BMP_Reader.hpp"
namespace BMP{
    
    void BMP_Reader::readHeader() {
            file_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
            if (header_.signature != 0x4D42) throw std::runtime_error("Not a BMP file");
            if (header_.bitsPerPixel != 24 && header_.bitsPerPixel != 32)
                throw std::runtime_error("Only 24/32-bit BMP supported");
            
            upperRightAnglePicture_.x = header_.width - 1;
            upperRightAnglePicture_.y = header_.height - 1;
            std::cout<<"Width: "<<upperRightAnglePicture_.x<<'\n'<<"Height: "<<upperRightAnglePicture_.y<<'\n';
        }

    void BMP_Reader::readPixels() {
        file_.seekg(header_.dataOffset, std::ios::beg);
        buffer_.resize(header_.height, std::vector<char>(header_.width, ' '));
            
        int rowSize = ((header_.width * header_.bitsPerPixel + 31) / 32) * 4;
        std::vector<uint8_t> row(rowSize);
            
        for (int y = 0; y < header_.height; y++) {
            file_.read(reinterpret_cast<char*>(row.data()), rowSize);
            for (int x = 0; x < header_.width; x++) {
                int offset = x * (header_.bitsPerPixel / 8);
                uint8_t r = row[offset + 2];
                uint8_t g = row[offset + 1];
                uint8_t b = row[offset];
                    
                if (r == 0 && g == 0 && b == 0) buffer_[y][x] = '#';
                else if (!(r == 255 && g == 255 && b == 255))
                    throw std::runtime_error("Image must be black and white only");
                }
            }
        }

    void BMP_Reader::read_bmp_file(const std::string &path) {
            if (file_.is_open()) {
                file_.close();
            }
            file_.open(path, std::ios::in | std::ios::binary);
            if (!file_.is_open()) {
                throw std::runtime_error("Failed to open file: " + path);
            }
            
            readHeader();
            readPixels();
            file_.close();
        }


    void BMP_Reader::modified_bmp_file(Coordinate& leftLower ,Coordinate& rightUpper){

        if(buffer_.empty() || buffer_[0].empty()) {
            std::cout<<"Empty buffer";
            return ;
        }
        if (is_valid_coordinate(leftLower,upperRightAnglePicture_)&& is_valid_coordinate(rightUpper,upperRightAnglePicture_)) {
            std::cout<<"Bad coordinate";
            return ;
        }
        
        Coordinate leftUpper=Coordinate(leftLower.x,rightUpper.y);
        Coordinate rightLower=Coordinate(rightUpper.x,leftLower.y);
        
        drawLine(leftLower,rightUpper,'*');
        drawLine(rightLower,leftUpper,'*');

    }

    void BMP_Reader::create_new_bmp_file(const std::string &path) const {
    if (buffer_.empty()) {
        throw std::runtime_error("No image data to save");
    }

    std::ofstream outFile(path, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to create output file: " + path);
    }

    outFile.write(reinterpret_cast<const char*>(&header_), sizeof(header_));

    const int bytesPerPixel = header_.bitsPerPixel / 8;
    const int rowSize = ((header_.width * bytesPerPixel + 3) / 4) * 4;
    const int paddingSize = rowSize - (header_.width * bytesPerPixel);


    std::vector<uint8_t> row(rowSize, 0);

    for (int y = 0; y < header_.height; y++) {
        for (int x = 0; x < header_.width; x++) {
            const int offset = x * bytesPerPixel;
            if (buffer_[y][x] == '#') {
                row[offset] = 0;
                row[offset+1] = 0;
                row[offset+2] = 0;
            } 
            else if (buffer_[y][x] == '*') {
                row[offset] = 0;
                row[offset+1] = 0;
                row[offset+2] = 255;
            }
            else {
                row[offset] = 255;
                row[offset+1] = 255;
                row[offset+2] = 255;
            }
            if (bytesPerPixel == 4) {
                row[offset+3] = 255;
            }
        }
        

        outFile.write(reinterpret_cast<const char*>(row.data()), rowSize);
    }

    outFile.close();
}

    void BMP_Reader::drawLine(Coordinate start, Coordinate end, char pixel){
        int dx = abs(end.x - start.x);
        int dy = -abs(end.y - start.y);
        int sx = (start.x < end.x) ? 1 : -1;
        int sy = (start.y < end.y) ? 1 : -1;
        int err = dx + dy;

        while (true) {
            buffer_[start.y][start.x] = pixel;

            if (start == end) break;
            int e2 = 2 * err;

            if (e2 >= dy) {
                err += dy;
                start.x += sx;
            }
            if (e2 <= dx) {
                err += dx;
                start.y += sy;
            }
        }
    }
    void BMP_Reader::display() const {
        if(buffer_.empty()) {std::cout<<"No image to display"; return ;}
        for (const auto& row : buffer_) {
                for (char c : row) std::cout << c;
                std::cout << '\n';
        }
    }
    

}