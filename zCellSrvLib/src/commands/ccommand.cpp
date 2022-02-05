#include "ccommand.h"

using namespace zcell_lib;

CCommand::CCommand(const std::string &name, const bool &multy_thread, const function_t &func) :
    CJob(func, CJob::jt_linear)
{
    m_name = name;
    m_multy_thread = multy_thread;
}

const bool &CCommand::is_multy_thread() const
{
    return m_multy_thread;
}

void CCommand::execute(const args_map_t &args)
{
    set_args(args);
    run(nullptr);
}
