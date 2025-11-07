#include "vector_algebra.h"
#include <iostream>
#include <cmath>

using namespace std;

double vectorLength(const Vector& v) { 
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector normalizeVector(const Vector& v) { 
    double len = vectorLength(v);
    if (len == 0) {
        cerr << "Ошибка: нельзя нормализировать нулевой вектор." << endl;
        exit(1);
    }
    return { v.x / len, v.y / len, v.z / len };
}

double dotProduct(const Vector& v1, const Vector& v2) { 
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

void printVector(const Vector& v) {
    cout << "<" << v.x << ", " << v.y << ", " << v.z << ">";
}