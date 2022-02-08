#include "ccommand.h"

#include <iostream>

using namespace zcell_lib;

/*!
 * \class CCmdExecution
 */
CCmdExecution::CCmdExecution() : CJobImpl(nullptr, CJob_::jt_linear)
{
}

CCmdExecution::CCmdExecution(const std::string &name, const function_t &func,
                             const args_map_t &_args) :
    CJobImpl(func, CJob_::jt_linear)
{
    m_name = name;
    set_args(_args);
}

const std::string &CCmdExecution::name()
{
    std::lock_guard lg(m_mtx_name);
    return m_name;
}

void CCmdExecution::set_name(const std::string &name)
{
    std::lock_guard lg(m_mtx_name);
    m_name = name;
}

void CCmdExecution::execute(const args_map_t &args)
{
    set_args(args);
    run(nullptr);
}

void CCmdExecution::do_function()
{
    m_function(this, args());
}

/*!
 * \class CCommand
 */
CCommand::CCommand() : CCmdExecution()
{
}

CCommand::CCommand(const std::string &name, const bool &multy_thread, const function_t &func) :
    CCmdExecution(name, func, args_map_t())
{
    m_async = multy_thread;
}

CCommand &CCommand::set_name(const std::string &name)
{
    CCmdExecution::set_name(name);
    return *this;
}

const bool &CCommand::is_async()
{
    std::lock_guard lg(m_mtx_async);
    return m_async;
}

CCommand &CCommand::set_async(const bool &async)
{
    std::lock_guard lg(m_mtx_async);
    m_async = async;
    return *this;
}

const std::map<std::string, converter_ptr_t> &CCommand::convertors()
{
    std::lock_guard lg(m_mtx_convertors);
    return m_convertors;
}

CCommand &CCommand::add_convertor(const std::string &name, const converter_ptr_t &convertor)
{
    std::lock_guard lg(m_mtx_convertors);
    if (m_convertors.find(name) == m_convertors.end())
        m_convertors[name] = convertor;
    return *this;
}

cmd_execution_ptr_t CCommand::get_cmd_execution_instace_()
{
    return std::make_unique<CCmdExecution>(name(), function(), args());
}

CCmdExecution *CCommand::get_cmd_execution_instace()
{
    auto cmd_exe = new CCmdExecution(name(), function(), args());
    cmd_exe->set_job_type(CJobBase::jt_linear_auto);
    return cmd_exe;
}
