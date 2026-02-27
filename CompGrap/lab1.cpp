#include <iostream>
#include <cmath>

const double EPS = 1e-9;

using namespace std;

struct Point2D {
    double x, y;
    Point2D(double x = 0, double y = 0) : x(x), y(y) {}
};

struct Point3D {
    double x, y, z;
    Point3D(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
};

// Задание 1
bool isPointOnLine(double A, double B, double C, const Point2D& P) {
    return fabs(A * P.x + B * P.y + C) < EPS;
}


// Задание 2
bool isPointOnRay(const Point2D& A, const Point2D& B, const Point2D& C) {
    double ABx = B.x - A.x;
    double ABy = B.y - A.y;
    double ACx = C.x - A.x;
    double ACy = C.y - A.y;

    double cross = ABx * ACy - ABy * ACx;
    if (fabs(cross) > EPS) 
        return false; // не коллинеарны

    // Проверка сонаправленности: скалярное произведение >= 0
    double dot = ABx * ACx + ABy * ACy;
    if (dot < -EPS) 
        return false; // противоположное направление

    if (fabs(ABx) < EPS && fabs(ABy) < EPS) {
        // A и B совпадают: луч — это точка A
        return fabs(ACx) < EPS && fabs(ACy) < EPS;
    }

    return true;
}

// Задание 3
int orientation(const Point2D& A, const Point2D& B, const Point2D& C) {
    double val = (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
    if (fabs(val) < EPS) 
        return 0;
    return (val > 0) ? 1 : -1;
}

// Задание 4
bool isPointOnPlane(double A, double B, double C, double D, const Point3D& P) {
    return fabs(A * P.x + B * P.y + C * P.z + D) < EPS;
}

int main() {
    // Задание 1
    setlocale(LC_ALL, "Russian");
    cout << "Задание 1: прямая 2x - 3y + 1 = 0\n";
    Point2D p1(1, 1);          
    Point2D p2(2, 2);         
    cout << "Точка (1,1): " << (isPointOnLine(2, -3, 1, p1) ? "принадлежит" : "не принадлежит") << "\n";
    cout << "Точка (2,2): " << (isPointOnLine(2, -3, 1, p2) ? "принадлежит" : "не принадлежит") << "\n\n";

    // Задание 2
    cout << "Задание 2: луч из A(0,0) через B(2,0)\n";
    Point2D A(0, 0), B(2, 0);
    Point2D C1(3, 0);  
    Point2D C2(-1, 0); 
    Point2D C3(1, 1);   
    cout << "Точка (3,0): " << (isPointOnRay(A, B, C1) ? "принадлежит лучу" : "не принадлежит") << "\n";
    cout << "Точка (-1,0): " << (isPointOnRay(A, B, C2) ? "принадлежит лучу" : "не принадлежит") << "\n";
    cout << "Точка (1,1): " << (isPointOnRay(A, B, C3) ? "принадлежит лучу" : "не принадлежит") << "\n\n";

    // Задание 3
    cout << "Задание 3: ориентация точек A(0,0), B(1,0), C(0,1)\n";
    Point2D A3(0, 0), B3(1, 0), C3_1(0, 1); 
    Point2D C3_2(1, 1);                    

    cout << "A(0,0) B(1,0) C(0,1): ";
    int ori = orientation(A3, B3, C3_1);
    if (ori == 1)
        cout << "против часовой стрелки\n";
    else if (ori == -1) 
        cout << "по часовой стрелке\n";
    else 
        cout << "коллинеарны\n";

    Point2D C3_3(1, -1);
    cout << "A(0,0) B(1,0) C(1,-1): ";
    ori = orientation(A3, B3, C3_3);
    if (ori == 1) 
        cout << "против часовой стрелки\n";
    else if (ori == -1) 
        cout << "по часовой стрелке\n";
    else 
        cout << "коллинеарны\n\n";

    // Задание 4
    cout << "\nЗадание 4: плоскость x + y + z - 1 = 0\n";
    Point3D P4_1(0, 0, 1);   // 0+0+1-1=0 -> принадлежит
    Point3D P4_2(1, 1, 1);   // 1+1+1-1=2 -> не принадлежит
    cout << "Точка (0,0,1): " << (isPointOnPlane(1, 1, 1, -1, P4_1) ? "принадлежит" : "не принадлежит") << "\n";
    cout << "Точка (1,1,1): " << (isPointOnPlane(1, 1, 1, -1, P4_2) ? "принадлежит" : "не принадлежит") << "\n";

    return 0;
}
