#ifndef NEAREST_NEIGHBOR_HPP
#define NEAREST_NEIGHBOR_HPP

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <list>


class Node {
public:
    Node(std::string name, double x, double y) : name(name), x(x), y(y) {}

    double distanceTo(const Node& otherNode) const {
        double xDistance = x - otherNode.x;
        double yDistance = y - otherNode.y;
        return std::sqrt(xDistance * xDistance + yDistance * yDistance);
    }

    std::string getName() const {
        return name;
    }

private:
    std::string name;
    double x;
    double y;
};

class NearestNeighbor {
public:
static void nearestNeighbor(std::string filename) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Node> nodes = readTSPFile(filename);
    std::vector<Node> unVisitedList(nodes.begin(), nodes.end());
    std::vector<Node> visitedList;

    Node current = unVisitedList[0];
    unVisitedList.erase(unVisitedList.begin());
    visitedList.push_back(current);

    while (!unVisitedList.empty()) {
        double minDistance = INFINITY;
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
    //go back to first place
    visitedList.push_back(visitedList[0]);
    

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Visited Nodes: ";
    for (const auto& node : visitedList) {
        std::cout << node.getName() << " ";
    }
    std::cout << std::endl;

    double totalDistance = calcDistance(visitedList);
    std::cout << "Total Distance: " << totalDistance << std::endl;
    std::cout << "Time in ms: " << duration.count() << std::endl;
}

private:
    // My helper functions 

    static double calcDistance(const std::vector<Node>& node){
      double dist = 0.0;

      for (int i = 0; i < node.size() - 1; i++){
        dist += node[i].distanceTo(node[i+1]);
      }
      
      dist += node[node.size() - 1].distanceTo(node[0]);
      return dist;
    }
    
    static std::vector<Node> readTSPFile(const std::string &filename){
        std::vector<Node> nodes;
        std::ifstream file(filename);

        if (!file.is_open()) {
          std::cerr << "Error: Unable to open file: " << filename << std::endl;
          return nodes;
        }

        std::string line;
        bool readNodes = false;

        while (std::getline(file, line)) {

          // ignore empty lines
          if (line.empty()) {
            continue; 
          }

          //set readNodes to true if file is formatted correctly to read nodes
          if (line == "NODE_COORD_SECTION") {
            readNodes = true;
            continue;
          }
          //check if its the end of the file
          if (line == "EOF") {
            break;
          }
        
          if (readNodes) {
            std::istringstream iss(line);
            std::string name;
            double x, y;

            // catch errors
            if (!(iss >> name >> x >> y)) {
              std::cerr << "Error: Unable to parse line: " << line << std::endl;
              continue;
            }
            //Add the read node to the vector
            nodes.emplace_back(name, x, y);

            // Check what was read
            //std::cout << "Read node: " << name << " (" << x << ", " << y << ")" << std::endl;
          }
        }

        file.close();

        if (nodes.empty()) {
          std::cerr << "Error: No nodes were read from the file." << std::endl;
        }

        return nodes;
      
    }
};

#endif // NEAREST_NEIGHBOR_HPP


// Nearest neighbor is most efficiently implemented by starting with a collection of unvisited nodes and calculating distances as you go, only calculating distances from a node once you visit it. The collection should have efficient deletion and iteration - something linked-list based would probably be a good choice.
// You'll probably want to make a NODE class with a distance method.
// Starting from node 1, find the nearest node, let's call it X, and visit it. Then find the nearest node to X and visit it, and repeat until all nodes have been visited. Don't forget to add the distance from the last node visited back to node 1.
// While doing this, create a list of nodes visited in order (anything ordered and iterable with efficient insertion works), and keep a sum of the total distance.
// At the end (after you stop timing the program), print the following to standard output:
// - The IDs of the nodes visited, in order, with a space between each (this should start and end with 1)
// - Total Distance: X
// - Time in ms: X
// For example:
// 1 5 3 4 2 1
// Total Distance: 58
// Time in ms: 4