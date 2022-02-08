#ifndef CJOB__H
#define CJOB__H

#include <functional>
#include <mutex>
#include <map>
#include <any>

#include "zCellSrvLib_global.h"
//#include "cworker.h"

namespace zcell_lib {

class CWorker;

class ZCELLSRVLIB_EXPORT CJobBase
{
public:
    enum job_type_t {jt_linear, jt_linear_auto, jt_ring};
    typedef std::map<std::string, std::any> args_map_t;
    CJobBase(const job_type_t &job_type = jt_linear_auto);

    const job_type_t &job_type();
    void set_job_type(const job_type_t &job_type);
    void set_id(const std::string &id);
    const std::string &id();
    void set_args(const args_map_t &args);
    const args_map_t &args();
    CWorker *worker() const;
    void set_worker(CWorker *worker);
    virtual void run(CWorker *worker) = 0;

    virtual ~CJobBase() = default;
private:
    std::mutex m_mtx_job_type;
    std::mutex m_mtx_id;
    std::mutex m_mtx_args;
    job_type_t m_job_type = jt_linear;
    std::string m_id;
    args_map_t m_args;
    CWorker *m_worker = nullptr;
};

template<typename func_type>
class ZCELLSRVLIB_EXPORT CJobImpl : public CJobBase
{
public:
    typedef std::function<func_type> function_t;
    CJobImpl(const job_type_t &job_type = jt_linear_auto) : CJobBase(job_type){}
    CJobImpl(const function_t &function, const job_type_t &job_type = jt_linear_auto) :
      CJobBase(job_type)
    {
        m_function = function;
    }

    const function_t &function()
    {
        std::lock_guard lg(m_mtx_function);
        return m_function;
    }
    void set_function(const function_t &function, const args_map_t &args)
    {
        set_function(function);
        set_args(args);
    }
    void set_function(const function_t &function)
    {
        std::lock_guard lg(m_mtx_function);
        m_function = function;
    }
    virtual void run(zcell_lib::CWorker *worker)
    {
        std::lock_guard lg(m_mtx_function);
        if (m_function != nullptr)
            do_function();
    }

    virtual ~CJobImpl() = default;
protected:
    std::mutex m_mtx_function;
    function_t m_function = nullptr;

    virtual void do_function() = 0;
};

class CJob_ : public CJobImpl<void(CJob_ *job, const CJobBase::args_map_t &args)>
{
public:
    CJob_(const job_type_t &job_type = jt_linear_auto) : CJobImpl(job_type){}
    CJob_(const function_t &function, const job_type_t &job_type = jt_linear_auto) :
        CJobImpl(function, job_type){}
protected:
    virtual void do_function() override;
};

} // namespace zcell_lib

#endif // CJOB__H
