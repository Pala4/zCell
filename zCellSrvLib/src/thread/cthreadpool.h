#ifndef CTHREADPOOL_H
#define CTHREADPOOL_H

#include <vector>
#include <mutex>
#include <thread>
#include <queue>
#include <functional>

#include "zCellSrvLib_global.h"

namespace zcell_lib {

typedef std::function<void()> job_t;

class CWorker
{
public:
    CWorker(const uint16_t &id){m_id = id;}

    const uint16_t &id() const{return m_id;}
    void start();
    const bool &is_active();
    void do_finish();
    uint32_t num_jobs() const;
    void add_job(const job_t &job);

    virtual ~CWorker();
private:
    uint16_t m_id = 0;
    std::thread m_thread;
    std::mutex m_mtx_active;
    std::mutex m_mtx_do_finish;
    std::mutex m_mtx_job_queue;
    bool m_active = false;
    bool m_do_finish = true;
    std::queue<job_t> m_job_queue;

    void set_active_state(const bool &active_sate);
    void set_do_finish_state(const bool &do_finish_state);
    const bool &is_do_finishing();
    void next_job();
    void work();
};

class ZCELLSRVLIB_EXPORT CThreadPool
{
public:
    CThreadPool();

    uint16_t optimal_thread_count() const;
    void set_num_threads(const uint16_t &num_threads);
    const uint16_t &num_threads() const;
    const bool &is_started() const;
    bool start();
    bool stop();

    virtual ~CThreadPool();
private:
    uint16_t m_num_threads = 0;
    bool m_started = false;
    std::vector<CWorker*> m_workers;
};

} // namespace zcell_lib

#endif // CTHREADPOOL_H
