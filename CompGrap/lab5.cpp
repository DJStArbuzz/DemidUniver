#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace sf;

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

int width, height;
int x_partition;
vector<vector<bool>> buffer;
vector<Point> vertices;
size_t currentEdge;
bool finished;

void processEdge(size_t i) {
    const Point& p1 = vertices[i];
    const Point& p2 = vertices[(i + 1) % vertices.size()];
    if (p1.y == p2.y) 
        return;

    int y1 = p1.y, y2 = p2.y;
    int x1 = p1.x, x2 = p2.x;
    if (y1 > y2) {
        swap(y1, y2);
        swap(x1, x2);
    }
    int dy = y2 - y1;
    int dx = x2 - x1;

    for (int y = y1; y <= y2; ++y) {
        if (y < 0 || y >= height) 
            continue;

        double t = (dy == 0) ? 0.0 : static_cast<double>(y - y1) / dy;
        double x_edge_double = x1 + dx * t;
        int x_edge = static_cast<int>(round(x_edge_double));

        int left = min(x_edge, x_partition);
        int right = max(x_edge, x_partition);
        for (int x = left; x <= right; ++x) {
            if (x >= 0 && x < width) {
                buffer[y][x] = !buffer[y][x];
            }
        }
    }
}

void drawPolygonOutline(RenderWindow& window) {
    if (vertices.size() < 3) return;
    VertexArray lines(LineStrip);
    for (const auto& p : vertices)
        lines.append(Vertex(Vector2f(float(p.x), float(p.y)), Color::Red));
    lines.append(Vertex(Vector2f(float(vertices[0].x), float(vertices[0].y)), Color::Red));
    window.draw(lines);
}

void clear() {
    for (auto& row : buffer)
        fill(row.begin(), row.end(), false);
    currentEdge = 0;
    finished = false;
}

bool step() {
    if (finished || vertices.empty()) 
        return false;
    if (currentEdge >= vertices.size()) {
        finished = true;
        return false;
    }
    processEdge(currentEdge);
    cout << "Processed edge " << currentEdge + 1 << " of " << vertices.size() << endl;
    ++currentEdge;
    if (currentEdge >= vertices.size()) {
        finished = true;
        cout << "Fill complete." << endl;
    }
    return true;
}

void fillAll() {
    while (step()) {}
}

void render(RenderWindow& window) {
    Image image;
    image.create(width, height, Color::Black);
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            if (buffer[y][x])
                image.setPixel(x, y, Color::Blue);
    Texture texture;
    texture.loadFromImage(image);
    Sprite sprite(texture);
    window.draw(sprite);

    drawPolygonOutline(window);

    Vertex partitionLine[] = {
        Vertex(Vector2f(float(x_partition), 0.0f), Color::Green),
        Vertex(Vector2f(float(x_partition), float(height)), Color::Green)
    };
    window.draw(partitionLine, 2, Lines);
}

int main() {
    width = 800;
    height = 600;
    RenderWindow window(VideoMode(width, height), "17.04.2026");
    window.setFramerateLimit(60);

    vertices = {
        {300, 100}, {500, 150}, {550, 300}, {400, 400}, {250, 350}
    };
    x_partition = 420;

    buffer.resize(height, vector<bool>(width, false));
    currentEdge = 0;
    finished = false;

    cout << "Controls: Space - step, R - reset, F - full fill" << endl;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Space) {
                    step();
                }
                else if (event.key.code == Keyboard::R) {
                    clear();
                    cout << "Reset. Start over." << endl;
                }
                else if (event.key.code == Keyboard::F) {
                    fillAll();
                }
            }
        }

        window.clear(Color::Black);
        render(window);
        window.display();
    }
    return 0;
}
