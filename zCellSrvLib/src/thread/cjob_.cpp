#include "cjob_.h"

using namespace zcell_lib;

/*!
 * \class CJob
 */
CJobBase::CJobBase(const job_type_t &job_type)
{
    m_job_type = job_type;
}

const CJobBase::job_type_t &CJobBase::job_type()
{
    std::lock_guard lg(m_mtx_job_type);
    return m_job_type;
}

void CJobBase::set_job_type(const job_type_t &job_type)
{
    std::lock_guard lg(m_mtx_job_type);
    m_job_type = job_type;
}

void CJobBase::set_id(const std::string &id)
{
    std::lock_guard lg(m_mtx_id);
    m_id = id;
}

const std::string &CJobBase::id()
{
    std::lock_guard lg(m_mtx_id);
    return m_id;
}

void CJobBase::set_args(const args_map_t &args)
{
    std::lock_guard lg(m_mtx_args);
    m_args = args;
}

const CJobBase::args_map_t &CJobBase::args()
{
    std::lock_guard lg(m_mtx_args);
    return m_args;
}

zcell_lib::CWorker *CJobBase::worker() const
{
    return m_worker;
}

void CJobBase::set_worker(zcell_lib::CWorker *worker)
{
    m_worker = worker;
}

void CJob_::do_function()
{
    m_function(this, args());
}
