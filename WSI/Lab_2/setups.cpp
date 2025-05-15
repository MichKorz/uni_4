#include "setups.hpp"

std::array<uint8_t, 16> generateRandomState() {
    std::array<uint8_t, 16> state;
    
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

std::array<uint8_t, 16> generateRandomEasyState() {
    std::array<uint8_t, 16> state = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 0
    };

    std::random_device rd;
    std::mt19937 rng(rd());

    int zeroIndex = 15; // Initially at bottom-right

    for (int moves = 0; moves < 30; ++moves) {
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

bool isSolvable(const std::array<uint8_t, 16>& state) {
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

    int rowFromBottom = 3 - blankRow;
    return (inversions + rowFromBottom) % 2 == 0;
}