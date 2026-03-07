#include "quadtree.hpp"

quadtree quadtree::build(std::vector<point> points) {
    quadtree result;
    result.points = std::move(points);
    result.boundingBox = bbox(result.points.begin(), result.points.end());
    result.root = build_impl(result,
        bbox(result.points.begin(), result.points.end()),
        result.points.begin(), result.points.end(), maxDepth);
    result.nodePointsBegin.push_back(result.points.size());
    return result;
}

void quadtree::query(float x, float y, float perceptionRadius, int Id, std::vector<point>& result) const{
    if (root == null) return;
    queryStack.clear();
    queryStack.emplace_back(root,boundingBox);
    const float r2 = perceptionRadius * perceptionRadius;

    while (!queryStack.empty()) {
        const stackEntry entry = queryStack.back();
        queryStack.pop_back();
        const nodeId id = entry.id;
        const box nodeBox = entry.bbox;

        if (!intersects(nodeBox, x, y, perceptionRadius)) continue;

        const std::uint32_t pBegin = nodePointsBegin[id];
        const std::uint32_t pEnd = nodePointsBegin[id + 1];
        for (std::uint32_t i = pBegin; i < pEnd; ++i) {
            if (points[i].id == Id) continue;
            const float dx = points[i].x - x;
            const float dy = points[i].y - y;
            if (dx * dx + dy * dy <= r2)
                result.push_back(points[i]);
        }
        node const& n = nodes[id];
        const point center = middle(nodeBox.min, nodeBox.max);
        const box childBoxes[2][2] = {
            {{nodeBox.min,center},{{center.x,nodeBox.min.y, -1}, {nodeBox.max.x, center.y, -1}}},
            {{{nodeBox.min.x,center.y,-1},{center.x, nodeBox.max.y,-1}},{center,nodeBox.max}}};

        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                if (n.children[i][j] != null)
                    queryStack.emplace_back(n.children[i][j], childBoxes[i][j]);
    }
}

void quadtree::clear() {
    nodes.clear();
    points.clear();
    nodePointsBegin.clear();
    root = null;
    boundingBox = {{inf,inf,-1},{-inf,-inf,-1}};
}

void quadtree::collectLines(sf::VertexArray& lines, box const& rootReplacement) {
    if (root == null) return;

    queryStack.clear();
    queryStack.emplace_back(root, rootReplacement);

    while (!queryStack.empty()) {
        auto back = queryStack.back();
        queryStack.pop_back();
        const box& b = back.bbox;

        sf::Vector2f topLeft(b.min.x, b.min.y);
        sf::Vector2f topRight(b.max.x, b.min.y);
        sf::Vector2f bottomRight(b.max.x, b.max.y);
        sf::Vector2f bottomLeft(b.min.x, b.max.y);

        lines.append(sf::Vertex{topLeft, color});
        lines.append(sf::Vertex{topRight, color});

        lines.append(sf::Vertex{topRight, color});
        lines.append(sf::Vertex{bottomRight, color});

        lines.append(sf::Vertex{bottomRight, color});
        lines.append(sf::Vertex{bottomLeft, color});

        lines.append(sf::Vertex{bottomLeft, color});
        lines.append(sf::Vertex{topLeft, color});

        node const& n = nodes[back.id];
        const point center = middle(b.min, b.max);

        const box childBoxes[2][2] = {
            {{b.min, center}, {{center.x, b.min.y,-1}, {b.max.x, center.y, -1}}},
            {{{b.min.x, center.y, -1}, {center.x, b.max.y, -1}}, {center, b.max}}
        };

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; ++j) {
                if (n.children[i][j] != null) {
                    queryStack.emplace_back(n.children[i][j], childBoxes[i][j]);
                }
            }
        }
    }

}

