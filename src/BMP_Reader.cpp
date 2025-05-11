#include "BMP_Reader.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <stdexcept>

namespace BMP {

void BMP_Reader::readHeader() {
    file_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
    if (header_.signature != 0x4D42) throw std::runtime_error("Not a BMP file");
    if (header_.bitsPerPixel != 24 && header_.bitsPerPixel != 32)
        throw std::runtime_error("Only 24/32-bit BMP supported");
    
    upperRightAnglePicture_ = {header_.width - 1, header_.height - 1};
    std::cout << "Width: " << upperRightAnglePicture_.x << '\n' 
              << "Height: " << upperRightAnglePicture_.y << '\n';
}

void BMP_Reader::readPixels() {
    file_.seekg(header_.dataOffset, std::ios::beg);
    buffer_.resize(header_.height, std::vector<char>(header_.width, ' '));

    const int bytesPerPixel = header_.bitsPerPixel / 8;
    const int rowSize = ((header_.width * bytesPerPixel + 3) / 4) * 4;
    std::vector<uint8_t> row(rowSize);

    for (int y = header_.height - 1; y >= 0; y--) {
        file_.read(reinterpret_cast<char*>(row.data()), rowSize);

        for (int x = 0; x < header_.width; x++) {
            const int offset = x * bytesPerPixel;
            const uint8_t b = row[offset];
            const uint8_t g = row[offset + 1];
            const uint8_t r = row[offset + 2];

            // Более гибкая проверка цветов
            const bool isBlack = (r <= 10 && g <= 10 && b <= 10);
            const bool isWhite = (r >= 250 && g >= 250 && b >= 250);

            if (isBlack) {
                buffer_[y][x] = '*';
            } else if (isWhite) {
                buffer_[y][x] = ' ';
            } else {
                std::cerr << "Non B/W pixel at (" << x << "," << y << "): "
                          << "R=" << (int)r << " G=" << (int)g << " B=" << (int)b << "\n";
                throw std::runtime_error("Image contains non-black/white pixels");
            }
        }
    }
}

void BMP_Reader::read_bmp_file(const std::string &path) {
    if (file_.is_open()) file_.close();
    
    file_.open(path, std::ios::in | std::ios::binary);
    if (!file_.is_open()) throw std::runtime_error("Failed to open file: " + path);
    
    readHeader();
    readPixels();
    file_.close();
}

void BMP_Reader::create_new_bmp_file(const std::string &path) const {
    if (buffer_.empty()) throw std::runtime_error("No image data to save");

    std::ofstream outFile(path, std::ios::binary);
    if (!outFile) throw std::runtime_error("Failed to create output file: " + path);

    BMPHeader newHeader = header_;
    const int bytesPerPixel = header_.bitsPerPixel / 8;
    const int rowSize = ((header_.width * bytesPerPixel + 3) / 4) * 4;
    
    newHeader.imageSize = rowSize * header_.height;
    newHeader.fileSize = sizeof(BMPHeader) + newHeader.imageSize;
    newHeader.dataOffset = sizeof(BMPHeader);

    outFile.write(reinterpret_cast<const char*>(&newHeader), sizeof(newHeader));

    std::vector<uint8_t> row(rowSize, 0);
    for (int y = header_.height - 1; y >= 0; y--) {
        for (int x = 0; x < header_.width; x++) {
            const int offset = x * bytesPerPixel;
            if (buffer_[y][x] == '*') {
                row[offset] = row[offset+1] = row[offset+2] = 0;
            } else {
                row[offset] = row[offset+1] = row[offset+2] = 255;
            }
            if (bytesPerPixel == 4) row[offset+3] = 255;
        }
        outFile.write(reinterpret_cast<const char*>(row.data()), rowSize);
    }
}

void BMP_Reader::modified_bmp_file(Coordinate& leftLower, Coordinate& rightUpper) {
    if (buffer_.empty() || buffer_[0].empty()) {
        std::cout << "Empty buffer";
        return;
    }
    if (!is_valid_coordinate(leftLower) || !is_valid_coordinate(rightUpper)) {
        throw std::runtime_error("bad coordinate");
    }
    
    Coordinate leftUpper(leftLower.x, rightUpper.y);
    Coordinate rightLower(rightUpper.x, leftLower.y);
    
    drawLine(leftLower, rightUpper, '*');
    drawLine(rightLower, leftUpper, '*');
}

void BMP_Reader::drawLine(Coordinate start, Coordinate end, char pixel) {
    if (!is_valid_coordinate(start) || !is_valid_coordinate(end)) {
        std::cout << "Invalid coordinates for line drawing\n";
        return;
    }
    
    int dx = abs(end.x - start.x);
    int dy = -abs(end.y - start.y);
    int sx = (start.x < end.x) ? 1 : -1;
    int sy = (start.y < end.y) ? 1 : -1;
    int err = dx + dy;

    while (true) {
        buffer_[start.y][start.x] = pixel;
        if (start == end) break;
        
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; start.x += sx; }
        if (e2 <= dx) { err += dx; start.y += sy; }
    }
}

void BMP_Reader::display() const {
    if (buffer_.empty()) {
        std::cout << "No image to display";
        return;
    }
    for (const auto& row : buffer_) {
        for (char c : row) std::cout << c;
        std::cout << '\n';
    }
}
}