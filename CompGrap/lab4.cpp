#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

using namespace sf;
using namespace std;

struct Point2D {
    float x, y;
    Point2D(float x = 0, float y = 0) : 
        x(x), y(y) {}
};

struct MyRect {
    float xmin, ymin, xmax, ymax;
    MyRect(float xmin = 0, float ymin = 0, float xmax = 0, float ymax = 0)
        : xmin(xmin), ymin(ymin), xmax(xmax), ymax(ymax) {
    }

    vector<Point2D> getVertices() const {
        return {
            {xmin, ymin},
            {xmin, ymax},
            {xmax, ymax},
            {xmax, ymin}
        };
    }
};

float dot(const Point2D& a, const Point2D& b) {
    return a.x * b.x + a.y * b.y;
}

Point2D operator-(const Point2D& a, const Point2D& b) {
    return { a.x - b.x, a.y - b.y };
}
Point2D operator+(const Point2D& a, const Point2D& b) {
    return { a.x + b.x, a.y + b.y };
}
Point2D operator*(float t, const Point2D& v) {
    return { t * v.x, t * v.y };
}

struct InPoint {
    Point2D point;
    bool isEntry;
};

bool CyrusBeck(const vector<Point2D>& poly, Point2D p1, Point2D p2,
    vector<InPoint>& allInt,
    Point2D& outP1, Point2D& outP2) {
    int n = poly.size();
    Point2D D = p2 - p1;
    float tE = 0.0f, tL = 1.0f;
    allInt.clear();

    for (int i = 0; i < n; ++i) {
        Point2D E = poly[i];
        Point2D E_next = poly[(i + 1) % n];
        Point2D edge = E_next - E;
        Point2D N = { -edge.y, edge.x };
        float len = hypot(N.x, N.y);
        if (len > 1e-6) { N.x /= len; N.y /= len; }
        Point2D W = p1 - E;
        float DdotN = dot(D, N);
        float WdotN = dot(W, N);

        if (fabs(DdotN) < 1e-6) {
            continue;
        }

        float t = -WdotN / DdotN;
        Point2D intersectPoint = p1 + t * D;

        bool isEntry = (DdotN < 0);
        allInt.push_back({ intersectPoint, isEntry });

        if (t >= 0.0f && t <= 1.0f) {
            if (DdotN < 0) {
                if (t > tE) tE = t;
            }
            else {
                if (t < tL) tL = t;
            }
        }
    }
    for (size_t i = 0; i < allInt.size(); ++i) {
        for (size_t j = i + 1; j < allInt.size(); ) {
            if (hypot(allInt[i].point.x - allInt[j].point.x,
                allInt[i].point.y - allInt[j].point.y) < 1e-5) {
                allInt.erase(allInt.begin() + j);
            }
            else {
                ++j;
            }
        }
    }

    if (tE <= tL) {
        outP1 = p1 + tE * D;
        outP2 = p1 + tL * D;
        return true;
    }
    return false;
}

const int INSIDE = 0, LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8;

int computeCode(float x, float y, const MyRect& rect) {
    int code = INSIDE;
    if (x < rect.xmin) 
        code |= LEFT;
    else if (x > rect.xmax) 
        code |= RIGHT;
    if (y < rect.ymin)
        code |= BOTTOM;
    else if (y > rect.ymax) 
        code |= TOP;
    return code;
}

bool Sutherland(Point2D& p1, Point2D& p2, const MyRect& rect) {
    int code1 = computeCode(p1.x, p1.y, rect);
    int code2 = computeCode(p2.x, p2.y, rect);
    bool accept = false;

    while (true) {
        if ((code1 | code2) == 0) {
            accept = true;
            break;
        }
        if (code1 & code2) 
            break;

        int codeOut = code1 ? code1 : code2;
        float x, y;

        if (codeOut & TOP) {
            x = p1.x + (p2.x - p1.x) * (rect.ymax - p1.y) / (p2.y - p1.y);
            y = rect.ymax;
        }
        else if (codeOut & BOTTOM) {
            x = p1.x + (p2.x - p1.x) * (rect.ymin - p1.y) / (p2.y - p1.y);
            y = rect.ymin;
        }
        else if (codeOut & RIGHT) {
            y = p1.y + (p2.y - p1.y) * (rect.xmax - p1.x) / (p2.x - p1.x);
            x = rect.xmax;
        }
        else {
            y = p1.y + (p2.y - p1.y) * (rect.xmin - p1.x) / (p2.x - p1.x);
            x = rect.xmin;
        }

        if (codeOut == code1) {
            p1 = { x, y };
            code1 = computeCode(p1.x, p1.y, rect);
        }
        else {
            p2 = { x, y };
            code2 = computeCode(p2.x, p2.y, rect);
        }
    }
    return accept;
}

const float EPS = 0.5f;

bool isInside(float x, float y, const MyRect& rect) {
    return (x >= rect.xmin && x <= rect.xmax && y >= rect.ymin && y <= rect.ymax);
}

