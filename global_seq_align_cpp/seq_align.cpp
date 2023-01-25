#include <stdio.h>
#include <math.h>
#include "params.h"
#include <cstdlib>
#include <iostream>
#include "ap_int.h"
#include "ap_fixed.h"

using namespace std;

//processing element
void PE (char local_ref_val, char local_query_val, ap_fixed<M,N> up_prev, ap_fixed<M,N> left_prev, ap_fixed<M,N> diag_prev, ap_fixed<M,N> *score,
		//ap_fixed<M,N> Ix_prev, ap_fixed<M,N> *Ix,
		//ap_fixed<M,N> Iy_prev, ap_fixed<M,N> *Iy,
		ap_fixed<M,N> *traceback, ap_fixed<M,N> *final){

//Calculating Iy and Ix matrix current score
    /**Iy = ((left_prev + opening_score) > (Iy_prev + extend_score)) ?
                (left_prev + opening_score) : (Iy_prev + extend_score);//d
    *Ix = ((up_prev + opening_score) > (Ix_prev + extend_score)) ?
                (up_prev + opening_score) : (Ix_prev + extend_score);//i*/

    ap_fixed<M,N> ma = diag_prev + match_score;//match score
    ap_fixed<M,N> mi = diag_prev + mismatch_score;//mismatch score
    ap_fixed<M,N> d = left_prev + opening_score;//match score
    ap_fixed<M,N> i = up_prev + opening_score;//mismatch score


    ap_fixed<M,N> match = (local_query_val == local_ref_val) ? ma : mi;//find if its a match or mismatch by comparing current query vs reference letter

   // Write back results
   // comparing max values between Ix, Iy and match/mismatch
    //ap_fixed<M,N> max_value = (( (*Iy > *Ix) ? *Iy : *Ix ) > match ) ? ( ( *Iy > *Ix) ? *Iy : *Ix ) : match ;

    ap_fixed<M,N> max_value = (( (d > i) ? d : i ) > match ) ? ( ( d > i) ? d : i ) : match ;


    //calculating traceback value
    //*traceback = (max_value == ma) ? 11 : (max_value == *Iy)? 10 : (max_value == *Ix) ? 1 : 2;
    *traceback = (max_value == mi) ? 11 : (max_value == d)? 10 : (max_value == i) ? 1 : 2;

    *score = max_value;

    //final score to be stored in final dp_matrix
    *final = *score;

}

