#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <string>
#include <memory>

#include "zCellSrvLib_global.h"
#include "thread/cjob_.h"
#include "cconvertor.h"

namespace zcell_lib {



class ZCELLSRVLIB_EXPORT CCommand :
        public CJobImpl<void(CCommand *cmd, const CJobBase::args_map_t &args)>
{
public:
    CCommand();
    CCommand(const std::string &name, const bool &multy_thread, const function_t &func);

    const std::string &name();
    CCommand& set_name(const std::string &name);
    const bool& is_multy_thread();
    CCommand& set_multy_thread(const bool &multy_thread);
    const std::map<std::string, converter_ptr_t> &convertors();
    CCommand& add_convertor(const std::string &name, const converter_ptr_t &convertor);

    void execute(const args_map_t &args);

    virtual ~CCommand();
protected:
    virtual void do_function();
private:
    std::mutex m_mtx_name;
    std::string m_name;
    std::mutex m_mtx_multy_thread;
    bool m_multy_thread = false;
    std::mutex m_mtx_convertors;
    std::map<std::string, converter_ptr_t> m_convertors;
};

typedef std::shared_ptr<CCommand> command_ptr_t;

} // namespace zcell_lib

#endif // CCOMMAND_H
