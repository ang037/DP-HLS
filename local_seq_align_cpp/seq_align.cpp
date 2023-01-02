#include <stdio.h>
#include <math.h>
#include "params.h"
#include <cstdlib>
#include <iostream>
#include "ap_int.h"

using namespace std;

void PE (char local_ref_val, char local_query_val, ap_uint<4> up_prev, ap_uint<4> left_prev, ap_uint<4> diag_prev, ap_uint<4> *score, ap_uint<4> *traceback, ap_uint<4> *final){

   //std::cout << "char local_ref_val is" <<  local_ref_val <<"local_query_val is" << local_query_val <<"int up_prev is" << up_prev << "left_prev is" << left_prev << "diag_prev is" << diag_prev << "*score is " << *score << endl;
    
    //#pragma HLS inline

	ap_int<4> d = left_prev + deletion_score;
	ap_int<4> i = up_prev + insertion_score;
	ap_int<4> temp = 0;
    //int max_val = (d > i) ? d : i;

	ap_int<4> match = (local_query_val == local_ref_val) ? diag_prev + match_score : diag_prev + mismatch_score;

	ap_int<4> max_value = (((d > i) ? d : i) > match ) ? ((d > i) ? d : i) : match ;

  // printf("d is %d, i is %d, match val is %d, max_value is %d\n", d, i, match, max_value );

    *traceback = (max_value == (diag_prev + match_score)) ? 11 : (max_value == d)? 10 : (max_value == i) ? 1 : 2;

    // Write back results
    *score = (max_value < temp) ? temp : max_value;

    *final = *score;

   // printf("score is %d\n", *score);

    //printf("local_ref_val is %d, local_query_val is %d, int up_prev is %d, int left_prev is %d, int diag_prev is %d, int *score is %d, Ix_prev is %d, int Ix_diag_prev is %d, int *Ix is %d, int Iy_prev is %d, int Iy_diag_prev is %d, int *Iy is %d, int *traceback_val is %d, int *final_value is %d \n", local_ref_val, local_query_val, up_prev, left_prev, diag_prev, *score, Ix_prev, Ix_diag_prev, *Ix, 
      //      Iy_prev, Iy_diag_prev, *Iy, *traceback_val, *final_value);

}

