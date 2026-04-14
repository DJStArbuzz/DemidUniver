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
    sort(points.begin(), points.end(), [&](const Point& a, const Point& b) {
        return compareAngle(pivot, a, b);
        });

    vector<Point> stack;
    stack.push_back(points[0]);
    stack.push_back(points[1]);

    steps.push_back(stack);

    for (size_t i = 2; i < points.size(); ++i) {
        while (stack.size() >= 2 && cross(stack[stack.size() - 2], stack.back(), points[i]) < 0) {
            stack.pop_back();
            steps.push_back(stack);
        }
        stack.push_back(points[i]);
        steps.push_back(stack);
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

    RenderWindow window(VideoMode(800, 600), "17042026");
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile("ofont.ru_Eastern Rhodopes.ttf")) {
        cout << "Failed to load font. Check file path and name.\n";
    }
    else {
        cout << "Font loaded successfully.\n";
    }
    Text stepText;
    stepText.setFont(font);
    stepText.setCharacterSize(18);
    stepText.setFillColor(Color::White);
    stepText.setPosition(10, 10);

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

        for (const auto& p : points) {
            CircleShape circle(5);
            circle.setFillColor(Color(150, 150, 150));
            circle.setPosition(p.x - 5, p.y - 5);
            window.draw(circle);
        }

        Point pivot = findPivot(const_cast<vector<Point>&>(points));
        CircleShape pivotCircle(6);
        pivotCircle.setFillColor(Color::Red);
        pivotCircle.setPosition(pivot.x - 6, pivot.y - 6);
        window.draw(pivotCircle);

        if (currentStep < (int)steps.size()) {
            const auto& stack = steps[currentStep];
            if (stack.size() >= 2) {
                for (size_t i = 0; i < stack.size() - 1; ++i) {
                    Vertex line[] = {
                        Vertex(Vector2f(stack[i].x, stack[i].y), Color::Green),
                        Vertex(Vector2f(stack[i + 1].x, stack[i + 1].y), Color::Green)
                    };
                    window.draw(line, 2, Lines);
                }
                if (animationFinished && stack.size() == hull.size()) {
                    Vertex closeLine[] = {
                        Vertex(Vector2f(stack.back().x, stack.back().y), Color::Yellow),
                        Vertex(Vector2f(stack[0].x, stack[0].y), Color::Yellow)
                    };
                    window.draw(closeLine, 2, Lines);
                }
            }
        }

        stepText.setString("Step: " + to_string(currentStep + 1) + " / " + to_string(steps.size()) +
            "\nSpace – next step, R – reset");
        if (font.loadFromFile("ofont.ru_Eastern Rhodopes.ttf"))
            window.draw(stepText);

        window.display();
    }

    cout << "Convex hull vertices:" << "\n";
    for (const auto& p : hull) {
        cout << "(" << p.x << ", " << p.y << ")" << "\n";
    }
    cout << "Total animation steps: " << steps.size() << "\n";

    return 0;
}
