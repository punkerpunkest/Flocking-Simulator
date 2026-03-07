#include "boid.hpp"
#include "point.hpp"   
#include "quadtree.hpp"
#include "box.hpp"
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>

int main() {
    constexpr int width = 1200;
    constexpr int height = 1200;
    int initialBoids = 100;

    std::vector<Boid> boids;
    std::vector<point> points;
    std::vector<sf::Vector2f> accelerations;
    std::vector<sf::Vector2f> velocities;
    std::vector<point> cache;
    sf::Vector2f alignSteering;
    sf::Vector2f cohesionSteering;
    sf::Vector2f separateSteering;

    boids.resize(initialBoids);
    accelerations.resize(initialBoids);
    velocities.resize(initialBoids);
    points.resize(initialBoids);
    cache.reserve(64);

    sf::VertexArray vertices(sf::PrimitiveType::Triangles, 3 * initialBoids);
    sf::VertexArray quadTreeLines(sf::PrimitiveType::Lines);

    box screenBox = {{0.0f, 0.0f, -1}, {(float)width, (float)height, -1}};

    //int frameCount = 0;
    //sf::Clock printClock; -> uncomment for FPS tracking :)
    bool showQuadTree = false;

    sf::RenderWindow window(sf::VideoMode({(unsigned)width, (unsigned)height}), "Flocking Simulator");
    window.setFramerateLimit(60);
    sf::View view(sf::FloatRect({0.0f, 0.0f}, {static_cast<float>(width), static_cast<float>(height)}));
    window.setView(view);
    for (int i = 0; i < initialBoids; i++) {
        boids[i] = Boid(static_cast<float>(rand() % width), static_cast<float>(rand() % height), i);
        accelerations[i] = sf::Vector2f(0.0f, 0.0f);
        velocities[i] = sf::Vector2f((rand() % 200 - 100) / 50.0f, (rand() % 200 - 100) / 50.0f);
        points[i] = point(boids[i].position.x, boids[i].position.y, i);
    }

    quadtree tree;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePress->button == sf::Mouse::Button::Left) {
                    sf::Vector2f worldPos = window.mapPixelToCoords(mousePress->position);
                    float x = worldPos.x;
                    float y = worldPos.y;
                    int loc = boids.size();
                    boids.emplace_back(x, y, loc); //emplace back for all of this ?
                    velocities.emplace_back((rand() % 200 - 100) / 50.0f, (rand() % 200 - 100) / 50.0f);
                    accelerations.emplace_back(0.0f, 0.0f);
                    points.emplace_back(x, y, loc);
                    vertices.resize((loc + 1) * 3);
                }
            }

            if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPress->code == sf::Keyboard::Key::Space) {
                    showQuadTree = !showQuadTree;
                }
            }
        }

        for (auto& boid : boids)
            points[boid.universalIndex] = point(boid.position.x, boid.position.y, boid.universalIndex);

        tree = quadtree{}.build(points);

        for (auto& boid : boids) {
            boid.flock(tree, velocities, accelerations, cache, alignSteering, separateSteering, cohesionSteering);
        }

        for (auto& boid : boids) {
            boid.update(velocities[boid.universalIndex], accelerations[boid.universalIndex]);
            boid.edges(width, height);
        }

        for (auto& boid : boids) {
            const float angle = std::atan2(velocities[boid.universalIndex].y, velocities[boid.universalIndex].x);
            const sf::Vector2f localShape[3] = {
                { 10.0f,  0.0f},
                {-5.0f,   5.0f},
                {-5.0f,  -5.0f}
            };
            const float cosA = std::cos(angle);
            const float sinA = std::sin(angle);

            for (int j = 0; j < 3; ++j) {
                const sf::Vector2f& local = localShape[j];
                sf::Vector2f rotated {local.x * cosA - local.y * sinA, local.x * sinA + local.y * cosA};
                vertices[boid.universalIndex * 3 + j].position = rotated + boid.position;
                vertices[boid.universalIndex * 3 + j].color = sf::Color::White;
            }
        }

        window.clear(sf::Color::Black);
        window.draw(vertices);

        if (showQuadTree) {
            quadTreeLines.clear();
            tree.collectLines(quadTreeLines, screenBox);
            window.draw(quadTreeLines);
        }

        window.display();

        tree.clear();

       // ++frameCount; counts FPS for rough benchmarking
       // if (printClock.getElapsedTime().asSeconds() >= 2.f) {
       //     float fps = frameCount / printClock.getElapsedTime().asSeconds();
       //     std::cout << "FPS: " << fps << std::endl;
       //     frameCount = 0;
       //     printClock.restart();
       // }
    }

    return 0;
}