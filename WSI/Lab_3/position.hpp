#ifndef POSITION_HPP
#define POSITION_HPP

#include <array>
#include <vector>
#include <limits>
#include <cstdint>
#include <algorithm>
#include <iostream>

#include "board.h"

constexpr int BOARD_SIZE = 5;

// Spiral order indices (precomputed)
inline constexpr std::array<int, 25> spiral_indices = {
    12, 13, 17, 16, 11,
    6, 7, 8, 9, 14,
    19, 24, 23, 22, 21,
    20, 15, 10, 5, 0,
    1, 2, 3, 4, 18
};

struct Position {
    int parent_move;
    int board_position[5][5];
    std::vector<Position*> children;

    void generate_children(int player);
    void free_children();
    int evaluate(int depth) const;
};

// Free all dynamically allocated children
inline void Position::free_children() {
    for (auto* child : children) {
        delete child;
    }
    children.clear();
}

// Generate all valid children for the current player
inline void Position::generate_children(int player) {
    free_children(); // Clear previous children before generating new ones
    for (int idx : spiral_indices) {
        if (board_position[idx / 5][idx % 5] == 0) {
            Position* child = new Position();
            for (int y = 0; y < 5; ++y) {
                for (int x = 0; x < 5; ++x) {
                    child->board_position[y][x] = board_position[y][x];
                }
            }
            child->board_position[idx / 5][idx % 5] = player;
            child->parent_move = (idx / 5 + 1) * 10 + (idx % 5 + 1);
            children.push_back(child);
        }
    }
}

// Evaluate the current board state
inline int Position::evaluate(int depth) const {
    int score = 0;

    if (winCheck(board_position, 1)) return 1000 + depth;
    if (loseCheck(board_position, 1)) return -1000 - depth;

    if (winCheck(board_position, 2)) return -1000 - depth;
    if (loseCheck(board_position, 2)) return 1000 + depth;


    return score;
}

// Minimax with alpha-beta pruning
inline int minimax(Position& pos, int depth, int alpha, int beta, bool maximizingPlayer, bool surface) {
    int parent_move;

    if (winCheck(pos.board_position, 1)) return 1000 + depth;
    if (loseCheck(pos.board_position, 1)) return -1000 - depth;

    if (winCheck(pos.board_position, 2)) return -1000 - depth;
    if (loseCheck(pos.board_position, 2)) return 1000 + depth;



    if (depth == 0) {
        return pos.evaluate(depth);
    }

    int current_player_for_children = maximizingPlayer ? 1 : 2;
    pos.generate_children(current_player_for_children);

    if (pos.children.empty()) {
        return pos.evaluate(depth);
    }

    int bestScore = 0;
    if (maximizingPlayer) {
        bestScore = std::numeric_limits<int>::min();
        for (Position* child : pos.children) {
            int score = minimax(*child, depth - 1, alpha, beta, false, false);

            if (surface)
            {
                for (int i = 0; i < 5; i++)
                {
                    for (int j = 0; j < 5; j++)
                    {
                        std::cout << child->board_position[i][j] << " ";
                    }
                }
            }
            
            if (surface) std::cout << " score: " << score;
            if (surface) std::cout << " parent_move: " << child->parent_move << std::endl;

            if (score > bestScore) parent_move = child->parent_move;
            bestScore = std::max(bestScore, score);
            alpha = std::max(alpha, score);
            if (beta <= alpha) {
                break;
            }
        }
        if (surface) std::cout << "NEW POSITION" << std::endl;
    } else { 
        bestScore = std::numeric_limits<int>::max();
        for (Position* child : pos.children) {
            int score = minimax(*child, depth - 1, alpha, beta, true, false);

            if (surface)
            {
                for (int i = 0; i < 5; i++)
                {
                    for (int j = 0; j < 5; j++)
                    {
                        std::cout << child->board_position[i][j] << " ";
                    }
                }
            }

            if (surface) std::cout << " score: " << score;
            if (surface) std::cout << " parent_move: " << child->parent_move << std::endl;

            if (score < bestScore) parent_move = child->parent_move;
            bestScore = std::min(bestScore, score);
            beta = std::min(beta, score);
            if (beta <= alpha) {
                break;
            }
        }
        if (surface) std::cout << "NEW POSITION" << std::endl;
    }
    pos.free_children(); // Clean up children generated at this level
    if (surface) return parent_move;
    return bestScore;
}

// Sync external board to internal Position
inline void set_my_board(Position& pos, const int board_external[BOARD_SIZE][BOARD_SIZE]) {
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            pos.board_position[y][x] = board_external[y][x];
        }
    }
}

#endif // POSITION_HPP