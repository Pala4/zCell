#ifndef CTHREADPOOL_H
#define CTHREADPOOL_H

#include <vector>

#include "zCellSrvLib_global.h"
#include "cworker.h"

namespace zcell_lib {

class ZCELLSRVLIB_EXPORT CThreadPool
{
public:
    CThreadPool();

    const bool &is_calc_idle_fps() const;
    void set_calc_idle_fps(const bool &calc_idle_fps);
    const uint32_t &idle_fps_vol() const;
    void set_idle_fps_vol(const uint32_t &idle_fps_vol);
    const uint8_t &num_idle_fps_average_period();
    void set_num_idle_fps_average_period(const uint8_t &num_idle_fps_average_period);

    uint16_t optimal_workers_count() const;
    void set_num_workers(const uint16_t &num_workers);
    const uint16_t &num_workers() const;
    CWorker *get_worker(const uint16_t &worker_idx);
    CWorker *get_min_load_worker() const;
    const bool &is_started() const;
    bool start();
    bool stop();
    bool add_job(const CJob_::function_t &function);
    bool add_job(CJobBase *job);
    void set_hld_last_job(const CWorker::hld_last_job_t &hld_last_job);

    virtual ~CThreadPool();
private:
    bool m_calc_idle_fps = false;
    uint32_t m_idle_fps_vol = 0;
    uint8_t m_num_idle_fps_average_period = 0;
    uint16_t m_num_workers = 0;
    bool m_started = false;
    std::vector<CWorker*> m_workers;
    CWorker::hld_last_job_t m_hld_last_job = nullptr;

    CWorker *add_worker(const uint16_t &id);
};

} // namespace zcell_lib

#endif // CTHREADPOOL_H
