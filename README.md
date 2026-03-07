# Flocking Simulator

![Flocking Simulator](https://github.com/user-attachments/assets/3c0b21fc-bd78-42ef-82b1-e52142cace2e)

From the land to the sea to the air, flocking behavior can be seen in many animals. We found it fascinating that by just modeling our agents to follow a couple of rules to avoid collision, this behavior can be closely mirrored. The project also allowed us to optimize many features. As of late, we've improved neighbor queries with the QuadTree data structure, reduced allocation overhead with a custom arena memory allocator, and benchmarked the results with a custom profiler.

## Flocking Features

### Performance and Spatial Optimizations
- **Quad Tree Partitioning**: Space subdivision for O(n log n) neighbor queries
- **Circular Range Queries**: Intersection tests with early pruning
- **Arena Allocator**: Reduced allocation overhead and improved cache locality
- **Custom Profile**: Benchmarking tool validates 4x speedup

### Profiling 
**Flame Graphs** : Alongside our custom profiler, we enlist the use of Brendan Gregg's Flame graph to visually depict the effect of our changed architecture. The first picture shows the first iteration, and the second our final version.
<img width="1196" height="459" alt="firstVersionFlame" src="https://github.com/user-attachments/assets/08017cb0-0398-496f-a814-91335df973a1" />

We see extensive time taken for drawing all the boids, and the queryNode function for finding Boid neighbours. 

<img width="2394" height="1322" alt="newVersionFlame" src="https://github.com/user-attachments/assets/75aab8a1-0dd9-4967-b6d2-18018fd9c87c" />



### Boid behaviors
- **Alignment**: Boids steer toward the average velocity of their neighbors
- **Cohesion**: Boids move towards the center of local flocks
- **Separation**: Boids avoid crowding

To try out this project, the following are necessary:

## Prerequisites

- C++20 compatible compiler
- SFML

## Installation

### macOS
```bash
brew install sfml 
```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get install libsfml-dev
```

## To run

```bash
make
make run
```
