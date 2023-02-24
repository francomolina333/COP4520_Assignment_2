#include <thread>
#include <array>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <iostream>
#include <random>

#define NUM_GUESTS 50

// tracks guests who ate the cupcake
std::array<bool, NUM_GUESTS> guestsPicked;
std::mutex mutex;
bool isCupcakeAvailable = true;
int count = 0;

// tracks guests in the labyrinth
unsigned int activeThreadIndex;

unsigned int generateRandNum(int min, int max)
{
    std::random_device seed;
    std::mt19937 rng(seed());
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
    return dist(rng);
}

void checkCupcake()
{
    while (count < NUM_GUESTS)
    {
        mutex.lock();

        if (activeThreadIndex == 0)
        {
            // update the count and replace the cupcake
            if (!isCupcakeAvailable)
            {
                count++;
                isCupcakeAvailable = true;
            }

            if (isCupcakeAvailable && !guestsPicked[0])
            {
                count++;
                isCupcakeAvailable = true;
                guestsPicked[0] = true;
            }
        }

        mutex.unlock();
    }
}

void labyrinth(unsigned int threadIndex)
{
    while (count < NUM_GUESTS)
    {
        mutex.lock();

        if (activeThreadIndex == threadIndex && isCupcakeAvailable && !guestsPicked[threadIndex])
        {
            isCupcakeAvailable = false;
            guestsPicked[threadIndex] = true;
            std::cout << "Guest " << threadIndex << " ate the cupcake" << std::endl;
        }

        mutex.unlock();
    }
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    std::array<std::thread, NUM_GUESTS> threads{};

    threads[0] = std::thread(checkCupcake);

    for (size_t i = 1; i < threads.size(); i++)
    {
        threads[i] = std::thread(labyrinth, i);
    }

    while (count < NUM_GUESTS)
    {
        activeThreadIndex = generateRandNum(0, NUM_GUESTS);
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);

    std::cout << "All " << count << " guests have entered the labyrinth." << std::endl;
    std::cout << "Finished in " << duration.count() << "ms" << std::endl;
}