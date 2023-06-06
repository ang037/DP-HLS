#ifndef PARAMS_H
#define PARAMS_H

#include <ap_fixed.h>

#define linear_gap_penalty -1
#define opening_score -2
#define extend_score -2
#define mismatch_score -3
#define match_score 3

#define max_query_length 256
#define max_reference_length 256

#define PE_num 8

#define numofreads 1

#define corner_case (max_query_length%PE_num != 0)
#define query_chunks (max_query_length/PE_num)
#define extra_PE_num (max_query_length%PE_num)

#define M 10
#define N 6

//#define DEBUG

#define N_BLOCKS 4

#define TB_PH 0  // this is place holder
#define	TB_LEFT 1
#define	TB_DIAG 2
#define	TB_UP 3
#define	TB_IX 4
#define	TB_IY 5


#define ALIGN_TYPE AlignLocalLinear

#define TB_LINE_SIZE 64  // This defines the length of a line of TB pointers. Must be larger than PE_num


#define chunk_width 16  // this must larger than PE_num

typedef ap_uint<3> char_t;
typedef ap_fixed<M, N> type_t;  // alias type_t with ap_fixed<M,N>

typedef ap_uint<3> tbp_t;

#define zero_fp ((type_t)0)



typedef char_t ref_buf[chunk_width];

// inflated ones takes care of corner case scenarios
#define inflated_query_length (query_chunks *PE_num + corner_case *PE_num)
#define inflated_query_chunks (query_chunks + corner_case)

#define DEBUG_OUTPUT_PATH 
#define DEBUG_FILENAME 

//enum tbp_t {
//	TB_LEFT,
//	TB_DIAG,
//	TB_UP,
//	TB_IX,
//	TB_IY
//};

#endif
