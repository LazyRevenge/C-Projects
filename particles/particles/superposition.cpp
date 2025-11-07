#include <iostream>
#include "particle.h"

using namespace std;

int main() {
    setlocale(LC_ALL, "russian");
    double a, T;
    Point p0;
    Vector v0;

    cout << "¬ведите длину ребра куба (a): ";
    cin >> a;
    cout << "¬ведите начальную точку (x y z): ";
    cin >> p0.x >> p0.y >> p0.z;
    cout << "¬ведите вектор скорости (x y z): ";
    cin >> v0.x >> v0.y >> v0.z;
    cout << "Enter time (T): ";
    cin >> T;

    if (!isInsideCube(p0, a)) {
        cerr << "ќшибка: начальна€ точка за пределами куба." << endl;
        return 1;
    }

    Point final = calculateFinalPosition(a, p0, v0, T);
    cout << "‘инальна€ позици€: (" << final.x << ", " << final.y << ", " << final.z << ")" << endl;

    return 0;
}


