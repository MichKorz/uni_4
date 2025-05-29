#ifndef POSITION_HPP
#define POSITION_HPP

#include <array>
#include <vector>
#include <limits>
#include <cstdint>
#include <algorithm>
#include <iostream>

constexpr int BOARD_SIZE = 5;

// Spiral order indices (precomputed)
inline constexpr std::array<int, 25> spiral_indices = {
    12, 13, 17, 16, 11,
    6, 7, 8, 9, 14,
    19, 24, 23, 22, 21,
    20, 15, 10, 5, 0,
    1, 2, 3, 4, 18
};

const int my_win[28][4][2] = {
    { {0, 0}, {0, 1}, {0, 2}, {0, 3} },
    { {1, 0}, {1, 1}, {1, 2}, {1, 3} },
    { {2, 0}, {2, 1}, {2, 2}, {2, 3} },
    { {3, 0}, {3, 1}, {3, 2}, {3, 3} },
    { {4, 0}, {4, 1}, {4, 2}, {4, 3} },
    { {0, 1}, {0, 2}, {0, 3}, {0, 4} },
    { {1, 1}, {1, 2}, {1, 3}, {1, 4} },
    { {2, 1}, {2, 2}, {2, 3}, {2, 4} },
    { {3, 1}, {3, 2}, {3, 3}, {3, 4} },
    { {4, 1}, {4, 2}, {4, 3}, {4, 4} },
    { {0, 0}, {1, 0}, {2, 0}, {3, 0} },
    { {0, 1}, {1, 1}, {2, 1}, {3, 1} },
    { {0, 2}, {1, 2}, {2, 2}, {3, 2} },
    { {0, 3}, {1, 3}, {2, 3}, {3, 3} },
    { {0, 4}, {1, 4}, {2, 4}, {3, 4} },
    { {1, 0}, {2, 0}, {3, 0}, {4, 0} },
    { {1, 1}, {2, 1}, {3, 1}, {4, 1} },
    { {1, 2}, {2, 2}, {3, 2}, {4, 2} },
    { {1, 3}, {2, 3}, {3, 3}, {4, 3} },
    { {1, 4}, {2, 4}, {3, 4}, {4, 4} },
    { {0, 1}, {1, 2}, {2, 3}, {3, 4} },
    { {0, 0}, {1, 1}, {2, 2}, {3, 3} },
    { {1, 1}, {2, 2}, {3, 3}, {4, 4} },
    { {1, 0}, {2, 1}, {3, 2}, {4, 3} },
    { {0, 3}, {1, 2}, {2, 1}, {3, 0} },
    { {0, 4}, {1, 3}, {2, 2}, {3, 1} },
    { {1, 3}, {2, 2}, {3, 1}, {4, 0} },
    { {1, 4}, {2, 3}, {3, 2}, {4, 1} }
  };
  
  const int my_lose[48][3][2] = {
    { {0, 0}, {0, 1}, {0, 2} }, { {0, 1}, {0, 2}, {0, 3} }, { {0, 2}, {0, 3}, {0, 4} },
    { {1, 0}, {1, 1}, {1, 2} }, { {1, 1}, {1, 2}, {1, 3} }, { {1, 2}, {1, 3}, {1, 4} },
    { {2, 0}, {2, 1}, {2, 2} }, { {2, 1}, {2, 2}, {2, 3} }, { {2, 2}, {2, 3}, {2, 4} },
    { {3, 0}, {3, 1}, {3, 2} }, { {3, 1}, {3, 2}, {3, 3} }, { {3, 2}, {3, 3}, {3, 4} },
    { {4, 0}, {4, 1}, {4, 2} }, { {4, 1}, {4, 2}, {4, 3} }, { {4, 2}, {4, 3}, {4, 4} },
    { {0, 0}, {1, 0}, {2, 0} }, { {1, 0}, {2, 0}, {3, 0} }, { {2, 0}, {3, 0}, {4, 0} },
    { {0, 1}, {1, 1}, {2, 1} }, { {1, 1}, {2, 1}, {3, 1} }, { {2, 1}, {3, 1}, {4, 1} },
    { {0, 2}, {1, 2}, {2, 2} }, { {1, 2}, {2, 2}, {3, 2} }, { {2, 2}, {3, 2}, {4, 2} },
    { {0, 3}, {1, 3}, {2, 3} }, { {1, 3}, {2, 3}, {3, 3} }, { {2, 3}, {3, 3}, {4, 3} },
    { {0, 4}, {1, 4}, {2, 4} }, { {1, 4}, {2, 4}, {3, 4} }, { {2, 4}, {3, 4}, {4, 4} },
    { {0, 2}, {1, 3}, {2, 4} }, { {0, 1}, {1, 2}, {2, 3} }, { {1, 2}, {2, 3}, {3, 4} },
    { {0, 0}, {1, 1}, {2, 2} }, { {1, 1}, {2, 2}, {3, 3} }, { {2, 2}, {3, 3}, {4, 4} },
    { {1, 0}, {2, 1}, {3, 2} }, { {2, 1}, {3, 2}, {4, 3} }, { {2, 0}, {3, 1}, {4, 2} },
    { {0, 2}, {1, 1}, {2, 0} }, { {0, 3}, {1, 2}, {2, 1} }, { {1, 2}, {2, 1}, {3, 0} },
    { {0, 4}, {1, 3}, {2, 2} }, { {1, 3}, {2, 2}, {3, 1} }, { {2, 2}, {3, 1}, {4, 0} },
    { {1, 4}, {2, 3}, {3, 2} }, { {2, 3}, {3, 2}, {4, 1} }, { {2, 4}, {3, 3}, {4, 2} }
  };