void seq_align (char query[query_length], char reference[ref_length], ap_fixed<M,N> *dummy)
{

    char local_query[query_length];
    char local_ref[ref_length];

    //partioning dp_mem, Iy_mem and Ix_mem completely to remove all data dependencies
    //dp_mem has 3 rows - which stores current computed dp_matrix diagonal along with past two diagonal values
    //Ix_mem and Iy_mem has 2 rows - which stores current computed diagonal value of Ix and Iy matrix along with previous diagonal value
    ap_fixed<M,N> dp_mem[3][PE_num];
	#pragma HLS ARRAY_PARTITION variable=dp_mem dim=0 complete
    ap_fixed<M,N> Iy_mem[2][PE_num];
	#pragma HLS ARRAY_PARTITION variable=Iy_mem dim=0 complete
    ap_fixed<M,N> Ix_mem[2][PE_num];
	#pragma HLS ARRAY_PARTITION variable=Ix_mem dim=0 complete

    printf("reaching here\n");

    //initializing matrices
    ap_fixed<M,N> dp_matrix[query_length][ref_length];
    ap_fixed<M,N> tb[query_length][ref_length];

    //last_pe_score[] is used to transfer last PE scores of previous query blocks to next query blocks while computing dp_matrix values
    //last_pe_scoreIx[] is used to transfer last PE score across query blocks while computing Ix matrix
    ap_fixed<M,N> last_pe_score[ref_length];
    ap_fixed<M,N> last_pe_scoreIx[ref_length];
    ap_fixed<M,N> temp;

    //initializing DP matrix
    //dp_matrix[0][0] = 0;

   /* init_dp_matrix1: for (int jp = 1; jp < query_length+1; jp ++){

    		dp_matrix[jp][0] = jp*mismatch_score;
    }

    init_dp_matrix2: for (int mp = 1; mp < ref_length+1; mp ++){

    		dp_matrix[0][mp] = mp*mismatch_score;
    }*/

    //having local values of query and reference length
    local_query_loop: for (int k = 0; k < query_length; k ++){
        #pragma HLS PIPELINE II=1
        local_query[k] = query[k];
    }

    local_ref_loop: for (int g = 0; g < ref_length; g ++){
        #pragma HLS PIPELINE II=1
        local_ref[g] = reference[g];
    }

    local_dpmem_loop: for (int gg = 0; gg < 3; gg ++){
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
    }


    //dp_mem[1][0] = dp_matrix[1][0];
    //Ix_mem[1][0] = dp_matrix[1][0];
    //Iy_mem[1][0] = dp_matrix[1][0];

    int fix = 0;

    //last_pe_score array will store initialized value of NW algo
    for (int ip = 0; ip < ref_length; ip ++){

    	last_pe_score[ip] = (ip+1)*mismatch_score;
    }

//kernel computation
   kernel: for(int qq = 0; qq < query_chunks; qq ++){//loop for query blocking

        kernel1: for (int ii = 0; ii < (PE_num + ref_length - 1); ii ++){//loop for diagonals within a query block

#pragma HLS PIPELINE II=1

        peloop:for(int kk = 0; kk < PE_num; kk ++){//chain of PEs computing together

		#pragma HLS UNROLL

        	if ((ii-kk) >= 0 && (ii-kk) < ref_length){//we don't need all PEs for every diagonal

                if (kk == 0){

                	if (ii == 0){

                		PE(local_ref[ii-kk], local_query[kk+fix], last_pe_score[0], mismatch_score*(kk+fix+1), mismatch_score*(kk+fix), &dp_mem[2][kk], /*last_pe_scoreIx[0], &Ix_mem[1][kk], 0, &Iy_mem[1][kk],*/&tb[kk+fix][ii-kk], &dp_matrix[kk+fix][ii-kk]);

                		temp = last_pe_score[0];
                	}
                	else {

                    	PE(local_ref[ii-kk], local_query[kk+fix], last_pe_score[ii], dp_mem[1][kk], temp /*last_pe_score[ii-1]*/, &dp_mem[2][kk], /*last_pe_scoreIx[ii], &Ix_mem[1][kk], Iy_mem[0][kk], &Iy_mem[1][kk],*/ &tb[kk+fix][ii-kk], &dp_matrix[kk+fix][ii-kk]);

                    	//printf("iteration is %d, diag_prev is %d, up_prev is %d, left_prev is %d\n", ii, (int)temp, (int)last_pe_score[ii], (int)dp_mem[1][kk]);

                    	temp = last_pe_score[ii];//up_prev value will become diag_prev value for next diagonal computation

                	}
                }
                else {

                	if ((ii-kk) == 0){

                		PE(local_ref[0], local_query[kk+fix], dp_mem[1][kk-1], mismatch_score*(kk+fix+1), mismatch_score*(kk+fix), &dp_mem[2][kk], /*Ix_mem[0][kk-1], &Ix_mem[1][kk], Iy_mem[0][kk], &Iy_mem[1][kk],*/ &tb[kk+fix][ii-kk], &dp_matrix[kk+fix][ii-kk]);
                	}

                	else PE(local_ref[ii-kk], local_query[kk+fix], dp_mem[1][kk-1], dp_mem[1][kk], dp_mem[0][kk-1], &dp_mem[2][kk], /*Ix_mem[0][kk-1], &Ix_mem[1][kk], Iy_mem[0][kk], &Iy_mem[1][kk],*/ &tb[kk+fix][ii-kk], &dp_matrix[kk+fix][ii-kk]);
                	//if (ii == 12 && fix == 4) printf("local_ref[ii-kk]is %c, local_query[kk+fix] is %c, dp_mem[1][kk-1] is %d, dp_mem[1][kk] is %d, dp_mem[0][kk-1] is %d, &dp_mem[2][kk] is %d\n",(int)local_ref[ii-kk], (int)local_query[kk+fix], (int)dp_mem[1][kk-1], (int)dp_mem[1][kk], (int)dp_mem[0][kk-1], (int)dp_mem[2][kk]);
                }
        	}

        	if (ii > PE_num - 2 && kk == PE_num -1) {//for each query block, last PE scores for each diagonal will be saved and used by next query block

                last_pe_score[ii-PE_num+1] = dp_mem[2][PE_num-1];
                //last_pe_scoreIx[ii-PE_num+1] = Ix_mem[1][PE_num-1];
            }

        }

        for (int ix = 0; ix < PE_num; ix++)//each time, three diagonal values stored in dp_mem will be shifted one by one, recent one shifted in 2nd row, the older one will be shifted out from 0th row
#pragma HLS UNROLL
        {
            dp_mem[0][ix] = dp_mem[1][ix];
            dp_mem[1][ix] = dp_mem[2][ix];
            //Ix_mem[0][ix] = Ix_mem[1][ix];
            //Iy_mem[0][ix] = Iy_mem[1][ix];
        }

      }

     fix = fix + PE_num;//for query block iteration
  }

    //finding max score in dp_matrix for traceback computation
    ap_fixed<M,N> max_dp = 0;
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

	 //storing row and col of max_score for originating the traceback
    ap_fixed<M,N> max_score = dp_matrix[max_row_value][max_col_value];
    int col_value = max_col_value;
    int row_value = max_row_value;

    //traceback logic (checking till matrix score will become 0)
    traceback_logic: while (row_value >= 0 || col_value >= 0){
    
            if (tb[row_value][col_value] == 2){

                max_score = max_score + mismatch_score;
                //printf("%c ,%c \n",local_query[row_value], local_ref[col_value]);
                //printf("%d ,%d \n",row_value, col_value);
                row_value = row_value - 1;
                col_value = col_value - 1;
            }

            else if (tb[row_value][col_value] == 11){

                max_score = max_score + match_score;
                //printf("%c ,%c \n",local_query[row_value], local_ref[col_value]);
                //printf("%d ,%d \n",row_value, col_value);
                row_value = row_value - 1;
                col_value = col_value - 1;

            }
            else if (tb[row_value][col_value] == 10){

                max_score = max_score + opening_score;
                //printf("%c ,%c \n",local_query[row_value], local_ref[col_value]);
                //printf("%d ,%d \n",row_value, col_value);
                col_value = col_value - 1;

            }
            else if (tb[row_value][col_value] == 1){

                max_score = max_score + extend_score;
                //printf("%c ,%c \n",local_query[row_value], local_ref[col_value]);
                //printf("%d ,%d \n",row_value, col_value);
                row_value = row_value - 1;

            }

            if ((row_value+1 ) == 0 || (col_value+1) == 0) break;
    }

    *dummy = max_score;//final alignment score

    /*for (int ip = 0; ip < query_length; ip ++){

    	for (int kp = 0; kp < ref_length; kp ++){

    		printf("%d ", (int)dp_matrix[ip][kp]);
    	}
    	printf("\n");
    }

    printf("\n");

    for (int ip = 0; ip < query_length; ip ++){

    	for (int kp = 0; kp < ref_length; kp ++){

    		printf("%d ", (int)tb[ip][kp]);
    	}
    	printf("\n");
    }*/

}



