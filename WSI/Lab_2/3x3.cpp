#include <array>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <random>
#include <iostream>
#include <vector>

struct Node {
    std::array<uint8_t, 9> state;
    int g, h, f;
    Node* parent;

    // Equality: compares puzzle states
    bool operator==(const Node& other) const {
        return state == other.state;
    }

    // Encodes the state into a unique 64-bit integer (4 bits per tile)
    uint64_t encode() const {
        uint64_t key = 0;
        for (int i = 0; i < 9; ++i) {
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

std::array<uint8_t, 9> generateRandomState() {
    std::array<uint8_t, 9> state;
    
    // Initialize the state with values 0 to 8
    for (int i = 0; i < 9; ++i) {
        state[i] = i;
    }

    // Create a random number generator
    std::random_device rd;
    std::mt19937 g(rd());

    // Randomly shuffle the array
    std::shuffle(state.begin(), state.end(), g);

    return state;
}

int manhattanDistance(const std::array<uint8_t, 9>& state) {
    int distance = 0;
    for (int i = 0; i < 9; ++i) {
        int value = state[i];
        if (value == 0) continue; // Skip the blank tile

        int targetRow = value / 3;
        int targetCol = value % 3;
        int currentRow = i / 3;
        int currentCol = i % 3;

        distance += std::abs(currentRow - targetRow) + std::abs(currentCol - targetCol);
    }
    return distance;
}

int linearConflict(const std::array<uint8_t, 9>& state) {
    int distance = 0;

    // Manhattan Distance
    for (int i = 0; i < 9; ++i) {
        int value = state[i];
        if (value == 0) continue;

        int targetRow = (value - 1) / 3;
        int targetCol = (value - 1) % 3;
        int currentRow = i / 3;
        int currentCol = i % 3;

        distance += std::abs(currentRow - targetRow) + std::abs(currentCol - targetCol);
    }

    // Linear Conflicts
    int linearConflicts = 0;

    // Row conflicts
    for (int row = 0; row < 3; ++row) {
        for (int i = 0; i < 3; ++i) {
            int index1 = row * 3 + i;
            int tile1 = state[index1];
            if (tile1 == 0 || (tile1 - 1) / 3 != row) continue;

            for (int j = i + 1; j < 3; ++j) {
                int index2 = row * 3 + j;
                int tile2 = state[index2];
                if (tile2 == 0 || (tile2 - 1) / 3 != row) continue;

                if (tile1 > tile2) ++linearConflicts;
            }
        }
    }

    // Column conflicts
    for (int col = 0; col < 3; ++col) {
        for (int i = 0; i < 3; ++i) {
            int index1 = i * 3 + col;
            int tile1 = state[index1];
            if (tile1 == 0 || (tile1 - 1) % 3 != col) continue;

            for (int j = i + 1; j < 3; ++j) {
                int index2 = j * 3 + col;
                int tile2 = state[index2];
                if (tile2 == 0 || (tile2 - 1) % 3 != col) continue;

                if (tile1 > tile2) ++linearConflicts;
            }
        }
    }

    return distance + 2 * linearConflicts;
}


bool isSolvable(const std::array<uint8_t, 9>& state) {
    int inversions = 0;
    for (int i = 0; i < 9; ++i) {
        if (state[i] == 0) continue;
        for (int j = i + 1; j < 9; ++j) {
            if (state[j] != 0 && state[i] > state[j]) {
                ++inversions;
            }
        }
    }

    return inversions % 2 == 0; // Solvability condition for 8-puzzle
}

std::vector<std::array<uint8_t, 9>> getNeighbours(const std::array<uint8_t, 9>& state) {
    std::vector<std::array<uint8_t, 9>> neighbours;
    int zeroIndex = 0;

    // Find the index of the blank tile (0)
    for (int i = 0; i < 9; ++i) {
        if (state[i] == 0) {
            zeroIndex = i;
            break;
        }
    }

    int row = zeroIndex / 3;
    int col = zeroIndex % 3;

    // Possible move directions: {row_offset, col_offset}
    const int dir[4][2] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };

    for (const auto& d : dir) {
        int newRow = row + d[0];
        int newCol = col + d[1];

        if (newRow >= 0 && newRow < 3 && newCol >= 0 && newCol < 3) {
            int newIndex = newRow * 3 + newCol;
            std::array<uint8_t, 9> newState = state;
            std::swap(newState[zeroIndex], newState[newIndex]);
            neighbours.push_back(newState);
        }
    }

    return neighbours;
}

long long nodesVisited = 0;

int main()
{
    // Create the target node
    Node *target = new Node();
    target->state = {
        1, 2, 3,
        4, 5, 6, 
        7, 8, 0
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
        std::vector<std::array<uint8_t, 9>> neighbours = getNeighbours(current->state);

        for (int i = 0; i < neighbours.size(); i++)
        {
            Node *neighbour = new Node();
            neighbour->state = neighbours[i];
            // Check if neighbour was already closed
            uint64_t neighbourHash = neighbour->encode();
            if (closed.find(neighbourHash) != closed.end()) continue;

            neighbour->g = current->g + 1;
            neighbour->h = linearConflict(neighbour->state);
            neighbour->f = neighbour->g + neighbour->h;
            neighbour->parent = current;
            open.push(neighbour);
        }
    }

    Node *current = target;
    while (current->parent != nullptr)
    {
        for (int i = 0; i < 9; ++i) 
        {
            std::cout << current->state[i] << " ";
            if ((i + 1) % 3 == 0) std::cout << std::endl;
        }
        std::cout << std::endl;
        current = current->parent;
    }
    for (int i = 0; i < 9; ++i) 
    {
        std::cout << current->state[i] << " ";
        if ((i + 1) % 3 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Nodes visited: " << nodesVisited << std::endl;
    std::cout << "Steps: " << target->g << std::endl;

    return 0;
}