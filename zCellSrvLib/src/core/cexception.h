#ifndef CEXCEPTION_H
#define CEXCEPTION_H

#include <stdexcept>

#include "zCellSrvLib_global.h"

namespace zcell_lib {

class ZCELLSRVLIB_EXPORT CException : public std::exception
{
public:
    CException(const std::string &msg);

    virtual const char *what() const noexcept;
private:
    std::string m_msg;
};

} // namespace zcell_lib

#endif // CEXCEPTION_H
