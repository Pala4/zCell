#include "cstringconvertor.h"

using namespace zcell_lib;

CStringConvertor::CStringConvertor()
{
}

bool CStringConvertor::convert(const std::string &val_str, std::any &any_val)
{
    any_val = std::make_any<std::string>(val_str);
    return true;
}
