#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <exception>

class MessageQueue
{
private:
    std::queue<int> queue;
    std::mutex mtx;
    std::condition_variable cv;
    bool is_terminated = false; // Flag to indicate termination of producers

public:
    // Add an element to the queue.
    void push(int msg)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if(is_terminated) {
            throw std::runtime_error("Cannot push to a terminated queue");
        }
        queue.push(msg);
        std::cout << "Pushed message " << msg << " into the queue.\n";
        cv.notify_one();
    }

    // Get the "front"-element.
    // If the queue is empty, wait till an element is available.
    int pop()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this](){ return !queue.empty() || is_terminated; });
        if(queue.empty()) {
            throw std::runtime_error("Cannot pop from an empty queue");
        }
        int val = queue.front();
        queue.pop();
        std::cout << "Popped message " << val << " from the queue.\n";
        return val;
    }

    // Indicate that no more elements will be pushed into the queue
    void terminate() {
        std::lock_guard<std::mutex> lock(mtx);
        is_terminated = true;
        cv.notify_all();
    }
};

void producer(MessageQueue& mq, int id) {
    for(int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        try {
            mq.push(i + id * 100);  // Different producers will push different ranges of numbers
        } catch(const std::exception& e) {
            std::cerr << "Producer " << id << " encountered an error: " << e.what() << '\n';
        }
    }
}

void consumer(MessageQueue& mq, int id) {
    while(true) {
        try {
            int val = mq.pop();
        } catch(const std::exception& e) {
            std::cerr << "Consumer " << id << " encountered an error: " << e.what() << '\n';
            return;
        }
    }
}

int main()
{
    const int num_producers = 3;
    const int num_consumers = 3;
    MessageQueue mq;

    std::vector<std::thread> producers;
    for (int i = 0; i < num_producers; ++i) {
        producers.emplace_back(producer, std::ref(mq), i);
    }

    std::vector<std::thread> consumers;
    for (int i = 0; i < num_consumers; ++i) {
        consumers.emplace_back(consumer, std::ref(mq), i);
    }

    // Wait for all producers to finish
    for (auto& t : producers) {
        t.join();
    }

    // Once all producers are done, terminate the queue
    mq.terminate();

    // Wait for all consumers to finish
    for (auto& t : consumers) {
        t.join();
    }

    return 0;
}

