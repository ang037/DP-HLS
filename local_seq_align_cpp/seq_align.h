//function prototype
#include "params.h"
#include "ap_int.h"

extern ap_uint<8> seq_align (char query[query_length], char reference[ref_length], ap_uint<8> dp_matrix1[query_length][ref_length], ap_uint<4> traceback1[query_length-1][ref_length-1]);

