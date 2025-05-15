#include <queue>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <iomanip> // Required for std::setw

#include "heuristics.hpp"
#include "setups.hpp"

struct Node {
    std::array<uint8_t, 16> state;
    int g, h, f;
    Node* parent;

    // Equality: compares puzzle states
    bool operator==(const Node& other) const {
        return state == other.state;
    }

    // Encodes the state into a unique 64-bit integer (4 bits per tile)
    uint64_t encode() const {
        uint64_t key = 0;
        for (int i = 0; i < 16; ++i) {
            key |= static_cast<uint64_t>(state[i] & 0xF) << (i * 4);
        }
        return key;
    }
};

// Comparator for priority queue (min-heap by f)
struct CompareF {
    bool operator()(const Node* a, const Node* b) const {
        return a->f > b->f; // lower f has higher priority
    }
};

std::vector<std::array<uint8_t, 16>> getNeighbours(const std::array<uint8_t, 16>& state) {
    std::vector<std::array<uint8_t, 16>> neighbours;
    int zeroIndex = 0;

    // Find the index of the blank tile (0)
    for (int i = 0; i < 16; ++i) {
        if (state[i] == 0) {
            zeroIndex = i;
            break;
        }
    }

    int row = zeroIndex / 4;
    int col = zeroIndex % 4;

    // Possible move directions: {row_offset, col_offset}
    const int dir[4][2] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };

    for (const auto& d : dir) {
        int newRow = row + d[0];
        int newCol = col + d[1];

        if (newRow >= 0 && newRow < 4 && newCol >= 0 && newCol < 4) {
            int newIndex = newRow * 4 + newCol;
            std::array<uint8_t, 16> newState = state;
            std::swap(newState[zeroIndex], newState[newIndex]);
            neighbours.push_back(newState);
        }
    }

    return neighbours;
}

long long nodesVisited = 0;

int main(int argv, char *argc[])
{
    // Przygotowanie

    Node *target = new Node();
    target->state = {
        1, 2, 3, 4,
        5, 6, 7, 8, 
        9, 10, 11, 12,
        13, 14, 15, 0
    };
    target->g = target->h = target->f = 0;
    target->parent = nullptr;

    std::priority_queue<Node*, std::vector<Node*>, CompareF> open;
    std::unordered_map<uint64_t, Node*> closed;

    /*std::array<uint8_t, 16> hardState = {
        0, 12, 9, 13,
        15, 11, 10, 14,
        3, 7, 2, 5,
        4, 8, 6, 1
    };*/

    Node *start = new Node();
    start->state = generateRandomEasyState();
    while (!isSolvable(start->state)) start->state = generateRandomEasyState();
    start->g = 0;
    start->h = start->f = manhattanDistance(start->state);
    start->parent = nullptr;
    open.push(start);

    // Algorytm A*

    while (true)
    {
        nodesVisited++;
        Node *current = open.top(); open.pop();
        // Check if the current node exists in closed
        uint64_t hash = current->encode();
        if (closed.find(hash) != closed.end()) continue;
        // Check if the current node is the target
        if (*current == *target)
        {
            target = current;
            break;
        }
        // Put current in closed
        closed[hash] = current;

        // Find neighbours of current node
        std::vector<std::array<uint8_t, 16>> neighbours = getNeighbours(current->state);

        for (int i = 0; i < neighbours.size(); i++)
        {
            // Posible optimalization, calculate hash without allocating neighbour node
            Node *neighbour = new Node();
            neighbour->state = neighbours[i];
            // Check if neighbour was already closed
            uint64_t neighbourHash = neighbour->encode();
            if (closed.find(neighbourHash) != closed.end()) continue;

            neighbour->g = current->g + 1;
            neighbour->h = manhattanDistance(neighbour->state);
            neighbour->f = neighbour->g + neighbour->h;
            neighbour->parent = current;
            open.push(neighbour);
        }
    }

    // Drukowanie rozwiązań

    if (argv != 1)
    {
        std::cout << nodesVisited << std::endl;
        std::cout << target->g << std::endl;
        return 0;
    }

    Node *current = target;
    while (current->parent != nullptr)
    {
        for (int i = 0; i < 16; ++i) 
        {
            std::cout << std::setw(2) << static_cast<int>(current->state[i]) << " ";
            if ((i + 1) % 4 == 0) std::cout << std::endl;
        }
        std::cout << std::endl;
        current = current->parent;
    }
    for (int i = 0; i < 16; ++i) 
    {
        std::cout << std::setw(2) << static_cast<int>(current->state[i]) << " ";
        if ((i + 1) % 4 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Nodes visited: " << nodesVisited << std::endl;
    std::cout << "Steps: " << target->g << std::endl;

    return 0;
}
