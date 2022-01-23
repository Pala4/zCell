#ifndef ZCELLSRVLIB_GLOBAL_H
#define ZCELLSRVLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ZCELLSRVLIB_LIBRARY)
#  define ZCELLSRVLIB_EXPORT Q_DECL_EXPORT
#else
#  define ZCELLSRVLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // ZCELLSRVLIB_GLOBAL_H