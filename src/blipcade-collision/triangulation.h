//
// Created by Pavlo Yevsehnieiev
//

#ifndef TRIANGULATION_H
#define TRIANGULATION_H
#include <raylib.h>
#include <vector>

namespace blipcade {
namespace collision {

    using Coord = double;
    using Point = std::array<Coord, 2>;
    using N = uint32_t;

class Triangulation {
public:
    Triangulation() = default;
    ~Triangulation() = default;

    static std::vector<unsigned> triangulate(const std::vector<Vector2> &vertices);

   static float computeSignedArea(const std::vector<Vector2> &vertices);

private:
    static std::vector<std::vector<Point> > preparePolygon(const std::vector<Vector2> &vertices);
};

} // collision
} // blipcade

#endif //TRIANGULATION_H
