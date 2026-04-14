#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

double f(double x, double t) {
    return cos(t / (1.0 + x * x) + 0.001 * x);
}

double trapezoid(double t, int n, double a, double b) {
    double h = (b - a) / n;
    double s = 0.5 * (f(a, t) + f(b, t));
    for (int i = 1; i < n; ++i)
        s += f(a + i * h, t);
    return h * s;
}

double runge_rule(double t, double eps, double a, double b, int &iter) {
    int n = 1;
    double I_old = trapezoid(t, n, a, b);
    iter = 1;
    while (true) {
        n *= 2;
        double I_new = trapezoid(t, n, a, b);
        iter++;
        if (fabs(I_new - I_old) < eps || n > 1048576)
            return I_new;
        I_old = I_new;
    }
}

double gauss3(double t, double a, double b) {
    const double x3[] = {-0.7745966692414834, 0.0, 0.7745966692414834};
    const double w3[] = {5.0/9.0, 8.0/9.0, 5.0/9.0};
    double tr = (b - a) / 2;
    double sh = (a + b) / 2;
    double sum = 0;
    for (int i = 0; i < 3; ++i)
        sum += w3[i] * f(tr * x3[i] + sh, t);
    return tr * sum;
}

double gauss4(double t, double a, double b) {
    const double x4[] = {-0.8611363115940526, -0.3399810435848563,
                          0.3399810435848563,  0.8611363115940526};
    const double w4[] = {0.3478548451374538, 0.6521451548625461,
                         0.6521451548625461, 0.3478548451374538};
    double tr = (b - a) / 2;
    double sh = (a + b) / 2;
    double sum = 0;
    for (int i = 0; i < 4; ++i)
        sum += w4[i] * f(tr * x4[i] + sh, t);
    return tr * sum;
}

int main() {
    const double a = -1.0, b = 0.0;
    const double c = 0.5, d = 1.5;
    const int m = 15;
    const double eps = 0.001;
    const double r = (d - c) / m;

    cout << fixed << setprecision(8);
    cout << "j\tt\t\tRunge (trapezoid)\tGauss3\t\tGauss4\t\titer\n";
    cout << "---------------------------------------------------------------\n";

    for (int j = 0; j <= m; ++j) {
        double t = c + j * r;
        int iter;
        double I_runge = runge_rule(t, eps, a, b, iter);
        double I_g3 = gauss3(t, a, b);
        double I_g4 = gauss4(t, a, b);
        cout << j << "\t" << t << "\t" << I_runge << "\t" << I_g3 << "\t" << I_g4 << "\t" << iter << "\n";
    }
    return 0;
}
