# CS 499 Milestone Two: Software Design and Engineering

**Student:** Parminder Singh  
**Original artifact:** `Original/QuestionableCode.cpp`  
**Enhanced artifact:** `Enhanced/SecureCodeReviewEnhanced.cpp`

## Artifact purpose

The original C++ artifact was created during earlier CS 405/CS 410 coursework to compare compiler feedback with static-analysis findings. It intentionally contains unsafe patterns, including endless recursion, a dangling pointer, unchecked array access, iterator invalidation, assertion side effects, and broken null traversal.

The enhanced artifact preserves the educational purpose of the original while redesigning the examples as maintainable, testable, and safer C++17 components.

## Software design and engineering improvements

- Organized the remediation examples into focused classes and functions with one clear responsibility.
- Replaced unsafe or unclear function contracts with explicit return values and validation.
- Used `std::optional`, `std::vector`, `std::set`, const-correct methods, and `[[nodiscard]]` annotations.
- Replaced assertion-dependent testing with a reusable runtime verification suite that works in debug and release builds.
- Added error handling for invalid ranges and safe handling for null inputs.
- Added comments that connect each design change to the original risk.

## Build and run

From the `Enhanced` directory:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror \
    SecureCodeReviewEnhanced.cpp -o secure_code_review
./secure_code_review
```

Optional sanitizer verification:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror \
    -fsanitize=address,undefined -fno-omit-frame-pointer \
    SecureCodeReviewEnhanced.cpp -o secure_code_review_sanitized
./secure_code_review_sanitized
```

The expected result is 13 passed checks and 0 failed checks.

## Files

- `Original/QuestionableCode.cpp`: unchanged original artifact.
- `Enhanced/SecureCodeReviewEnhanced.cpp`: completed software design and engineering enhancement.
- `Verification/build-and-test-results.txt`: compiler and runtime verification evidence.
