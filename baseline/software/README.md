## Software Implementation and Baseline Evaluations

#### System Requirements
1. **gcc:** At least support for `C++ 17`, tested with `g++ 10.3`
2. **cmake:** `3.16.3`
3. **python3-pip**

#### Clone DP-HLS repository
```
git clone --recursive https://github.com/TurakhiaLab/DP-HLS.git
cd DP-HLS/baseline/software
```

#### Running the Baseline
```
./run_baseline
```

#### Baseline Tools
We have used [SeqAn3](https://github.com/seqan/seqan3) and [Minimap2](https://github.com/lh3/minimap2) as our baselines. Cloning the repository above will clone SeqAn3 and Minimap2 repositories.
