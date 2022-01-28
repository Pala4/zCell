#include "cthreadpool.h"

#include <chrono>
#include <algorithm>

using namespace zcell_lib;

/*!
 * \class CWorker
 */
void CWorker::start()
{
    if (!is_active())
        m_thread = std::thread(&CWorker::work, this);
}

const bool &CWorker::is_active()
{
    std::lock_guard lg(m_mtx_active);
    return m_active;
}

void CWorker::do_finish()
{
    set_do_finish_state(true);
}

uint32_t CWorker::num_jobs()
{
    std::lock_guard lg(m_mtx_job_queue);
    return m_job_queue.size();
}

void CWorker::add_job(const job_t &job)
{
    std::lock_guard lg(m_mtx_job_queue);
    m_job_queue.push(job);
}

CWorker::~CWorker()
{
    if (m_thread.joinable())
        m_thread.join();
}

void CWorker::set_active_state(const bool &active_sate)
{
    std::lock_guard lg(m_mtx_active);
    m_active = active_sate;
}

void CWorker::set_do_finish_state(const bool &do_finish_state)
{
    std::lock_guard lg(m_mtx_do_finish);
    m_do_finish = do_finish_state;
}

const bool &CWorker::is_do_finishing()
{
    std::lock_guard lg(m_mtx_do_finish);
    return m_do_finish;
}

void CWorker::next_job()
{
    m_mtx_job_queue.lock();
    if (!m_job_queue.empty()) {
        job_t job = m_job_queue.front();
        m_mtx_job_queue.unlock();
        if (job != nullptr)
            job();
        m_mtx_job_queue.lock();
        m_job_queue.pop();
    }
    m_mtx_job_queue.unlock();
}

void CWorker::work()
{
    set_active_state(true);
    set_do_finish_state(false);
    while (true && !is_do_finishing())
        next_job();
    set_active_state(false);
    set_do_finish_state(true);
}

/*!
 * \class CThreadPool
 */
CThreadPool::CThreadPool()
{
    m_num_threads = std::thread::hardware_concurrency();
    for (uint16_t i = 0; i < m_num_threads; ++i)
        m_workers.push_back(new CWorker(i));
}

uint16_t CThreadPool::optimal_thread_count() const
{
    return std::thread::hardware_concurrency();
}

void CThreadPool::set_num_threads(const uint16_t &num_threads)
{
    uint16_t current_num_th = static_cast<uint16_t>(m_workers.size());
    if (current_num_th == num_threads)
        return;
    if (current_num_th < num_threads) {
        for (uint16_t i = 0; i < num_threads - current_num_th; ++i)
            m_workers.push_back(new CWorker(current_num_th + i));
    } else {
        for (uint16_t i = current_num_th - 1; i > num_threads - 1; --i) {
            CWorker *worker = m_workers[i];
            if (worker != nullptr) {
                worker->do_finish();
                delete  worker;
            }
            m_workers[i] = nullptr;
        }
        std::vector<CWorker*>::iterator first_it, end_it;
        first_it = m_workers.begin();
        std::advance(first_it, num_threads);
        m_workers.erase(first_it, m_workers.end());
    }

    m_num_threads = static_cast<uint16_t>(m_workers.size());
}

const uint16_t &CThreadPool::num_threads() const
{
    return m_num_threads;
}

const bool &CThreadPool::is_started() const
{
    return m_started;
}

bool CThreadPool::start()
{
    if (m_started || (m_num_threads == 0))
        return false;

    foreach (CWorker *worker, m_workers) {
        if (worker != nullptr)
            worker->start();
    }
    m_started = true;

    return true;
}

bool CThreadPool::stop()
{
    if (!m_started || (m_num_threads == 0))
        return false;

    foreach (CWorker *worker, m_workers) {
        if (worker != nullptr)
            worker->do_finish();
    }
    m_started = false;

    return true;
}

void CThreadPool::add_job(const job_t &job)
{
    if (!m_started)
        return;
    CWorker *min_job_worker = *std::min_element(m_workers.begin(), m_workers.end(),
        [](CWorker *wrk_a, CWorker *wrk_b){
            if ((wrk_a != nullptr) && (wrk_b != nullptr))
                return wrk_a->num_jobs() < wrk_b->num_jobs();
            return false;
        });
    if (min_job_worker != nullptr)
        min_job_worker->add_job(job);
}

CThreadPool::~CThreadPool()
{
    foreach(CWorker *worker, m_workers) {
        if (worker != nullptr) {
            worker->do_finish();
            delete worker;
        }
    }
}
