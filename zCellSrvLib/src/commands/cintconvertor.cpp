#include "cintconvertor.h"

#include <stdexcept>

#include "core/cexception.h"

using namespace zcell_lib;

std::any CIntConvertor::convert(const std::string &val_str)
{
    int val_int = 0;
    try {
        val_int = std::stoi(val_str);
    } catch (std::invalid_argument const &ex) {
        throw CException("CIntConvertor error: incorrect value [" + val_str + "]" +
                         " (" + ex.what() + ")\n");
    } catch (std::out_of_range const &ex) {
        throw CException("CIntConvertor error: incorrect value [" + val_str + "]" +
                         " (" + ex.what() + ")\n");
    }
    return std::make_any<int>(val_int);
}
