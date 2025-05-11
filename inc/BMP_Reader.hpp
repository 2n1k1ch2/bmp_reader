#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <compare>
#include <utility>
#include <fstream>
namespace BMP {
#pragma pack(push, 1)
    struct BMPHeader {
        uint16_t signature;
        uint32_t fileSize;
        uint32_t reserved;
        uint32_t dataOffset;
        uint32_t headerSize;
        int32_t width;
        int32_t height;
        uint16_t planes;
        uint16_t bitsPerPixel;
        uint32_t compression;
        uint32_t imageSize;
    };
#pragma pack(pop)

struct Coordinate ;
class BMP_Reader {
    private:
    std::vector<std::vector<char>> buffer_;
    std::fstream file_;
    Coordinate upperRightAnglePicture_;
    Coordinate lowerLeftAnglePicture_;
    BMPHeader header_;
    void readHeader();
    void readPixels();
    public:
    BMP_Reader();
    void read_bmp_file(const std::string& path);
    void modified_bmp_file(Coordinate& first ,Coordinate& second);
    void create_new_bmp_file(const std::string& path) const;

    void drawLine( Coordinate start, Coordinate end, char pixel) ;
    void display() const;
    bool is_valid_coordinate(Coordinate &point, Coordinate &angle) const
    {
        return (point.x>angle.x || point.y>angle.y ?  false :  true);
    }
};
    
struct Coordinate{
    uint32_t x;
    uint32_t y;  
    

    bool operator==(const Coordinate& other) const {
        return x == other.x && y == other.y;
    }
    explicit Coordinate (uint32_t x,uint32_t y) : x(x) , y(y) {};
    Coordinate(const Coordinate& other) = default;
    Coordinate(Coordinate&& other) noexcept
        : x(std::exchange(other.x, 0)),
          y(std::exchange(other.y, 0)) {}

    Coordinate& operator=(Coordinate&& other) noexcept {
        x = std::exchange(other.x, 0);
        y = std::exchange(other.y, 0);
        return *this;}
};

 

}
