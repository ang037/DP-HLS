#ifndef PARAMS_H
#define PARAMS_H

#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_vector.h>

// These need to be there to do CMake Simulation, but SHOULD TAKE OFF WHEN COMPIING BITSTREAM
#define PE_NUM 16
#define MAX_QUERY_LENGTH 256
#define MAX_REFERENCE_LENGTH 256
#define N_BLOCKS 1

#define ALIGN_TYPE BandingGlobalTwoPieceAffine
#define N_LAYERS 5
#define LAYER_MAXIMIUM 1  // We need to indicate from which layer (main matrix) is the maximum score stored.

#define BANDING RECTANGULAR

//#define THRESHOLD 5 // this needs to be less than the query and reference length

// Primitive Types
typedef ap_uint<2> char_t;  // Sequence Alphabet
typedef ap_fixed<32, 20> type_t;  // Scores Type <width, integer_width>
typedef short idx_t;  // Indexing Type, could be much less than 32. ap_uint<8>
typedef ap_uint<7> tbp_t;  // Traceback Pointer Type

// Bit 0, 1, 2: specify which of H, I, I', D, D' matrices to go to
// Bit 4, 5: specify which movement for I, I', D, D'

// Defien upper and lower bound for score type, aka type_t
#define INF 65536
#define NINF -65536

// Legacy Debugger Configuration
#define DEBUG_OUTPUT_PATH "/home/yic033@AD.UCSD.EDU/DP-HLS-Debug/global_affine/"
#define DEBUG_FILENAME "debug_kernel"

struct Penalties {
    type_t open;
    type_t extend;
    type_t mismatch;
    type_t match;
    type_t linear_gap;
    type_t long_open;
    type_t long_extend;
};

enum TB_STATE {
    MM = 0,   // Match/Mismatch
    INS = 1,  // Insertion
    DEL = 2,  // Deletion
    END = 3,   // End
    LONG_INS = 4, // Long Insertion
    LONG_DEL = 5 // Long Deletion
};

#define ZERO_CHAR (char_t(0))
#define zero_fp ((type_t)0)


// Define Traceback Pointer Origin (H - Main, I, I', D, D')
#define TB_MAIN (tbp_t) 0b000
#define TB_INSERT (tbp_t) 0b001
#define TB_LONG_INSERT (tbp_t) 0b010
#define TB_DELETE (tbp_t) 0b011
#define TB_LONG_DELETE (tbp_t) 0b100

// Define Traceback Pointer Navigation Matrix
#define TB_INS_EXTEND (tbp_t) 0b0001000
#define TB_LONG_INS_EXTEND (tbp_t) 0b0010000
#define TB_DEL_EXTEND (tbp_t) 0b0100000
#define TB_LONG_DEL_EXTEND (tbp_t) 0b1000000

#endif
