#include "cstringconvertor.h"

using namespace zcell_lib;

CStringConvertor::CStringConvertor()
{
}

std::any CStringConvertor::convert(const std::string &val_str)
{
    return std::make_any<std::string>(val_str);
}
