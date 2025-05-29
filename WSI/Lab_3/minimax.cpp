#include <vector>
#include <cstdint>
#include <array>
#include <limits>

constexpr int BOARD_SIZE = 5;
constexpr int NUM_TILES = BOARD_SIZE * BOARD_SIZE;

// Spiral index order: filled manually or computed once
const std::array<int, NUM_TILES> spiral_indices = {
    12, 13, 18, 17, 16,
    11,  6,  7,  8, 15,
    10,  5,  0,  1, 14,
     9,  4,  3,  2, 19,
    20, 21, 22, 23, 24
};

struct Position {
    uint8_t board[NUM_TILES]; // 0 = empty, 1 = P1, 2 = P2
    std::vector<Position*> children;

    Position() {
        for (int i = 0; i < NUM_TILES; ++i) {
            board[i] = 0;
        }
    }

    ~Position() = default;

    void free_children() {
        for (Position* child : children) {
            if (child) {
                child->free_children();
                delete child;
            }
        }
        children.clear();
    }

    void generate_children(uint8_t player) {
        for (int idx : spiral_indices) {
            if (board[idx] == 0) {
                Position* child = new Position();
                std::copy(std::begin(board), std::end(board), std::begin(child->board));
                child->board[idx] = player;
                children.push_back(child);
            }
        }
    }

    int evaluate() const {
        const int directions[4][2] = {
            {1, 0},   // horizontal →
            {0, 1},   // vertical ↓
            {1, 1},   // diagonal ↘
            {-1, 1}   // anti-diagonal ↙
        };
    
        auto get_tile = [&](int x, int y) -> uint8_t {
            if (x < 0 || y < 0 || x >= BOARD_SIZE || y >= BOARD_SIZE) return 0xFF;
            return board[y * BOARD_SIZE + x];
        };
    
        // ==== 1. Check for wins (4 in a row) ====
        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                for (auto& dir : directions) {
                    int dx = dir[0], dy = dir[1];
                    bool in_bounds = true;
                    uint8_t first = get_tile(x, y);
                    if (first == 0 || first == 0xFF) continue;
    
                    for (int i = 1; i < 4; ++i) {
                        uint8_t val = get_tile(x + dx * i, y + dy * i);
                        if (val != first) {
                            in_bounds = false;
                            break;
                        }
                    }
    
                    if (in_bounds) {
                        return (first == 1) ? 1000000 : -1000000;
                    }
                }
            }
        }
    
        // ==== 2. Check for losses (exactly 3 in a row not part of 4) ====
        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                for (auto& dir : directions) {
                    int dx = dir[0], dy = dir[1];
                    std::array<uint8_t, 3> line3;
                    bool in_bounds = true;
    
                    for (int i = 0; i < 3; ++i) {
                        uint8_t val = get_tile(x + dx * i, y + dy * i);
                        if (val == 0xFF) {
                            in_bounds = false;
                            break;
                        }
                        line3[i] = val;
                    }
    
                    if (!in_bounds) continue;
    
                    uint8_t p = line3[0];
                    if (p != 0 && line3[1] == p && line3[2] == p) {
                        uint8_t prev = get_tile(x - dx, y - dy);
                        uint8_t next = get_tile(x + dx * 3, y + dy * 3);
                        if (prev != p && next != p) {
                            return (p == 1) ? -1000000 : 1000000;
                        }
                    }
                }
            }
        }
    
        // ==== 3. Check 4-length patterns for scoring (opportunities) ====
        int score = 0;
        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                for (auto& dir : directions) {
                    int dx = dir[0], dy = dir[1];
                    std::array<uint8_t, 4> line4;
                    bool in_bounds = true;
                    for (int i = 0; i < 4; ++i) {
                        uint8_t val = get_tile(x + dx * i, y + dy * i);
                        if (val == 0xFF) {
                            in_bounds = false;
                            break;
                        }
                        line4[i] = val;
                    }
    
                    if (!in_bounds) continue;
    
                    int p1 = 0, p2 = 0;
                    for (uint8_t v : line4) {
                        if (v == 1) ++p1;
                        else if (v == 2) ++p2;
                    }
    
                    if (p1 > 0 && p2 == 0) score += p1 * 100;
                    else if (p2 > 0 && p1 == 0) score -= p2 * 100;
                }
            }
        }
    
        // ==== 4. Check 2-length patterns for smaller advantages ====
        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                for (auto& dir : directions) {
                    int dx = dir[0], dy = dir[1];
                    uint8_t a = get_tile(x, y);
                    uint8_t b = get_tile(x + dx, y + dy);
                    if (a == 0xFF || b == 0xFF) continue;
    
                    if (a == 1 && b == 1) score += 10;
                    else if (a == 2 && b == 2) score -= 10;
                }
            }
        }
    
        return score;
    }    
};


int minimax(Position& pos, int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0) {
        return pos.evaluate();
    }

    int current_player = maximizingPlayer ? 1 : 2;
    pos.generate_children(current_player);

    if (pos.children.empty()) {
        return pos.evaluate(); // Terminal or draw
    }

    int bestScore;
    if (maximizingPlayer) {
        bestScore = std::numeric_limits<int>::min();
        bestScore = INT64_MIN;
        for (Position* child : pos.children) {
            int score = minimax(*child, depth - 1, alpha, beta, false);
            bestScore = std::max(bestScore, score);
            alpha = std::max(alpha, score);
            if (beta <= alpha) break; // Beta cut-off
        }
    } else {
        bestScore = std::numeric_limits<int>::max();
        for (Position* child : pos.children) {
            int score = minimax(*child, depth - 1, alpha, beta, true);
            bestScore = std::min(bestScore, score);
            beta = std::min(beta, score);
            if (beta <= alpha) break; // Alpha cut-off
        }
    }

    pos.free_children(); // Free up memory
    return bestScore;
}

void select_best_move_overwrite(Position& root, int depth, int current_player) {
    root.generate_children(current_player);

    if (root.children.empty()) return;

    Position* best_move = nullptr;
    int best_score = current_player == 1
        ? std::numeric_limits<int>::min()
        : std::numeric_limits<int>::max();

    for (Position* child : root.children) {
        int score = minimax(*child, depth - 1,
                            std::numeric_limits<int>::min(),
                            std::numeric_limits<int>::max(),
                            current_player == 2); // flip player

        if ((current_player == 1 && score > best_score) ||
            (current_player == 2 && score < best_score)) {
            best_score = score;
            best_move = child;
        }
    }

    // Overwrite root with best move
    if (best_move) {
        Position temp = *best_move; // copy best move
        for (Position* child : root.children) {
            delete child;
        }
        root.children.clear();
        root = std::move(temp); // move best move into root
    }
}
