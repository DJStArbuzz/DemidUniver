#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;
using namespace sf;

struct Point {
    double x, y;
    Point(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

double cross(const Point& o, const Point& a, const Point& b) {
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

double distSq(const Point& a, const Point& b) {
    double dx = a.x - b.x, dy = a.y - b.y;
    return dx * dx + dy * dy;
}

Point findPivot(vector<Point>& points) {
    Point pivot = points[0];
    for (const auto& p : points) {
        if (p.y < pivot.y || (p.y == pivot.y && p.x < pivot.x))
            pivot = p;
    }
    return pivot;
}

bool compareAngle(const Point& pivot, const Point& a, const Point& b) {
    double crossVal = cross(pivot, a, b);
    if (crossVal != 0)
        return crossVal > 0;
    return distSq(pivot, a) < distSq(pivot, b);
}

vector<Point> grahamHull(vector<Point> points, vector<vector<Point>>& steps) {
    if (points.size() < 3) return points;

    Point pivot = findPivot(points);
    cout << "Pivot point: (" << pivot.x << ", " << pivot.y << ")\n";

    sort(points.begin(), points.end(), [&](const Point& a, const Point& b) {
        return compareAngle(pivot, a, b);
        });

    cout << "Points after sorting:\n";
    for (size_t i = 0; i < points.size(); ++i) {
        cout << "  " << i << ": (" << points[i].x << ", " << points[i].y << ")" << endl;
    }

    vector<Point> stack;
    stack.push_back(points[0]);
    stack.push_back(points[1]);

    steps.push_back(stack);
    cout << "Initial stack: [" << stack[0].x << "," << stack[0].y << "] [" << stack[1].x << "," << stack[1].y << "]\n";

    for (size_t i = 2; i < points.size(); ++i) {
        cout << "\nProcessing point " << i << ": (" << points[i].x << ", " << points[i].y << ")\n";
        while (stack.size() >= 2 && cross(stack[stack.size() - 2], stack.back(), points[i]) < 0) {
            Point popped = stack.back();
            stack.pop_back();
            cout << "  Popped (" << popped.x << ", " << popped.y << ") because right turn\n";
            steps.push_back(stack);
        }
        stack.push_back(points[i]);
        steps.push_back(stack);
        cout << "  Stack now: ";
        for (const auto& p : stack) cout << "(" << p.x << "," << p.y << ") ";
        cout << endl;
    }
    return stack;
}

int main() {
    vector<Point> points = {
        {100, 300}, {200, 200}, {300, 100}, {400, 150}, {500, 250},
        {450, 400}, {350, 450}, {250, 350}, {150, 400}, {50, 200},
        {120, 180}, {280, 220}, {320, 380}, {420, 300}, {200, 100}
    };

    vector<vector<Point>> steps;
    vector<Point> hull = grahamHull(points, steps);

    RenderWindow window(VideoMode(800, 600), "Graham's Algorithm – Convex Hull");
    window.setFramerateLimit(60);

     Font font;
    bool fontLoaded = font.loadFromFile("Мирослав Bold.ttf"); 
    if (!fontLoaded) {
        cout << "Font not loaded. Text will be hidden.\n";
    }
    Text stepText;
    if (fontLoaded) {
        stepText.setFont(font);
        stepText.setCharacterSize(18);
        stepText.setFillColor(Color::White);
        stepText.setPosition(10, 10);
    }

    int currentStep = 0;
    bool animationFinished = false;
    Clock clock;
    const float stepDelay = 1.0f;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space) {
                if (currentStep < (int)steps.size() - 1) {
                    currentStep++;
                }
                else if (!animationFinished) {
                    animationFinished = true;
                }
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::R) {
                currentStep = 0;
                animationFinished = false;
            }
        }
        if (!animationFinished && clock.getElapsedTime().asSeconds() >= stepDelay) {
            if (currentStep < (int)steps.size() - 1) {
                currentStep++;
            }
            else {
                animationFinished = true;
            }
            clock.restart();
        }

        window.clear(Color::Black);

        int windowHeight = window.getSize().y; 
        for (const auto& p : points) {
            CircleShape circle(5);
            circle.setFillColor(Color(150, 150, 150));
            float screenX = p.x;
            float screenY = windowHeight - p.y;
            circle.setPosition(screenX - 5, screenY - 5);
            window.draw(circle);
        }

        Point pivot = findPivot(const_cast<vector<Point>&>(points));
        CircleShape pivotCircle(6);
        pivotCircle.setFillColor(Color::Red);
        float pivotScreenX = pivot.x;
        float pivotScreenY = windowHeight - pivot.y;
        pivotCircle.setPosition(pivotScreenX - 6, pivotScreenY - 6);
        window.draw(pivotCircle);

        if (currentStep < (int)steps.size()) {
            const auto& stack = steps[currentStep];
            if (stack.size() >= 2) {
                for (size_t i = 0; i < stack.size() - 1; ++i) {
                    Vertex line[] = {
                        Vertex(Vector2f(stack[i].x, windowHeight - stack[i].y), Color::Green),
                        Vertex(Vector2f(stack[i + 1].x, windowHeight - stack[i + 1].y), Color::Green)
                    };
                    window.draw(line, 2, Lines);
                }
                if (animationFinished && stack.size() == hull.size()) {
                    Vertex closeLine[] = {
                        Vertex(Vector2f(stack.back().x, windowHeight - stack.back().y), Color::Yellow),
                        Vertex(Vector2f(stack[0].x, windowHeight - stack[0].y), Color::Yellow)
                    };
                    window.draw(closeLine, 2, Lines);
                }
            }
        }

        window.display();
    }

    cout << "\nConvex hull vertices:\n";
    for (const auto& p : hull) {
        cout << "(" << p.x << ", " << p.y << ")\n";
    }

    return 0;
}
