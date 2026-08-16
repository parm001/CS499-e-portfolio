# CS 499 Milestone Three: Algorithms and Data Structures

**Student:** Parminder Singh  
**Original artifact:** `Original/QuestionableCode.cpp`  
**Enhanced artifact:** reusable C++17 components in `Enhanced/`

## Artifact purpose

The original C++ artifact was created during earlier CS 405/CS 410 coursework as a static-analysis exercise. It intentionally demonstrates unsafe recursion, unchecked array access, iterator invalidation, and broken pointer traversal. The original file is included unchanged for comparison and should not be treated as production-ready code.

The enhanced artifact focuses specifically on algorithms and data structures. It replaces the risky examples with appropriate standard-library containers, explicit contracts, and complexity-conscious algorithms. The reusable implementation, demonstration program, verification suite, and performance comparison are separated so application code does not produce test output.

## Enhancement summary

| Original issue | Enhancement | Time complexity | Auxiliary space |
|---|---|---:|---:|
| Recursive type membership check | `std::unordered_set` direct lookup | Average O(1), worst O(n) | O(n) |
| Unchecked fixed-array write | Bounds-checked `std::vector` update | O(1) | O(1) |
| Iterator invalidation during erase | Erase-remove idiom | O(n) | O(1) |
| Loop-based inclusive range sum | Arithmetic-series formula with overflow checks | O(1) | O(1) |
| Null-unsafe or endless token traversal | Floyd cycle detection plus unique-node count | O(n) | O(1) |

## Design trade-offs

- `std::unordered_set` improves average membership lookup to O(1), but it does not preserve sorted order and has a theoretical O(n) worst case when many values collide.
- The erase-remove idiom makes one linear pass and preserves the relative order of retained values, but it does not reduce vector capacity.
- The arithmetic-series formula avoids an O(n) loop, but it requires explicit overflow checks before addition and multiplication.
- Floyd's cycle-detection algorithm uses constant auxiliary space. A visited-node set could simplify counting and provide all visited addresses, but it would require O(n) extra memory.

## Instructor feedback incorporated

The final revision separates the artifact into clear responsibilities:

- `Enhanced/AlgorithmsDataStructuresEnhanced.hpp` exposes the reusable API.
- `Enhanced/AlgorithmsDataStructuresEnhanced.cpp` implements the algorithms without a `main` function or test output.
- `Enhanced/AlgorithmsDemo.cpp` demonstrates ordinary use of the reusable API.
- `Verification/AlgorithmsDataStructuresTests.cpp` contains all 19 verification checks and pass/fail reporting.
- `Verification/PerformanceComparison.cpp` contains the large-input timing comparison.

This structure allows another application to compile and link the algorithms without including the demonstration program or verification output.

## Large-input performance comparison

A representative run used 5,000 type names and repeated a lookup for the last name 250 times. It also calculated the inclusive sum from 0 through 50,000,000. Results vary by computer, but this run made the growth difference visible:

| Comparison | Earlier approach | Enhanced approach | Observed improvement |
|---|---:|---:|---:|
| 250 membership lookups | Recursive linear search: 3.095 ms | Hash lookup: 0.002 ms | About 1,602x faster |
| Sum 0 through 50,000,000 | Loop: 27.836 ms | Formula: 8.393 ns per call | About 3.32 million times faster |

In plain language, the hash table avoids checking thousands of names one at a time, and the arithmetic-series formula calculates the total without visiting all 50,000,001 values.

## Build and run

From the project root, build and run the demonstration program:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -IEnhanced \
    Enhanced/AlgorithmsDataStructuresEnhanced.cpp \
    Enhanced/AlgorithmsDemo.cpp -o algorithms_demo
./algorithms_demo
```

Build and run the separate verification suite:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -IEnhanced \
    Enhanced/AlgorithmsDataStructuresEnhanced.cpp \
    Verification/AlgorithmsDataStructuresTests.cpp -o algorithms_tests
./algorithms_tests
```

The expected result is **19 passed checks and 0 failed checks**.

Build and run the performance comparison:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror -IEnhanced \
    Enhanced/AlgorithmsDataStructuresEnhanced.cpp \
    Verification/PerformanceComparison.cpp -o performance_comparison
./performance_comparison
```

Optional sanitizer verification:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror \
    -fsanitize=address,undefined -fno-omit-frame-pointer -IEnhanced \
    Enhanced/AlgorithmsDataStructuresEnhanced.cpp \
    Verification/AlgorithmsDataStructuresTests.cpp -o algorithms_tests_sanitized
./algorithms_tests_sanitized
```

## Submission contents

- `Original/QuestionableCode.cpp`: unchanged original artifact.
- `Enhanced/AlgorithmsDataStructuresEnhanced.hpp`: reusable public interface.
- `Enhanced/AlgorithmsDataStructuresEnhanced.cpp`: reusable algorithm implementation.
- `Enhanced/AlgorithmsDemo.cpp`: demonstration program without verification output.
- `Verification/AlgorithmsDataStructuresTests.cpp`: separate 19-check verification suite.
- `Verification/PerformanceComparison.cpp`: large-input benchmark requested in instructor feedback.
- `Verification/build-and-test-results.txt`: compiler, runtime, sanitizer, and performance evidence.
- `README.md`: enhancement map, complexity analysis, feedback response, and build instructions.
