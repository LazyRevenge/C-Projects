#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <cmath>

using namespace std;

struct Vector3D {
    double x, y, z;

    void normalize();

    Vector3D reflect(const Vector3D& normal) const;
};
    

class Particle {
public:
    Vector3D position;  
    Vector3D direction; 
    double speed;       
    double cubeSize;    

    Particle(Vector3D pos, Vector3D dir, double spd, double size);
    void update(double time);
};

#endif 
