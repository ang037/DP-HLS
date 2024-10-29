`timescale 1ns / 1ps

module tb_BSW_ArrayTop();
  localparam test_cycles = 50000;
  parameter HalfClkPeriod = 2.5;
  
  localparam ClkPeriod = 2*HalfClkPeriod;
  reg [31:0] cycle;

  reg           clk;
  reg           rst;
  wire          done;
  reg          clear_done;
  reg start;
  
  wire [8:0] num_query_bases;
  wire [8:0] num_ref_bases;
  wire [17:0] num_tb_steps;
  wire [8:0] query_max_pos;
  wire       ready;
  wire [8:0] ref_max_pos;
  wire [15:0] req_id_out;
  wire [9:0]  tile_score;
  wire [1:0] dir;
  wire  dir_valid;
  wire [7:0] band_size = 64;
  wire [31:0] tile_id;
  wire [31:0] array_id;

  reg [7:0] ref_len;
  reg [7:0] query_len;

  reg [9:0] max_tb_steps;

  reg [9:0]  score_threshold;
  reg [207:0] in_params;

  reg [9:0] params [0:11];
  reg [7:0] align_fields;
  reg [15:0] req_id_in;

  reg set_param;
  wire [3:0] ref_addr;
  wire [3:0] query_addr;

  reg [127:0] ref_in = 128'h43414747434154474147434341435441435443435447545454545454414741474741544154414741544147414154474741544343544754475443434341544141544141415454414147474743414143545447544341434143434343545443434154414341414147414354474141544341474341474143414343414341474343414141544341474147474741414747415447474341544747474354544743545447475454414147434141434147414154414143414743414154414154414143415441414154415441415454474341415454544154474147545443545447545441545454474343414747545443544754414154544141544743434154434154544143;
  reg [127:0] query_in = 128'h41415441434354475454545454414741474754415441475441415441474147544147415447544743435443434341544141544141415441474747435441435454475441434141415441434343414343545443434141434141414747414343544141544341474341474143414341414741474343414141474341474147434741414747414154474341434154474747435454414743545447544141414743414141474147544141434147434141414141415443415441414154544141415454544343414154545441474754544341545454434154544743434147475441544347414154434141544747435447415441545441435441544354414354545454544754;
  reg ref_wr_en;
  reg query_wr_en;

  reg [5:0] ref_addr_in; 
  reg [5:0] query_addr_in;

  wire [511:0] tile_output; 

  // Generate Clock
  initial clk = 1;
  always #(HalfClkPeriod) clk = ~clk;

  BSW_Array #(
      .NUM_PE(32),
      .PE_WIDTH(16),
      .BLOCK_WIDTH(4),
      .MAX_TILE_SIZE(512),
      .LOG_MAX_TILE_SIZE(8)
  ) dut (
      .clk(clk),
      .rst(rst),
      .start(start),
      .ready(ready),
      .set_param(set_param),
      .done(done),
      .clear_done(clear_done),
      .in_params(in_params),
      .band_size(band_size),
      .align_fields(align_fields),
      .tile_id(tile_id),
      .array_id(array_id),
      .ref_wr_en(ref_wr_en),
      .query_wr_en(query_wr_en),
      .ref_addr(ref_addr),
      .query_addr(query_addr),
      .ref_in(ref_in),
      .query_in(query_in),
      .ref_len(ref_len),
      .query_len(query_len),
      .tile_output(tile_output)    
  );

  // Run simulation 
  initial begin
      $dumpfile("test.10.vcd");
      $dumpvars(0, dut); 
      cycle = 0;
      start = 0;
      set_param = 0;

      clear_done = 1;
      
      ref_len = 256;
      query_len = 256;
      ref_wr_en = 0;
      query_wr_en = 0;
      
      align_fields = (1<<5);
      align_fields[0] = 1;

      params[11] = 1;
      params[10] = -1;
      params[9] = -1;
      params[8] = -1;

      params[7] = 1;
      params[6] = -1;
      params[5] = -1;

      params[4] = 1;
      params[3] = -1;

      params[2] = 1;

      params[1] = -1;
      params[0] = -1;

      in_params = {params[11], params[10], params[9], params[8], params[7], params[6], params[5], params[4], params[3], params[2], params[1], params[0]};            
      $display("---- Performing Reset ----");
      rst = 1; 
      
      #(1*ClkPeriod); 
      rst = 0;
      
      #(1*ClkPeriod); 
      set_param = 1;

      #(1*ClkPeriod); 
      set_param = 0;
      
      #(1*ClkPeriod); 
      $display("---- Starting Simulation ----");
      start = 1;

      #(1*ClkPeriod); 
      start = 1;

      //#(1*ClkPeriod);
  end
  
  always @ (posedge clk) begin
      cycle <= cycle + 1;
      if (cycle > test_cycles) begin
	  $display("Cycle count: %d", cycle);
          $display("---- Done ----");
          $finish();
      end
  end

  always @ (posedge clk) begin
      if (dir_valid) begin
          $display("%d", dir);
      end
      if (done) begin
          $display("Cycle count: %d", cycle);
	  $display("---- Done with one alignment ----");
      end
  end

endmodule