void Midpoint(Point2D a, Point2D b, const MyRect& rect, vector<Point2D>& out) {
    bool a_in = isInside(a.x, a.y, rect);
    bool b_in = isInside(b.x, b.y, rect);

    if (a_in && b_in) {
        out.push_back(a);
        out.push_back(b);
        return;
    }

    if (hypot(a.x - b.x, a.y - b.y) < EPS) 
        return;

    Point2D mid = { (a.x + b.x) / 2, (a.y + b.y) / 2 };
    Midpoint(a, mid, rect, out);
    Midpoint(mid, b, rect, out);
}

bool readInput(const string& filename, MyRect& rect, Point2D& p1, Point2D& p2) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    string line;
    int state = 0;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') 
            continue;
        istringstream iss(line);
        if (state == 0) {
            float xmin, ymin, xmax, ymax;
            if (iss >> xmin >> ymin >> xmax >> ymax) {
                rect = MyRect(xmin, ymin, xmax, ymax);
                state = 1;
            }
        }
        else if (state == 1) {
            float x1, y1, x2, y2;
            if (iss >> x1 >> y1 >> x2 >> y2) {
                p1 = Point2D(x1, y1);
                p2 = Point2D(x2, y2);
                return true;
            }
        }
    }
    return false;
}

class CoordConverter {
    Vector2f center;
    float scale;
public:
    CoordConverter(const Vector2u& windowSize, float worldMinX, float worldMaxX,
        float worldMinY, float worldMaxY) {
        float maxAbsX = max(fabs(worldMinX), fabs(worldMaxX));
        float maxAbsY = max(fabs(worldMinY), fabs(worldMaxY));

        float worldWidth = 2 * maxAbsX;
        float worldHeight = 2 * maxAbsY;
        if (worldWidth < 1e-6) 
            worldWidth = 1;
        if (worldHeight < 1e-6) 
            worldHeight = 1;
        float margin = 100.0f;
        float scaleX = (windowSize.x - 2 * margin) / worldWidth;
        float scaleY = (windowSize.y - 2 * margin) / worldHeight;
        scale = min(scaleX, scaleY);

        center = Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f);
    }
    Vector2f worldToScreen(float x, float y) const {
        return Vector2f(center.x + x * scale, center.y - y * scale);
    }
};

void drawGrid(RenderWindow& window, const CoordConverter& conv) {
    Vector2u size = window.getSize();
    float width = static_cast<float>(size.x);
    float height = static_cast<float>(size.y);

    VertexArray lines(Lines);
    for (float x = 0; x <= width; x += 50) {
        lines.append(Vertex(Vector2f(x, 0), Color(200, 200, 200)));
        lines.append(Vertex(Vector2f(x, height), Color(200, 200, 200)));
    }
    for (float y = 0; y <= height; y += 50) {
        lines.append(Vertex(Vector2f(0, y), Color(200, 200, 200)));
        lines.append(Vertex(Vector2f(width, y), Color(200, 200, 200)));
    }
    window.draw(lines);

    VertexArray axes(Lines);
    Vector2f origin = conv.worldToScreen(0, 0);
    axes.append(Vertex(Vector2f(0, origin.y), Color(150, 150, 150)));
    axes.append(Vertex(Vector2f(width, origin.y), Color(150, 150, 150)));
    axes.append(Vertex(Vector2f(origin.x, 0), Color(150, 150, 150)));
    axes.append(Vertex(Vector2f(origin.x, height), Color(150, 150, 150)));
    window.draw(axes);
}

