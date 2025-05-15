#ifndef HEURISTICS
#define HEURISTICS

#pragma once
#include <array>
#include <cstdint>
#include <algorithm>

int manhattanDistance(const std::array<uint8_t, 16>& state);
int linearConflict(const std::array<uint8_t, 16>& state);

#endif