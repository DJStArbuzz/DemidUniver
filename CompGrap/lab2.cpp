#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <functional>
#include <iomanip>
#include <limits>

#ifdef _WIN32
#include <windows.h> /
#endif
#define DEBUG_BBOX

using namespace std;
using namespace sf;

// Точка в однородных координатах (x, y, 1)
class Point {
public:
    float coords[3];

    Point(float x = 0, float y = 0) {
        coords[0] = x;
        coords[1] = y;
        coords[2] = 1.0f;
    }

    float x() const { return coords[0]; }
    float y() const { return coords[1]; }
};

// Фигура – набор точек
class Figure {
public:
    vector<Point> points;

    Figure(const vector<Point>& pts) : points(pts) {}
};

// Умножение матрицы 3x3 на вектор-строку (p * M)
Point multiplyMatrix(const Point& p, const float mat[3][3]) {
    float x = p.coords[0];
    float y = p.coords[1];
    float w = p.coords[2];

    float nx = x * mat[0][0] + y * mat[1][0] + w * mat[2][0];
    float ny = x * mat[0][1] + y * mat[1][1] + w * mat[2][1];
    float nw = x * mat[0][2] + y * mat[1][2] + w * mat[2][2];

    if (abs(nw - 1.0f) > 1e-6) {
        nx /= nw;
        ny /= nw;
    }
    return Point(nx, ny);
}

// Матрицы преобразований
// перенос
void translationMatrix(float dx, float dy, float mat[3][3]) {
    mat[0][0] = 1; mat[0][1] = 0; mat[0][2] = 0;
    mat[1][0] = 0; mat[1][1] = 1; mat[1][2] = 0;
    mat[2][0] = dx; mat[2][1] = dy; mat[2][2] = 1;
}
// отражение относительно оси OX
void reflectionOXMatrix(float mat[3][3]) {
    mat[0][0] = 1; mat[0][1] = 0; mat[0][2] = 0;
    mat[1][0] = 0; mat[1][1] = -1; mat[1][2] = 0;
    mat[2][0] = 0; mat[2][1] = 0; mat[2][2] = 1;
}
// отражение относительно оси OY
void reflectionOYMatrix(float mat[3][3]) {
    mat[0][0] = -1; mat[0][1] = 0; mat[0][2] = 0;
    mat[1][0] = 0; mat[1][1] = 1; mat[1][2] = 0;
    mat[2][0] = 0; mat[2][1] = 0; mat[2][2] = 1;
}
// отражение относительно y = x
void reflectionYXMatrix(float mat[3][3]) {
    mat[0][0] = 0; mat[0][1] = 1; mat[0][2] = 0;
    mat[1][0] = 1; mat[1][1] = 0; mat[1][2] = 0;
    mat[2][0] = 0; mat[2][1] = 0; mat[2][2] = 1;
}
// масштабирование
void scalingMatrix(float sx, float sy, float mat[3][3]) {
    mat[0][0] = sx; mat[0][1] = 0; mat[0][2] = 0;
    mat[1][0] = 0; mat[1][1] = sy; mat[1][2] = 0;
    mat[2][0] = 0; mat[2][1] = 0; mat[2][2] = 1;
}
// поворот
void rotationMatrix(float angleDeg, float mat[3][3]) {
    float rad = angleDeg * 3.14159265f / 180.0f;
    float c = cos(rad);
    float s = sin(rad);
    mat[0][0] = c; mat[0][1] = s; mat[0][2] = 0;
    mat[1][0] = -s; mat[1][1] = c; mat[1][2] = 0;
    mat[2][0] = 0; mat[2][1] = 0; mat[2][2] = 1;
}
// перемножение матриц
void multiplyMatrices(const float a[3][3], const float b[3][3], float result[3][3]) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result[i][j] = 0;
            for (int k = 0; k < 3; ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}
// поворот вокруг точки
void rotationAroundPointMatrix(float angleDeg, float x0, float y0, float mat[3][3]) {
    float t1[3][3], r[3][3], t2[3][3], temp[3][3];
    translationMatrix(-x0, -y0, t1);
    rotationMatrix(angleDeg, r);
    translationMatrix(x0, y0, t2);
    multiplyMatrices(t1, r, temp);
    multiplyMatrices(temp, t2, mat);
}

