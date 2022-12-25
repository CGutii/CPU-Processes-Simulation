# CPU-Processes-Simulation
This repo holds three different projects: A cache storage simulator, a CPU scheduler, and a 2-bit GShare branch predictor.
Below are three (more)in-depth details of each of those projects.

Cache Storage Simulator:
This project simulates how cache works in a CPU. Inlcuding how it is accessed and how it is stored.
In specific, this project simulates the LRU and FIFO replacement policies.
Also known as Least Recently Used & First In First Out, respectively.
The code itself takes in the initial input of the size you want to give you cache,
its n-way associativity, the replacement policy you,the write back policy (0 = write-through , 1 = write-back),
and the trace file. The trace file contains only two things, an instruction and a hex address.
For example, these could be two lines found in the trace file:
R 0x2311257 
W 0x250076
Finally, the program outputs the total amount of reads and writes & the ratio of hits to misses.


CPU Scheduler:
The CPU scheduler is a simulator of how processes are executed inside of a computer. In a computer, processes occur all the time. 
Anytime a keystroke is pressed, a file is being saved, or an app is being open, just to name a few times a process can be started.
However, processes can not just happen instanteously or at the same as something else. For example, you cannot open two apps at the EXACT same time. 
Instead, the CPU takes the instruction or process of opening the applications and finishes them "usually" around the same time.
The way the CPU decides/organizes these processes and executions are through scheduling algorithms, which is what my code implements.
The algorithms implemented were SJF, FIFO, and Round-Robin. Also known as Shortest Job First, First Come First Serve, and RR respectively.
The first two algorithms are self-explanatory through their name. But round-robin is a bit more complicated. Round-Robin is more time-based
because every execeution gets an equal share of running/execution time. Also the time that each process runs is based on something called a time quantum,
which is essentially that time-limit. So no process can run longer than that time, but it can run for less if it were to finish for example.


Branch Predictor:
