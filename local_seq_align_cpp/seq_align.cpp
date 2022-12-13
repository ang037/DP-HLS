#include <stdio.h>
#include <math.h>
#include "params.h"
#include <cstdlib>
#include <iostream>
#include "ap_int.h"

using namespace std;

void PE (char local_ref_val, char local_query_val, ap_uint<8> up_prev, ap_uint<8> left_prev, ap_uint<8> diag_prev, ap_uint<8> *score, ap_uint<4> *traceback_val){

    //#pragma HLS inline

    ap_int<8> d = left_prev + deletion_score;
    ap_int<8> i = up_prev + insertion_score;
    ap_int<8> mi = diag_prev + mismatch_score;
    ap_int<8> ma = diag_prev + match_score;
    ap_int<8> max_value = 0;
    ap_int<8> match = 0;
    ap_int<8> temp = 0;

    //std::cout << "d, i, mi, ma is" << d << "\t" << i << "\t" << mi << "\t" << ma << endl;

    //printf("d is %d, i is %d, mi is  %d, ma is %d, max is %d\n", d, i, mi, ma, max_value );

    match = (local_query_val == local_ref_val) ? ma : mi;

    max_value = (( (d>i) ? d : i ) > match ) ? ( (d>i) ? d : i ) : match ;

    //printf("updated max val is %d\n", max_value);

    // Write back results

    *traceback_val = (max_value == i) ? 1 : ((max_value == d) ? 10 : 11);

    *score = ((max_value < temp)? temp : max_value);

    //std::cout << "score is" << *score << endl;

    //std::cout << "traceback value is" << *traceback_val << endl;

}

ap_uint<8> seq_align (char query[query_length], char reference[ref_length], ap_uint<8> dp_matrix1[query_length][ref_length], ap_uint<4> traceback1[query_length-1][ref_length-1])
{

/*#pragma HLS INTERFACE m_axi port=query offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=reference offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=dp_matrix1 offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi port=traceback1 offset=slave bundle=gmem3

#pragma HLS INTERFACE s_axilite port=query bundle=control
#pragma HLS INTERFACE s_axilite port=reference bundle=control
#pragma HLS INTERFACE s_axilite port=dp_matrix1 bundle=control
#pragma HLS INTERFACE s_axilite port=traceback1 bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control*/

#pragma HLS INTERFACE bram port=query
#pragma HLS INTERFACE bram port=reference
#pragma HLS INTERFACE bram port=dp_matrix1
#pragma HLS INTERFACE bram port=traceback1

	//std::cout <<"executing the function" << endl;

    ap_uint<8> max_dp = 0;
    int max_row_value = 0;
    int max_col_value = 0;
    char local_query[query_length-1];
    #pragma HLS ARRAY_PARTITION variable=local_query factor=4 cyclic
    char local_ref[ref_length-1];
    #pragma HLS ARRAY_PARTITION variable=local_ref factor=4 cyclic
    ap_uint<8> dp_matrix[query_length][ref_length];
	#pragma HLS ARRAY_PARTITION variable=dp_matrix dim=0 complete
    ap_uint<4> traceback[query_length-1][ref_length-1];
    #pragma HLS ARRAY_PARTITION variable=traceback dim=1 complete

    local_query_loop: for (int k = 1; k < query_length; k ++){
        #pragma HLS PIPELINE II=1
        local_query[k-1] = query[k];
        //std::cout << local_query[k-1];
        //printf("%d\t ", local_query[k-1]);
    }

    local_ref_loop: for (int g = 1; g < ref_length; g ++){
        #pragma HLS PIPELINE II=1
        local_ref[g-1] = reference[g];
        //std::cout << local_ref[g-1];
        //printf("%d\t ", local_ref[g-1]);
    }

    //std::cout << "printed local loops" << endl;

   local_dp_matrix_loop: for (int p = 0; p < query_length; p ++){
    	for (int s = 0; s < ref_length; s ++){
    		//#pragma HLS PIPELINE II=1
    		dp_matrix[p][s] = dp_matrix1[p][s];
    		//std::cout << dp_matrix[p][s];
    	}
    }


    local_traceback_loop: for (int z = 0; z < query_length-1; z ++){
    	for (int w = 0; w < ref_length-1; w ++){
    		//#pragma HLS PIPELINE II=1
    		traceback[z][w] = traceback1[z][w];
    		//std::cout << dp_matrix[z][w];
    	}
    }

    //std::cout << "reaching to kernel" << endl;

    kernel: for(int ii = 0; ii < query_chunks; ii ++){
#pragma HLS PIPELINE II=258
        pe0: for (int i = 0; i < 258; i ++){

        //#pragma HLS LOOP_TRIPCOUNT min=7 max=7
        #pragma HLS PIPELINE II=1

            for (int j = 0; j < PE_num; j ++){

                if ((i-j) >= 0 && (i-j) < (ref_length-1)){

                    PE(local_ref[i-j], local_query[j+PE_num*ii], dp_matrix[j+PE_num*ii][i-j+1], dp_matrix[j+1+PE_num*ii][i-j], dp_matrix[j+PE_num*ii][i-j], &dp_matrix[j+1+PE_num*ii][i-j+1], &traceback[j+PE_num*ii][i-j]);

                    //std::cout << local_ref[i-j] << "\t" << local_query[j+PE_num*ii] <<"\t" << dp_matrix[j+PE_num*ii][i-j+1] <<"\t" << dp_matrix[j+1+PE_num*ii][i-j] <<"\t" << dp_matrix[j+PE_num*ii][i-j] <<"\t" << dp_matrix[j+1+PE_num*ii][i-j+1] <<"\t" << traceback[j+PE_num*ii][i-j] << endl;

                    //std::cout << "iteration " << i << endl;
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

    //std::cout << "max_col_value is" << max_col_value << "max row_value is" << max_row_value;

    ap_uint<8> max_score = dp_matrix[max_row_value][max_col_value];
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


   /*local_dp_matrix1_loop: for (ap_uint<4> u = 0; u < query_length; u ++){
    	for (ap_uint<8> v = 0; v < ref_length; v ++){
    		//#pragma HLS PIPELINE II=1
    		dp_matrix1[u][v] = dp_matrix[u][v];
    		//printf("%d\t ", local_ref[g-1]);
    	}
    }*/



 return max_score;

}


