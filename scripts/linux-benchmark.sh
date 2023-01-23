#!/bin/sh

# Run this script before benchmarking on a linux CPU to prevent CPU
# frequency scaling that could invalidate the numbers.

echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor > /dev/null
echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo > /dev/null

out/build/Release/benchmarks --benchmark_filter=Interpolator --benchmark_out_format=csv --benchmark_out=original.csv

echo powersave | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor > /dev/null
echo 0 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo > /dev/null
