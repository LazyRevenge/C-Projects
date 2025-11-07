#include "particle.h"
#include "vector_algebra.h"
#include <vector>
#include <iostream>  

using namespace std;

static Vector reflectVelocity(const Vector& v, const Vector& normal) {
    double dot = dotProduct(v, normal);
    return {
        v.x - 2 * dot * normal.x,
        v.y - 2 * dot * normal.y,
        v.z - 2 * dot * normal.z
    };
}

bool isInsideCube(const Point& p, double a) {
    const double eps = 1e-9; 
    return (p.x >= -eps && p.x <= a + eps) &&
        (p.y >= -eps && p.y <= a + eps) &&
        (p.z >= -eps && p.z <= a + eps);
}

Point calculateFinalPosition(double a, Point p0, Vector v0, double T) {
    const double INF = 1e20;

    if (!isInsideCube(p0, a)) {
        cerr << "Ошибка: начальная точка за пределами куба" << endl;
        exit(1);
    }

    Vector velocity = v0;
    double remaining_time = T;

    while (remaining_time > 0) {
        double times[3];

        for (int i = 0; i < 3; ++i) {
            double pos = (i == 0) ? p0.x : (i == 1) ? p0.y : p0.z;
            double vel = (i == 0) ? velocity.x : (i == 1) ? velocity.y : velocity.z;

            if (vel == 0) {
                times[i] = INF;
            }
            else if (vel > 0) {
                times[i] = (a - pos) / vel;
                if (abs(pos - a) < 1e-9) times[i] = 0;  
            }
            else {
                times[i] = pos / (-vel);
                if (abs(pos) < 1e-9) times[i] = 0;  
            }
        }

        double t_min = INF;
        for (double t : times) {
            if (t >= 0 && t < t_min) t_min = t;  
        }

        if (t_min > remaining_time) {
            p0.x += velocity.x * remaining_time;
            p0.y += velocity.y * remaining_time;
            p0.z += velocity.z * remaining_time;
            remaining_time = 0;
        }
        else {
            p0.x += velocity.x * t_min;
            p0.y += velocity.y * t_min;
            p0.z += velocity.z * t_min;
            remaining_time -= t_min;

            vector<Vector> normals;
            for (int i = 0; i < 3; ++i) {
                if (abs(times[i] - t_min) < 1e-9) {
                    if (i == 0) {
                        normals.push_back(velocity.x > 0 ? Vector{ -1,0,0 } : Vector{ 1,0,0 });
                    }
                    else if (i == 1) {
                        normals.push_back(velocity.y > 0 ? Vector{ 0,-1,0 } : Vector{ 0,1,0 });
                    }
                    else {
                        normals.push_back(velocity.z > 0 ? Vector{ 0,0,-1 } : Vector{ 0,0,1 });
                    }
                }
            }

            Vector sum_normal = { 0,0,0 };
            for (const auto& n : normals) {
                sum_normal.x += n.x;
                sum_normal.y += n.y;
                sum_normal.z += n.z;
            }

            Vector normal = normalizeVector(sum_normal);
            velocity = reflectVelocity(velocity, normal);
        }
    }

    return p0;
}