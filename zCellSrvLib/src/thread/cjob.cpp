#include "cjob.h"

using namespace zcell_lib;

/*!
 * \class CJob
 */
CJob::CJob(const job_type_t &job_type) : CJob(nullptr, job_type)
{
}

CJob::CJob(const function_t &function, const job_type_t &job_type)
{
    m_job_type = job_type;
    m_function = function;
}

const CJob::job_type_t &CJob::job_type()
{
    std::lock_guard lg(m_mtx_job_type);
    return m_job_type;
}

void CJob::set_job_type(const job_type_t &job_type)
{
    std::lock_guard lg(m_mtx_job_type);
    m_job_type = job_type;
}

const CJob::function_t &CJob::function()
{
    std::lock_guard lg(m_mtx_function);
    return m_function;
}

void CJob::set_id(const std::string &id)
{
    std::lock_guard lg(m_mtx_id);
    m_id = id;
}

const std::string &CJob::id()
{
    std::lock_guard lg(m_mtx_id);
    return m_id;
}

void CJob::set_function(const function_t &function, const args_map_t &args)
{
    set_function(function);
    set_args(args);
}

void CJob::set_function(const function_t &function)
{
    std::lock_guard lg(m_mtx_function);
    m_function = function;
}

void CJob::set_args(const args_map_t &args)
{
    std::lock_guard lg(m_mtx_args);
    m_args = args;
}

CWorker *CJob::worker() const
{
    return m_worker;
}

void CJob::set_worker(CWorker *worker)
{
    m_worker = worker;
}

void CJob::run(CWorker *worker)
{
    std::lock_guard lg(m_mtx_function);
    if (m_function != nullptr)
        m_function(this, m_args);
}
