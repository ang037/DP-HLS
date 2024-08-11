<div align="center">

# DP-HLS: A High-Level Synthesis Framework for Accelerating Dynamic Programming Algorithms in Bioinformatics

<div align="center">

<img src="docs/docs/images/dp_hls_logo.png" style="height: 350px; width: auto;">

</div>

</div>

## Table of Contents
- [Overview](#overview)
- [Quick Install](#install)
- [Quick Start](#start)
- [Customize and Synthesize your own kernels](#usage)
- [Citing DP-HLS](#citation)

<br>

## <a name="overview"></a> Overview

Welcome to the official repository of DP-HLS, a framework designed to accelerate algorithms based on the 2-D Dynamic Programming paradigm using High-Level Synthesis (HLS). Built with the [AMD Xilinx Vitis HLS tool](https://docs.amd.com/r/en-US/ug1399-vitis-hls/Introduction), DP-HLS offers an easy-to-use template with extensive customizability, enabling users to create and implement FPGA-accelerated kernels tailored to their specific applications without requiring in-depth RTL design expertise. With the DP-HLS framework, you can develop efficient DP-based kernels that achieves performance comparable to its hand-coded RTL version, all within a development timeframe of just a few days, and deploy them on [Amazon EC2 F1 FPGA](https://aws.amazon.com/ec2/instance-types/f1/) instances.

**For more detailed information on all the features and settings of DP-HLS, please refer to our [Wiki](https://turakhialab.github.io/DP-HLS/).**

<br>

## <a name="install"></a> Quick Install
Clone the DP-HLS repository:
```
git clone --recursive https://github.com/DP-HLS
cd DP-HLS
```
<br>

## <a name="start"></a> Quick Start

<br>

## <a name="usage"></a> Customize and Synthesize your own kernels

Modify the parameters in `params.h` file as per your use-case before running our code. Currently our code generates random query and reference sequences as the inputs. You can configure this by reading the sequences from an input file in `seq_align_test.cpp`

Create a project in Xilinx Vitis HLS tool and select our code as source files. Select a device and clock frequency, then synthesize the code. Follow the steps for synthesizing the code from [their documentation](https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Using-Vitis-HLS).

<br>

## <a name="citation"></a> Citing DP-HLS

If you use DP-HLS in your research or publications, please cite the following paper:
