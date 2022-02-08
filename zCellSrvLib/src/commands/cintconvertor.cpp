#include "cintconvertor.h"

#include <stdexcept>

using namespace zcell_lib;

CIntConvertor::CIntConvertor()
{
}

bool CIntConvertor::convert(const std::string &val_str, std::any &any_val)
{
    int val_int = 0;
    try {
        val_int = std::stoi(val_str);
    } catch (std::invalid_argument const &ex) {
        throw ex.what();
        return false;
    } catch (std::out_of_range const &ex) {
        throw ex.what();
        return false;
    }
    any_val = std::make_any<int>(val_int);
    return true;
}
