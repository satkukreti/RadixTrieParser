This is a comparision between the performance difference of a radix trie and the standard hash table, std::map().

Input file is a string, indicated by "", and an integer. The purpose of the program is to lexicographically print each unique string and the highest value associated with it.
In my current implementation, the trie is >2x faster than std::map().

To create a test file, use make test. The LINES variable determines the lines associated with the test file.
Use make to create the standard (map) and optimized (trie) executables.
To clean everything, use make call.
