<div align="center">

# DP-HLS: A High-Level Synthesis Framework for Accelerating Dynamic Programming Algorithms in Bioinformatics

<div align="center">

<img src="docs/docs/images/dp_hls_logo.png" style="height: 350px; width: auto;">

</div>

</div>

## Table of Contents
- [Overview](#overview)
- [Getting Started](#start)
  - [Install dependencies](#install) 
  - [Customize your kernel](#custom)
  - [Synthesize your kernels](#synthesize)
  - [Test the implementation](#impl)
  - [Deploy the kernel on FPGA](#deploy)
- [Citing DP-HLS](#citation)

<br>

## <a name="overview"></a> Overview

Welcome to the official repository of DP-HLS, a framework designed to accelerate algorithms based on the 2-D Dynamic Programming paradigm using High-Level Synthesis (HLS). 

Built with the [AMD Xilinx Vitis HLS tool](https://docs.amd.com/r/en-US/ug1399-vitis-hls/Introduction), DP-HLS offers a user-friendly template with extensive customization options. This allows you to create and implement FPGA-accelerated kernels tailored to your specific needs, without the necessity of deep RTL design expertise.

With the DP-HLS framework, you can develop efficient DP-based kernels that achieves performance comparable to its hand-coded RTL version, all within a development timeframe of just a few days, and deploy them on [Amazon EC2 F1 FPGA](https://aws.amazon.com/ec2/instance-types/f1/) instances.

**For more detailed information on all the features and settings of DP-HLS, please refer to our [Wiki](https://turakhialab.github.io/DP-HLS/).**

<br>

## <a name="start"></a> Getting Started

To get started with DP-HLS, follow these steps:


### <a name="install"></a> Step 1: Install the required dependencies.

```
git clone --recursive https://github.com/DP-HLS
cd DP-HLS
```

For more details on installation, check this - [Installation Guide](https://turakhia.ucsd.edu/DP-HLS/#installation-guide).

### <a name="custom"></a> Step 2: Customize your own kernel.

Modify the parameters in `params.h` file as per your use-case before running our code. Currently our code generates random query and reference sequences as the inputs. You can configure this by reading the sequences from an input file in `seq_align_test.cpp`

Create a project in Xilinx Vitis HLS tool and select our code as source files. Select a device and clock frequency, then synthesize the code.

More details on customization of kernels can be found here - [Customize new kernels](https://turakhia.ucsd.edu/DP-HLS/#customize-new-kernels).

### <a name="synthesize"></a> Step 3: Synthesize your kernel.

For more details, check this - [Synthesize new kernels](https://turakhia.ucsd.edu/DP-HLS/#synthesize-new-kernels).

### <a name="impl"></a> Step 4: Test and verify your implementation.

For more details on each and every generated output files, check this - [Analyze Output](https://turakhia.ucsd.edu/DP-HLS/#analyze-output).

### <a name="deploy"></a> Step 5: Deploy your FPGA-accelerated kernel.

Check this out for more details - [Deploy new kernels to FPGA](https://turakhia.ucsd.edu/DP-HLS/#deploy-new-kernels-to-fpga).


**Happy coding with DP-HLS!**

<br>

## <a name="citation"></a> Citing DP-HLS

If you use DP-HLS in your research or publications, please cite the following paper:
