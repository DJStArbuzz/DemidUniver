#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <vector>

using namespace sf;
using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 600;
const float CENTER_X = WINDOW_WIDTH / 2.0f;
const float CENTER_Y = WINDOW_HEIGHT / 2.0f;

const float TABLE_WIDTH = 900.0f;
const float TABLE_HEIGHT = 500.0f;
const float PADDLE_WIDTH = 20.0f;
const float PADDLE_HEIGHT = 100.0f;
const float BALL_RADIUS = 10.0f;
const float PADDLE_SPEED = 8.0f;
const float BALL_SPEED = 5.0f;

class Point {
public:
    float coords[3];

    Point(float x = 0, float y = 0, float w = 1.0f) {
        coords[0] = x;
        coords[1] = y;
        coords[2] = w;
    }

    float x() const { return coords[0]; }
    float y() const { return coords[1]; }
    float w() const { return coords[2]; }

    void set(float x, float y, float w = 1.0f) {
        coords[0] = x;
        coords[1] = y;
        coords[2] = w;
    }
};

Point multiplyMatrix(const Point& p, const float mat[3][3]) {
    float x = p.coords[0];
    float y = p.coords[1];
    float w = p.coords[2];

    float nx = x * mat[0][0] + y * mat[1][0] + w * mat[2][0];
    float ny = x * mat[0][1] + y * mat[1][1] + w * mat[2][1];
    float nw = x * mat[0][2] + y * mat[1][2] + w * mat[2][2];

    return Point(nx, ny, nw);
}

void translationMatrix(float dx, float dy, float mat[3][3]) {
    mat[0][0] = 1; mat[0][1] = 0; mat[0][2] = 0;
    mat[1][0] = 0; mat[1][1] = 1; mat[1][2] = 0;
    mat[2][0] = dx; mat[2][1] = dy; mat[2][2] = 1;
}

void reflectionOXMatrix(float mat[3][3]) {
    mat[0][0] = 1; mat[0][1] = 0; mat[0][2] = 0;
    mat[1][0] = 0; mat[1][1] = -1; mat[1][2] = 0;
    mat[2][0] = 0; mat[2][1] = 0; mat[2][2] = 1;
}

void reflectionOYMatrix(float mat[3][3]) {
    mat[0][0] = -1; mat[0][1] = 0; mat[0][2] = 0;
    mat[1][0] = 0; mat[1][1] = 1; mat[1][2] = 0;
    mat[2][0] = 0; mat[2][1] = 0; mat[2][2] = 1;
}

class Paddle {
public:
    float x, y;
    float width, height;
    int score;

    Paddle(float startX, float startY, float w, float h)
        : x(startX), y(startY), width(w), height(h), score(0) {
    }

    void move(float dy) {
        y += dy;
        float halfH = height / 2.0f;
        float limit = TABLE_HEIGHT / 2.0f - halfH;

        if (y > limit) 
            y = limit;
        if (y < -limit) 
            y = -limit;
    }

    FloatRect getRect() const {
        return FloatRect(x - width / 2, y - height / 2, width, height);
    }
};

class Ball {
public:
    Point position;  
    float vx, vy;    
    float radius;

    Ball(float startX, float startY, float r)
        : position(startX, startY, 1.0f), radius(r)
    {
        vx = (rand() % 2 == 0) ? BALL_SPEED : -BALL_SPEED;
        vy = (rand() % 2 == 0) ? BALL_SPEED : -BALL_SPEED;
    }

    void update() {
        float mat[3][3];
        translationMatrix(vx, vy, mat);
        position = multiplyMatrix(position, mat);
    }

    void bounceWalls(float tableHalfHeight) {
        if (position.y() + radius > tableHalfHeight || position.y() - radius < -tableHalfHeight) {
            float mat[3][3];
            reflectionOXMatrix(mat);
            Point vel(vx, vy, 0.0f);
            vel = multiplyMatrix(vel, mat);
            vx = vel.x();
            vy = vel.y();

            if (position.y() + radius > tableHalfHeight)
                position.set(position.x(), tableHalfHeight - radius, 1.0f);
            else
                position.set(position.x(), -tableHalfHeight + radius, 1.0f);
        }
    }

