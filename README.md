Timing Analyzer
Learning focus:
State machines, interrupts, timing, structs
In complex applications, different tasks are processed with different priorities and within certain timing constraints. It is important, that each interrupt is executed with correct timing, and that we also check the execution time, in order to keep an eye on the CPU load.
For simple applications the debugger works well – but as you already have learned is, there is no perfect synchronization of the CPU and the hardware when a breakpoint is reached. That means, that e.g. a hardware counter might go on counting a little bit while the CPU is already stopped. That means, that debugging in the presence of interrupts does not make sense.
But, how can we check, which interrupt preempts another interrupt and how long? Therefore, we mustn’t stop the system. We’re simply measuring the timing behavior. We have two options for that:
• Measure internally, by counting CPU cycles or using a timer.
• Measure externally, by attaching a logic analyzer.
Therefore, your task is to develop an easy to use API, which we can use for time measurement of any code regions in your code. Multiple analyzers must be working in parallel. This API will be used in the next lab experiments as well.