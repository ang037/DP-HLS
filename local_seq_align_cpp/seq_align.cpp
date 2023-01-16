#include <stdio.h>
#include <math.h>
#include "params.h"
#include <cstdlib>
#include <iostream>
#include "ap_int.h"

using namespace std;

void PE (char local_ref_val, char local_query_val, ap_int<16> up_prev, ap_int<16> left_prev, ap_int<16> diag_prev, ap_int<16> *score,
		ap_int<16> Ix_prev, ap_int<16> *Ix,
		ap_int<16> Iy_prev, ap_int<16> *Iy,
		ap_int<16> *traceback, ap_int<16> *final){

    //#pragma HLS inline

	ap_int<16> a1 = left_prev + opening_score;
	ap_int<16> a2 = Iy_prev + extend_score;
	ap_int<16> a3 = up_prev + opening_score;
	ap_int<16> a4 = Ix_prev + extend_score;

   /* *Iy = ((left_prev + opening_score) > (Iy_prev + extend_score)) ?
                (left_prev + opening_score) : (Iy_prev + extend_score);//d
    *Ix = ((up_prev + opening_score) > (Ix_prev + extend_score)) ?
                (up_prev + opening_score) : (Ix_prev + extend_score);//i*/

	*Iy = a1 > a2 ? a1 : a2;
	*Ix = a3 > a4 ? a3 : a4;

	//ap_int<16> d = opening_score  + left_prev;
	//ap_int<16> i = opening_score + up_prev;
    ap_int<16> temp = 0;

    //printf("d is %d, i is %d, mi is  %d, ma is %d, max is %d\n", d, i, mi, ma, max_value );

    //int max_diag_prev = (( (Iy_diag_prev > Ix_diag_prev) ? Iy_diag_prev : Ix_diag_prev ) > diag_prev ) ? ( ( Iy_diag_prev > Ix_diag_prev) ? Iy_diag_prev : Ix_diag_prev ) : diag_prev ;

    ap_int<16> match = (local_query_val == local_ref_val) ? diag_prev + match_score : diag_prev + mismatch_score;

    //printf("updated max val is %d\n", max_value);

    //ap_int<16> a5 = (*Iy > *Ix) ? *Iy : *Ix;

    // Write back results
   //ap_int<16> max_value = (( (*Iy > *Ix) ? *Iy : *Ix ) > match ) ? ( ( *Iy > *Ix) ? *Iy : *Ix ) : match ;
    ap_int<16> max_value = match;//(a5 > match ) ? a5 : match ;

    *traceback = (max_value == (diag_prev + match_score)) ? 11 : (max_value == *Iy)? 10 : (max_value == *Ix) ? 1 : 2;

    //ap_int<16> max_value = match;//(( (d > i) ? d : i ) > match ) ? ( ( d > i) ? d : i ) : match ;

       //*traceback = (max_value == (diag_prev + match_score)) ? 11 : (max_value == d)? 10 : (max_value == i) ? 1 : 2;

    *score = (max_value < temp) ? temp : max_value;

    *final = *score;

    //printf("local_ref_val is %d, local_query_val is %d, int up_prev is %d, int left_prev is %d, int diag_prev is %d, int *score is %d, Ix_prev is %d, int Ix_diag_prev is %d, int *Ix is %d, int Iy_prev is %d, int Iy_diag_prev is %d, int *Iy is %d, int *traceback_val is %d, int *final_value is %d \n", local_ref_val, local_query_val, up_prev, left_prev, diag_prev, *score, Ix_prev, Ix_diag_prev, *Ix, 
      //      Iy_prev, Iy_diag_prev, *Iy, *traceback_val, *final_value);

}

