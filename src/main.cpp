#include "profiler.hpp"
#include <SFML/Graphics.hpp>
#include "Boid.hpp"
#include "quadtree.hpp"
#include "quadtreenode.hpp"
#include "arena.hpp"
#include "point.hpp"
#include <vector>
#include <iostream>

int main() {
    constexpr int WIDTH = 12000;
    constexpr int HEIGHT = 12000;    

    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Boids Flocking");
    window.setFramerateLimit(60);

    sf::View view(sf::FloatRect({0,0}, {WIDTH, HEIGHT}));
    window.setView(view);
    
    sf::Clock fpsClock;
    int fpsFrames = 0;
    float fps = 0.0f;

    
    std::vector<Boid> boids;
    boids.reserve(20000);

    int frameCount = 0;
    sf::Clock printClock;
    for (int i = 0; i < 20000; i++) {
        float x = rand() % WIDTH;
        float y = rand() % HEIGHT;
        boids.emplace_back(x, y);
    }
    Arena* arena = createArena(1024 * 1024);

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePress->button == sf::Mouse::Button::Left) {
                    boids.emplace_back(mousePress->position.x, mousePress->position.y);
                }
            }
            
            //if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
             //   if (keyPress->code == sf::Keyboard::Key::Space) {
              //      showQuadTree = !showQuadTree;
    
              //  }
            //}
        }
        
        frameCount++;
        resetArena(arena);

        QuadTreeNode boundary(WIDTH / 2, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
        QuadTree tree(boundary, 8, arena);

        {
            for (auto& boid : boids) {
                Point p(boid.getX(), boid.getY(), &boid);
                tree.insertPoint(p);
            }
        }
        
        {
            for (auto& boid: boids) {
                boid.flock(tree);
                boid.update();
                boid.edges(WIDTH, HEIGHT);
            }
        }
        
        window.clear(sf::Color::Black);
        
        for (auto& boid: boids) {
            boid.draw(window);
        }

            ++fpsFrames;
    const float elapsed = fpsClock.getElapsedTime().asSeconds();
    if (elapsed >= 2.0f) {
        fps = static_cast<float>(fpsFrames) / elapsed;
        std::cout << fps << "\n";
        fpsFrames = 0;
        fpsClock.restart();
    }
        
       window.display();
    }

    freeArena(arena);
    return 0;
}
