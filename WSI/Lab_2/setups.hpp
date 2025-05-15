#ifndef SETUPS
#define SETUPS

#pragma once 
#include <array>
#include <cstdint>
#include <random>
#include <algorithm>

std::array<uint8_t, 16> generateRandomState();
std::array<uint8_t, 16> generateRandomEasyState();
bool isSolvable(const std::array<uint8_t, 16>& state);

#endif