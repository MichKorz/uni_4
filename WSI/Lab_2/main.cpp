#include <array>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <random>
#include <iostream>
#include <vector>


struct Node {
    std::array<int, 16> state;
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

std::array<int, 16> generateRandomState() {
    std::array<int, 16> state;
    
    // Initialize the state with values 0 to 15
    for (int i = 0; i < 16; ++i) {
        state[i] = i;
    }

    // Create a random number generator
    std::random_device rd;
    std::mt19937 g(rd());

    // Randomly shuffle the array
    std::shuffle(state.begin(), state.end(), g);

    return state;
}

int manhattanDistance(const std::array<int, 16>& state) {
    int distance = 0;
    for (int i = 0; i < 16; ++i) {
        int value = state[i];
        if (value == 0) continue; // Skip the blank tile

        int targetRow = value / 4;
        int targetCol = value % 4;
        int currentRow = i / 4;
        int currentCol = i % 4;

        distance += std::abs(currentRow - targetRow) + std::abs(currentCol - targetCol);
    }
    return distance;
}

bool isSolvable(const std::array<int, 16>& state) {
    int inversions = 0;
    for (int i = 0; i < 16; ++i) {
        if (state[i] == 0) continue;
        for (int j = i + 1; j < 16; ++j) {
            if (state[j] != 0 && state[i] > state[j]) {
                ++inversions;
            }
        }
    }

    int blankRow = 0;
    for (int i = 0; i < 16; ++i) {
        if (state[i] == 0) {
            blankRow = i / 4;
            break;
        }
    }

    // Solvability condition:
    // - If blank is on even row from bottom (1st, 3rd), inversions must be odd
    // - If blank is on odd row from bottom (2nd, 4th), inversions must be even
    int rowFromBottom = 3 - blankRow; // 0-based: 3 is bottom row
    return (inversions + rowFromBottom) % 2 == 0;
}

std::vector<std::array<int, 16>> getNeighbours(const std::array<int, 16>& state) {
    std::vector<std::array<int, 16>> neighbours;
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
            std::array<int, 16> newState = state;
            std::swap(newState[zeroIndex], newState[newIndex]);
            neighbours.push_back(newState);
        }
    }

    return neighbours;
}

int main()
{
    // Create the target node
    Node *target = new Node();
    target->state = {
        1, 2, 3, 4,
        5, 6, 7, 8, 
        9, 10, 11, 12,
        13, 14, 15, 0
    };
    target->g = target->h = target->f = 0;
    target->parent = nullptr;

    // Open list: nodes to explore
    std::priority_queue<Node*, std::vector<Node*>, CompareF> open;

    // Closed list: visited states
    std::unordered_map<uint64_t, Node*> closed;

    Node *start = new Node();
    start->state = generateRandomState();
    while (!isSolvable(start->state)) start->state = generateRandomState();
    start->g = 0;
    start->h = start->f = manhattanDistance(start->state);
    start->parent = nullptr;
    open.push(start);

    while (true)
    {
        Node *current = open.top(); open.pop();
        // Check if the current node exists in closed
        uint64_t hash = current->encode();
        if (closed.find(hash) != closed.end()) continue;
        // Check if the current node is the target
        if (*current == *target)
        {
            target->parent = current;
            break;
        }
        // Put current in closed
        closed[hash] = current;

        // Find neighbours of current node
        std::vector<std::array<int, 16>> neighbours = getNeighbours(current->state);

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

    return 0;
}
