#pragma once
#include "box.hpp"
#include "point.hpp"
#include <vector>
#include <cstdint>
#include <SFML/Graphics.hpp>

using nodeId = std::uint32_t;

static constexpr nodeId null = nodeId(-1);
inline uint8_t maxCapacity = 8;
inline uint8_t maxDepth = 8;
inline sf::Color color = sf::Color(255,255,255,80);

struct node {
    nodeId children[2][2] {
        {null, null},
        {null, null}
    };
};

struct stackEntry {
    nodeId id;
    box bbox;
    stackEntry(nodeId id2, box boundingBox) {
        id = id2;
        bbox = std::move(boundingBox);
    }
};

struct quadtree {
    box boundingBox;
    nodeId root;
    std::vector<node> nodes;
    std::vector<point> points;
    std::vector<std::uint32_t> nodePointsBegin;
    mutable std::vector<stackEntry> queryStack;
    quadtree build(std::vector<point> points);
    void query(float x, float y, float perceptionRadius, int Id, std::vector<point>& result) const;
    void clear();
    void collectLines(sf::VertexArray& lines, box const& rootReplacement);

    template <typename Iterator>
    nodeId build_impl(quadtree & tree, box const& bbox, Iterator begin, Iterator end, std::size_t depthLimit) {
        if (begin == end) return null;
        nodeId result = tree.nodes.size();
        tree.nodes.emplace_back();
        tree.nodePointsBegin.push_back(begin - tree.points.begin());

        if (depthLimit == 0) return result;
        if (std::distance(begin, end) <= maxCapacity) return result;

        point center = middle(bbox.min, bbox.max);

        auto bottom = [center](point const & p){ return p.y < center.y; };
        auto left = [center](point const & p){ return p.x < center.x; };

        Iterator split_y = std::partition(begin, end, bottom);
        Iterator split_x_lower = std::partition(begin, split_y, left);
        Iterator split_x_upper = std::partition(split_y, end, left);
        
        tree.nodes[result].children[0][0] = build_impl(tree, box{ bbox.min, center}, begin, split_x_lower, depthLimit - 1);
        tree.nodes[result].children[0][1] = build_impl(tree, box{ { center.x, bbox.min.y, -1}, { bbox.max.x, center.y, -1} }, split_x_lower, split_y, depthLimit - 1);
        tree.nodes[result].children[1][0] = build_impl(tree, box{ { bbox.min.x, center.y, -1}, { center.x, bbox.max.y, -1} }, split_y, split_x_upper, depthLimit - 1);
        tree.nodes[result].children[1][1] = build_impl(tree, box{ center, bbox.max}, split_x_upper, end, depthLimit - 1);

        return result;
    }

    static bool intersects(box const& b, float cx, float cy, float r) {
        float dx = std::max({ b.min.x - cx, 0.0f, cx - b.max.x });
        float dy = std::max({ b.min.y - cy, 0.0f, cy - b.max.y });
        return dx * dx + dy * dy <= r * r;
    }
};