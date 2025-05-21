# Comparison of tree insertion times in different languages

I was initially suprised that my implementation was so much slower than in Java, 
however it turns out that C++'s implementation is also much slower than Java's

To avoid unwanted compiler optimiztions, random numbers are read from stdin, 
instead of some predictable number generation in code

All this IO is a big handicap to Java's speed, but it still holds up and is faster than my search tree and C++
Every benchmark was run 5 times and the best times were chosen per language

Ran on my laptop through WSL on an AMD 5500U processor

| Language/Datastructure | IO and insertion time | Element iteration | 
| ---------------------- | --------------------- | ----------------- |
| C / My own treemap.h   | 2.08 s                | 0.092 s           |
| Java / TreeSet         | 1.415 s               | 0.046 s           |
| C++ / Set              | 1.649 s               | 0.16 s            |
| Rust / BTreeSet        | 0.472 s               | 0.026 s           |
