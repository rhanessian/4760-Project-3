# 4760-Project-3

This program will spawn children processes that write to a shared file. 
Their usage of the file is controlled with semaphores.

Note: page 1, step 5 lists default time before termination as 100s; page 4 lists default
time as 5 minutes. Thus, I used 100s as the default, as I did in project 2.

Compile using: make
Run using: ./master -t ss n
	where ss is number of seconds master runs and n is the number of child processes spawned

