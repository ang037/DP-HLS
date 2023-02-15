# DP-HLS: Dynamic Programming FPGA Acceleration using High-Level Synthesis

HLS Tool: Vitis HLS<br>
Device: <br>
Frequency: 250MHz <br>
Length of the reference sequence:<br> 64
Length of the query sequence:<br> 64
Number of Processing Elements (PE):<br> 16
Following is the table summarizing the resource usage with various set of sequence alignment algorithms:<br>

<table>  
<thead> 
<tr>
      <th rowspan="2">Sequence Alignment Algorithms</th>
      <th rowspan="2">Gap Penalty</th>
      <th rowspan="2">Banding</th>
      <th colspan='3'>Resource Utilization </th>
    </tr>
    <tr>
   <th>FF</th>
   <th>LUT</th> 
    <th>BRAM</th> 
    </tr>
  </thead>
  <tbody>
    <tr>
      <td rowspan="4">Smith Waterman</td>
      <td rowspan="2">Linear</td>
      <td><input type="text"/>Non Banded</td>
      <td><input type="text"/>1939</td>
	 <td><input type="text" />9067</td>
	  <td><input type="text" />16</td>
    </tr>
    <tr>
      <td><input type="text"/>Banded</td>
      <td><input type="text" /></td>
      <td><input type="text" /></td>
       <td><input type="text" /></td>
    </tr>
        <tr>
      <td rowspan="2">Affine</td>
      <td><input type="text" />Non Banded</td>
      <td><input type="text" /></td>
	 <td><input type="text" /></td>
	  <td><input type="text" /></td>
    </tr>
    <tr>
      <td><input type="text" />Banded</td>
      <td><input type="text" /></td>
      <td><input type="text" /></td>
       <td><input type="text" /></td>
    </tr>
     <tr>
      <td rowspan="2">Needleman Wunsch</td>
      <td>Linear</td>
      <td><input type="text" /></td>
      <td><input type="text" /></td>
	 <td><input type="text" /></td>
	  <td><input type="text" /></td>
    </tr>
    <tr>
      <td>Affine</td>
      <td><input type="text" /></td>
      <td><input type="text" /></td>
      <td><input type="text" /></td>
       <td><input type="text" /></td>
    </tr>
  </tbody>
</table>