struct Position {
    int parent_move;
    int board_position[5][5];
    std::vector<Position*> children;

    void generate_children(int player);
    void free_children();
    int evaluate() const;
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
inline int Position::evaluate() const {
    int score = 0;

    int player = 1;

    bool w = false;
    for (int i = 0; i < 28; i++)
        if ( (board_position[my_win[i][0][0]][my_win[i][0][1]] == player) &&
            (board_position[my_win[i][1][0]][my_win[i][1][1]] == player) &&
            (board_position[my_win[i][2][0]][my_win[i][2][1]] == player) &&
            (board_position[my_win[i][3][0]][my_win[i][3][1]] == player) )
        w = true;

    if (w) return 1000;
      
    bool l = false;
    for (int i = 0; i < 48; i++)
        if ( (board_position[my_lose[i][0][0]][my_lose[i][0][1]] == player) &&
            (board_position[my_lose[i][1][0]][my_lose[i][1][1]] == player) &&
            (board_position[my_lose[i][2][0]][my_lose[i][2][1]] == player) )
        l = true;
    
    if (l) return -1000;


    player = 2;

    w = false;
    for (int i = 0; i < 28; i++)
        if ( (board_position[my_win[i][0][0]][my_win[i][0][1]] == player) &&
            (board_position[my_win[i][1][0]][my_win[i][1][1]] == player) &&
            (board_position[my_win[i][2][0]][my_win[i][2][1]] == player) &&
            (board_position[my_win[i][3][0]][my_win[i][3][1]] == player) )
        w = true;

    if (w) return -1000;
      
    l = false;
    for (int i = 0; i < 48; i++)
        if ( (board_position[my_lose[i][0][0]][my_lose[i][0][1]] == player) &&
            (board_position[my_lose[i][1][0]][my_lose[i][1][1]] == player) &&
            (board_position[my_lose[i][2][0]][my_lose[i][2][1]] == player) )
        l = true;
    
    if (l) return 1000;


    return score;
}

// Minimax with alpha-beta pruning
inline int minimax(Position& pos, int depth, int alpha, int beta, bool maximizingPlayer, bool surface) {
    int parent_move;

    int player = 1;

    bool w = false;
    for (int i = 0; i < 28; i++)
        if ( (pos.board_position[my_win[i][0][0]][my_win[i][0][1]] == player) &&
            (pos.board_position[my_win[i][1][0]][my_win[i][1][1]] == player) &&
            (pos.board_position[my_win[i][2][0]][my_win[i][2][1]] == player) &&
            (pos.board_position[my_win[i][3][0]][my_win[i][3][1]] == player) )
        w = true;

    if (w) return 1000;
      
    bool l = false;
    for (int i = 0; i < 48; i++)
        if ( (pos.board_position[my_lose[i][0][0]][my_lose[i][0][1]] == player) &&
            (pos.board_position[my_lose[i][1][0]][my_lose[i][1][1]] == player) &&
            (pos.board_position[my_lose[i][2][0]][my_lose[i][2][1]] == player) )
        l = true;
    
    if (l) return -1000;

    if (depth == 0) {
        return pos.evaluate();
    }

    int current_player_for_children = maximizingPlayer ? 1 : 2;
    pos.generate_children(current_player_for_children);

    if (pos.children.empty()) {
        return pos.evaluate();
    }

    int bestScore;
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
            // if (beta <= alpha) {
            //     break;
            // }
        }
        if (surface) std::cout << "NEW POSITION" << std::endl;
    } else { 
        bestScore = std::numeric_limits<int>::max();
        for (Position* child : pos.children) {
            int score = minimax(*child, depth - 1, alpha, beta, true, false);
            if (score < bestScore) parent_move = child->parent_move;
            bestScore = std::min(bestScore, score);
            beta = std::min(beta, score);
            // if (beta <= alpha) {
            //     break;
            // }
        }
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