#include "geometry.h"

int main() {
    setlocale(LC_ALL, "russian");
    double cubeSize, T, speed;
    Vector3D p0, u0;

    cout << "Введите размер куба: ";
    cin >> cubeSize;

    cout << "Введите начальную позицию (x y z): ";
    cin >> p0.x >> p0.y >> p0.z;

    cout << "Введите направление движения (x y z): ";
    cin >> u0.x >> u0.y >> u0.z;

    cout << "Введите скорость: ";
    cin >> speed;

    cout << "Введите время движения: ";
    cin >> T;

    Particle particle(p0, u0, speed, cubeSize);
    particle.update(T);

    cout << "Позиция частицы через " << T << " секунд: ("
        << particle.position.x << ", "
        << particle.position.y << ", "
        << particle.position.z << ")\n";

    return 0;
}










































//#include <iostream>
//#include <cmath>
//#include <vector>
//#include <stdlib.h>
//
//using namespace std;
//
//;
//
//int sizeofcube(int a) {
//	int sizecube;
//	if (a > 0){
//		sizecube = a * a * a;
//	}
//	if (a == 0) {
//		cout << "Куб не существует";
//	}
//	if (a < 0) {
//		sizecube = abs(a * a * a);
//	}
//	return sizecube;
//}
//
//
//
//
//int main() {
//	int a, x, y, z, t, s;
//	cout << "length cube: ";
//	cin >> a;
//	cout << "coordinates: ";
//	cin >> x >> y >> z;
//	cout << "time: ";
//	cin >> t;
//	cout << "speed: ";
//	cin >> s;
//	cout << sizeofcube(a) << endl;
//	return 0;
//}