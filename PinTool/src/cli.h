#ifndef HEAD_CLI_H
#define HEAD_CLI_H

#include "pin.H"
#include <fstream>
#include <string>
#include <vector>

void disp_ascii_text_banner();
void disp_usage();
std::string GetUpper(const std::string &str);

INT32 init_TrDat();
INT32 init_TrSym();
INT32 init_TrCut();
INT32 init_TrSca();

VOID fini_files(INT32 C, VOID *V);

extern std::ofstream            TrDat;
extern std::ofstream            TrSym;
extern std::vector<std::string> TrCut;
extern INT32                    TrSca;

#endif