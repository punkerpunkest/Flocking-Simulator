#ifndef BOID_HPP
#define BOID_HPP
#include <SFML/Graphics.hpp>
#include "point.hpp"
#include <vector>
#include "quadtree.hpp"

class Boid {
public:
  sf::Vector2f position;
  sf::Vector2f velocity;
  sf::Vector2f acceleration;
  float maxSpeed = 4.0f;
  float maxForce = 0.1f;
  float perceptionRadius = 50.0f;

  Boid(float x, float y) noexcept;

  sf::Vector2f align(QuadTree &tree) const noexcept;
  sf::Vector2f cohesion(QuadTree &tree) const noexcept;
  sf::Vector2f separation(QuadTree &tree) const noexcept;
  void flock(QuadTree &tree) noexcept;

  void update() noexcept;
  void edges(const float width, const float height) noexcept;
  void draw(sf::RenderWindow &window) const;

  float getX() const noexcept { return position.x; }
  float getY() const noexcept { return position.y; }
  sf::Vector2f getPosition() const noexcept { return position; }

private:
  constexpr float magnitude(const sf::Vector2f &v) const noexcept;
  constexpr sf::Vector2f normalize(const sf::Vector2f &v) const noexcept;
  constexpr sf::Vector2f setMag(const sf::Vector2f &v,
                                const float newMag) const noexcept;
  constexpr sf::Vector2f limit(const sf::Vector2f &v,
                               const float max) const noexcept;
  std::vector<Point> neighbourCache;
};

#endif