int main() {
    ContextSettings settings;
    settings.antialiasingLevel = 0;
    RenderWindow window(VideoMode(800, 600), "100426", Style::Default, settings);
    window.setFramerateLimit(60);

    MyRect rect;
    Point2D lineP1, lineP2;
    if (!readInput("input.txt", rect, lineP1, lineP2)) {
        rect = MyRect(-300, -200, 300, 200);
        lineP1 = Point2D(-400, 0);
        lineP2 = Point2D(400, 100);
    }

    vector<Point2D> verts = rect.getVertices();
    float minX = verts[0].x, maxX = verts[0].x;
    float minY = verts[0].y, maxY = verts[0].y;
    for (const auto& p : verts) {
        minX = min(minX, p.x); maxX = max(maxX, p.x);
        minY = min(minY, p.y); maxY = max(maxY, p.y);
    }
    minX = min({ minX, lineP1.x, lineP2.x });
    maxX = max({ maxX, lineP1.x, lineP2.x });
    minY = min({ minY, lineP1.y, lineP2.y });
    maxY = max({ maxY, lineP1.y, lineP2.y });

    CoordConverter converter(window.getSize(), minX, maxX, minY, maxY);

    Point2D clippedP1, clippedP2;
    vector<InPoint> cyrusIntersections;
    vector<Point2D> midpointPoints;
    int currentAlgo = 1;
    bool visible = false;


    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Num1) 
                    currentAlgo = 1;
                if (event.key.code == Keyboard::Num2) 
                    currentAlgo = 2;
                if (event.key.code == Keyboard::Num3) 
                    currentAlgo = 3;
            }
        }

        if (currentAlgo == 1) {
            visible = CyrusBeck(rect.getVertices(), lineP1, lineP2,
                cyrusIntersections, clippedP1, clippedP2);
        }
        else if (currentAlgo == 2) {
            Point2D p1 = lineP1, p2 = lineP2;
            visible = Sutherland(p1, p2, rect);
            if (visible) {
                clippedP1 = p1;
                clippedP2 = p2;
            }
        }
        else {
            midpointPoints.clear();
            Midpoint(lineP1, lineP2, rect, midpointPoints);
            visible = !midpointPoints.empty();
        }

        window.clear(Color::White);
        drawGrid(window, converter);

        VertexArray rectLines(LinesStrip, 5);
        for (int i = 0; i < 4; ++i) {
            rectLines[i].position = converter.worldToScreen(verts[i].x, verts[i].y);
            rectLines[i].color = Color::Blue;
        }
        rectLines[4].position = converter.worldToScreen(verts[0].x, verts[0].y);
        rectLines[4].color = Color::Blue;
        window.draw(rectLines);

        auto tparam = [&](const Point2D& pt) -> float {
            float dx = lineP2.x - lineP1.x;
            float dy = lineP2.y - lineP1.y;
            if (fabs(dx) > fabs(dy))
                return (pt.x - lineP1.x) / dx;
            else
                return (pt.y - lineP1.y) / dy;
            };

        if ((currentAlgo == 1 || currentAlgo == 2) && visible) {
            Point2D entry, exit;
            if (currentAlgo == 1) {
                if (cyrusIntersections.size() >= 2) {
                    entry = cyrusIntersections[0].point;
                    exit = cyrusIntersections[1].point;

                    if (tparam(entry) > tparam(exit)) 
                        swap(entry, exit);
                }
                else {
                    visible = false;
                }
            }
            else {
                entry = clippedP1;
                exit = clippedP2;

                if (tparam(entry) > tparam(exit)) 
                    swap(entry, exit);
            }

            if (visible) {
                float t_entry = tparam(entry);
                float t_exit = tparam(exit);

                if (t_entry > 1e-6) {
                    VertexArray seg(Lines, 2);
                    seg[0].position = converter.worldToScreen(lineP1.x, lineP1.y);
                    seg[0].color = Color::Green;
                    seg[1].position = converter.worldToScreen(entry.x, entry.y);
                    seg[1].color = Color::Green;
                    window.draw(seg);
                }
                if (t_exit - t_entry > 1e-6) {
                    Vector2f p1s = converter.worldToScreen(entry.x, entry.y);
                    Vector2f p2s = converter.worldToScreen(exit.x, exit.y);
                    Vector2f dir = p2s - p1s;
                    float len = hypot(dir.x, dir.y);
                    
                    if (len > 1e-6) {
                        dir /= len;
                        p1s = p1s - dir * 0.5f;
                        p2s = p2s + dir * 0.5f;
                    }
                    
                    VertexArray yellow(Lines, 2);
                    yellow[0].position = p1s;
                    yellow[0].color = Color::Yellow;
                    yellow[1].position = p2s;
                    yellow[1].color = Color::Yellow;
                    window.draw(yellow);
                }
                if (t_exit < 1 - 1e-6) {
                    VertexArray seg(Lines, 2);
                    seg[0].position = converter.worldToScreen(exit.x, exit.y);
                    seg[0].color = Color::Green;
                    seg[1].position = converter.worldToScreen(lineP2.x, lineP2.y);
                    seg[1].color = Color::Green;
                    window.draw(seg);
                }
            }
        }
        else if (currentAlgo == 3 && visible) {
            VertexArray full(Lines, 2);
            full[0].position = converter.worldToScreen(lineP1.x, lineP1.y);
            full[0].color = Color::Green;
            full[1].position = converter.worldToScreen(lineP2.x, lineP2.y);
            full[1].color = Color::Green;
            window.draw(full);

            if (midpointPoints.size() >= 2) {
                VertexArray poly(LinesStrip, midpointPoints.size());
                for (size_t i = 0; i < midpointPoints.size(); ++i) {
                    poly[i].position = converter.worldToScreen(midpointPoints[i].x, midpointPoints[i].y);
                    poly[i].color = Color::Yellow;
                }
                window.draw(poly);
            }
        }
        else {
            VertexArray full(Lines, 2);
            full[0].position = converter.worldToScreen(lineP1.x, lineP1.y);
            full[0].color = Color::Green;
            full[1].position = converter.worldToScreen(lineP2.x, lineP2.y);
            full[1].color = Color::Green;
            window.draw(full);
        }

        if (currentAlgo == 1 && visible && !cyrusIntersections.empty()) {
            for (const auto& ip : cyrusIntersections) {
                CircleShape circle(5);
                Vector2f pos = converter.worldToScreen(ip.point.x, ip.point.y);
                circle.setPosition(pos.x - 5, pos.y - 5);
                circle.setFillColor(ip.isEntry ? Color::Green : Color::Red);
                window.draw(circle);
            }
        }

        window.display();
    }
    return 0;
}
