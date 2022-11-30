#include <stdio.h>
#include "seq_align.h"
#include "params.h"

int main ()
{
    char query_string[query_length] = {'-', 'G', 'G', 'T', 'T', 'G', 'A', 'C', 'T'};

    //char reference_string[ref_length] = {'-','T', 'G', 'T', 'T', 'A', 'C', 'G', 'G'};

    int dp_matrix[query_length][ref_length];

    int max_score;

    srand(time(NULL));

    char reference_string[ref_length];

    reference_string[0] = '-';

    char alphabet[26] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G',
                          'H', 'I', 'J', 'K', 'L', 'M', 'N',
                          'O', 'P', 'Q', 'R', 'S', 'T', 'U',
                          'V', 'W', 'X', 'Y', 'Z' };

    for (int i = 1; i < ref_length; i++){
        reference_string[i] = alphabet[rand() % 26];
        //printf("res is %c\t", res[i]);
    }

    //printf("query_length, ref length is %d, %d respectively\n", query_length, ref_length);

    for (int m = 0; m < query_length; m ++)
    {
        for (int n = 0; n < ref_length; n ++){

            dp_matrix[m][n] = 0;
        }
    }

    int traceback[query_length-1][ref_length-1];

    for (int q = 0; q < query_length-1; q ++)
    {
        for (int t = 0; t < ref_length-1; t ++){

            traceback[q][t] = 0;
        }
    }

    max_score = seq_align(query_string, reference_string, dp_matrix, traceback);

    /*for (int r = 0; r < query_length; r ++)
    {
        for (int s = 0; s < ref_length; s ++){

            printf("%d\t", dp_matrix[r][s]);
        }
        printf("\n");
    }*/

    printf("max score is %d\n", max_score);

    return 0;

}
