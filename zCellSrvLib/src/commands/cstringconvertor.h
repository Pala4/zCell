#ifndef CSTRINGCONVERTOR_H
#define CSTRINGCONVERTOR_H

#include "cconvertor.h"

namespace zcell_lib {

class ZCELLSRVLIB_EXPORT CStringConvertor : public CConvertor
{
public:
    CStringConvertor();

    virtual bool convert(const std::string &val_str, std::any &any_val);
};

} // namespace zcell_lib

#endif // CSTRINGCONVERTOR_H
