# primes
Counts primes using a bitblock array, multithreading, and outputs found primes
-------------------------------------------------------------------------------------------------
compile with:
make

run it at the command line with:

-t#: number of threads
-u#: the upper bound of primes to be found
-h: help for more details
-v: verbose mode which will print the primes

-------------------------------------------------------------------------------------------------
recommended command:
./primesMT -t20 -u10000000 > tenMillion.txt

fastest recorded for 1 billion was 9.86s using 30 threads

the output function could certainly be improved to use multithreading and only printing when verbose mode

generating primes is not as efficient as it could be
