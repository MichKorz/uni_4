#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <stdexcept>
#include <algorithm>  // For std::sort and std::greater

int main(int argc, char* argv[]) 
{
    if (argc != 2) 
    {
        std::cerr << "Usage: " << argv[0] << " <n>" << std::endl;
        return 1;
    }

    int n;
    try 
    {
        n = std::stoi(argv[1]);
    } 
    catch (const std::exception&) 
    {
        std::cerr << "Invalid input: Please enter a valid positive integer." << std::endl;
        return 1;
    }

    if (n <= 0) 
    {
        std::cerr << "Please enter a positive integer for n." << std::endl;
        return 1;
    }

    // Improved seeding strategy
    std::random_device rd;
    std::seed_seq seed{rd(), static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())};
    std::mt19937 mt(seed);
    std::uniform_int_distribution<int> dist(0, 2 * n - 1);

    // Generate numbers and store them in a vector
    std::vector<int> numbers(n);
    for (int i = 0; i < n; ++i) 
    {
        numbers[i] = dist(mt);
    }

    // Sort the numbers in descending order
    std::sort(numbers.begin(), numbers.end(), std::greater<int>());

    std::cout << n << '\n';

    // Print the sorted numbers
    for (int num : numbers) 
    {
        std::cout << num << '\n';  // Using '\n' for efficiency
    }

    return 0;
}
