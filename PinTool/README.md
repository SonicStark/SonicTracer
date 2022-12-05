# TracerCore - a Pintool which produces the trace

*This `README` is a brief summary of our Pintool. Please refer to the contents in `docs` under the project's root directory for more details.*

## Build

First, make sure everything necessary in `PinKit` is ready.

#### :dart: For *Intel® 64 Architecture*

```shell
make TracerCore64
```

#### :dart: For *IA-32 Architecture*

```shell
make TracerCore32
```

#### :dart: Clear all built

```shell
make reset
```

## Use

Assume that the directory's name of *Pin Kit Directory Tree* in `PinKit` is `pin-latest-gcc-linux`. Now we'll use it against our example target `microBug-normal` on *Intel® 64 Architecture*.

Run

```shell
./PinKit/pin-latest-gcc-linux/pin -t ./PinTool/build/intel64/TracerCore.so -- ./examples/microBug/build/microBug-normal
```

and immediately we will get the following output

```text
[+] Welcome to Igor's powerful
'########:'########:::::'###:::::'######::'########:'########::
... ##..:: ##.... ##:::'## ##:::'##... ##: ##.....:: ##.... ##:
::: ##:::: ##:::: ##::'##:. ##:: ##:::..:: ##::::::: ##:::: ##:
::: ##:::: ########::'##:::. ##: ##::::::: ######::: ########::
::: ##:::: ##.. ##::: #########: ##::::::: ##...:::: ##.. ##:::
::: ##:::: ##::. ##:: ##.... ##: ##::: ##: ##::::::: ##::. ##::
::: ##:::: ##:::. ##: ##:::: ##:. ######:: ########: ##:::. ##:
:::..:::::..:::::..::..:::::..:::......:::........::..:::::..::
[+] TracerCore - a Pintool which produces the trace you need.

Pin tools switches

-TrCutName  [default ]
        Specify a series of function names whose all instruction addresses would be screened. Please append ';' at the
        end of each name for separation. Both reading names and finding matches are case insensitive.
-TrDatPath  [default ]
        Specify the path of output trace file.
-TrScaType  [default bbl]
        Specify the granularity of trace. Must be 'ins' or 'bbl' or 'cal'.
-TrSymPath  [default ]
        Specify the path of output trace symbol file.
-logfile  [default pintool.log]
        The log file path and file name
-unique_logfile  [default 0]
        The log file names will contain the pid

Line information controls

-discard_line_info 
        Discard line information for specific module. Module name should be a short name without path, not a symbolic
        link
-discard_line_info_all  [default 0]
        Discard line information for all modules.
-dwarf_file 
        Point pin to a different file for debug information. Syntax: app_executable:<path_to_different_fileExample
        (macOS*): -dwarf_file get_source_app:get_source_app.dSYM/Contents/Resources/DWARF/get_source_app

Symbols controls

-ignore_debug_info  [default 0]
        Ignore debug info for the image. Symbols are taken from the symbol tables.
-reduce_rtn_size_mode  [default auto]
        Mode for RTN size reduction: delete trailing instructions after RET if there is no jump to the rtn part after
        the RET. Possible modes are: auto/never/always
-short_name  [default 0]
        Use the shortest name for the RTN. Names with version substrings are preferred over the same name without the
        substring.
-support_jit_api  [default 0]
        Enables the Jitted Functions Support
-unrestricted_rtn_size  [default 0]
        Use the unrestricted RTN size. When set the RTN size defined by the distance between RTN start to the
        beginning of next RTN.

Statistic switches

-profile  [default 0]
        print amount of memory dynamically allocated but not yet freed by the tool
-statistic  [default 0]
        print general statistics

General switches (available in pin and tool)

-h  [default 0]
        Print help message (same as -help)
-help  [default 0]
        Print help message
-ifeellucky  [default 0]
        skip warning message for unsupported platforms and convert some errors to warnings
-slow_asserts  [default 0]
        Perform expensive sanity checks

[!] Bad KNOB_TrDatPath
```

---

<p>:christmas_tree::gift::tada:<em>Best of luck in the year to come</em>:tada::gift::christmas_tree:</p>
<p><em>Sincerely yours,</em></p>
<p><em>Sonic</em></p>
<p><em>Dec 5, 2022</em></p>
