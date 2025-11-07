#include "geometry.h"

void Vector3D::normalize() {
    double length = sqrt(x * x + y * y + z * z);
    if (length > 0) {
        x /= length;
        y /= length;
        z /= length;
    }
}
int Cube(int a) {
    int sizeofcube;
    if (a > 0) {
        sizeofcube = a * a * a;
    }
    if (a == 0) {
        cout << "Куб не существует";
    }
    if (a < 0) {
        sizeofcube = abs(a * a * a);
    }
    return sizeofcube;
}

Vector3D Vector3D::reflect(const Vector3D& normal) const {
    double dotProduct = x * normal.x + y * normal.y + z * normal.z;
    return { x - 2 * dotProduct * normal.x,
            y - 2 * dotProduct * normal.y,
            z - 2 * dotProduct * normal.z };
}

Particle::Particle(Vector3D pos, Vector3D dir, double spd, double size)
    : position(pos), direction(dir), speed(spd), cubeSize(size) {
    direction.normalize();
}

void Particle::update(double time) {
    double dt = 0.01;
    double elapsed = 0; 

    while (elapsed < time) {
        double step = min(dt, time - elapsed); 

        position.x += direction.x * speed * step;
        position.y += direction.y * speed * step;
        position.z += direction.z * speed * step;

        bool (position.x < 0 || position.x > cubeSize);
        bool (position.y < 0 || position.y > cubeSize);
        bool (position.z < 0 || position.z > cubeSize);

        elapsed += step; 
    }
}

