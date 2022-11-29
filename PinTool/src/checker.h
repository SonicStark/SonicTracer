#ifndef HEAD_CHECKER_H
#define HEAD_CHECKER_H

#include "pin.H"

inline bool IsBlockedName(const std::string &SN);
bool IsBlocked(const std::string &name);
bool IsBlocked(ADDRINT            addr);
bool IsBlocked(RTN               &rtni);

inline bool IsInsideMain(ADDRINT addr);
inline bool IsInsideMain(RTN    &rtni);

#endif