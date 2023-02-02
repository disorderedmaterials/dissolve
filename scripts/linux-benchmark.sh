#!/bin/sh

# Run this script before benchmarking on a linux CPU to prevent CPU
# frequency scaling that could invalidate the numbers.

if [ "$#" -ne 2 ] || ! [ -x "$1" ]; then
  echo "Usage: $0 BENCHMARK_EXECUTABLE OUTPUT_FILE" >&2
  exit 1
fi

echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor > /dev/null
echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo > /dev/null

$1 --benchmark_filter=Interpolator --benchmark_out_format=csv --benchmark_out=$2

echo powersave | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor > /dev/null
echo 0 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo > /dev/null
