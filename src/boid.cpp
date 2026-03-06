#include "Boid.hpp"
#include "Profiler.hpp"
#include "circle.hpp"
#include "point.hpp"
#include <cmath>
#include <iostream>

Boid::Boid(const float x, const float y) noexcept
    : position(x, y), acceleration(0.0f, 0.0f),
      velocity((rand() % 200 - 100) / 50.0f, (rand() % 200 - 100) / 50.0f) {}

void Boid::flock(QuadTree& tree) noexcept {
    Circle range(position.x, position.y, perceptionRadius);
    std::vector<Point> neighbourCache;
    tree.queryCircle(tree, range, neighbourCache);
    sf::Vector2f alignSteering(0.0f, 0.0f);
    sf::Vector2f separateSteering(0.0f, 0.0f);
    sf::Vector2f cohesionSteering(0.0f, 0.0f);
    int total = 0;

    for (auto& point : neighbourCache) {
        Boid* other = point.getBoid();
        if (!other || other == this) continue;
        alignSteering += other->velocity;
        cohesionSteering += other->position;
        const float d = magnitude(position - other->position);
        if (d > 0) {
            sf::Vector2f diff = position - other->position;
            diff = normalize(diff);
            diff /= (d * d);
            separateSteering += diff;
        }
        ++total;
    }
    const float totalFloat = static_cast<float>(total);
    if (total > 0) {
    alignSteering /= totalFloat;
    alignSteering = setMag(alignSteering, maxSpeed);
    alignSteering -= velocity;
    alignSteering = limit(alignSteering, maxForce);
    
    cohesionSteering /= totalFloat;
    cohesionSteering -= position;
    cohesionSteering = setMag(cohesionSteering, maxSpeed);
    cohesionSteering -= velocity;
    cohesionSteering = limit(cohesionSteering, maxForce);

    separateSteering /= totalFloat;
    separateSteering = setMag(separateSteering, maxSpeed);
    separateSteering -= velocity;
    separateSteering = limit(separateSteering, maxForce);
    }
    acceleration += alignSteering * 1.0f;
    acceleration += cohesionSteering * 1.0f;
    acceleration += separateSteering * 1.0f;
}

void Boid::update() noexcept {
  velocity += acceleration;
  velocity = limit(velocity, maxSpeed);
  position += velocity;
  acceleration = sf::Vector2f(0.0f, 0.0f);
}

void Boid::edges(const float width, const float height) noexcept {
  if (position.x > width)
    position.x = 0.0f;
  else if (position.x < 0.0f)
    position.x = width;

  if (position.y > height)
    position.y = 0.0f;
  else if (position.y < 0.0f)
    position.y = height;
}

void Boid::draw(sf::RenderWindow &window) const {
  sf::ConvexShape triangle(3);

  const float angle = std::atan2(velocity.y, velocity.x);

  triangle.setPoint(0, sf::Vector2f(10.0f, 0.0f));
  triangle.setPoint(1, sf::Vector2f(-5.0f, 5.0f));
  triangle.setPoint(2, sf::Vector2f(-5.0f, -5.0f));

  triangle.setFillColor(sf::Color::White);
  triangle.setPosition(position);
  triangle.setRotation(sf::degrees(angle * 180.0f / 3.14159f));
  window.draw(triangle);
}

constexpr float Boid::magnitude(const sf::Vector2f &v) const noexcept {
  return std::sqrt(v.x * v.x + v.y * v.y);
}

constexpr sf::Vector2f Boid::normalize(const sf::Vector2f &v) const noexcept {
  const float mag = magnitude(v);
  if (mag > 0.0f)
    return v / mag;
  return v;
}

constexpr sf::Vector2f Boid::setMag(const sf::Vector2f &v,
                                    const float newMag) const noexcept {
  return normalize(v) * newMag;
}

constexpr sf::Vector2f Boid::limit(const sf::Vector2f &v,
                                   const float max) const noexcept {
  const float mag = magnitude(v);
  if (mag > max) {
    return (v / mag) * max;
  }
  return v;
}
