#include <stdio.h>
#include "seq_align.h"
#include "params.h"
#include <cstdlib>
#include <iostream>
#include "ap_int.h"

int main ()
{
    char query_string[query_length] = {'C', 'A', 'G', 'A'/* , 'C', 'C', 'T', 'A'*/};

    //char reference_string[ref_length] = {'C', 'C', 'G', 'T', 'A', 'C', 'T', 'A'};

    //int dp_matrix[query_length][ref_length];

    //ap_uint<8> max_score;

    srand(time(NULL));

    char reference_string[ref_length];

    char alphabet[4] = { 'A', 'C', 'G','T'};
 
    for (int i = 0; i < ref_length; i++){
        reference_string[i] = alphabet[rand() % 4];
        //printf("res is %c\t", res[i]);
    }

    ap_uint<4> dummy;

    seq_align(query_string, reference_string, &dummy/*traceback, Ix_matrix, Ix_matrix, final_matrix*/ );

    //printf("max score is %d\n", max_score);

    return 0;

}
