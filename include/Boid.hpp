#pragma once
#include "point.hpp"
#include "quadtree.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

class Boid {
    public:
        sf::Vector2f position;
        int32_t universalIndex;

        float maxSpeed = 4.0f;
        float maxForce = 0.1f;
        float perceptionRadius = 50.0f;
        Boid(float x, float y, int32_t index);
        Boid();
        void flock(quadtree& tree, std::vector<sf::Vector2f>& velocities, std::vector<sf::Vector2f>& acclerations, std::vector<point>& cache, sf::Vector2f& alignSteering, sf::Vector2f& separateSteering, sf::Vector2f& cohesionSteering) noexcept;
        void update(sf::Vector2f& velocity, sf::Vector2f& acceleration) noexcept;
        void edges(float width, float height) noexcept;

        constexpr float magnitude(const sf::Vector2f &v) const noexcept {
            return std::sqrt(v.x * v.x + v.y * v.y);
        }

        constexpr sf::Vector2f normalize(const sf::Vector2f &v) const noexcept {
            const float mag = magnitude(v);
            if (mag > 0.0f)
                return v / mag;
            return v;
        }

        constexpr sf::Vector2f setMag(const sf::Vector2f &v,const float newMag) const noexcept {
            return normalize(v) * newMag;
        }

        constexpr sf::Vector2f limit(const sf::Vector2f &v,const float max) const noexcept {
            const float mag = magnitude(v);
            if (mag > max) {
                return (v / mag) * max;
            }
            return v;
        }
};