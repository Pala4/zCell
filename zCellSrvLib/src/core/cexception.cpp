#include "cexception.h"

using namespace zcell_lib;

CException::CException(const std::string &msg)
{
    m_msg = msg;
}

const char *CException::what() const noexcept
{
    return m_msg.c_str();
}
