#ifndef CCMDMANAGER_H
#define CCMDMANAGER_H

#include <map>
#include <string>
#include <memory>

#include "zCellSrvLib_global.h"
#include "thread/cjob.h"

namespace zcell_lib {

class CCommand;
class CThreadPool;

class ZCELLSRVLIB_EXPORT CCmdManager
{
public:
    typedef std::shared_ptr<CCommand> command_ptr_t;

    CCmdManager() = default;

    void set_thread_pool(CThreadPool *thread_pool);
    void create_command(const std::string &cmd_name, const bool &multy_thread,
                        const CJob::function_t &func);
    void execute(const std::string &cmd_line, const CJob::args_map_t &ext_args = CJob::args_map_t());

    virtual ~CCmdManager() = default;
private:
    CThreadPool *m_thread_pool = nullptr;
    std::map<std::string, command_ptr_t> m_commands;
    std::vector<std::string> split(const std::string &s, char delim);
};

} // namespace zcell_lib

#endif // CCMDMANAGER_H
