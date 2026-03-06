#include "circle.hpp"
#include "quadtreenode.hpp"
#include "arena.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#pragma once
class QuadTree {
private:
  int capacity;
  bool divided;
  QuadTreeNode boundary;
  struct QuadTreeNodes {
    QuadTree* northEast = nullptr;  
    QuadTree* northWest = nullptr;  
    QuadTree* southEast = nullptr;  
    QuadTree* southWest = nullptr;  
  };
  Arena* arena; 
  QuadTreeNodes nodes;
  std::vector<Point> points;

public:
  QuadTree(QuadTreeNode boundary, int capacity, Arena* arena);
  bool canContain(Point p);
  void insertPoint(Point p);
  void subDivide();
  void query(QuadTree &node, std::vector<Point> &found);
  void queryCircle(QuadTree &node, Circle &c, std::vector<Point> &found);
  void draw(sf::RenderWindow &window, sf::Color = sf::Color::White);
};
