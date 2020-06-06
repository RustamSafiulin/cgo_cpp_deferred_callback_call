
#include <functional>
#include <queue>
#include <thread>
#include <memory>
#include <iostream>
#include <atomic>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "proxy.h"

namespace proxy 
{
    class calculation_service final
    {
    public:
        calculation_service(notify_result_callback result_callback)
            : m_result_callback(result_callback)
            , m_stopped(false)
        {
            m_calculation_thread = std::thread(std::bind(&calculation_service::calculate_thread_func, this));
        }

        ~calculation_service()
        {
            m_stopped.store(true);

            m_tasks_cv.notify_one();

            if (m_calculation_thread.joinable())
                m_calculation_thread.join();
        }

        void add_new_task(int fake_task)
        {
            std::lock_guard<decltype(m_tasks_mutex)> lock(m_tasks_mutex);
            std::cout << "Passed task value: " << fake_task << std::endl;
            m_tasks_queue.push(fake_task);
            m_tasks_cv.notify_one();
        }

    private:
        void calculate_thread_func()
        {
            while (!m_stopped)
            {
                std::unique_lock<decltype(m_tasks_mutex)> lock(m_tasks_mutex);
                m_tasks_cv.wait(lock, [&] { return !m_stopped || !m_tasks_queue.empty(); });

                if (!m_tasks_queue.empty())
                {
                    const auto task = m_tasks_queue.front();
                    m_tasks_queue.pop();

                    std::cout << "Work with task: " << task << std::endl;

                    std::this_thread::sleep_for(std::chrono::seconds(5));

                    std::cout << "Task done. Notify" << std::endl;
                    const auto task_result = task * 5;

                    if (m_result_callback)
                        m_result_callback(task_result, "result description");
                }
            }
        }

    private:
        notify_result_callback m_result_callback;

        std::atomic<bool> m_stopped;
        std::mutex m_tasks_mutex;
        std::condition_variable m_tasks_cv;
        std::queue<int> m_tasks_queue;
        std::thread  m_calculation_thread;
    };

    std::shared_ptr<calculation_service> service_instance = nullptr;
    std::once_flag once_flag;

    void initialize_lifecycle(notify_result_callback result_callback)
    {
        std::call_once(once_flag, [&] {
            service_instance = std::make_shared<calculation_service>(result_callback);
        });
    }
}

void deinitialize_lifecycle()
{
    using namespace proxy;

    if (service_instance)
        service_instance.reset();
}

int initialize_service(notify_result_callback result_callback)
{
    using namespace proxy;

    try
    {
        initialize_lifecycle(result_callback);
        return 0;
    } catch (const std::exception &e)
    {
        std::cout << "Error during service initialization. Reason: " << e.what() << std::endl;
        return -1;
    }
}

void start_calculation(int task_value)
{
    using namespace proxy;

    if (service_instance)
        service_instance->add_new_task(task_value);
}

void deinitialize_service()
{
    deinitialize_lifecycle();
}
