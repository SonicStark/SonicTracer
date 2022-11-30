#ifndef HEAD_EMSG_H
#define HEAD_EMSG_H

#define EVIL_ARG ((INT32) -1)
#define GOOD_ARG ((INT32) 0 )

#define TL_INS ((INT32) 100)
#define TL_BBL ((INT32) 200)
#define TL_CAL ((INT32) 300)

#define GOOD_EXIT      ((int) 0)
#define EVIL_EXIT_INIT ((int) 10) //about `PIN_Init`
#define EVIL_EXIT_SYMA ((int) 11) //about `PIN_InitSymbolsAlt`
#define EVIL_EXIT_VSCA ((int) 100) //about `KNOB_TrScaType`
#define EVIL_EXIT_VCUT ((int) 101) //about `KNOB_TrCutName`
#define EVIL_EXIT_VDAT ((int) 102) //failed file-open on `KNOB_TrDatPath`
#define EVIL_EXIT_VSYM ((int) 103) //failed file-open on `KNOB_TrSymPath`

#endif