void applyTransform(vector<Figure>& figures, const float mat[3][3]) {
    for (auto& fig : figures) {
        for (auto& p : fig.points) {
            p = multiplyMatrix(p, mat);
        }
    }

#ifdef DEBUG_BBOX
    float minX = numeric_limits<float>::max();
    float minY = numeric_limits<float>::max();
    float maxX = -numeric_limits<float>::max();
    float maxY = -numeric_limits<float>::max();
    for (const auto& fig : figures) {
        for (const auto& p : fig.points) {
            if (p.x() < minX) minX = p.x();
            if (p.x() > maxX) maxX = p.x();
            if (p.y() < minY) minY = p.y();
            if (p.y() > maxY) maxY = p.y();
        }
    }
    //cout << fixed << setprecision(1);
    //cout << "Границы: X [" << minX << ", " << maxX << "], Y [" << minY << ", " << maxY << "]\n";
#endif
}

vector<Figure> createOriginalFigures() {
    vector<Figure> figs;

    Point center(0, 0);
    // Внешние точки
    vector<Point> outer = {
        Point(0, 60),
        Point(0, -60),
        Point(40, 30),
        Point(-40, -30),
        Point(40, -30),
        Point(-40, 30)
    };
    // Внутренние точки 
    vector<Point> inner = {
        Point(10, 20),
        Point(-10, -20),
        Point(-10, 20),
        Point(10, -20),
        Point(20, 0),
        Point(-20, 0)
    };

    vector<Point> allPoints;
    allPoints.insert(allPoints.end(), outer.begin(), outer.end());
    allPoints.insert(allPoints.end(), inner.begin(), inner.end());

    sort(allPoints.begin(), allPoints.end(), [](const Point& a, const Point& b) {
        double angleA = atan2(a.y(), a.x());
        double angleB = atan2(b.y(), b.x());
        return angleA > angleB;  
        });

    for (const auto& pt : inner) {
        figs.push_back(Figure({ center, pt }));
    }
    figs.push_back(Figure(allPoints));

    return figs;
}

struct Button {
    FloatRect rect;
    string label;
    function<void()> action;
};


