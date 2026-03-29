#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;
using namespace sf;

vector<Vector2i> bresenham(int xa, int ya, int xb, int yb) {
    vector<Vector2i> points;

    int dx = abs(xb - xa);
    int dy = abs(yb - ya);

    int xp = (xa < xb) ? 1 : -1;
    int yp = (ya < yb) ? 1 : -1;

    int e1 = dx - dy;

    int x = xa, y = ya;


    while (true) {
        points.emplace_back(x, y);

        if (x == xb && y == yb)
            break;

        int e2 = 2 * e1;
        if (e2 > -dy) {
            e1 -= dy;
            x += xp;
        }

        if (e2 < dx) {
            e1 += dx;
            y += yp;
        }
    }
    return points;
}

int main() {
    int xa = -1, ya = 1;
    int xb = -1, yb = 8;

    auto pixels = bresenham(xa, ya, xb, yb);

    int minX = min(xa, xb);
    int maxX = max(xa, xb);
    int minY = min(ya, yb);
    int maxY = max(ya, yb);

    for (const auto& p : pixels) {
        minX = min(minX, p.x);
        maxX = max(maxX, p.x);
        minY = min(minY, p.y);
        maxY = max(maxY, p.y);
    }
    int padding = 2;
    minX -= padding;
    maxX += padding;
    minY -= padding;
    maxY += padding;

    const int cellSize = 30;
    const int windowWidth = (maxX - minX + 1) * cellSize;
    const int windowHeight = (maxY - minY + 1) * cellSize;

    RenderWindow window(VideoMode(windowWidth, windowHeight), "270326");
    window.setFramerateLimit(60);

    auto toScreen = [&](int x, int y) -> Vector2f {
        float screenX = (x - minX) * cellSize;
        float screenY = (maxY - y) * cellSize;
        return Vector2f(screenX, screenY);
        };

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear(Color::White);

        VertexArray gridLines(Lines);
        for (int x = minX; x <= maxX; ++x) {
            Vector2f p1 = toScreen(x, minY);
            Vector2f p2 = toScreen(x, maxY);
            gridLines.append(Vertex(p1, Color(200, 200, 200)));
            gridLines.append(Vertex(p2, Color(200, 200, 200)));
        }
        for (int y = minY; y <= maxY; ++y) {
            Vector2f p1 = toScreen(minX, y);
            Vector2f p2 = toScreen(maxX, y);
            gridLines.append(Vertex(p1, Color(200, 200, 200)));
            gridLines.append(Vertex(p2, Color(200, 200, 200)));
        }
        window.draw(gridLines);

        VertexArray line(Lines, 2);
        line[0].position = toScreen(xa, ya);
        line[1].position = toScreen(xb, yb);
        line[0].color = Color::Red;
        line[1].color = Color::Red;
        window.draw(line);


        float pointRadius = cellSize * 0.2f;
        CircleShape pixelPoint(pointRadius);
        pixelPoint.setFillColor(Color(50, 100, 200));
        pixelPoint.setOutlineThickness(1);
        pixelPoint.setOutlineColor(Color(0, 0, 100));

        for (const auto& p : pixels) {
            Vector2f nodePos = toScreen(p.x, p.y);
            pixelPoint.setPosition(nodePos.x - pointRadius, nodePos.y - pointRadius);
            window.draw(pixelPoint);
        }

        float endRadius = cellSize * 0.3f;
        CircleShape endPoint(endRadius);
        endPoint.setFillColor(Color::Green);
        endPoint.setOutlineThickness(2);
        endPoint.setOutlineColor(Color::Green);

        Vector2f startNode = toScreen(xa, ya);
        endPoint.setPosition(startNode.x - endRadius, startNode.y - endRadius);
        window.draw(endPoint);

        Vector2f endNode = toScreen(xb, yb);
        endPoint.setPosition(endNode.x - endRadius, endNode.y - endRadius);
        window.draw(endPoint);

        window.display();
    }

    cout << "Res:\n";
    for (const auto& p : pixels) {
        cout << "(" << p.x << ", " << p.y << ")\n";
    }

    return 0;
}
