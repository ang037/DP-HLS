#include <stdio.h>
#include "seq_align.h"
#include "params.h"
#include <cstdlib>
#include <iostream>
#include "ap_int.h"

using namespace std;

int main ()
{
    char query_string[query_length] = {'-', 'G', 'G', 'T', 'T', 'G', 'A', 'C', 'T'};

    //char reference_string[ref_length] = {'-','T', 'G', 'T', 'T', 'A', 'C', 'G', 'G'};

    ap_uint<8> dp_matrix[query_length][ref_length];

    ap_uint<8> max_score;

    srand(time(NULL));

    char reference_string[ref_length];

    reference_string[0] = '-';

    char alphabet[4] = { 'A', 'C', 'G', 'T'};

    for (int i = 1; i < ref_length; i++){
        reference_string[i] = alphabet[rand() % 4];
        //printf("res is %c\t", res[i]);
        std::cout << reference_string[i] << "\t";
    }

    std::cout << "query length is" << query_length << "and ref length is" << ref_length << endl;

    //printf("query_length, ref length is %d, %d respectively\n", query_length, ref_length);

    for (ap_uint<4> m = 0; m < query_length; m ++)
    {
        for (ap_uint<8> n = 0; n < ref_length; n ++){

            dp_matrix[m][n] = 0;
        }
    }

    ap_uint<4> traceback[query_length-1][ref_length-1];

    for (ap_uint<4> q = 0; q < query_length-1; q ++)
    {
        for (ap_uint<8> t = 0; t < ref_length-1; t ++){

            traceback[q][t] = 0;
        }
    }

    //std::cout << "reaching here" << endl;

    //std::cout << "function getting called" << endl;

    max_score = seq_align(query_string, reference_string, dp_matrix, traceback);

    /*for (int r = 0; r < query_length; r ++)
    {
        for (int s = 0; s < ref_length; s ++){

            std::cout << dp_matrix[r][s] << "\t";
        }
        std:cout << endl;
    }*/


    //printf("max score is %d\n", max_score);
    std::cout << "max score is " << max_score << endl;

    return 0;

}
