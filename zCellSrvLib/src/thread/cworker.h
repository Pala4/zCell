#ifndef CWORKER_H
#define CWORKER_H

#include <mutex>
#include <thread>
#include <queue>

#include "zCellSrvLib_global.h"
#include "cjob_.h"

namespace zcell_lib {

class ZCELLSRVLIB_EXPORT CWorker
{
public:
    typedef std::function<void(CJobBase *job)> hld_last_job_t;

    CWorker(const uint16_t &id){m_id = id;}

    const uint16_t &id() const{return m_id;}
    void start();
    const bool &is_active();
    void do_finish();
    uint32_t num_jobs();
    bool add_job(const CJob_::function_t &function);
    bool add_job(CJobBase *job);
    const bool &is_calc_idle_fps();
    void set_calc_idle_fps(const bool &calc_idle_fps);
    const uint32_t &idle_fps_vol();
    void set_idle_fps_vol(const uint32_t &idle_fps_vol);
    const uint8_t &num_idle_fps_average_period();
    void set_num_idle_fps_average_period(const uint &num_idle_average_period);
    const float &idle_fps();
    void set_hld_last_job(const hld_last_job_t &hld_last_job);

    virtual ~CWorker();
private:
    uint16_t m_job_id_counter = 0;
    uint16_t m_id = 0;
    std::thread m_thread;
    std::mutex m_mtx_active;
    std::mutex m_mtx_do_finish;
    std::mutex m_mtx_job_queue;
    std::mutex m_mtx_calc_idle_fps;
    std::mutex m_mtx_idle_fps_vol;
    std::mutex m_mtx_num_idle_fps_average_priod;
    std::mutex m_mtx_idle_fps;
    std::mutex m_mtx_hld_last_job;
    bool m_active = false;
    bool m_do_finish = true;
    std::queue<CJobBase*> m_job_queue;
    bool m_calc_idle_fps = false;
    uint32_t m_idle_fps_vol = 0;
    uint8_t m_num_idle_fps_average_priod = 0;
    float m_idle_fps = 0.0f;
    hld_last_job_t m_hld_last_job = nullptr;

    void set_active_state(const bool &active_sate);
    void set_do_finish_state(const bool &do_finish_state);
    void set_idle_fps(const float &idle_fps);
    const bool &is_do_finishing();
    CJobBase *next_job();
    void work();
};

} // namespace zcell_lib

#endif // CWORKER_H
