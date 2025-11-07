#pragma once

struct Point {
    double x;
    double y;
    double z;
};

struct Vector {
    double x;
    double y;
    double z;
};

Vector getVector(const Point& p1, const Point& p2);
double vectorLength(const Vector& v);
Vector normalizeVector(const Vector& v);
double dotProduct(const Vector& v1, const Vector& v2);  
void printVector(const Vector& v);


