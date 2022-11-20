#include "ccommand.h"

#include <iostream>

#include "core/cexception.h"

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

void CCmdExecution::execute()
{
    run(nullptr);
}

void CCmdExecution::execute(const args_map_t &args)
{
    set_args(args);
    execute();
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

const std::string &CCommand::description()
{
    std::lock_guard lg(m_mtx_description);
    return m_description;
}

CCommand &CCommand::set_description(const std::string &description)
{
    std::lock_guard lg(m_mtx_description);
    m_description = description;
    return *this;
}

const std::string &CCommand::usage()
{
    std::lock_guard lg(m_mtx_usage);
    return m_usage;
}

CCommand &CCommand::set_usage(const std::string &usage)
{
    std::lock_guard lg(m_mtx_usage);
    m_usage = usage;
    return *this;
}

const CCommand::argument_defs_t &CCommand::argument_defs()
{
    std::lock_guard lg(m_mtx_argument_defs);
    return m_argument_defs;
}

CCommand &CCommand::add_argument_def(const std::string &name,
                                     const converter_ptr_t &convertor,
                                     bool required,
                                     const std::string &usage)
{
    std::lock_guard lg(m_mtx_argument_defs);
    auto it = std::find_if(m_argument_defs.begin(), m_argument_defs.end(),
                           [&name](const auto &argument_def){return (argument_def.name == name);});
    if (it == m_argument_defs.end()) {
        argument_def arg_def;
        arg_def.name = name;
        arg_def.convertor = convertor;
        arg_def.required = required;
        arg_def.usage = usage;
        m_argument_defs.push_back(arg_def);
    }
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
