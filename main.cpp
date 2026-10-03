#include <SFML/Graphics.hpp>
#include <deque>
#include <random>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

const int CELL_SIZE = 25;
const int WIDTH = 800;
const int HEIGHT = 600;

struct Position {
    int x;
    int y;

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

Position generateFood(const deque<Position>& snake, mt19937& rng) {
    uniform_int_distribution<int> xDist(0, WIDTH / CELL_SIZE - 1);
    uniform_int_distribution<int> yDist(0, HEIGHT / CELL_SIZE - 1);

    Position food;

    do {
        food = {xDist(rng), yDist(rng)};
    } while (find(snake.begin(), snake.end(), food) != snake.end());

    return food;
}

int main() {
    sf::RenderWindow window(
        sf::VideoMode({WIDTH, HEIGHT}),
        "Snake Game"
    );

    window.setFramerateLimit(60);

    deque<Position> snake = {
        {10, 10},
        {9, 10},
        {8, 10}
    };

    Position direction = {1, 0};

    mt19937 rng(random_device{}());
    Position food = generateFood(snake, rng);

    sf::Clock clock;
    float moveTimer = 0.0f;
    float moveDelay = 0.12f;

    int score = 0;
    bool gameOver = false;

    while (window.isOpen()) {

        while (auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>())
                window.close();

            if (auto key = event->getIf<sf::Event::KeyPressed>()) {

                if (key->scancode == sf::Keyboard::Scancode::W &&
                    direction.y != 1)
                    direction = {0, -1};

                if (key->scancode == sf::Keyboard::Scancode::S &&
                    direction.y != -1)
                    direction = {0, 1};

                if (key->scancode == sf::Keyboard::Scancode::A &&
                    direction.x != 1)
                    direction = {-1, 0};

                if (key->scancode == sf::Keyboard::Scancode::D &&
                    direction.x != -1)
                    direction = {1, 0};

                if (key->scancode == sf::Keyboard::Scancode::R &&
                    gameOver) {

                    snake = {
                        {10, 10},
                        {9, 10},
                        {8, 10}
                    };

                    direction = {1, 0};
                    food = generateFood(snake, rng);
                    score = 0;
                    gameOver = false;
                    moveTimer = 0;
                    moveDelay = 0.12f;
                }
            }
        }

        float dt = clock.restart().asSeconds();
        moveTimer += dt;

        if (!gameOver && moveTimer >= moveDelay) {

            moveTimer = 0;

            Position newHead = {
                snake.front().x + direction.x,
                snake.front().y + direction.y
            };

            if (newHead.x < 0 ||
                newHead.x >= WIDTH / CELL_SIZE ||
                newHead.y < 0 ||
                newHead.y >= HEIGHT / CELL_SIZE) {
                gameOver = true;
            }

            for (const auto& part : snake) {
                if (newHead == part) {
                    gameOver = true;
                    break;
                }
            }

            if (!gameOver) {
                snake.push_front(newHead);

                if (newHead == food) {
                    score++;
                    food = generateFood(snake, rng);

                    if (moveDelay > 0.05f)
                        moveDelay -= 0.003f;
                }
                else {
                    snake.pop_back();
                }
            }
        }

        // Dark green textured-looking background.
        window.clear(sf::Color(12, 20, 16));

        // Subtle grid.
        sf::RectangleShape gridLine;
        gridLine.setFillColor(sf::Color(20, 34, 27, 180));

        for (int x = 0; x <= WIDTH; x += CELL_SIZE) {
            gridLine.setSize({1.f, static_cast<float>(HEIGHT)});
            gridLine.setPosition({static_cast<float>(x), 0.f});
            window.draw(gridLine);
        }

        for (int y = 0; y <= HEIGHT; y += CELL_SIZE) {
            gridLine.setSize({static_cast<float>(WIDTH), 1.f});
            gridLine.setPosition({0.f, static_cast<float>(y)});
            window.draw(gridLine);
        }

        // Draw body first so the head can have a distinct design.
        bool first = true;

        for (const auto& part : snake) {
            float px = static_cast<float>(part.x * CELL_SIZE);
            float py = static_cast<float>(part.y * CELL_SIZE);

            if (first) {
                // Head: rounded-ish circular shape with a darker outline.
                sf::CircleShape head(11.f);
                head.setOrigin({11.f, 11.f});
                head.setPosition({
                    px + CELL_SIZE / 2.f,
                    py + CELL_SIZE / 2.f
                });
                head.setFillColor(sf::Color(46, 170, 75));
                head.setOutlineThickness(2.f);
                head.setOutlineColor(sf::Color(10, 70, 28));
                window.draw(head);

                // Eyes placed according to movement direction.
                float side = 4.2f;
                float forward = 5.2f;

                sf::Vector2f center(
                    px + CELL_SIZE / 2.f,
                    py + CELL_SIZE / 2.f
                );

                sf::Vector2f front(
                    direction.x * forward,
                    direction.y * forward
                );

                sf::Vector2f perpendicular(
                    -direction.y * side,
                    direction.x * side
                );

                sf::CircleShape eyeWhite(3.4f);
                eyeWhite.setOrigin({3.4f, 3.4f});
                eyeWhite.setFillColor(sf::Color(235, 245, 235));

                eyeWhite.setPosition(center + front + perpendicular);
                window.draw(eyeWhite);

                eyeWhite.setPosition(center + front - perpendicular);
                window.draw(eyeWhite);

                sf::CircleShape pupil(1.7f);
                pupil.setOrigin({1.7f, 1.7f});
                pupil.setFillColor(sf::Color(15, 15, 15));

                pupil.setPosition(center + front + perpendicular);
                window.draw(pupil);

                pupil.setPosition(center + front - perpendicular);
                window.draw(pupil);

                // Small tongue.
                sf::RectangleShape tongue;
                tongue.setFillColor(sf::Color(220, 70, 80));
                tongue.setSize({7.f, 2.f});

                float tx = center.x + direction.x * 12.f;
                float ty = center.y + direction.y * 12.f;

                tongue.setOrigin({0.f, 1.f});
                tongue.setPosition({tx, ty});

                if (direction.x != 0)
                    tongue.setRotation(sf::degrees(direction.x > 0 ? 0.f : 180.f));
                else
                    tongue.setRotation(sf::degrees(direction.y > 0 ? 90.f : -90.f));

                window.draw(tongue);

                first = false;
            }
            else {
                // Body: alternating shades and a highlight to make it less flat.
                sf::RectangleShape body(
                    sf::Vector2f(CELL_SIZE - 3.f, CELL_SIZE - 3.f)
                );

                int shade = (part.x + part.y) % 2;

                body.setFillColor(
                    shade
                    ? sf::Color(35, 145, 60)
                    : sf::Color(43, 160, 67)
                );

                body.setOutlineThickness(1.5f);
                body.setOutlineColor(sf::Color(12, 80, 32));

                body.setPosition({
                    px + 1.5f,
                    py + 1.5f
                });

                window.draw(body);

                // Small body highlight.
                sf::RectangleShape highlight(
                    sf::Vector2f(CELL_SIZE - 9.f, 3.f)
                );
                highlight.setFillColor(sf::Color(80, 190, 90, 130));
                highlight.setPosition({
                    px + 4.f,
                    py + 4.f
                });

                window.draw(highlight);
            }
        }

        // Food: red apple-like object with highlight and leaf.
        float foodX = food.x * CELL_SIZE + CELL_SIZE / 2.f;
        float foodY = food.y * CELL_SIZE + CELL_SIZE / 2.f;

        sf::CircleShape apple(9.f);
        apple.setOrigin({9.f, 9.f});
        apple.setPosition({foodX, foodY + 2.f});
        apple.setFillColor(sf::Color(205, 45, 48));
        apple.setOutlineThickness(2.f);
        apple.setOutlineColor(sf::Color(115, 20, 25));
        window.draw(apple);

        sf::CircleShape shine(2.5f);
        shine.setOrigin({2.5f, 2.5f});
        shine.setFillColor(sf::Color(255, 180, 180));
        shine.setPosition({foodX - 3.f, foodY - 2.f});
        window.draw(shine);

        sf::RectangleShape stem;
        stem.setSize({2.5f, 6.f});
        stem.setFillColor(sf::Color(75, 45, 20));
        stem.setPosition({foodX, foodY - 10.f});
        stem.setRotation(sf::degrees(-15.f));
        window.draw(stem);

        sf::CircleShape leaf(4.f);
        leaf.setScale({1.5f, 0.55f});
        leaf.setFillColor(sf::Color(60, 150, 65));
        leaf.setPosition({foodX + 3.f, foodY - 10.f});
        leaf.setRotation(sf::degrees(-25.f));
        window.draw(leaf);

        window.display();
    }

    return 0;
}
