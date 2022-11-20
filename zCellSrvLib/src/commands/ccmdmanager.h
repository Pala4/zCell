#ifndef CCMDMANAGER_H
#define CCMDMANAGER_H

#include <map>
#include <string>

#include "zCellSrvLib_global.h"
#include "thread/cjob_.h"
#include "ccommand.h"

namespace zcell_lib {

class CThreadPool;

class ZCELLSRVLIB_EXPORT CCmdManager
{
public:
    typedef std::map<std::string, command_ptr_t> commands_map_t;

    CCmdManager() = default;

    void set_thread_pool(CThreadPool *thread_pool);
    command_ptr_t create_command(const std::string &cmd_name, const bool &multy_thread,
                                 const CCommand::function_t &func);
    bool add_command(const command_ptr_t &command);
    const commands_map_t &commands() const;
    void execute(const std::string &cmd_line,
                 const CCommand::args_map_t &ext_args = CCommand::args_map_t());

    virtual ~CCmdManager() = default;
private:
    CThreadPool *m_thread_pool = nullptr;
    commands_map_t m_commands;
    std::vector<std::string> split(const std::string &s, char delim);
    command_ptr_t pars(const std::string &cmd_line, const CJobBase::args_map_t &ext_args);
};

} // namespace zcell_lib

#endif // CCMDMANAGER_H
