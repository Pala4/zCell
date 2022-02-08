#include "cthreadpool.h"

#include <iostream>
#include <cstring>
#include <algorithm>

#include "cworker.h"

using namespace zcell_lib;

/*!
 * \class CThreadPool
 */
CThreadPool::CThreadPool()
{
    m_num_workers = std::thread::hardware_concurrency();
    for (uint16_t i = 0; i < m_num_workers; ++i)
        add_worker(i);
}

const bool &CThreadPool::is_calc_idle_fps() const
{
    return m_calc_idle_fps;
}

void CThreadPool::set_calc_idle_fps(const bool &calc_idle_fps)
{
    if (m_calc_idle_fps == calc_idle_fps)
        return;
    m_calc_idle_fps = calc_idle_fps;
    foreach (CWorker *worker, m_workers) {
        if (worker != nullptr)
            worker->set_calc_idle_fps(m_calc_idle_fps);
    }
}

const uint32_t &CThreadPool::idle_fps_vol() const
{
    return m_idle_fps_vol;
}

void CThreadPool::set_idle_fps_vol(const uint32_t &idle_fps_vol)
{
    if (m_idle_fps_vol == idle_fps_vol)
        return;
    m_idle_fps_vol = idle_fps_vol;
    foreach (CWorker *worker, m_workers) {
        if (worker != nullptr)
            worker->set_idle_fps_vol(m_idle_fps_vol);
    }
}

const uint8_t &CThreadPool::num_idle_fps_average_period()
{
    return m_num_idle_fps_average_period;
}

void CThreadPool::set_num_idle_fps_average_period(const uint8_t &num_idle_fps_average_period)
{
    if (m_num_idle_fps_average_period == num_idle_fps_average_period)
        return;
    m_num_idle_fps_average_period = num_idle_fps_average_period;
    foreach (CWorker *worker, m_workers) {
        if (worker != nullptr)
            worker->set_num_idle_fps_average_period(m_num_idle_fps_average_period);
    }
}

uint16_t CThreadPool::optimal_workers_count() const
{
    return std::thread::hardware_concurrency();
}

void CThreadPool::set_num_workers(const uint16_t &num_workers)
{
    uint16_t current_num_th = static_cast<uint16_t>(m_workers.size());
    if (current_num_th == num_workers)
        return;
    if (current_num_th < num_workers) {
        for (uint16_t i = 0; i < num_workers - current_num_th; ++i)
            add_worker(current_num_th + i);
    } else {
        for (uint16_t i = current_num_th - 1; i > num_workers - 1; --i) {
            CWorker *worker = m_workers[i];
            if (worker != nullptr) {
                worker->do_finish();
                delete  worker;
            }
            m_workers[i] = nullptr;
        }
        std::vector<CWorker*>::iterator first_it, end_it;
        first_it = m_workers.begin();
        std::advance(first_it, num_workers);
        m_workers.erase(first_it, m_workers.end());
    }

    m_num_workers = static_cast<uint16_t>(m_workers.size());
}

const uint16_t &CThreadPool::num_workers() const
{
    return m_num_workers;
}

CWorker *CThreadPool::get_worker(const uint16_t &worker_idx)
{
    if (worker_idx >= m_workers.size())
        return nullptr;
    return m_workers.at(worker_idx);
}

CWorker *CThreadPool::get_min_load_worker() const
{
    CWorker *min_wrk = nullptr;
    for (CWorker *wrk : m_workers) {
        if (wrk == nullptr)
            continue;
        if ((min_wrk == nullptr) || ((min_wrk != nullptr) && (wrk->num_jobs() < min_wrk->num_jobs())))
            min_wrk = wrk;
    }
    return min_wrk;
}

const bool &CThreadPool::is_started() const
{
    return m_started;
}

bool CThreadPool::start()
{
    if (m_started || (m_num_workers == 0))
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
    if (!m_started || (m_num_workers == 0))
        return false;

    foreach (CWorker *worker, m_workers) {
        if (worker != nullptr)
            worker->do_finish();
    }
    m_started = false;

    return true;
}

bool CThreadPool::add_job(const CJob_::function_t &function)
{
    if (function == nullptr)
        return false;
    CWorker *min_load_worker = get_min_load_worker();
    if (min_load_worker == nullptr)
        return false;
    return min_load_worker->add_job(function);
}

bool CThreadPool::add_job(CJobBase *job)
{
    if (job == nullptr)
        return false;
    CWorker *min_load_worker = get_min_load_worker();
    if (min_load_worker == nullptr)
        return false;
    if (!min_load_worker->add_job(job)) {
        if (job->job_type() == CJob_::jt_linear_auto)
            delete job;
        return false;
    }
    return true;
}

void CThreadPool::set_hld_last_job(const CWorker::hld_last_job_t &hld_last_job)
{
    m_hld_last_job = hld_last_job;
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

CWorker *CThreadPool::add_worker(const uint16_t &id)
{
    CWorker *worker = new CWorker(id);
    worker->set_calc_idle_fps(m_calc_idle_fps);
    worker->set_idle_fps_vol(m_idle_fps_vol);
    worker->set_num_idle_fps_average_period(m_num_idle_fps_average_period);
    worker->set_hld_last_job([this](CJobBase *last_job) {
        if (last_job != nullptr) {
            if (m_hld_last_job != nullptr)
                m_hld_last_job(last_job);
        }
    });
    m_workers.push_back(worker);
    return worker;
}
