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

struct Point {
    float x, y;
    Point(float x = 0, float y = 0) : x(x), y(y) {}
};

struct MyRect {
    float xmin, ymin, xmax, ymax;
    MyRect(float xmin = 0, float ymin = 0, float xmax = 0, float ymax = 0)
        : xmin(xmin), ymin(ymin), xmax(xmax), ymax(ymax) {
    }

    vector<Point> getVertices() const {
        return {
            {xmin, ymin},
            {xmax, ymin},
            {xmax, ymax},
            {xmin, ymax}
        };
    }
};

float dot(const Point& a, const Point& b) {
    return a.x * b.x + a.y * b.y;
}

Point operator-(const Point& a, const Point& b) {
    return { a.x - b.x, a.y - b.y };
}
Point operator+(const Point& a, const Point& b) {
    return { a.x + b.x, a.y + b.y };
}
Point operator*(float t, const Point& v) {
    return { t * v.x, t * v.y };
}

bool CyrusBeckClip(const vector<Point>& poly, Point p1, Point p2, Point& outP1, Point& outP2) {
    int n = poly.size();
    Point D = p2 - p1;
    float tE = 0.0f, tL = 1.0f;
    bool hasIntersection = false;

    for (int i = 0; i < n; ++i) {
        Point E = poly[i];
        Point E_next = poly[(i + 1) % n];
        Point edge = E_next - E;
        Point N = { edge.y, -edge.x };

        float len = hypot(N.x, N.y);
        if (len > 1e-6) 
        { 
            N.x /= len; 
            N.y /= len; 
        }

        float DdotN = dot(D, N);
        float WdotN = dot(p1 - E, N);
        if (fabs(DdotN) < 1e-6) {
            if (WdotN < -1e-5) return false;
            continue;
        }

        float t = -WdotN / DdotN;
        if (t >= 0.0f && t <= 1.0f) {
            hasIntersection = true;
            if (DdotN < 0) {
                if (t > tE)
                    tE = t;
            }
            else {
                if (t < tL)
                    tL = t;
            }
        }
    }

    if (!hasIntersection) {
        auto inside = [&](Point pt) {
            for (int i = 0; i < n; ++i) {
                Point E = poly[i];
                Point E_next = poly[(i + 1) % n];
                Point edge = E_next - E;
                Point N = { edge.y, -edge.x };
                float len = hypot(N.x, N.y);

                if (len > 1e-6) 
                { 
                    N.x /= len; 
                    N.y /= len; 
                }

                if (dot(pt - E, N) > 1e-5) 
                    return false;
            }
            return true;
            };

        if (inside(p1) && inside(p2)) {
            outP1 = p1; outP2 = p2;
            return true;
        }
        return false;
    }

    if (tE <= tL) {
        outP1 = p1 + tE * D;
        outP2 = p1 + tL * D;
        return true;
    }
    return false;
}

vector<pair<Point, bool>> GetAllPotentialPoints(const vector<Point>& poly, Point p1, Point p2) {
    vector<pair<Point, bool>> res;
    Point D = p2 - p1;
    int n = poly.size();
    
    for (int i = 0; i < n; ++i) {
        Point E = poly[i];
        Point E_next = poly[(i + 1) % n];
        Point edge = E_next - E;
        Point N = { edge.y, -edge.x };
        float len = hypot(N.x, N.y);
        if (len > 1e-6) 
        {
            N.x /= len; 
            N.y /= len; 
        }

        float DdotN = dot(D, N);
        if (fabs(DdotN) < 1e-6) 
            continue;
        
        float t = -dot(p1 - E, N) / DdotN;
        Point pt = p1 + t * D;
        bool isEntry = (DdotN < 0);
        res.push_back({ pt, isEntry });
    }

    for (size_t i = 0; i < res.size(); ++i) {
        for (size_t j = i + 1; j < res.size(); ) {
            if (hypot(res[i].first.x - res[j].first.x,
                res[i].first.y - res[j].first.y) < 1e-5) {
                res.erase(res.begin() + j);
            }
            else ++j;
        }
    }
    return res;
}


const int INSIDE = 0, LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8;

int computeCode(float x, float y, const MyRect& rect) {
    int code = INSIDE;
    if (x < rect.xmin) code |= LEFT;
    else if (x > rect.xmax) code |= RIGHT;
    if (y < rect.ymin) code |= BOTTOM;
    else if (y > rect.ymax) code |= TOP;
    return code;
}

