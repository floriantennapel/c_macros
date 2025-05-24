# Comparison of hashset insertion times in different languages

To test the performance of my hashmap implementation, I have run some tests
comparing:
- insertion of keys into hashset
- queries on tree
- iteration over elements in hashset
On a list of n = 10^7 randomly genereated 32 bit integers

Every benchmark was run 3 times and the best times were chosen per language
Ran on my laptop through WSL on an AMD 5500U processor

For hashmap.h I tested both siphash and a simple identify hash (no hash)

| Language/Datastructure   | Insertion | Queries  | Element iteration | 
| ------------------------ | --------- | -------  | ----------------- |
| C / hashmap.h (SipHash)  |  1.995 s  |  1.701 s |  0.050 s          |
| C / hashmap.h (identity) |  0.963 s  |  0.593 s |  0.051 s          |
| C++ / std::unordered_set |  5.143 s  |  0.686 s |  0.943 s          |
| Java / HashSet           |  3.371 s  |  0.925 s |  0.678 s          |
| Rust / HashSet           |  0.967 s  |  1.294 s |  0.017 s          |

# Takeaways
 - C++ suprisingly comes in last, with very slow insertion, and iteration.
    I have not looked at the source code, but I speculate that it uses chaining on collisions
    and therefore deals with a lot of heap allocated linked list nodes, 
    causing terrible cache locality
 - Rust, supriingly has slower Queries than insertion, I have no idea why, but this is a little curious
 - It seems to be perfectly fine to use identity hashes on integers, clearly there were not many collisions
