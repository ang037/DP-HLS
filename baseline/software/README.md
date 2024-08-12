## Software Implementation and Baseline Evaluations

#### System Requirements
1. **gcc:** At least support for `C++ 17`, tested with `g++ 10.3`
2. **cmake:** `3.16.3`
3. **python3-pip**

#### Baseline Quick Start
```
git clone --recursive https://github.com/TurakhiaLab/DP-HLS
cd DP-HLS/baseline/software
```
Note that the `--recursive` option is needed to clone all the Seqan3 and minimap2 submodules.  

#### Build Instructions
To build Seqan3, run
```
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../source
make
```
To build Minimap2, run 
```
cd minimap2
make
```

#### Running the Baselines
```
./run_baseline [-kernels [local_linear] [local_affine] [global_linear] [global_affine]
					  [banded_local_linear] [two_piece_affine] [overlap_linear] [semiglobal_linear]]
```
Specifying `-kernels` is optional and doing so will run baselines only for the specified kernels. Otherwise, simply running `./run_baseline` will run all available baselines.

#### Baseline Tools
We have used [SeqAn3](https://github.com/seqan/seqan3) and [Minimap2](https://github.com/lh3/minimap2) as our baselines. Cloning the repository above will clone SeqAn3 and Minimap2 repositories.
