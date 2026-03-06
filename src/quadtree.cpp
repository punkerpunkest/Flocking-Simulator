#include "Profiler.hpp" 
#include "quadtree.hpp"
#include "circle.hpp"
#include "quadtreenode.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

QuadTree::QuadTree(QuadTreeNode boundary, int capacity, Arena* arena)
    : boundary(boundary), capacity(capacity), divided(false), arena(arena) {};

bool QuadTree::canContain(Point p) {
  return boundary.canContain(p);
}

void QuadTree::insertPoint(Point p) {
  if (!canContain(p)) {
    return;
  }
  
  if (points.size() < capacity && !divided) {
    points.push_back(p);
  } else {
    if (!divided) {
      if (boundary.getWidth() < 1.0f || boundary.getHeight() < 1.0f) {
        points.push_back(p);
        return;
      }
      this->subDivide();
      auto oldPoints = std::move(points);
      for (auto& p : oldPoints) {
        if (nodes.northEast->canContain(p)) {
          nodes.northEast->insertPoint(p);
        } else if (nodes.northWest->canContain(p)) {
          nodes.northWest->insertPoint(p);
        } else if (nodes.southEast->canContain(p)) {
          nodes.southEast->insertPoint(p);
        } else if (nodes.southWest->canContain(p)) {
          nodes.southWest->insertPoint(p);
        }
      }
    }
    
    if (nodes.northEast->canContain(p)) {
      nodes.northEast->insertPoint(p);
    } else if (nodes.northWest->canContain(p)) {
      nodes.northWest->insertPoint(p);
    } else if (nodes.southEast->canContain(p)) {
      nodes.southEast->insertPoint(p);
    } else if (nodes.southWest->canContain(p)) {
      nodes.southWest->insertPoint(p);
    }
  }
}

void QuadTree::subDivide() {
  auto width = boundary.getWidth();
  auto height = boundary.getHeight();

  if (width < 1.0f || height < 1.0f) {
    return;  
  }

  auto xCoord = boundary.getXCoord();
  auto yCoord = boundary.getYCoord();

  auto northEastBoundary = QuadTreeNode(
      width / 2, height / 2, xCoord + width / 2, yCoord - height / 2);
  auto northWestBoundary = QuadTreeNode(
      width / 2, height / 2, xCoord - width / 2, yCoord - height / 2);
  auto southEastBoundary = QuadTreeNode(
      width / 2, height / 2, xCoord + width / 2, yCoord + height / 2);
  auto southWestBoundary = QuadTreeNode(
      width / 2, height / 2, xCoord - width / 2, yCoord + height / 2);

  nodes.northEast = allocArenaType<QuadTree>(arena);
  new (nodes.northEast) QuadTree(northEastBoundary, capacity, arena);

  nodes.northWest = allocArenaType<QuadTree>(arena);
  new (nodes.northWest) QuadTree(northWestBoundary, capacity, arena);

  nodes.southEast = allocArenaType<QuadTree>(arena);
  new (nodes.southEast) QuadTree(southEastBoundary, capacity, arena);

  nodes.southWest = allocArenaType<QuadTree>(arena);
  new (nodes.southWest) QuadTree(southWestBoundary, capacity, arena);
  
  divided = true;
}

void QuadTree::query(QuadTree &node, std::vector<Point> &found) {
  if (!node.boundary.intersects(boundary)) {
    return;
  }

  if (boundary.contains(node.boundary)) {
    for (auto &p : node.points) {
      found.push_back(p);
    }
    return;
  }

  for (auto &p : node.points) {
    if (boundary.canContain(p)) {
      found.push_back(p);
    }
  }

  if (node.divided) {
    query(*node.nodes.northEast, found);
    query(*node.nodes.northWest, found);
    query(*node.nodes.southEast, found);
    query(*node.nodes.southWest, found);
  }
}

void QuadTree::queryCircle(QuadTree &node, Circle &c, std::vector<Point> &found) {
  {
    if (!c.intersects(node.boundary)) {
      return;
    }
  }

  {
    if (c.completeContains(node.boundary)) {
      for (auto &p : node.points) {
        found.push_back(p);
      }
      if (node.divided) {
        queryCircle(*node.nodes.northEast, c, found);
        queryCircle(*node.nodes.northWest, c, found);
        queryCircle(*node.nodes.southEast, c, found);
        queryCircle(*node.nodes.southWest, c, found);
      }
      return;
    }
  }
  
  {
    for (auto &p : node.points) {
      if (c.canContain(p)) {
        found.push_back(p);
      }
    }
  }

  if (node.divided) {
    queryCircle(*node.nodes.northEast, c, found);
    queryCircle(*node.nodes.northWest, c, found);
    queryCircle(*node.nodes.southEast, c, found);
    queryCircle(*node.nodes.southWest, c, found);
  }
}

void QuadTree::draw(sf::RenderWindow &window, sf::Color color) {
  sf::RectangleShape rect;
  rect.setSize(sf::Vector2f(boundary.getWidth() * 2, boundary.getHeight() * 2));
  rect.setPosition(sf::Vector2f(
      boundary.getXCoord() - boundary.getWidth(), 
      boundary.getYCoord() - boundary.getHeight()
  ));
  rect.setFillColor(sf::Color::Transparent);
  rect.setOutlineColor(color);
  rect.setOutlineThickness(1.0f);
  window.draw(rect);

  for (auto &p : points) {
    sf::CircleShape point(2.0f);
    point.setPosition(sf::Vector2f(p.getX() - 2.0f, p.getY() - 2.0f));
    point.setFillColor(sf::Color::Green);
    window.draw(point);
  }

  if (divided) {
    nodes.northEast->draw(window, color);
    nodes.northWest->draw(window, color);
    nodes.southEast->draw(window, color);
    nodes.southWest->draw(window, color);
  }
}
