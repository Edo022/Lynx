#include "LuxEngine/Types/Vectors/Vectors.hpp"


namespace lux{
    struct Octree{
        f32v3 pos;
        Octree* elms[8];
        Octree* parent;
    };
}