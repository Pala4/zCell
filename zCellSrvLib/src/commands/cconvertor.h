#ifndef CCONVERTOR_H
#define CCONVERTOR_H

#include <any>
#include <string>
#include <memory>

#include "zCellSrvLib_global.h"

namespace zcell_lib {

class ZCELLSRVLIB_EXPORT CConvertor
{
public:
    CConvertor();

    virtual std::any convert(const std::string &val_str) = 0;

    virtual ~CConvertor();
};

typedef std::shared_ptr<CConvertor> converter_ptr_t;

template<typename T>
converter_ptr_t convertor()
{
    return std::make_shared<T>();
}


} // namespace zcell_lib

#endif // CCONVERTOR_H
