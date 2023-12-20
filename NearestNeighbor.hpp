#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#include <chrono>

struct Node {
    std::string name;
    double x;
    double y;

    Node(std::string name, double x, double y) : name(std::move(name)), x(x), y(y) {}

    double distanceTo(const Node& otherNode) const {
        double xDistance = x - otherNode.x;
        double yDistance = y - otherNode.y;
        return std::sqrt(xDistance * xDistance + yDistance * yDistance);
    }

    bool operator==(const Node& other) const {
        return name == other.name && x == other.x && y == other.y;
    }
};

static double calcDistance(const std::list<Node>& nodes) {
    double dist = 0.0;

    auto it = nodes.begin();
    while (std::next(it) != nodes.end()) {
        dist += it->distanceTo(*std::next(it));
        ++it;
    }

    dist += nodes.back().distanceTo(nodes.front());
    return dist;
}

static std::list<Node> readTSPFile(const std::string& filename) {
    std::list<Node> nodes;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << filename << std::endl;
        return nodes;
    }

    std::string line;
    bool readNodes = false;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        if (line == "NODE_COORD_SECTION") {
            readNodes = true;
            continue;
        }

        if (line == "EOF") {
            break;
        }

        if (readNodes) {
            std::istringstream iss(line);
            std::string name;
            double x, y;

            if (!(iss >> name >> x >> y)) {
                std::cerr << "Error: Unable to parse line: " << line << std::endl;
                continue;
            }

            nodes.emplace_back(name, x, y);
        }
    }

    file.close();

    if (nodes.empty()) {
        std::cerr << "Error: No nodes were read from the file." << std::endl;
    }

    return nodes;
}

void nearestNeighbor(std::string& filename) {
    auto start = std::chrono::high_resolution_clock::now();

    std::list<Node> nodes = readTSPFile(filename);
    std::list<Node> unVisitedList(nodes.begin(), nodes.end());
    std::list<Node> visitedList;

    auto current = unVisitedList.begin();
    visitedList.push_back(*current);
    unVisitedList.erase(current);

    while (!unVisitedList.empty()) {
        double minDistance = std::numeric_limits<double>::infinity();
        auto nearestNode = current;

        for (auto it = unVisitedList.begin(); it != unVisitedList.end(); ++it) {
            double distance = current->distanceTo(*it);
            if (distance < minDistance) {
                minDistance = distance;
                nearestNode = it;
            }
        }

        current = nearestNode;
        visitedList.push_back(*current);
        unVisitedList.erase(current);
    }

    visitedList.push_back(visitedList.front());

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    for (auto it = visitedList.begin(); it != visitedList.end(); ++it) {
        std::cout << it->name;

        // Check if it is not the last node
        if (std::next(it) != visitedList.end()) {
            std::cout << " ";
        }
    }

    std::cout << std::endl;

    double totalDistance = calcDistance(visitedList);
    std::cout << "Total Distance: " << totalDistance << std::endl;
    std::cout << "Time in ms: " << duration.count() << std::endl;
}
