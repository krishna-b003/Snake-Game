#include <SFML/Graphics.hpp>
#include <deque>
#include <random>
#include <iostream>
#include <algorithm>

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
                    direction.y != 1) {
                    direction = {0, -1};
                }

                if (key->scancode == sf::Keyboard::Scancode::S &&
                    direction.y != -1) {
                    direction = {0, 1};
                }

                if (key->scancode == sf::Keyboard::Scancode::A &&
                    direction.x != 1) {
                    direction = {-1, 0};
                }

                if (key->scancode == sf::Keyboard::Scancode::D &&
                    direction.x != -1) {
                    direction = {1, 0};
                }

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

            // Wall collision
            if (newHead.x < 0 ||
                newHead.x >= WIDTH / CELL_SIZE ||
                newHead.y < 0 ||
                newHead.y >= HEIGHT / CELL_SIZE) {

                gameOver = true;
            }

            // Self collision
            for (const auto& part : snake) {
                if (newHead == part) {
                    gameOver = true;
                    break;
                }
            }

            if (!gameOver) {

                snake.push_front(newHead);

                // Food collision
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

        window.clear(sf::Color::Black);

        // Draw snake
        sf::RectangleShape snakePart(
            sf::Vector2f(
                CELL_SIZE - 2,
                CELL_SIZE - 2
            )
        );

        snakePart.setFillColor(sf::Color::Green);

        for (const auto& part : snake) {
            snakePart.setPosition({
                static_cast<float>(part.x * CELL_SIZE + 1),
                static_cast<float>(part.y * CELL_SIZE + 1)
            });

            window.draw(snakePart);
        }

        // Draw food
        sf::RectangleShape foodShape(
            sf::Vector2f(
                CELL_SIZE - 2,
                CELL_SIZE - 2
            )
        );

        foodShape.setFillColor(sf::Color::Red);

        foodShape.setPosition({
            static_cast<float>(food.x * CELL_SIZE + 1),
            static_cast<float>(food.y * CELL_SIZE + 1)
        });

        window.draw(foodShape);

        window.display();
    }

    return 0;
}