/**
 * @file seq_align.h
 * @author Yingqi Cao (yic033@ucsd.edu)
 * @brief This file contains codes for align functions.
 * @version 0.1
 * @date 2023-09-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef SEQ_ALIGN_H
#define SEQ_ALIGN_H

#include <ap_int.h>
// #include <hls_task.h>
#include <hls_vector.h>
#include <hls_stream.h>
// #include <hls_streamofblocks.h>

#include "params.h"
#include "PE.h"
#include "utils.h"
#include "frontend.h"

#ifdef CMAKEDEBUG
#include "./debug.h"
// #include "./pyapi.h"
#endif // DBEUG

using namespace hls;

namespace Align
{

	/**
	 * @brief Copy PE_NUM number of query values to the local
	 * query buffer.
	 *
	 * @param offset: Offset in the query to copy.
	 * @param len: Length to copy.
	 */
	void PrepareLocalQuery(
		char_t (&query)[MAX_QUERY_LENGTH],
		char_t (&local_query)[PE_NUM],
		idx_t offset,
		idx_t len);

	void DPMemUpdateBlock(
		hls::stream_of_blocks<dp_mem_block_t> &dp_mem_in,
		hls::stream_of_blocks<wavefront_scores_t> &score_in,
		hls::stream_of_blocks<dp_mem_block_t> &dp_mem_out);

	void DPMemUpdateArr(
		dp_mem_block_t &dp_mem_in,
		wavefront_scores_t &score_in);

	void InitializeChunkColScore(
		score_vec_t (&init_col_scr)[PE_NUM],
		stream_of_blocks<dp_mem_block_t> &dp_mem_out);

	/**
	 * @brief Prepare blocks of left, up, an diagonal scores for ExpandComputeBlock function.
	 *
	 * @param dp_mem_in: DP memory that holds the scores of previous two wavefronts.
	 * @param last_chunk_0: Last row scores from the previous chunk.
	 * @param last_chunk_1: Last row scores from the previous chunk backup.
	 * @param up_out: Blocks of up scores.
	 * @param diag_out: Blocks of diagonal scores.
	 * @param left_out: Blocks of left scores.
	 */
	void PrepareScoresBlock(
		hls::stream_of_blocks<dp_mem_block_t> &dp_mem_in,
		score_vec_t (&init_col_scr)[PE_NUM], int id,
		hls::stream_of_blocks<score_vec_t[2]> &last_chunk_scr,
		hls::stream_of_blocks<wavefront_scores_t> &up_out,
		hls::stream_of_blocks<wavefront_scores_t> &diag_out,
		hls::stream_of_blocks<wavefront_scores_t> &left_out,
		hls::stream_of_blocks<dp_mem_block_t> &dp_mem_out);

	void PrepareScoresArr(
		dp_mem_block_t &dp_mem_in,
		score_vec_t (&init_col_scr)[PE_NUM], int id,
		score_vec_t (&last_chunk_scr)[2],
		wavefront_scores_t &up_out,
		wavefront_scores_t &diag_out,
		wavefront_scores_t &left_out);

	/**
	 * @brief Compute the traceback pointers for a chunk of the size PE_NUM * REFERENCE_LENGTH.
	 *
	 * @param chunk_row_offset : The row offset in the whole traceback matrix the beginning of the chunk.
	 * @param query : Query sequence with length PE_NUM.
	 * @param init_col_scr : Initial score for the first column of this chunk.
	 * @param query_length : Length of the query < PE_NUM.
	 * @param reference_length : Length of the reference < MAX_REFERENCE_LENGTH.
	 * @param max : Score pack of the maximium score of this chunk.
	 */
	void ChunkCompute(
		idx_t chunk_row_offset,
		idx_t chunk_start_col,
		input_char_block_t &query,
		char_t (&reference)[MAX_REFERENCE_LENGTH],
		chunk_col_scores_inf_t &init_col_scr,
		hls::vector<type_t, N_LAYERS> (&init_row_scr)[MAX_REFERENCE_LENGTH],
		// hls::vector<idx_t, PE_NUM> &ics, hls::vector<idx_t, PE_NUM> &jcs,
		idx_t (&ics)[PE_NUM], idx_t (&jcs)[PE_NUM],
		idx_t (&p_cols)[PE_NUM], idx_t ck_idx,
		int global_query_length, int query_length, int reference_length,
		const Penalties &penalties,
		hls::vector<type_t, N_LAYERS> (&preserved_row_scr)[MAX_REFERENCE_LENGTH],
		ScorePack (&max)[PE_NUM], // write out so must pass by reference
		tbp_t (&chunk_tbp_out)[PE_NUM][MAX_QUERY_LENGTH / PE_NUM * MAX_REFERENCE_LENGTH]
#ifdef CMAKEDEBUG
		,
		Container &debugger
#endif
	);

	/**
	 * @brief Initialize two lists of coordinates, x coordinate and y coordinate, for each chunk.
	 * 		Theis function is called within the chunk compute function.
	 * 		This is especially useful if the chunk doesn't start at the beginning, i.e. after chunk
	 * 		column offset is developed.
	 * @param chunk_row_offset Initial Row of a Chunk
	 * @param chunk_col_offset Initial Column of a Chunk.
	 * @param ic Actual Global Coordinates for i.
	 * @param jc Actual Global Coordinates for j.
	 */
	void InitializeChunkCoordinates(idx_t chunk_row_offset, idx_t chunk_col_offset, hls::vector<idx_t, PE_NUM> &ic, hls::vector<idx_t, PE_NUM> &jc);

	/**
	 * @brief This function is used to setup the standard column initial coordinates
	 *
	 * @param jc
	 */
	void InitializeColumnCoordinates(idx_t (&jc)[PE_NUM]);

	/**
	 * @brief This function is used to initialize the initial row coordinates
	 *
	 * @param ic
	 */
	void InitializeRowCoordinates(idx_t (&ic)[PE_NUM]);

	void ArrangeScores(
		dp_mem_block_t &tbp_in,
		bool (&predicate)[PE_NUM], idx_t (&pe_offset)[PE_NUM],
		hls::vector<type_t, N_LAYERS> (*chunk_score_out)[MAX_REFERENCE_LENGTH]);

	void WriteInitialColScore(int i, score_vec_t (&init_scores)[PE_NUM],
							  hls::stream_of_blocks<dp_mem_block_t> &dp_mem_in,
							  hls::stream_of_blocks<dp_mem_block_t> &scores_out);


	/**
	 * @brief Arrange the traceback pointers of PE at the correct location in the traceback pointer matrix, based on the
	 * predicate and current pe_offset.
	 * @param tbp_in
	 * @param predicate
	 * @param pe_offset
	 * @param chunk_tbp_out
	 */
	void ArrangeTBP(
		const tbp_vec_t &tbp_in,
		const idx_t (&p_cols)[PE_NUM],
		const bool (&predicate)[PE_NUM],
		tbp_t (&chunk_tbp_out)[PE_NUM][MAX_QUERY_LENGTH / PE_NUM * MAX_REFERENCE_LENGTH]);

	/**
	 * @brief Determine the shift of predicate sccording to the desired query legnth, reference length, and the current
	 * wavefront index.
	 * @param predicate
	 * @param idx
	 * @param query_len
	 * @param reference_len
	 */
	void ShiftPredicate(bool (&predicate)[PE_NUM], int idx, int query_len, int reference_len);

	/**
	 * @brief Logics to map the global coordinates of a wavefront of PE to their prediate values.
	 * MapPredicateSquare is a function F: (pe_row: int, pe_col: int) -> (predicate_balue: boolean)
	 * It's unrolled for PE_NUM applying to each PE.
	 * @param ics Global Row Coordinates of a Wavefront of PE.
	 * @param jcs Global Column Coordinates of a Wavefront of PE.
	 * @param ref_len Actual Reference Length.
	 * @param predicate Predicate Array.
	 */
	void MapPredicateSquare(
		// hls::vector<idx_t, PE_NUM> &ics,
		// hls::vector<idx_t, PE_NUM> &jcs,
		idx_t (&ics)[PE_NUM], idx_t (&jcs)[PE_NUM],
		const idx_t ref_len,
		bool (&predicate)[PE_NUM]);

	/**
	 * @brief Predicate mapping function for banded alignment.
	 * FIXME: Add necessary parameter to determine whether a PE with
	 * index i and j is computing in the band.
	 * @param ref_len
	 */
	void MapPredicateBanded(
		int start_index, 
		int stop_index,
		idx_t chunk_row_offset,
		idx_t (&ics)[PE_NUM], idx_t (&jcs)[PE_NUM],
		idx_t (&col_lim_left)[PE_NUM], idx_t (&col_lim_right)[PE_NUM],
		const int query_len,
		const idx_t ref_len,
		bool (&predicate)[PE_NUM]);

	/**
	 * @brief Shift into the local reference a new reference element, given current wavefront index and reference length.
	 * @param local_reference
	 * @param reference
	 * @param idx
	 * @param ref_len
	 */
	void ShiftReferece(
		char_t (&local_reference)[PE_NUM], char_t (&reference)[MAX_REFERENCE_LENGTH],
		int idx, int ref_len);

	void PreserveRowScore(
		hls::vector<type_t, N_LAYERS> (&preserved_row_scr)[MAX_REFERENCE_LENGTH],
		const score_vec_t score_vec,
		const bool predicate_pe_last,
		const idx_t idx);

	template <typename T, int LEN>
	void CoordinateArrayCopy(T (&src)[LEN], T (&dst)[LEN])
	{
		for (int i = 0; i < LEN; i++)
		{
#pragma HLS unroll
			dst[i] = src[i];
		}
	}

	template <int LEN>
	void CoordinateInitializeUniformReverse(idx_t (&jcs)[LEN], idx_t starting)
	{
		for (size_t i = 0; i < LEN; i++)
		{
#pragma HLS unroll
			jcs[i] = starting - i;
		}
	}

	template <int LEN>
	void CoordinateInitializeUniform(idx_t (&jcs)[LEN], idx_t starting)
	{
		for (size_t i = 0; i < LEN; i++)
		{
#pragma HLS unroll
			jcs[i] = starting + i;
		}
	}

	template <int LEN>
	void CoordinateInitializeEquals(idx_t (&ics)[LEN], idx_t index)
	{
		for (size_t i = 0; i < LEN; i++)
		{
#pragma HLS unroll
			ics[i] = index;
		}
	}

	template <int LEN>
	void CoordinateArrayOffset(idx_t (&arr)[LEN])
	{
#pragma HLS inline off
		for (int i = 0; i < LEN; i++)
		{
#pragma HLS unroll
			arr[i]++;
		}
	}

	template <int LEN, int NUM>
	void CoordinateArrayOffsetGeneric(idx_t (&arr)[LEN])
	{
#pragma HLS inline off
		for (int i = 0; i < LEN; i++)
		{
#pragma HLS unroll
			arr[i] += NUM;
		}
	}

	void ArrangeSingleTBP(
		const idx_t i, const idx_t j, const bool pred, const tbp_t tbp_in,
		tbp_t (&chunk_tbp_out)[MAX_QUERY_LENGTH][MAX_REFERENCE_LENGTH]);

	/**
	 * @brief Prepare dp_mem at the beginning of a cycle of chunk compute.
	 *
	 * @param dp_mem
	 * @param i
	 * @param init_col_scr
	 * @param init_row_scr
	 */
	void UpdateDPMem(dp_mem_block_t &dp_mem, idx_t i, chunk_col_scores_inf_t &init_col_scr, score_vec_t (&init_row_scr)[MAX_REFERENCE_LENGTH]);
	void UpdateDPMemShift(dp_mem_block_t &dp_mem);
	void UpdateDPMemSet(dp_mem_block_t &dp_mem, idx_t i, chunk_col_scores_inf_t &init_col_scr, score_vec_t (&init_row_scr)[MAX_REFERENCE_LENGTH]);

	/**
	 * Namespace related to functions used to find the maximum elements in
	 * in the score matrix.
	 */
	namespace FindMax
	{
		/**
		 * @brief Extract a layer of a score array.
		 *
		 * @param scores Score matrices.
		 * @param layer Layer in which the score will be extracted.
		 * @param extracted Container for extracted scores.
		 */
		void ExtractScoresLayer(wavefront_scores_t &scores, idx_t layer, type_t (&extracted)[PE_NUM]);

		/**
		 * @brief Extract the maximum score from PE's local maximums
		 * @param max Array of PE's local maximum.
		 * @param chunk_max The maximum of the chunk.
		 */
		void ReductionMaxScores(ScorePack (&max)[PE_NUM], ScorePack &chunk_max);

	};

	/**
	 * @brief Perform Pairwise alignment for two sequences.
	 *
	 * @param query: Query sequence buffer.
	 * @param reference: Reference sequence buffer.
	 * @param query_length: Length of the query.
	 * @param reference_length: Length of the reference.
	 * @param tb_streams: Output traceback path.
	 */
	void AlignStatic(
		char_t (&querys)[MAX_QUERY_LENGTH],
		char_t (&references)[MAX_REFERENCE_LENGTH],
		idx_t query_length,
		idx_t reference_length,
		const Penalties &penalties,
		idx_t &tb_i, idx_t &tb_j,
		tbr_t (&tb_out)[MAX_REFERENCE_LENGTH + MAX_QUERY_LENGTH]
#ifdef CMAKEDEBUG
		,
		Container &debugger
#endif
	);

	/**
	 * @brief Initialize initial scores for the first column and
	 * row on the device.
	 *
	 * @param init_col_scr: Initial scores for the first column.
	 * @param init_row_scr: Initial scores for the first row.
	 */
	void InitializeScores(
		score_vec_t (&init_col_scr)[MAX_QUERY_LENGTH],
		score_vec_t (&init_row_scr)[MAX_REFERENCE_LENGTH],
		const Penalties &penalties);

	void ChunkMax(ScorePack &max, ScorePack new_scr);

	void UpdatePEOffset(idx_t (&pe_offset)[PE_NUM], bool (&predicate)[PE_NUM]);

	void CopyColScore(chunk_col_scores_inf_t &init_col_scr_local, score_vec_t (&init_col_scr)[MAX_QUERY_LENGTH], idx_t i);

	void UpdateDPMemSep(
		score_vec_t (&dp_mem)[PE_NUM + 1][2],
		score_vec_t (&score_in)[PE_NUM + 1]);

	void PrepareScoreBuffer(
		score_vec_t (&score_buff)[PE_NUM + 1],
		int i,
		chunk_col_scores_inf_t(&init_col_scr),
		score_vec_t (&init_row_scr)[MAX_REFERENCE_LENGTH]);

}

#endif // !SEQ_ALIGN_H