void seq_align (char query[query_length], char reference[ref_length], ap_uint<8> *dummy)
{

    char local_query[query_length];
    //#pragma HLS ARRAY_PARTITION variable=local_query dim=0 complete
    char local_ref[ref_length];
    //#pragma HLS ARRAY_PARTITION variable=local_ref dim=0 complete
    ap_uint<4> dp_mem[3][PE_num];
	#pragma HLS ARRAY_PARTITION variable=dp_mem dim=0 complete
    ap_uint<4> tb[query_length][ref_length];
//#pragma HLS ARRAY_PARTITION variable=tb dim=0 complete
    ap_uint<4> dp_matrix[query_length][ref_length];
    // last_pe_score[ref_length];
	//#pragma HLS ARRAY_PARTITION variable=last_pe_score dim=0 complete

    //std::cout << "query sequence is" << endl;

    local_query_loop: for (int k = 0; k < query_length; k ++){
        #pragma HLS PIPELINE II=1
        local_query[k] = query[k];
     //   printf("%c\t ", local_query[k]);
    }

   //std::cout << "ref sequence is" << endl;

    local_ref_loop: for (int g = 0; g < ref_length; g ++){
        #pragma HLS PIPELINE II=1
        local_ref[g] = reference[g];
     //   printf("%c\t ", local_ref[g]);
    }

    local_dpmem_loop: for (int gg = 0; gg < 3; gg ++){
        for (int ij = 0; ij < PE_num; ij++)
        {
            //#pragma HLS PIPELINE II=1
            dp_mem[gg][ij] = 0;
            //printf("%c\t ", local_ref[g]);

        }
    }

    kernel: //for (int qq = 0; qq < query_chunks; qq++){
//#pragma HLS PIPELINE II=83

    	for (int ii = 0; ii < (ref_length + PE_num-1); ii ++){

        #pragma HLS PIPELINE II=1

        for(int kk = 0; kk < PE_num; kk ++){
		#pragma HLS UNROLL

            if ((ii-kk) >= 0 && (ii-kk) < ref_length){

              //std::cout << "pe num is" << kk <<" and iteration is"<< ii << endl;

                if (kk == 0) PE(local_ref[ii-kk], local_query[kk], 0, dp_mem[1][kk], 0, &dp_mem[2][kk], &tb[kk][ii-kk], &dp_matrix[kk][ii-kk]);

                //else if (kk == 0 && qq > 0 && ii == 0) PE(local_ref[ii-kk], local_query[kk+PE_num*qq], last_pe_score[0], 0, 0, &dp_mem[2][kk], &tb[kk+PE_num*qq][ii-kk], &dp_matrix[kk+PE_num*qq][ii-kk]);

                //else if (kk == 0 && qq > 0 && ii > 0) PE(local_ref[ii-kk], local_query[kk+PE_num*qq], last_pe_score[ii], dp_mem[1][kk], last_pe_score[ii-1], &dp_mem[2][kk], &tb[kk+PE_num*qq][ii-kk], &dp_matrix[kk+PE_num*qq][ii-kk]);

                else PE(local_ref[ii-kk], local_query[kk], dp_mem[1][kk-1], dp_mem[1][kk], dp_mem[0][kk-1], &dp_mem[2][kk], &tb[kk][ii-kk], &dp_matrix[kk][ii-kk]);
                //std::cout << "Reached here" << endl;

                //if (ii > PE_num - 2 && kk == PE_num -1) last_pe_score[ii-PE_num+1] = dp_mem[2][PE_num-1];

            }
        }

        for (int ix = 0; ix < PE_num; ix++)
#pragma HLS UNROLL
        {
            dp_mem[0][ix] = dp_mem[1][ix];
            dp_mem[1][ix] = dp_mem[2][ix];
            //std::cout << "Reached here" << endl;
        }
    }

   //}

    ap_uint<4> max_dp = 0;
    int max_row_value;
    int max_col_value;

    	pe1: for (int x = 0; x < query_length; x ++){

           pe2: for (int y = 0; y < ref_length; y ++){

                if (dp_matrix[x][y] > max_dp)
                {
                    max_dp = dp_matrix[x][y];
                    max_row_value = x;
                    max_col_value = y;
                }

            }

        }

    	//std::cout << "reaching here " << endl;

    	ap_int<4> max_score = 0;
        int col_value = max_col_value;
        int row_value = max_row_value;
        //printf("%d\t", max_score);


        traceback_logic: while (dp_matrix[row_value][col_value] > 0){


            if (tb[row_value][col_value] == 2){

                max_score = max_score + mismatch_score;
                //printf("%d\t", final_matrix[row_value][col_value]);
                row_value = row_value - 1;
                col_value = col_value - 1;
                //std::cout <<"case0 " << max_score << endl;
                //std::cout <<"row and col is " << row_value << col_value << endl;
            }

            else if (tb[row_value][col_value] == 11){

                max_score = max_score + match_score;
                //printf("%d\t", final_matrix[row_value][col_value]);
                row_value = row_value - 1;
                col_value = col_value - 1;
                //std::cout <<"case1 " << max_score << endl;
                //std::cout <<"row and col is " << row_value << col_value <<  endl;

            }
            else if (tb[row_value][col_value] == 10){

                max_score = max_score + deletion_score;
                //printf("%d\t", final_matrix[row_value][col_value]);
                col_value = col_value - 1;
                //std::cout <<"case2 " << max_score << endl;
                //std::cout <<"row and col is " << row_value << col_value <<  endl;

            }
            else if (tb[row_value][col_value] == 1){

                max_score = max_score + insertion_score;
                //printf("%d\t", final_matrix[row_value][col_value]);
                row_value = row_value - 1;
                //std::cout <<"case3 " << max_score << endl;
                //std::cout <<"row and col is " << row_value << col_value <<  endl;

            }

            if ((row_value+1 ) == 0 || (col_value+1) == 0) break;
        }

    *dummy = max_score;

    /*printf("\nprinting final dp Matrix\n");
    for (int r = 0; r < query_length; r ++)
    {
        for (int s = 0; s < ref_length; s ++){

            printf("%d\t", dp_matrix[r][s]);
        }
        printf("\n");
    }*/

}





