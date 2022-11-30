#include "pin.H"
#include "amsg.h"
#include "cli.h"
#include "payload.h"
#include <iostream>

/**
 * The main procedure of the tool.
 * This function is called when the application image is loaded but not yet started.
 * @param argc total number of elements in the argv array
 * @param argv array of command line arguments, including pin -t <toolname> -- ...
 */
int main(int argc, char* argv[])
{
    disp_ascii_text_banner();

    if (!PIN_InitSymbolsAlt(DEBUG_OR_EXPORT_SYMBOLS)) {
        std::cout << "[!] PIN_InitSymbolsAlt failed" << std::endl;
        return EVIL_EXIT_SYMA;
    }
    
    if (PIN_Init(argc, argv)) {
        disp_usage();
        std::cout << "[!] PIN_Init failed" << std::endl;
        return EVIL_EXIT_INIT;
    }
    
    if (EVIL_ARG == init_TrDat()) {
        disp_usage();
        std::cout << "[!] Bad KNOB_TrDatPath" << std::endl;
        return EVIL_EXIT_VDAT;
    }

    if (EVIL_ARG == init_TrSym()) {
        disp_usage();
        std::cout << "[!] Bad KNOB_TrSymPath" << std::endl;
        return EVIL_EXIT_VSYM;
    }

    if (EVIL_ARG == init_TrSca()){
        disp_usage();
        std::cout << "[!] Bad KNOB_TrScaType" << std::endl;
        return EVIL_EXIT_VSCA;
    }

    if (EVIL_ARG == init_TrCut()){
        disp_usage();
        std::cout << "[!] Bad KNOB_TrCutName" << std::endl;
        return EVIL_EXIT_VCUT;
    }

    switch (TrSca)
    {
        case TL_INS:
            INS_AddInstrumentFunction(AnalyseINS, 0);
            break;
        case TL_BBL:
            TRACE_AddInstrumentFunction(AnalyseBBL, 0);
            break;
        case TL_CAL:
            RTN_AddInstrumentFunction(AnalyseCAL, 0);
            break;
        default:
            std::cout << "[!] Bad KNOB_TrScaType" << std::endl;
            return EVIL_EXIT_VSCA;
    }

    PIN_AddFiniFunction(fini_files, 0);
    PIN_StartProgram();
    return GOOD_EXIT;
}