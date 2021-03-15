#include "LuxEngine/Types/Vectors/Vectors.hpp"




namespace lux{
    struct Octree{
        f32v3 pos;          //Sphere position
        f32   r;            //Sphere radius

        u32 elmsNum;        //Number of childre //TODO use static array
        Octree* elms[8];    //Cell children
        Octree* parent;     //Parent cell

        struct Opt{         //Optimization structures
            Octree *xp;         //Adjacent cell on the positive x axis
            Octree *xn;         //Adjacent cell on the negative x axis
            Octree *yp;         //Adjacent cell on the positive y axis
            Octree *yn;         //Adjacent cell on the negative y axis
            Octree *zp;         //Adjacent cell on the positive z axis
            Octree *zn;         //Adjacent cell on the negative z axis
        } opt;

        // inline bool isHbx() const noexcept { return r != 0; } //Returns true if the cell is an hitbox or can be used as such
        inline bool isBbx() const noexcept {  } //Returns true if the cell can be used as bounding box
    };
}