#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <string>
#include <memory>

#include "zCellSrvLib_global.h"
#include "thread/cjob_.h"
#include "cconvertor.h"

namespace zcell_lib {

class ZCELLSRVLIB_EXPORT CCmdExecution :
        public CJobImpl<void(CCmdExecution *cmd_execution, const CJobBase::args_map_t &args)>
{
public:
    CCmdExecution();
    CCmdExecution(const std::string &name, const function_t &funcconst, const args_map_t &_args);

    const std::string &name();
    void set_name(const std::string &name);
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
    CCommand();
    CCommand(const std::string &name, const bool &multy_thread, const function_t &func);

    CCommand& set_name(const std::string &name);
    const bool& is_async();
    CCommand& set_async(const bool &async);
    const std::map<std::string, converter_ptr_t> &convertors();
    CCommand& add_convertor(const std::string &name, const converter_ptr_t &convertor);

    cmd_execution_ptr_t get_cmd_execution_instace_();
    CCmdExecution *get_cmd_execution_instace();

    virtual ~CCommand() = default;
private:
    std::mutex m_mtx_async;
    bool m_async = false;
    std::mutex m_mtx_convertors;
    std::map<std::string, converter_ptr_t> m_convertors;
};

typedef std::shared_ptr<CCommand> command_ptr_t;

} // namespace zcell_lib

#endif // CCOMMAND_H
