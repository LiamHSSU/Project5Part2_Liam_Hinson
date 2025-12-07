# Project 5 Part 2

#Info: 
 Name: Liam Hinson
 Student ID: 008449409
 Repository Link: https://github.com/LiamHSSU/Project5Part2_Liam_Hinson

 Commands to run: 
 
cd build
make -j8 
cmake .. 
./LRUTraceGenerator [value] ../[wordfile].txt
./LRUHarness ../lruTraces/lru_profile_N_[value]_S_23.trace

*Notes* the .. before the file is a recursive call. To make sure that you can run trace generator, just put any .txt files in the base directory, the same one the CMakeLists.txt and main.cpp is in.

Sources: AI when I was having directory navigation issues when it came to compiling the project on blue, other than that, GitHub Desktop for big file upload and AI when it came to incorporating the 7 run average	 calculation program.

Method: Reused ideas from the batch the drain project, but had to incorporate new concepts and work around other constraints and formats. The main one was to have a warm up run before starting a for loop of 7 iterations to get a more accurate results, but not too many where there are diminishing returns in relation to overall runtime for the program's entire run.` 

Testing: Blue gave me a bunch of trouble, and downloading whole files back and forth was very troubling for me, other than that they all worked in the end, but testing the data was hard since trying to format it was almost impossible at some points.






 