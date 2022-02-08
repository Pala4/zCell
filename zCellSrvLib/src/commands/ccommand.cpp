#include "ccommand.h"

using namespace zcell_lib;

CCommand::CCommand() : CJobImpl(nullptr, CJob_::jt_linear)
{
}

CCommand::CCommand(const std::string &name, const bool &multy_thread, const function_t &func) :
    CJobImpl(func, CJob_::jt_linear)
{
    m_name = name;
    m_multy_thread = multy_thread;
}

const std::string &CCommand::name()
{
    std::lock_guard lg(m_mtx_name);
    return m_name;
}

CCommand &CCommand::set_name(const std::string &name)
{
    std::lock_guard lg(m_mtx_name);
    m_name = name;
    return *this;
}

const bool &CCommand::is_multy_thread()
{
    std::lock_guard lg(m_mtx_multy_thread);
    return m_multy_thread;
}

CCommand &CCommand::set_multy_thread(const bool &multy_thread)
{
    std::lock_guard lg(m_mtx_multy_thread);
    m_multy_thread = multy_thread;
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

void CCommand::execute(const args_map_t &args)
{
    set_args(args);
    run(nullptr);
}

CCommand::~CCommand()
{
}

void CCommand::do_function()
{
    m_function(this, args());
}
