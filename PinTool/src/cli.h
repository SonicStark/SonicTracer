#ifndef HEAD_CLI_H
#define HEAD_CLI_H

#include "pin.H"
#include <fstream>
#include <string>
#include <vector>

void disp_usage();
std::string GetUpper(const std::string &str);

INT32 init_cli();
void fini_files();

extern std::ofstream            TrDat;
extern std::ofstream            TrSym;
extern std::vector<std::string> TrCut;
extern INT32                    TrSca;

#endif