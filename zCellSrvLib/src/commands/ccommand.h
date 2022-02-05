#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <string>

#include "zCellSrvLib_global.h"
#include "thread/cjob.h"

namespace zcell_lib {

class ZCELLSRVLIB_EXPORT CCommand : public CJob
{
public:
    CCommand(const std::string &name, const bool &multy_thread, const CJob::function_t &func);

    const bool &is_multy_thread() const;
    void execute(const CJob::args_map_t &args);
private:
    std::string m_name;
    bool m_multy_thread = false;
};

} // namespace zcell_lib

#endif // CCOMMAND_H
