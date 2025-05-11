#pragma once
#include <vector>
#include <fstream>
#include <cstdint>
#include <stdexcept>

namespace BMP {
#pragma pack(push, 1)
struct BMPHeader {
    // BITMAPFILEHEADER (14 bytes)
    uint16_t signature;      // 'BM'
    uint32_t fileSize;       // Размер файла
    uint32_t reserved;       // 0
    uint32_t dataOffset;     // Смещение данных
    
    // BITMAPINFOHEADER (40 bytes)
    uint32_t headerSize;     // 40
    int32_t width;           // Ширина (>0)
    int32_t height;          // Высота (>0)
    uint16_t planes;         // 1
    uint16_t bitsPerPixel;   // 24 или 32
    uint32_t compression;    // 0 (BI_RGB)
    uint32_t imageSize;      // Размер данных пикселей
    int32_t xPixelsPerMeter; // Разрешение (можно 0)
    int32_t yPixelsPerMeter; // Разрешение (можно 0)
    uint32_t colorsUsed;     // 0
    uint32_t colorsImportant;// 0
};
#pragma pack(pop)

struct Coordinate {
    uint32_t x;
    uint32_t y;
    
    bool operator==(const Coordinate& other) const {
        return x == other.x && y == other.y;
    }
    Coordinate() = default;
    Coordinate(uint32_t x, uint32_t y) : x(x), y(y) {}
};

class BMP_Reader {
private:
    std::vector<std::vector<char>> buffer_;
    std::fstream file_;
    Coordinate upperRightAnglePicture_;
    BMPHeader header_;

    bool is_valid_coordinate(const Coordinate& point) const {
        return point.x <= upperRightAnglePicture_.x && 
               point.y <= upperRightAnglePicture_.y;
    }

public:
    BMP_Reader() = default;

    void readHeader();

    void readPixels();

    void read_bmp_file(const std::string &path);
    void create_new_bmp_file(const std::string& path) const;
    void modified_bmp_file(Coordinate& first, Coordinate& second);
    
    uint32_t get_height() const { return buffer_.size(); }
    uint32_t get_width() const { 
        return buffer_.empty() ? 0 : buffer_[0].size(); 
    }
    
    void drawLine(Coordinate start, Coordinate end, char pixel);
    void display() const;
};
}