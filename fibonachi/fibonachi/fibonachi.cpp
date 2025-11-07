#include <iostream>
using namespace std;

void multiplyMatrices(int a[2][2], int b[2][2]) {
    int result[2][2];
    result[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0];
    result[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1];
    result[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0];
    result[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1];

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            a[i][j] = result[i][j];
}

void powerMatrix(int matrix[2][2], int n) {
    if (n <= 1)
        return;

    int base[2][2] = { {1, 1}, {1, 0} };

    powerMatrix(matrix, n / 2);
    multiplyMatrices(matrix, matrix);

    if (n % 2 != 0)
        multiplyMatrices(matrix, base);
}

int fibonacci(int n) {
    if (n == 0)
        return 0;

    int matrix[2][2] = { {1, 1}, {1, 0} };

    powerMatrix(matrix, n); 
    cout << matrix[0][0] << ' ' << matrix[0][1] << endl << matrix[1][0] << ' ' << matrix[1][1] << endl;

    return matrix[0][1];
}

int main() {
    int n;
    cout << "Enter n: ";
    cin >> n;

    cout << "Fibonacci number " << n << " is: " << fibonacci(n) << endl;

    return 0;
}
