#include <stdio.h>
#include <math.h>
#include "params.h"

void PE (char local_ref_val, char local_query_val, int up_prev, int left_prev, int diag_prev, int *score, int *traceback_val){

    #pragma HLS inline

    int d = left_prev + deletion_score;
    int i = up_prev + insertion_score;
    int mi = diag_prev + mismatch_score;
    int ma = diag_prev + match_score;
    int max_value = 0;
    int match = 0;

    match = (local_query_val == local_ref_val) ? ma : mi;

    max_value = (( (d>i) ? d : i ) > match ) ? ( (d>i) ? d : i ) : match ;

    // Write back results
    *score = ((max_value < 0)? 0 : max_value);

    *traceback_val = (*score == i) ? 1 : ((*score == d) ? 10 : 11);

}

int seq_align (char query[query_length], char reference[ref_length], int dp_matrix1[query_length][ref_length], int traceback1[query_length-1][ref_length-1])
{

#pragma HLS INTERFACE bram port=query
#pragma HLS INTERFACE bram port=reference
#pragma HLS INTERFACE bram port=dp_matrix1
#pragma HLS INTERFACE bram port=traceback1

    int max_dp = 0;
    int max_row_value = 0;
    int max_col_value = 0;
    char local_query[query_length-1];
    #pragma HLS ARRAY_PARTITION variable=local_query factor=4 cyclic
    char local_ref[ref_length-1];
    #pragma HLS ARRAY_PARTITION variable=local_ref factor=4 cyclic
    int dp_matrix[query_length][ref_length];
	#pragma HLS ARRAY_PARTITION variable=dp_matrix dim=0 complete
    int traceback[query_length-1][ref_length-1];
    #pragma HLS ARRAY_PARTITION variable=traceback dim=1 complete

    local_query_loop: for (int k = 1; k < query_length; k ++){
        local_query[k-1] = query[k];
    }

    local_ref_loop: for (int g = 1; g < ref_length; g ++){
        local_ref[g-1] = reference[g];
    }

   local_dp_matrix_loop: for (int p = 0; p < query_length; p ++){
    	for (int s = 0; s < ref_length; s ++){
    		//#pragma HLS PIPELINE II=1
    		dp_matrix[p][s] = dp_matrix1[p][s];
    		//printf("%d\t ", local_ref[g-1]);
    	}
    }

    local_traceback_loop: for (int z = 0; z < query_length-1; z ++){
    	for (int w = 0; w < ref_length-1; w ++){
    		traceback[z][w] = traceback1[z][w];
    	}
    }

    kernel: for(int ii = 0; ii < query_chunks; ii ++){
#pragma HLS PIPELINE II=83
        pe0: for (int i = 0; i < 83; i ++){

        #pragma HLS PIPELINE II=1

            for (int j = 0; j < PE_num; j ++){

                if ((i-j) >= 0 && (i-j) < (ref_length - 1)){

                    PE(local_ref[i-j], local_query[j+PE_num*ii], dp_matrix[j+PE_num*ii][i-j+1], dp_matrix[j+1+PE_num*ii][i-j], dp_matrix[j+PE_num*ii][i-j], &dp_matrix[j+1+PE_num*ii][i-j+1], &traceback[j+PE_num*ii][i-j]);

                }
            }
        }
    }
    
    pe1: for (int x = 1; x < query_length; x ++){

        pe2: for (int y = 1; y < ref_length; y ++){

            if (dp_matrix[x][y] > max_dp)
            {
                max_dp = dp_matrix[x][y];
                max_row_value = x;
                max_col_value = y;
            }
                
        }

    }

    int max_score = dp_matrix[max_row_value][max_col_value];
    int col_value = max_col_value;
    int row_value = max_row_value;

    traceback_logic: while (dp_matrix[row_value][col_value] > 0){
    
        if (traceback[row_value-1][col_value-1] == 11){

            max_score = max_score + dp_matrix[row_value-1][col_value-1];
            row_value = row_value - 1;
            col_value = col_value - 1;

        }
        else if (traceback[row_value-1][col_value-1] == 10){

            max_score = max_score + dp_matrix[row_value][col_value-1];
            col_value = col_value - 1;
        }
        else if (traceback[row_value-1][col_value-1] == 01){

            max_score = max_score + dp_matrix[row_value-1][col_value];
            row_value = row_value - 1;
        }

        if (dp_matrix[row_value][col_value] == 0) break;
    }


   local_dp_matrix1_loop: for (int u = 0; u < query_length; u ++){
    	for (int v = 0; v < ref_length; v ++){
    		dp_matrix1[u][v] = dp_matrix[u][v];
    	}
    }



 return max_score;

}


