---
title: CLI Options
description: All of Dissolve's command-line options explained
---

| Switch (&lt;arg&gt;)  | CLI?   | GUI?   | Description |
|:----------------------|:-------|:-------|:------------|
|    -c                 |&#10004;|        | Check input and set-up only - don't perform any main-loop iterations |
|    -f &lt;file&gt;    |&#10004;|        | Redirect output from individual processes to `file.N`, where N is the process rank |
|    -h                 |&#10004;|&#10004;| Print available CLI options |
|    -i                 |&#10004;|&#10004;| Ignore restart file if present |
|    -I                 |        |&#10004;| Ignore GUI state file if present |
|    -m                 |&#10004;|        | Restrict output to be from the master process alone (parallel code only) |
|    -n &lt;n&gt;       |&#10004;|        | Run `n` main loop iterations, then stop |
|    -q                 |&#10004;|&#10004;| Quiet mode - print no output |
|    -i &lt;n&gt;       |&#10004;|&#10004;| Set restart file frequency to every `n` iterations |
|    -s                 |&#10004;|        | Perform single main loop iteration and then quit (equivalent to `-n 1`) |
|    -t &lt;file&gt;    |&#10004;|        | Load restart data from specified `file` (but still write to associated restart file) |
|    -v                 |&#10004;|&#10004;| Verbose mode - be a little more descriptive throughout |
|    -w &lt;file&gt;    |&#10004;|        | Write input to specified `file` after reading it, and then quit. Mostly used for testing consistency between read / written input files |
|    -x                 |&#10004;|&#10004;| Don't write restart or heartbeat files (but still read in the restart file if present) |

