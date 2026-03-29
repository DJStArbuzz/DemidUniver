#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;
using namespace sf;

vector<Vector2i> bresenham_circle(int xa, int ya, int r) {
    vector<Vector2i> points;

    int x = 0;
    int y = r;

    int d = 1 - 2 * r;
    int e = 0;

    while (y >= x) {
        points.emplace_back(xa + x, ya + y);
        points.emplace_back(xa + x, ya - y);
        points.emplace_back(xa - x, ya + y);
        points.emplace_back(xa - x, ya - y);

        points.emplace_back(xa + y, ya + x);
        points.emplace_back(xa + y, ya - x);
        points.emplace_back(xa - y, ya + x);
        points.emplace_back(xa - y, ya - x);

        e = 2 * (d + y) - 1;

        if ((d < 0) && (e <= 0)) {
            d += 2 * (++x) + 1;
            continue;
        }

        if ((d > 0) && (e > 0)) {
            d -= 2 * (--y) + 1;
            continue;
        }
        d += 2 * (++x - --y);
    }

    return points;
}

int main() {
    int cx = 0, cy = 0;
    int r = 5;

    auto pixels = bresenham_circle(cx, cy, r);

    int minX = cx - r;
    int maxX = cx + r;
    int minY = cy - r;
    int maxY = cy + r;
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

    RenderWindow window(VideoMode(windowWidth, windowHeight), "060426");
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

        // Идеальная окружность
        CircleShape idealCircle(r * cellSize);
        idealCircle.setFillColor(Color::Transparent);
        idealCircle.setOutlineThickness(2.f);
        idealCircle.setOutlineColor(Color::Red);
        Vector2f centerScreen = toScreen(cx, cy);
        idealCircle.setPosition(centerScreen.x - r * cellSize, centerScreen.y - r * cellSize);
        window.draw(idealCircle);

        // Пиксели по Брезенхему
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


        float centerRadius = cellSize * 0.25f;
        CircleShape centerPoint(centerRadius);
        centerPoint.setFillColor(Color::Green);
        centerPoint.setOutlineThickness(1);
        centerPoint.setOutlineColor(Color::Green);
        Vector2f centerScreenPos = toScreen(cx, cy);
        centerPoint.setPosition(centerScreenPos.x - centerRadius, centerScreenPos.y - centerRadius);
        window.draw(centerPoint);

        window.display();
    }

    cout << "res:\n";
    for (const auto& p : pixels) {
        cout << "(" << p.x << ", " << p.y << ")\n";
    }

    return 0;
}
