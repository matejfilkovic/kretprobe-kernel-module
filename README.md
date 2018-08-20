# kretprobe-kernel-module

This is an example kernel module which shows the use of return probes to report
total time taken for a probed function to run. To measure time,
module configures and reads cycles directly from PMU unit. The sample
targets Cortex-A7 CPU which can be found in Raspberry PI 2. I made this example
a few years ago, and the sole purpose of it was to show how cumbersome is to configure
a PMU directly. To utilize PMU, perf_events subsystem should be used since it
abstracts an access to PMU. It also takes scheduling and overflows
into account. When used through perf command one can also attach
kprobe to desired kernel function.