    bool checkPaddleCollision(const Paddle& paddle) {
        FloatRect ballRect(position.x() - radius, position.y() - radius, 2 * radius, 2 * radius);
        if (ballRect.intersects(paddle.getRect())) {
            float mat[3][3];
            reflectionOYMatrix(mat);
            Point vel(vx, vy, 0.0f);
            vel = multiplyMatrix(vel, mat);
            vx = vel.x();
            vy = vel.y();

            float hitPos = (position.y() - paddle.y) / (paddle.height / 2);
            vy += hitPos * 2.0f;

            float speed = sqrt(vx * vx + vy * vy);
            if (speed > BALL_SPEED * 2) {
                vx = vx / speed * BALL_SPEED * 2;
                vy = vy / speed * BALL_SPEED * 2;
            }
            return true;
        }
        return false;
    }

    void reset() {
        position.set(0, 0, 1.0f);
        vx = (rand() % 2 == 0) ? BALL_SPEED : -BALL_SPEED;
        vy = (rand() % 2 == 0) ? BALL_SPEED : -BALL_SPEED;
    }
};

int main() {
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pong");
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile("arial.ttf")) {
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            cerr << "Не удалось загрузить шрифт! Счёт не будет отображаться." << endl;
        }
    }

    Paddle leftPaddle(-TABLE_WIDTH / 2 + 50, 0, PADDLE_WIDTH, PADDLE_HEIGHT);
    Paddle rightPaddle(TABLE_WIDTH / 2 - 50, 0, PADDLE_WIDTH, PADDLE_HEIGHT);
    Ball ball(0, 0, BALL_RADIUS);

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(Color::Black);
    scoreText.setPosition(CENTER_X - 50, 20);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        if (Keyboard::isKeyPressed(Keyboard::W)) 
            leftPaddle.move(PADDLE_SPEED);
        if (Keyboard::isKeyPressed(Keyboard::S)) 
            leftPaddle.move(-PADDLE_SPEED);
        if (Keyboard::isKeyPressed(Keyboard::Up))
            rightPaddle.move(PADDLE_SPEED);
        if (Keyboard::isKeyPressed(Keyboard::Down)) 
            rightPaddle.move(-PADDLE_SPEED);

        ball.update();
        ball.bounceWalls(TABLE_HEIGHT / 2.0f);
        if (ball.vx < 0) {
            ball.checkPaddleCollision(leftPaddle);
        }
        else {
            ball.checkPaddleCollision(rightPaddle);
        }
        if (ball.position.x() - ball.radius < -TABLE_WIDTH / 2) {
            rightPaddle.score++;
            ball.reset();
        }
        if (ball.position.x() + ball.radius > TABLE_WIDTH / 2) {
            leftPaddle.score++;
            ball.reset();
        }

        window.clear(Color::White);

        RectangleShape table(Vector2f(TABLE_WIDTH, TABLE_HEIGHT));
        table.setPosition(CENTER_X - TABLE_WIDTH / 2, CENTER_Y - TABLE_HEIGHT / 2);
        table.setFillColor(Color(0, 150, 0));
        table.setOutlineColor(Color::Black);
        table.setOutlineThickness(2);
        window.draw(table);

        auto toScreen = [](float wx, float wy) -> Vector2f {
            return Vector2f(CENTER_X + wx, CENTER_Y - wy);
            };

        Vector2f leftPos = toScreen(leftPaddle.x, leftPaddle.y);
        RectangleShape leftRect(Vector2f(leftPaddle.width, leftPaddle.height));
        leftRect.setPosition(leftPos.x - leftPaddle.width / 2, leftPos.y - leftPaddle.height / 2);
        leftRect.setFillColor(Color::Blue);
        window.draw(leftRect);

        Vector2f rightPos = toScreen(rightPaddle.x, rightPaddle.y);
        RectangleShape rightRect(Vector2f(rightPaddle.width, rightPaddle.height));
        rightRect.setPosition(rightPos.x - rightPaddle.width / 2, rightPos.y - rightPaddle.height / 2);
        rightRect.setFillColor(Color::Red);
        window.draw(rightRect);

        Vector2f ballPos = toScreen(ball.position.x(), ball.position.y());
        CircleShape ballCircle(ball.radius);
        ballCircle.setPosition(ballPos.x - ball.radius, ballPos.y - ball.radius);
        ballCircle.setFillColor(Color::Yellow);
        ballCircle.setOutlineColor(Color::Black);
        ballCircle.setOutlineThickness(2);
        window.draw(ballCircle);

        stringstream ss;
        ss << leftPaddle.score << "  :  " << rightPaddle.score;
        scoreText.setString(ss.str());
        window.draw(scoreText);

        window.display();
    }

    return 0;
}
