#include <array>
#include <chrono>
#include <deque>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <unordered_set>

#define NUM_GUESTS 50

enum Status
{
    AVAILABLE,
    BUSY
};

std::unordered_set<std::thread::id> guestsVisited{};
std::mutex mutex;
Status roomStatus = Status::AVAILABLE;

unsigned int generateRandomNumber(int min, int max)
{
    std::random_device seed;
    std::mt19937 rng(seed());
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
    return dist(rng);
}

void admireVase(int threadIndex)
{
    std::thread::id threadId = std::this_thread::get_id();

    while (guestsVisited.size() < NUM_GUESTS)
    {
        mutex.lock();

        if (roomStatus == Status::AVAILABLE && guestsVisited.find(threadId) == guestsVisited.end())
        {
            roomStatus = Status::BUSY;
            std::cout << "Guest " << threadIndex << " is admiring the vase" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(generateRandomNumber(10, 200)));
            roomStatus = Status::AVAILABLE;

            guestsVisited.insert(threadId);
        }

        mutex.unlock();
    }
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    std::array<std::thread, NUM_GUESTS> threads{};

    for (size_t i = 0; i < threads.size(); i++)
    {
        threads[i] = std::thread(admireVase, i);
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);

    std::cout << "All guests have viewed the vase." << std::endl;
    std::cout << "Finished in " << duration.count() << "ms" << std::endl;
}
