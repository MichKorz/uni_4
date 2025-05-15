#include "heuristics.hpp"


int manhattanDistance(const std::array<uint8_t, 16>& state) {
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


int linearConflict(const std::array<uint8_t, 16>& state) {
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