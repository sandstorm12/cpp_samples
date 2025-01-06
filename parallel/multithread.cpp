#include <iostream>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>


#define MAX_DELAY_MS 1000


unsigned long randlong(unsigned long max) {
    static bool seeded = false;
    if (!seeded) {
        std::srand(std::time(0));
        seeded = true;
    }

    return (unsigned long) (((double) rand() / (RAND_MAX)) * max);
}

struct Job {
    std::string id;
    long delay_preprocess;
    long delay_mainprocess;
    long delay_postprocess;

    Job(std::string id, unsigned long max_delay_ms) {
        this-> id = id;
        this-> delay_preprocess = randlong(max_delay_ms);
        this-> delay_mainprocess = randlong(max_delay_ms);
        this-> delay_postprocess = randlong(max_delay_ms);
    }

    void preprocess() {
        std::cout << "Working on preprocessing job for " << this->delay_preprocess << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(this->delay_preprocess));
    }

    void mainprocess() {
        std::cout << "Working on processing job for " << this->delay_mainprocess << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(this->delay_mainprocess));
    }

    void postprocess() {
        std::cout << "Working on postprocessing job for " << this->delay_postprocess << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(this->delay_postprocess));
    }

    std::string getId() {
        return this->id;
    }
};

template<typename T>
class ThreadSafeQueue {
    private:
        std::queue<T> queue;
        std::mutex mutex;
        std::condition_variable condition;

    public:
        void push(T item) {
            std::unique_lock<std::mutex> lock(this->mutex);
            this->queue.push(item);
            this->condition.notify_one();
        }

        T pop() {
            std::unique_lock<std::mutex> lock(this->mutex);
            this->condition.wait(lock, [this]() { return !this->queue.empty(); });
            T item = this->queue.front();
            this->queue.pop();

            return item;
        }
};

void worker_preprocess(ThreadSafeQueue<std::shared_ptr<Job>>& in,
        ThreadSafeQueue<std::shared_ptr<Job>>& out) {
    std::cout << "Worker preprocess thread started" << std::endl;

    bool alive = true;
    while (alive) {
        std::shared_ptr<Job> job = in.pop();

        if (job->getId() == "terminate") {
            alive = false;
        } else {
            std::cout << "Preprocessing job: " << job->getId() << std::endl;
            job->preprocess();
        }
        out.push(job);
    }

    std::cout << "Worker preprocess thread finished" << std::endl;
}

void worker_mainprocess(ThreadSafeQueue<std::shared_ptr<Job>>& in,
        ThreadSafeQueue<std::shared_ptr<Job>>& out) {
    std::cout << "Worker mainprocess thread started" << std::endl;

    bool alive = true;
    while (alive) {
        std::shared_ptr<Job> job = in.pop();

        if (job->getId() == "terminate") {
            alive = false;
        } else {
            std::cout << "Mainprocessing job: " << job->getId() << std::endl;
            job->mainprocess();
        }
        out.push(job);
    }

    std::cout << "Worker mainprocess thread finished" << std::endl;
}

void worker_postprocess(ThreadSafeQueue<std::shared_ptr<Job>>& in,
        ThreadSafeQueue<std::shared_ptr<Job>>& out) {
    std::cout << "Worker postprocess thread started" << std::endl;

    bool alive = true;
    while (alive) {
        std::shared_ptr<Job> job = in.pop();

        if (job->getId() == "terminate") {
            alive = false;
        } else {
            std::cout << "Postprocessing job: " << job->getId() << std::endl;
            job->postprocess();
        }
        out.push(job);
    }

    std::cout << "Worker postprocess thread finished" << std::endl;
}


ThreadSafeQueue<std::shared_ptr<Job>> input_queue;
ThreadSafeQueue<std::shared_ptr<Job>> preprocess_queue;
ThreadSafeQueue<std::shared_ptr<Job>> mainprocess_queue;
ThreadSafeQueue<std::shared_ptr<Job>> output_queue;


int main() {
    std::cout << "Main thread started" << std::endl;

    std::thread thread_preprocess([&]() { worker_preprocess(input_queue, preprocess_queue); });
    std::thread thread_mainprocess([&]() { worker_mainprocess(preprocess_queue, mainprocess_queue); });
    std::thread thread_postprocess([&]() { worker_postprocess(mainprocess_queue, output_queue); });

    Job jobs[] = {
        Job("job1", 1000),
        Job("job2", 1000),
        Job("job3", 1000),
        Job("job4", 1000),
        Job("terminate", 0),
    };

    for (Job job : jobs) {
        input_queue.push(std::make_shared<Job>(job));
    }

    std::cout << "Main thread waiting for worker thread" << std::endl;
    thread_preprocess.join();
    thread_mainprocess.join();
    thread_postprocess.join();

    std::cout << "Main thread finished" << std::endl;

    return 0;
}