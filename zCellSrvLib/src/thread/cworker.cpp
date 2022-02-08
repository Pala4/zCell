#include "cworker.h"

#include <numeric>

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

bool CWorker::add_job(const CJob_::function_t &function)
{
    CJob_ *job = new CJob_(function, CJob_::jt_linear_auto);
    if (!add_job(job)) {
        delete job;
        return false;
    }
    return true;
}

bool CWorker::add_job(CJobBase *job)
{
    if (job == nullptr)
        return false;
    std::lock_guard lg(m_mtx_job_queue);
    m_job_queue.push(job);
    auto time = std::chrono::system_clock::to_time_t(std::chrono::high_resolution_clock::now());
    std::string time_str = std::to_string(time);
    time_str.resize(time_str.size() - 1);
    job->set_id(std::to_string(id()) + ':' + std::to_string(m_job_queue.size()) + ':' +
                std::to_string(m_job_id_counter) + ':' + time_str);
    ++m_job_id_counter;
    if (m_job_id_counter == std::numeric_limits<uint16_t>::max())
        m_job_id_counter = 0;
    job->set_worker(this);
    return true;
}

const bool &CWorker::is_calc_idle_fps()
{
    std::lock_guard lg(m_mtx_calc_idle_fps);
    return m_calc_idle_fps;
}

void CWorker::set_calc_idle_fps(const bool &calc_idle_fps)
{
    std::lock_guard lg(m_mtx_calc_idle_fps);
    m_calc_idle_fps = calc_idle_fps;
}

const uint32_t &CWorker::idle_fps_vol()
{
    std::lock_guard lg(m_mtx_idle_fps_vol);
    return m_idle_fps_vol;
}

void CWorker::set_idle_fps_vol(const uint32_t &idle_fps_vol)
{
    std::lock_guard lg(m_mtx_idle_fps_vol);
    m_idle_fps_vol = idle_fps_vol;
}

const uint8_t &CWorker::num_idle_fps_average_period()
{
    std::lock_guard lg(m_mtx_num_idle_fps_average_priod);
    return m_num_idle_fps_average_priod;
}

void CWorker::set_num_idle_fps_average_period(const uint &num_idle_average_period)
{
    std::lock_guard lg(m_mtx_num_idle_fps_average_priod);
    m_num_idle_fps_average_priod = num_idle_average_period;
}

const float &CWorker::idle_fps()
{
    std::lock_guard lg(m_mtx_idle_fps);
    return m_idle_fps;
}

void CWorker::set_hld_last_job(const hld_last_job_t &hld_last_job)
{
    std::lock_guard lg(m_mtx_hld_last_job);
    m_hld_last_job = hld_last_job;
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

void CWorker::set_idle_fps(const float &idle_fps)
{
    std::lock_guard lg(m_mtx_idle_fps);
    m_idle_fps = idle_fps;
}

const bool &CWorker::is_do_finishing()
{
    std::lock_guard lg(m_mtx_do_finish);
    return m_do_finish;
}

/*!
 * \brief CWorker::next_job process next job from jobs queue
 * \return last processed job if job has type CJob::jt_linear.
 * If job has type CJob::jt_linear_auto - job object will be
 * destroyed, and return value is nullptr. Else if type -
 * CJob::jt_ring, job after processing, will be moved to the
 * last position in queue, and return - nullptr
 */
CJobBase *CWorker::next_job()
{
    CJobBase *job = nullptr;
    m_mtx_job_queue.lock();
    if (!m_job_queue.empty())
        job = m_job_queue.front();
    m_mtx_job_queue.unlock();
    if (job != nullptr) {
        job->run(this);
        m_mtx_job_queue.lock();
        m_job_queue.pop();
        m_mtx_job_queue.unlock();
        if (job->job_type() == CJob_::jt_linear_auto) {
            delete job;
        } else if (job->job_type() == CJob_::jt_ring) {
            m_mtx_job_queue.lock();
            m_job_queue.push(job);
            m_mtx_job_queue.unlock();
        } else if (job->job_type() == CJob_::jt_linear)
            return job;
    }
    return nullptr;
}

void CWorker::work()
{
    set_active_state(true);
    set_do_finish_state(false);
    uint32_t idle_fps_counter = 0;
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    std::vector<float> idle_fps_average_buffer;
    uint8_t idle_fps_average_buffer_idx = 0;
    while (true && !is_do_finishing()) {
        //calc potentional perfomance index
        if (is_calc_idle_fps()) {
            if (idle_fps_counter == 0)
                start = std::chrono::high_resolution_clock::now();
            ++idle_fps_counter;
            if ((idle_fps_counter == idle_fps_vol()) || (num_jobs() != 0)) {
                idle_fps_counter = 0;
                end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float> dur = end - start;
                float fps = 1.0f/dur.count();
                //average fps
                if (num_idle_fps_average_period() > 0) {
                    idle_fps_average_buffer.push_back(fps);
                    float sum = std::accumulate(idle_fps_average_buffer.begin(),
                                                idle_fps_average_buffer.end(), 0);
                    fps = sum/float(idle_fps_average_buffer.size());
                    if (idle_fps_average_buffer.size() >= num_idle_fps_average_period())
                        idle_fps_average_buffer.erase(idle_fps_average_buffer.begin());
                }
                set_idle_fps(fps);
            }
        }
        CJobBase *last_job = next_job();
        if (last_job != nullptr) {
            if (m_hld_last_job != nullptr)
                m_hld_last_job(last_job);
        }
    }
    set_active_state(false);
    set_do_finish_state(true);
}
