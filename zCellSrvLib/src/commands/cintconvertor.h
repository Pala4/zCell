#ifndef CINTCONVERTOR_H
#define CINTCONVERTOR_H

#include "zCellSrvLib_global.h"
#include "cconvertor.h"

namespace zcell_lib {

class ZCELLSRVLIB_EXPORT CIntConvertor : public CConvertor
{
public:
    CIntConvertor() = default;

    virtual bool convert(const std::string &val_str, std::any &any_val);
};

} // namespace zcell_lib

#endif // CINTCONVERTOR_H
