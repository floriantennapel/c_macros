# Comparison of tree insertion times in different languages

To test the performance of my search tree implementation, I have run some tests
comparing:
    * insertion of keys into tree
    * queries on tree
    * iteration over elements in tree
On a list of n = 10^7 randomly genereated 32 bit integers

Every benchmark was run 3 times and the best times were chosen per language
Ran on my laptop through WSL on an AMD 5500U processor

| Language/Datastructure | Insertion | Queries  | Element iteration | 
| ---------------------- | --------- | -------  | ----------------- |
| C / treemap.h (b-tree) |  4.801 s  |  5.632 s |  0.150 s          |
| C++ / std::set         | 18.171 s  | 18.208 s |  1.864 s          |
| Java / TreeSet         | 18.471 s  | 16.623 s |  0.494 s          |
| Rust / BTreeSet        |  4.943 s  |  5.464 s |  0.189 s          |

To my suprise C++'s std::set is really inefficient, and is beaten by Java.
Looking at the iteration speed, it seems like the source of the problems is terrible cache locality.