int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    const int WINDOW_WIDTH = 1000;
    const int WINDOW_HEIGHT = 600;
    const int BUTTON_WIDTH = 200;
    const int CENTER_X = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;
    const int CENTER_Y = WINDOW_HEIGHT / 2;

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2D Transformations");
    window.setFramerateLimit(60);

    vector<Figure> originalFigs = createOriginalFigures();
    vector<Figure> figure = originalFigs;
    Font font;
    bool fontLoaded = false;
    vector<string> fontPaths = {
        "Мирослав Regular.ttf",                              
        "C:/Windows/Fonts/arial.ttf",                 
        "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf",  
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/System/Library/Fonts/Arial.ttf"                
    };

    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }
    if (!font.loadFromFile("C:\\fonts\\Miroslav.ttf")) {
        cerr << "Шрифт не загружен, текст не отобразится\n";
    }

    vector<Button> buttons;
    float buttonY = 20.0f;
    const float buttonHeight = 40.0f;
    const float buttonSpacing = 5.0f;

    auto addButton = [&](const string& label, function<void()> action) {
        Button btn;
        btn.rect = FloatRect(WINDOW_WIDTH - BUTTON_WIDTH + 10, buttonY, BUTTON_WIDTH - 20, buttonHeight);
        btn.label = label;
        btn.action = action;
        buttons.push_back(btn);
        buttonY += buttonHeight + buttonSpacing;
        };
    addButton("Sdvig OH", [&]() {
        float dx;
        cout << "Введите смещение по оси OX: ";
        cin >> dx;
        float mat[3][3];
        translationMatrix(dx, 0, mat);
        applyTransform(figure, mat);
        });

    addButton("Sdvig OU", [&]() {
        float dy;
        cout << "Введите смещение по оси OY: ";
        cin >> dy;
        float mat[3][3];
        translationMatrix(0, dy, mat);
        applyTransform(figure, mat);
        });

    addButton("Otra`enie OH", [&]() {
        float mat[3][3];
        reflectionOXMatrix(mat);
        applyTransform(figure, mat);
        });

    addButton("Otra`enie OU", [&]() {
        float mat[3][3];
        reflectionOYMatrix(mat);
        applyTransform(figure, mat);
        });

    addButton("Otra`enie H=U", [&]() {
        float mat[3][3];
        reflectionYXMatrix(mat);
        applyTransform(figure, mat);
        });

    addButton("Mass{tab", [&]() {
        float sx, sy;
        cout << "Введите масштаб по X: ";
        cin >> sx;
        cout << "Введите масштаб по Y: ";
        cin >> sy;
        float mat[3][3];
        scalingMatrix(sx, sy, mat);
        applyTransform(figure, mat);
        });

    addButton("Povorot (centr)", [&]() {
        float ang;
        cout << "Введите угол в градусах (против часовой стрелки): ";
        cin >> ang;
        float mat[3][3];
        rotationMatrix(ang, mat);
        applyTransform(figure, mat);
        });

    addButton("Povorot (to~ka)", [&]() {
        float x0, y0, ang;
        cout << "Введите координату X точки: ";
        cin >> x0;
        cout << "Введите координату Y точки: ";
        cin >> y0;
        cout << "Введите угол в градусах (против часовой стрелки): ";
        cin >> ang;
        float mat[3][3];
        rotationAroundPointMatrix(ang, x0, y0, mat);
        applyTransform(figure, mat);
        });

    addButton("Sbros", [&]() {
        figure = originalFigs;
        cout << "Сброс к исходному состоянию.\n";
        });


    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mouse(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                for (auto& btn : buttons) {
                    if (btn.rect.contains(mouse)) {
                        btn.action();
                        break;
                    }
                }
            }
        }

        window.clear(Color::White);

        // Оси координат
        VertexArray axisX(Lines, 2);
        axisX[0].position = Vector2f(0, CENTER_Y);
        axisX[1].position = Vector2f(WINDOW_WIDTH - BUTTON_WIDTH, CENTER_Y);
        axisX[0].color = Color(200, 200, 200);
        axisX[1].color = Color(200, 200, 200);
        window.draw(axisX);

        VertexArray axisY(Lines, 2);
        axisY[0].position = Vector2f(CENTER_X, 0);
        axisY[1].position = Vector2f(CENTER_X, WINDOW_HEIGHT);
        axisY[0].color = Color(200, 200, 200);
        axisY[1].color = Color(200, 200, 200);
        window.draw(axisY);

        // Фигура
        for (const auto& fig : figure) {
            if (fig.points.size() < 2) continue;
            vector<Vertex> vertices;
            for (const auto& p : fig.points) {
                float sx = CENTER_X + p.x();
                float sy = CENTER_Y - p.y(); 
                vertices.push_back(Vertex(Vector2f(sx, sy), Color::Blue));
            }
            if (fig.points.size() > 2) {
                const auto& first = fig.points[0];
                float sx = CENTER_X + first.x();
                float sy = CENTER_Y - first.y();
                vertices.push_back(Vertex(Vector2f(sx, sy), Color::Blue));
            }
            window.draw(&vertices[0], vertices.size(), LineStrip);
        }

        // Кнопки
        RectangleShape buttonRect(Vector2f(BUTTON_WIDTH - 20, buttonHeight));
        for (const auto& btn : buttons) {
            buttonRect.setPosition(btn.rect.left, btn.rect.top);
            buttonRect.setFillColor(Color(220, 220, 220));
            buttonRect.setOutlineColor(Color::Black);
            buttonRect.setOutlineThickness(1);
            window.draw(buttonRect);

            if (fontLoaded) {
                Text text(btn.label, font, 16);
                text.setFillColor(Color::Black);
                text.setPosition(btn.rect.left + 5, btn.rect.top + 5);
                window.draw(text);
            }
        }

        window.display();
    }

    return 0;
}
