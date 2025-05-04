#include <iostream>
#include <random>
#include <cstdlib> // For std::atoi

int main(int argc, char* argv[]) 
{
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <n> <i> <m>" <<std::endl;
        return 1;
    }

    int n = std::atoi(argv[1]); // Convert argument to integer
    int i = std::atoi(argv[2]); // Convert argument to integer
    int m = std::atoi(argv[3]); // Convert argument to integer

    if (n <= 0) 
    {
        std::cerr << "Please enter a positive integer for n." << std::endl;
        return 1;
    }

    std::cout << n << "\n";
    std::cout << i << "\n";
    std::cout << m << "\n";

    std::random_device rd;
    std::mt19937 mt(rd()); // Mersenne Twister engine
    std::uniform_int_distribution<int> dist(0, 2 * n - 1);

    for (int j = 0; j < n; ++j) 
    {
        std::cout << dist(mt) << "\n";  // Print each number on a new line
    }

    return 0;
}
