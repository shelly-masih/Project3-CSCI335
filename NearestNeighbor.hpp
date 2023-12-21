#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <list>

struct Node {
    std::string name;
    double x;
    double y;

    Node(std::string name, double x, double y) : name(std::move(name)), x(x), y(y) {}

    // Method to calculate the Euclidean distance between two nodes
    double distanceTo(const Node& otherNode) const {
        double xDistance = x - otherNode.x;
        double yDistance = y - otherNode.y;
        return std::sqrt(xDistance * xDistance + yDistance * yDistance);
    }

    // Overloaded equality operator for comparing nodes
    bool operator==(const Node& other) const {
        return name == other.name && x == other.x && y == other.y;
    }
};

// Function to calculate the total distance of a given tour
static int calcDistance(const std::vector<Node>& nodes) {
    int dist = 0;
    
    // Iterate over nodes and sum up the distances between consecutive nodes
    for (std::vector<Node>::size_type i = 0; i < nodes.size() - 1; i++) {
        dist += nodes[i].distanceTo(nodes[i + 1]);
    }
    
     // Add distance from the last node back to the starting node
    dist += nodes[nodes.size() - 1].distanceTo(nodes[0]);
    return dist;
}

// Function to read TSP file and extract nodes
static std::vector<Node> readTSPFile(const std::string& filename) {
    std::vector<Node> nodes;
    std::ifstream file(filename);
    
    // Check if file is successfully opened
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << filename << std::endl;
        return nodes;
    }

    std::string line;
    bool readNodes = false;
    
     // Iterate over lines in the file
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        // Check if the NODE_COORD_SECTION is encountered
        if (line.substr(0,18) == "NODE_COORD_SECTION") {
            readNodes = true;
            continue;
        }
        
        // Check if end of file is reached
        if (line.substr(0,3) == "EOF") {
            break;
        }

        // Read nodes if NODE_COORD_SECTION has been encountered
        if (readNodes) {
            std::istringstream iss(line);
            std::string name;
            double x, y;
            // Parse line to extract node information
            if (!(iss >> name >> x >> y)) {
                std::cerr << "Error: Unable to parse line: " << line << std::endl;
                continue;
            }
            // Add the read node to the vector
            nodes.emplace_back(name, x, y);
            //std::cout << "Read node: " << name << " (" << x << ", " << y << ")" << std::endl;
        }
    }

    file.close();
    
    // Display error message if no nodes were read
    if (nodes.empty()) {
        std::cerr << "Error: No nodes were read from the file." << std::endl;
    }

    return nodes;
}

// Function to solve TSP using nearest neighbor algorithm
void nearestNeighbor(std::string filename) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Read nodes from the TSP file
    std::vector<Node> nodes = readTSPFile(filename);
    std::vector<Node> unVisitedList(nodes.begin(), nodes.end());
    std::vector<Node> visitedList;
    
    // Start from the first node
    Node current = unVisitedList[0];
    unVisitedList.erase(unVisitedList.begin());
    visitedList.push_back(current);
    
    // Iterate until all nodes are visited
    while (!unVisitedList.empty()) {
        double minDistance = std::numeric_limits<double>::infinity();
        Node nearestNode = current;
        
         // Find the nearest unvisited node
        for (Node node : unVisitedList) {
            double distance = current.distanceTo(node);
            if (distance < minDistance) {
                minDistance = distance;
                nearestNode = node;
            }
        }
        // Move to the nearest node
        current = nearestNode;
        visitedList.push_back(current);
        unVisitedList.erase(std::remove(unVisitedList.begin(), unVisitedList.end(), current), unVisitedList.end());
    }
    // Complete the tour by adding the starting node to the end
    visitedList.push_back(visitedList[0]);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // Print the tour and total distance
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
