#ifndef CJOB_H
#define CJOB_H

#include <functional>
#include <mutex>
#include <map>
#include <any>

#include "zCellSrvLib_global.h"

namespace zcell_lib {

class CWorker;

class ZCELLSRVLIB_EXPORT CJob
{
public:
    enum job_type_t {jt_linear, jt_linear_auto, jt_ring};
    typedef std::map<std::string, std::any> args_map_t;
    typedef std::function<void(CJob *job, const args_map_t&)> function_t;
    CJob(const job_type_t &job_type = jt_linear_auto);
    CJob(const function_t &function, const job_type_t &job_type = jt_linear_auto);

    const job_type_t &job_type();
    void set_job_type(const job_type_t &job_type);
    const function_t &function();
    void set_id(const std::string &id);
    const std::string &id();
    void set_function(const function_t &function, const args_map_t &args);
    void set_function(const function_t &function);
    void set_args(const args_map_t &args);
    CWorker *worker() const;
    void set_worker(CWorker *worker);
    virtual void run(CWorker *worker);

    virtual ~CJob() = default;
private:
    std::mutex m_mtx_job_type;
    std::mutex m_mtx_function;
    std::mutex m_mtx_id;
    std::mutex m_mtx_args;
    job_type_t m_job_type = jt_linear;
    function_t m_function = nullptr;
    std::string m_id;
    args_map_t m_args;
    CWorker *m_worker = nullptr;
};

} // namespace zcell_lib

#endif // CJOB_H
