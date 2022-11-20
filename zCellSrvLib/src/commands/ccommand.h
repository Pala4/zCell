#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <string>
#include <memory>

#include "zCellSrvLib_global.h"
#include "thread/cjob_.h"
#include "cconvertor.h"

namespace zcell_lib {

struct argument_def
{
    std::string name;
    converter_ptr_t convertor;
    bool required = true;
    std::string usage;
};

class ZCELLSRVLIB_EXPORT CCmdExecution :
        public CJobImpl<void(CCmdExecution *cmd_execution, const CJobBase::args_map_t &args)>
{
public:
    CCmdExecution();
    CCmdExecution(const std::string &name, const function_t &funcconst, const args_map_t &_args);

    const std::string &name();
    void set_name(const std::string &name);
    void execute();
    void execute(const args_map_t &args);

    virtual ~CCmdExecution() = default;
protected:
    virtual void do_function() override;
private:
    std::mutex m_mtx_name;
    std::string m_name;
};

typedef std::unique_ptr<CCmdExecution> cmd_execution_ptr_t;

class ZCELLSRVLIB_EXPORT CCommand : public CCmdExecution
{
public:
    typedef std::vector<argument_def> argument_defs_t;

    CCommand();
    CCommand(const std::string &name, const bool &multy_thread, const function_t &func);

    CCommand &set_name(const std::string &name);
    const bool& is_async();
    CCommand &set_async(const bool &async);
    const std::string &description();
    CCommand &set_description(const std::string &description);
    const std::string &usage();
    CCommand &set_usage(const std::string &usage);
    const argument_defs_t &argument_defs();
    CCommand& add_argument_def(const std::string &name,
                               const converter_ptr_t &convertor,
                               bool required = true,
                               const std::string &usage = std::string());

    cmd_execution_ptr_t get_cmd_execution_instace_();
    CCmdExecution *get_cmd_execution_instace();

    virtual ~CCommand() = default;
private:
    std::mutex m_mtx_async;
    bool m_async = false;
    std::mutex m_mtx_argument_defs;
    argument_defs_t m_argument_defs;
    std::mutex m_mtx_description;
    std::string m_description;
    std::mutex m_mtx_usage;
    std::string m_usage;
};

typedef std::shared_ptr<CCommand> command_ptr_t;

} // namespace zcell_lib

#endif // CCOMMAND_H