bool Sutherland(Point& p1, Point& p2, const MyRect& rect) {
    int code1 = computeCode(p1.x, p1.y, rect);
    int code2 = computeCode(p2.x, p2.y, rect);
    bool accept = false;
    while (true) {
        if ((code1 | code2) == 0) { accept = true; break; }
        if (code1 & code2) break;
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

void Midpoint(Point a, Point b, const MyRect& rect, vector<Point>& out) {
    bool a_in = isInside(a.x, a.y, rect);
    bool b_in = isInside(b.x, b.y, rect);
    if (a_in && b_in) {
        out.push_back(a); out.push_back(b);
        return;
    }
    if (hypot(a.x - b.x, a.y - b.y) < EPS) return;
    Point mid = { (a.x + b.x) / 2, (a.y + b.y) / 2 };
    Midpoint(a, mid, rect, out);
    Midpoint(mid, b, rect, out);
}


bool readInput(const string& filename, MyRect& rect, Point& p1, Point& p2) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    string line;
    int state = 0;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
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
                p1 = { x1, y1 }; p2 = { x2, y2 };
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
    CoordConverter(const Vector2u& winSize, float minX, float maxX, float minY, float maxY) {
        float maxAbsX = max(fabs(minX), fabs(maxX));
        float maxAbsY = max(fabs(minY), fabs(maxY));
        float worldW = 2 * maxAbsX;
        float worldH = 2 * maxAbsY;
        if (worldW < 1e-6) worldW = 1;
        if (worldH < 1e-6) worldH = 1;
        float margin = 150.0f;
        float sx = (winSize.x - 2 * margin) / worldW;
        float sy = (winSize.y - 2 * margin) / worldH;
        scale = min(sx, sy);
        center = Vector2f(winSize.x / 2.0f, winSize.y / 2.0f);
    }
    Vector2f worldToScreen(float x, float y) const {
        return Vector2f(center.x + x * scale, center.y - y * scale);
    }
};

void drawGrid(RenderWindow& win, const CoordConverter& conv) {
    Vector2u sz = win.getSize();
    float w = sz.x, h = sz.y;
    VertexArray lines(Lines);
    for (float x = 0; x <= w; x += 50) {
        lines.append(Vertex(Vector2f(x, 0), Color(200, 200, 200)));
        lines.append(Vertex(Vector2f(x, h), Color(200, 200, 200)));
    }
    for (float y = 0; y <= h; y += 50) {
        lines.append(Vertex(Vector2f(0, y), Color(200, 200, 200)));
        lines.append(Vertex(Vector2f(w, y), Color(200, 200, 200)));
    }
    win.draw(lines);
    Vector2f orig = conv.worldToScreen(0, 0);
    VertexArray axes(Lines);
    axes.append(Vertex(Vector2f(0, orig.y), Color(150, 150, 150)));
    axes.append(Vertex(Vector2f(w, orig.y), Color(150, 150, 150)));
    axes.append(Vertex(Vector2f(orig.x, 0), Color(150, 150, 150)));
    axes.append(Vertex(Vector2f(orig.x, h), Color(150, 150, 150)));
    win.draw(axes);
}

int main() {
    ContextSettings settings;
    settings.antialiasingLevel = 0;
    RenderWindow window(VideoMode(800, 600), "Line Clipping", Style::Default, settings);
    window.setFramerateLimit(60);

    MyRect rect;
    Point p1, p2;
    if (!readInput("input.txt", rect, p1, p2)) {
        rect = MyRect(-300, -200, 300, 200);
        p1 = { -400,0 }; p2 = { 400,100 };
    }

    vector<Point> verts = rect.getVertices();
    float minX = verts[0].x, maxX = verts[0].x;
    float minY = verts[0].y, maxY = verts[0].y;
    for (auto& v : verts) {
        minX = min(minX, v.x); maxX = max(maxX, v.x);
        minY = min(minY, v.y); maxY = max(maxY, v.y);
    }
    minX = min({ minX, p1.x, p2.x });
    maxX = max({ maxX, p1.x, p2.x });
    minY = min({ minY, p1.y, p2.y });
    maxY = max({ maxY, p1.y, p2.y });

    auto potentials = GetAllPotentialPoints(rect.getVertices(), p1, p2);
    for (auto& pt : potentials) {
        minX = min(minX, pt.first.x);
        maxX = max(maxX, pt.first.x);
        minY = min(minY, pt.first.y);
        maxY = max(maxY, pt.first.y);
    }
    float dx = maxX - minX, dy = maxY - minY;
    minX -= dx * 0.1f; maxX += dx * 0.1f;
    minY -= dy * 0.1f; maxY += dy * 0.1f;

    CoordConverter conv(window.getSize(), minX, maxX, minY, maxY);

    Point clippedP1, clippedP2;
    vector<Point> midPoints;
    int algo = 1;
    bool visible = false;

    Font font;
    if (!font.loadFromFile("arial.ttf")) {}

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) window.close();
            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Num1) algo = 1;
                if (e.key.code == Keyboard::Num2) algo = 2;
                if (e.key.code == Keyboard::Num3) algo = 3;
            }
        }

        if (algo == 1) {
            visible = CyrusBeckClip(rect.getVertices(), p1, p2, clippedP1, clippedP2);
            potentials = GetAllPotentialPoints(rect.getVertices(), p1, p2);
        }
        else if (algo == 2) {
            Point a = p1, b = p2;
            visible = Sutherland(a, b, rect);
            if (visible) { clippedP1 = a; clippedP2 = b; }
        }
        else {
            midPoints.clear();
            Midpoint(p1, p2, rect, midPoints);
            visible = !midPoints.empty();
        }

        window.clear(Color::White);
        drawGrid(window, conv);

        VertexArray rectLines(LinesStrip, 5);
        for (int i = 0; i < 4; ++i) {
            rectLines[i].position = conv.worldToScreen(verts[i].x, verts[i].y);
            rectLines[i].color = Color::Blue;
        }
        rectLines[4].position = conv.worldToScreen(verts[0].x, verts[0].y);
        rectLines[4].color = Color::Blue;
        window.draw(rectLines);

        auto tparam = [&](Point pt) -> float {
            float dx = p2.x - p1.x, dy = p2.y - p1.y;
            if (fabs(dx) > fabs(dy)) return (pt.x - p1.x) / dx;
            else return (pt.y - p1.y) / dy;
            };

        if ((algo == 1 || algo == 2) && visible) {
            Point entry = clippedP1, exit = clippedP2;
            if (tparam(entry) > tparam(exit)) swap(entry, exit);
            float te = tparam(entry), tl = tparam(exit);
            if (te > 1e-6) {
                VertexArray seg(Lines, 2);
                seg[0].position = conv.worldToScreen(p1.x, p1.y);
                seg[0].color = Color::Green;
                seg[1].position = conv.worldToScreen(entry.x, entry.y);
                seg[1].color = Color::Green;
                window.draw(seg);
            }
            if (tl - te > 1e-6) {
                Vector2f a = conv.worldToScreen(entry.x, entry.y);
                Vector2f b = conv.worldToScreen(exit.x, exit.y);
                Vector2f dir = b - a;
                float len = hypot(dir.x, dir.y);
                if (len > 1e-6) {
                    dir /= len;
                    a = a - dir * 0.5f;
                    b = b + dir * 0.5f;
                }
                VertexArray yellow(Lines, 2);
                yellow[0].position = a; yellow[0].color = Color::Yellow;
                yellow[1].position = b; yellow[1].color = Color::Yellow;
                window.draw(yellow);
            }
            if (tl < 1 - 1e-6) {
                VertexArray seg(Lines, 2);
                seg[0].position = conv.worldToScreen(exit.x, exit.y);
                seg[0].color = Color::Green;
                seg[1].position = conv.worldToScreen(p2.x, p2.y);
                seg[1].color = Color::Green;
                window.draw(seg);
            }
        }
        else if (algo == 3 && visible) {
            VertexArray full(Lines, 2);
            full[0].position = conv.worldToScreen(p1.x, p1.y);
            full[0].color = Color::Green;
            full[1].position = conv.worldToScreen(p2.x, p2.y);
            full[1].color = Color::Green;
            window.draw(full);
            if (midPoints.size() >= 2) {
                VertexArray poly(LinesStrip, midPoints.size());
                for (size_t i = 0; i < midPoints.size(); ++i) {
                    poly[i].position = conv.worldToScreen(midPoints[i].x, midPoints[i].y);
                    poly[i].color = Color::Yellow;
                }
                window.draw(poly);
            }
        }
        else {
            VertexArray full(Lines, 2);
            full[0].position = conv.worldToScreen(p1.x, p1.y);
            full[0].color = Color::Green;
            full[1].position = conv.worldToScreen(p2.x, p2.y);
            full[1].color = Color::Green;
            window.draw(full);
        }

        if (algo == 1 && !potentials.empty()) {
            for (auto& pt : potentials) {
                CircleShape circ(5);
                Vector2f pos = conv.worldToScreen(pt.first.x, pt.first.y);
                circ.setPosition(pos.x - 5, pos.y - 5);
                circ.setFillColor(pt.second ? Color::Green : Color::Red);
                window.draw(circ);
            }
        }

        Text info;
        info.setFont(font);
        info.setCharacterSize(16);
        info.setFillColor(Color::Black);
        info.setPosition(10, 10);
        string name = (algo == 1) ? "Cyrus-Beck (1)" : (algo == 2) ? "Cohen-Sutherland (2)" : "Midpoint (3)";
        info.setString(name + "\nGreen - outside, Yellow - inside\nCircles: potential entry/exit (algo 1)");
        window.draw(info);

        window.display();
    }
    return 0;
}
