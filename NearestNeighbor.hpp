#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <list>
#include <limits>


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

static double calcDistance(const std::vector<Node>& nodes) {
    double dist = 0.0;

    for (int i = 0; i < nodes.size() - 1; i++) {
        dist += nodes[i].distanceTo(nodes[i + 1]);
    }

    dist += nodes[nodes.size() - 1].distanceTo(nodes[0]);
    return dist;
}

static std::vector<Node> readTSPFile(const std::string& filename) {
    std::vector<Node> nodes;
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
            //std::cout << "Read node: " << name << " (" << x << ", " << y << ")" << std::endl;
        }
    }

    file.close();

    if (nodes.empty()) {
        std::cerr << "Error: No nodes were read from the file." << std::endl;
    }

    return nodes;
}

static void nearestNeighbor(const std::string& filename) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<Node> nodes = readTSPFile(filename);
    std::vector<Node> unVisitedList(nodes.begin(), nodes.end());
    std::vector<Node> visitedList;

    Node current = unVisitedList[0];
    unVisitedList.erase(unVisitedList.begin());
    visitedList.push_back(current);

    while (!unVisitedList.empty()) {
        double minDistance = std::numeric_limits<double>::infinity();
        Node nearestNode = current;

        for (Node node : unVisitedList) {
            double distance = current.distanceTo(node);
            if (distance < minDistance) {
                minDistance = distance;
                nearestNode = node;
            }
        }

        current = nearestNode;
        visitedList.push_back(current);
        unVisitedList.erase(std::remove(unVisitedList.begin(), unVisitedList.end(), current), unVisitedList.end());
    }

    visitedList.push_back(visitedList[0]);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    for (const auto& node : visitedList) {
        std::cout << node.name << " ";
    }
    std::cout << std::endl;

    double totalDistance = calcDistance(visitedList);
    std::cout << "Total Distance: " << totalDistance << std::endl;
    std::cout << "Time in ms: " << duration.count() << std::endl;
}
