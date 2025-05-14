#include <array>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <random>
#include <iostream>
#include <vector>
#include <iomanip> // Required for std::setw


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

std::array<int, 16> generateRandomEasyState() {
    std::array<int, 16> state = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 0
    };

    std::random_device rd;
    std::mt19937 rng(rd());

    int zeroIndex = 15; // Initially at bottom-right

    for (int moves = 0; moves < 80; ++moves) {
        int row = zeroIndex / 4;
        int col = zeroIndex % 4;

        std::vector<int> possibleMoves;

        // Determine valid moves
        if (row > 0) possibleMoves.push_back(zeroIndex - 4);     // Up
        if (row < 3) possibleMoves.push_back(zeroIndex + 4);     // Down
        if (col > 0) possibleMoves.push_back(zeroIndex - 1);     // Left
        if (col < 3) possibleMoves.push_back(zeroIndex + 1);     // Right

        std::uniform_int_distribution<int> dist(0, possibleMoves.size() - 1);
        int nextIndex = possibleMoves[dist(rng)];

        std::swap(state[zeroIndex], state[nextIndex]);
        zeroIndex = nextIndex;
    }

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

int linearConflict(const std::array<int, 16>& state) {
    int distance = 0;

    // Compute Manhattan Distance
    for (int i = 0; i < 16; ++i) {
        int value = state[i];
        if (value == 0) continue;

        int targetRow = (value - 1) / 4;
        int targetCol = (value - 1) % 4;
        int currentRow = i / 4;
        int currentCol = i % 4;

        distance += std::abs(currentRow - targetRow) + std::abs(currentCol - targetCol);
    }

    // Add linear conflict cost
    int linearConflicts = 0;

    // Row conflicts
    for (int row = 0; row < 4; ++row) {
        for (int i = 0; i < 4; ++i) {
            int index1 = row * 4 + i;
            int tile1 = state[index1];
            if (tile1 == 0 || (tile1 - 1) / 4 != row) continue;

            for (int j = i + 1; j < 4; ++j) {
                int index2 = row * 4 + j;
                int tile2 = state[index2];
                if (tile2 == 0 || (tile2 - 1) / 4 != row) continue;

                if (tile1 > tile2) ++linearConflicts;
            }
        }
    }

    // Column conflicts
    for (int col = 0; col < 4; ++col) {
        for (int i = 0; i < 4; ++i) {
            int index1 = i * 4 + col;
            int tile1 = state[index1];
            if (tile1 == 0 || (tile1 - 1) % 4 != col) continue;

            for (int j = i + 1; j < 4; ++j) {
                int index2 = j * 4 + col;
                int tile2 = state[index2];
                if (tile2 == 0 || (tile2 - 1) % 4 != col) continue;

                if (tile1 > tile2) ++linearConflicts;
            }
        }
    }

    return distance + 2 * linearConflicts;
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

long long nodesVisited = 0;

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
    start->state = generateRandomEasyState();
    while (!isSolvable(start->state)) start->state = generateRandomState();
    start->g = 0;
    start->h = start->f = linearConflict(start->state);
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

    Node *current = target;
    while (current->parent != nullptr)
    {
        for (int i = 0; i < 16; ++i) 
        {
            std::cout << std::setw(2) << current->state[i] << " ";
            if ((i + 1) % 4 == 0) std::cout << std::endl;
        }
        std::cout << std::endl;
        current = current->parent;
    }
    for (int i = 0; i < 16; ++i) 
    {
        std::cout << std::setw(2) << current->state[i] << " ";
        if ((i + 1) % 4 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Nodes visited: " << nodesVisited << std::endl;
    std::cout << "Steps: " << target->g << std::endl;

    return 0;
}
