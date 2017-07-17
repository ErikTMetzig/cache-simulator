# cache-simulator
A basic simulator tool for testing the efficacy of various cache dimensions.

Uage: csim [-hv] -s <num> -E <num> -b <num> -t <file>
Options:
-h          Print the help message.]
-v          Optional verbose flag.
-s <num>    Number of set index bits
-E <num>    Number of lines per set.
-b <num>    Number of block offset bits.
-t <file>   Trace file.

Examples:
  linux>  csim -s 4 -E 1 -b 4 -t sample_trace1
  linux>  csim -v -s 8 -E 2 -b 4 -t sample_trace2
