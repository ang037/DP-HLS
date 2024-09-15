## Software Implementation and Baseline Evaluations

#### System Requirements
1. **gcc:** At least support for `C++ 20`, tested with `g++ 10.5.0` (`g++ >= 11` highly recommended)
2. **cmake:** >= `3.5`
3. **sdsl:** >= `3.0.3`

#### Baseline Quick Start
```
git clone --recursive https://github.com/TurakhiaLab/DP-HLS
cd DP-HLS/baseline/software
```
Note that the `--recursive` option is needed to clone all the Seqan3 and minimap2 submodules.  

#### Build Instructions
SeqAn3 is compiled internally by the `run_baseline.sh` script. To build Minimap2, run 
```
cd minimap2
make
```

#### Running the Baselines
```
./run_baseline [-t [num_threads] -kernels [local_linear] [local_affine] [global_linear] [global_affine]
					  [banded_local_linear] [two_piece_affine] [overlap_linear] [semig_lobal_linear]]
```
Specifying `-kernels` is optional and doing so will run baselines only for the specified kernels. Otherwise, simply running `./run_baseline` will run all available baselines. Specifying `-t` will run the program with `num_threads` threads (8 by default).

#### Baseline Tools
We have used [SeqAn3](https://github.com/seqan/seqan3) and [Minimap2](https://github.com/lh3/minimap2) as our baselines. Cloning the repository above will clone SeqAn3 and Minimap2 repositories.