void seq_align (char query[query_length], char reference[ref_length], ap_int<16> *dummy)
{

    char local_query[query_length];
    //#pragma HLS ARRAY_PARTITION variable=local_query dim=0 complete
    char local_ref[ref_length];
    //#pragma HLS ARRAY_PARTITION variable=local_ref dim=0 complete
    ap_int<16> dp_mem[3][PE_num];
	#pragma HLS ARRAY_PARTITION variable=dp_mem dim=0 complete
    ap_int<16> Iy_mem[2][PE_num];
	#pragma HLS ARRAY_PARTITION variable=Iy_mem dim=0 complete
    ap_int<16> Ix_mem[2][PE_num];
	#pragma HLS ARRAY_PARTITION variable=Ix_mem dim=0 complete
    ap_int<16> dp_matrix[query_length][ref_length];
	//#pragma HLS ARRAY_PARTITION variable=dp_matrix dim=0 complete
    ap_int<16> tb[query_length][ref_length];
    //#pragma HLS ARRAY_PARTITION variable=traceback dim=0 complete
    ap_int<16> last_pe_score[ref_length];
//#pragma HLS ARRAY_PARTITION variable=last_pe_score dim=0 complete
    ap_int<16> last_pe_scoreIx[ref_length];
    ap_int<16> temp;

    printf("query sequence is:\n");

    local_query_loop: for (int k = 0; k < query_length; k ++){
        #pragma HLS PIPELINE II=1
        local_query[k] = query[k];
        //printf("%c\t ", local_query[k-1]);
    }

    printf("\nref sequence is:\n");

    local_ref_loop: for (int g = 0; g < ref_length; g ++){
        #pragma HLS PIPELINE II=1
        local_ref[g] = reference[g];
        //printf("%c\t ", local_ref[g-1]);
    }

    /*local_dpmem_loop: for (int gg = 0; gg < 3; gg ++){
        for (int ij = 0; ij < PE_num; ij++)
        {
            //#pragma HLS PIPELINE II=1
            dp_mem[gg][ij] = 0;
            //printf("%c\t ", local_ref[g]);

        }
    }

    local_Ixmem_loop: for (int mm = 0; mm < 2; mm ++){
        for (int nn = 0; nn < PE_num; nn++)
        {
            //#pragma HLS PIPELINE II=1
            Ix_mem[mm][nn] = 0;
            //printf("%c\t ", local_ref[g]);

        }
    }

    local_Iymem_loop: for (int pp = 0; pp < 2; pp ++){
        for (int rr = 0; rr < PE_num; rr ++)
        {
            //#pragma HLS PIPELINE II=1
            Iy_mem[pp][rr] = 0;
            //printf("%c\t ", local_ref[g]);

        }
    }*/
    int fix = 0;

   kernel: for(int qq = 0; qq < query_chunks; qq ++){

  // while (fix < 16){

    	//printf(" value of fix is %d", fix);

//#pragma HLS PIPELINE rewind

        kernel1: for (int ii = 0; ii < (PE_num + ref_length - 1); ii ++){

#pragma HLS PIPELINE II=1

        peloop:for(int kk = 0; kk < PE_num; kk ++){

		#pragma HLS UNROLL

        	if ((ii-kk) >= 0 && (ii-kk) < ref_length){

                //printf("\npe num is %d, chunk value is %d, iteration is %d\n", kk, qq, ii);

                if (kk == 0) {

                	if (fix == 0) {

                		PE(local_ref[ii-kk], local_query[kk+fix], 0, dp_mem[1][kk], 0, &dp_mem[2][kk], 0, &Ix_mem[1][kk], Iy_mem[0][kk], &Iy_mem[1][kk], &tb[kk+fix][ii-kk], &dp_matrix[kk+fix][ii-kk]);
                	}
                	else {

                		if (ii == 0) {

                			PE(local_ref[ii-kk], local_query[kk+fix], last_pe_score[0], 0, 0, &dp_mem[2][kk], last_pe_scoreIx[0], &Ix_mem[1][kk], 0, &Iy_mem[1][kk],&tb[kk+fix][ii-kk], &dp_matrix[kk+fix][ii-kk]);

                			temp = last_pe_score[0];
                		}
                		else if (ii < ref_length){

                			PE(local_ref[ii-kk], local_query[kk+fix], last_pe_score[ii], dp_mem[1][kk], temp /*last_pe_score[ii-1]*/, &dp_mem[2][kk], last_pe_scoreIx[ii], &Ix_mem[1][kk], Iy_mem[0][kk], &Iy_mem[1][kk], &tb[kk+fix][ii-kk], &dp_matrix[kk+fix][ii-kk]);

                			temp = last_pe_score[ii];
                		}
                	}
                }
                else {

                	PE(local_ref[ii-kk], local_query[kk+fix], dp_mem[1][kk-1], dp_mem[1][kk], dp_mem[0][kk-1], &dp_mem[2][kk], Ix_mem[0][kk-1], &Ix_mem[1][kk], Iy_mem[0][kk], &Iy_mem[1][kk], &tb[kk+fix][ii-kk], &dp_matrix[kk+fix][ii-kk]);
                }

            }

        	if (ii > PE_num - 2 && kk == PE_num -1) {

                last_pe_score[ii-PE_num+1] = dp_mem[2][PE_num-1];
                last_pe_scoreIx[ii-PE_num+1] = Ix_mem[1][PE_num-1];
            }

        }

        for (int ix = 0; ix < PE_num; ix++)
#pragma HLS UNROLL
        {
            dp_mem[0][ix] = dp_mem[1][ix];
            dp_mem[1][ix] = dp_mem[2][ix];
            Ix_mem[0][ix] = Ix_mem[1][ix];
            Iy_mem[0][ix] = Iy_mem[1][ix];
        }

      }

     fix = fix + PE_num;
  }

    ap_int<16> max_dp = 0;
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

    //printf("max row and col value is %d and %d \n", max_row_value, max_col_value );

    ap_int<16> max_score = dp_matrix[max_row_value][max_col_value];
    int col_value = max_col_value;
    int row_value = max_row_value;
    //printf("%d\t", max_score);


    traceback_logic: while (dp_matrix[row_value][col_value] > 0){
    
            if (tb[row_value][col_value] == 2){

                max_score = max_score + mismatch_score;
                //printf("%d\n", max_score);
                row_value = row_value - 1;
                col_value = col_value - 1;
            }

            else if (tb[row_value][col_value] == 11){

                max_score = max_score + match_score;
                //printf("%d\n", max_score);
                row_value = row_value - 1;
                col_value = col_value - 1;

            }
            else if (tb[row_value][col_value] == 10){

                max_score = max_score + opening_score;
                //printf("%d\n", max_score);
                col_value = col_value - 1;

            }
            else if (tb[row_value][col_value] == 1){

                max_score = max_score + extend_score;
                //printf("%d\n", max_score);
                row_value = row_value - 1;

            }

            if ((row_value+1 ) == 0 || (col_value+1) == 0) break;
    }

    //printf("max score is %d\n\n", max_score);

    *dummy = max_score;

    /*for (int r = 0; r < query_length; r ++)
    {
        for (int s = 0; s < ref_length; s ++){

            printf("%d\t", dp_matrix[r][s]);
        }
        printf("\n");
    }
*/
}



