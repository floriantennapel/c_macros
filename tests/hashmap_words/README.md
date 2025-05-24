# Comparison of hashset insertion times in different languages

To test the performance of my hashmap implementation, I have run some tests
comparing:
- insertion of keys into hashset
- queries on tree
- iteration over elements in hashset
On a list of n = 3*10^6 most common english words: <https://github.com/ScriptSmith/topwords>

Every benchmark was run 5 times and the best times were chosen per language
Ran on my laptop through WSL on an AMD 5500U processor

| Language/Datastructure           | Insertion | Queries  | Element iteration | 
| -------------------------------- | --------- | -------  | ----------------- |
| Java / HashSet                   |  0.781 s  |  0.276 s |  0.199 s          |
| C / hashmap.h (SipHash, cached)  |  0.955 s  |  0.382 s |  0.137 s          |
| Rust / HashSet                   |  1.269 s  |  0.734 s |  0.051 s          |
| C++ / std::unordered_set         |  1.929 s  |  0.569 s |  0.328 s          |
| C / hashmap.h (Java Latin1 hash) |  2.078 s  |  0.541 s |  0.128 s          |
| C / hashmap.h (SipHash)          |  2.190 s  |  0.766 s |  0.136 s          |

# Takeaways
in the second test (SipHash, cached) and java, string are hashed when they are first read
This makes them much faster than the languages where hashes are recomputed every time
Java's hash function for string is very simple, this makes it quick to compute
but leads to higher number of collision than SipHash, in these specific tests it seems the simple java hash performed slightly better

The source code for the java hash is found here: <https://github.com/AdoptOpenJDK/openjdk-jdk11/blob/master/src/java.base/share/classes/java/lang/StringLatin1.java>
