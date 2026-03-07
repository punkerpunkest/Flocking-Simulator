#include "boid.hpp"

Boid::Boid(float x, float y, int32_t index) {
    position.x = x;
    position.y = y;
    universalIndex = index;
}

Boid::Boid() {
    position.x = 0.0f;
    position.y = 0.0f;
    universalIndex = -1;
}

void Boid::flock(quadtree& tree, std::vector<sf::Vector2f>& velocities, std::vector<sf::Vector2f>& accelerations, std::vector<point>& cache, sf::Vector2f& alignSteering, sf::Vector2f& separateSteering, sf::Vector2f& cohesionSteering) noexcept {
    alignSteering = {0.0f, 0.0f};
    cohesionSteering = {0.0f, 0.0f};
    separateSteering = {0.0f, 0.0f};
    cache.clear();
    tree.query(position.x, position.y, perceptionRadius, universalIndex, cache);
    int total = 0;

    for (auto& point: cache) {
        sf::Vector2f otherPosition(point.x, point.y);
        alignSteering += velocities[point.id];
        cohesionSteering += otherPosition;
        const float d = magnitude(position - otherPosition);

        if (d > 0) {
            sf::Vector2f diff = position - otherPosition;
            diff = normalize(diff);
            diff /= (d*d);
            separateSteering += diff;
        }
        ++total;
    }

    const float totalFloat = static_cast<float>(total);
    if (total > 0) {
        alignSteering /= totalFloat;
        alignSteering = setMag(alignSteering, maxSpeed);
        alignSteering -= velocities[universalIndex];
        alignSteering = limit(alignSteering, maxForce);

        cohesionSteering /= totalFloat;
        cohesionSteering -= position;
        cohesionSteering = setMag(cohesionSteering, maxSpeed);
        cohesionSteering -= velocities[universalIndex];
        cohesionSteering = limit(cohesionSteering, maxForce);

        separateSteering /= totalFloat;
        separateSteering = setMag(separateSteering, maxSpeed);
        separateSteering -= velocities[universalIndex];
        separateSteering = limit(separateSteering, maxForce);
    }

    accelerations[universalIndex] += alignSteering * 1.0f;
    accelerations[universalIndex] += cohesionSteering * 1.0f;
    accelerations[universalIndex] += separateSteering * 1.0f;

}

void Boid::update(sf::Vector2f& velocity, sf::Vector2f& acceleration) noexcept {
        velocity += acceleration;
        velocity = limit(velocity, maxSpeed);
        position += velocity;
        acceleration = sf::Vector2f(0.0f, 0.0f);
}

void Boid::edges(float width, float height) noexcept{
    if (position.x > width) {
        position.x = 0.0f;
    }
    else if (position.x < 0.0f) {
        position.x = width;
    }
    if (position.y > height) {
        position.y = 0.0f;
    }
    else if (position.y < 0.0f) {
        position.y = height;
    }
}

