#include <algorithm>
#include <cstddef>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace secure_review
{

// Replaces the original endless-recursion type check with a direct lookup.
class TypeRegistry
{
public:
    void add_type(int type)
    {
        known_types_.insert(type);
    }

    [[nodiscard]] bool is_type(int type) const
    {
        return known_types_.find(type) != known_types_.end();
    }

private:
    std::set<int> known_types_;
};

// Uses the compiler-generated copy operation so the complete state is copied.
class CopyableValue
{
public:
    explicit CopyableValue(int value) : value_(value) {}
    CopyableValue(const CopyableValue&) = default;
    CopyableValue& operator=(const CopyableValue&) = default;

    [[nodiscard]] int value() const noexcept
    {
        return value_;
    }

private:
    int value_;
};

// A noexcept function reports status instead of throwing and terminating the program.
class SafeOperation
{
public:
    [[nodiscard]] bool try_operation() const noexcept
    {
        return true;
    }
};

// Returning a value object avoids exposing a pointer to a destroyed local variable.
[[nodiscard]] std::optional<int> safe_local_value()
{
    return 1;
}

// Uses a vector and an explicit contract instead of unchecked fixed-array access.
[[nodiscard]] bool set_array_value(
    std::vector<int>& values,
    std::size_t index,
    int value)
{
    if (index >= values.size())
    {
        return false;
    }

    values[index] = value;
    return true;
}

[[nodiscard]] long long sum_range(int start, int end)
{
    if (end < start)
    {
        throw std::invalid_argument("end must be greater than or equal to start");
    }

    long long sum = 0;
    for (int value = start; value <= end; ++value)
    {
        sum += value;
    }

    return sum;
}

// The erase-remove idiom avoids invalidating an iterator that is still in use.
[[nodiscard]] std::size_t remove_value(std::vector<int>& items, int value)
{
    const auto original_size = items.size();
    items.erase(std::remove(items.begin(), items.end(), value), items.end());
    return original_size - items.size();
}

// The calculation is separate from verification, so behavior does not depend on assert.
[[nodiscard]] int compute_value() noexcept
{
    return 1 + 2;
}

struct Token
{
    Token* next_token = nullptr;

    [[nodiscard]] const Token* next() const noexcept
    {
        return next_token;
    }
};

// The loop condition explicitly checks for null and advances on every iteration.
[[nodiscard]] int count_tokens(const Token* token) noexcept
{
    int count = 0;
    while (token != nullptr)
    {
        ++count;
        token = token->next();
    }

    return count;
}

// Runtime verification is used instead of assertions so checks also run in release builds.
class VerificationSuite
{
public:
    void check(bool condition, std::string description)
    {
        if (condition)
        {
            ++passed_;
            std::cout << "[PASS] " << description << '\n';
        }
        else
        {
            ++failed_;
            std::cerr << "[FAIL] " << description << '\n';
        }
    }

    [[nodiscard]] int exit_code() const noexcept
    {
        return failed_ == 0 ? 0 : 1;
    }

    void print_summary() const
    {
        std::cout << "\nVerification summary: " << passed_ << " passed, "
                  << failed_ << " failed.\n";
    }

private:
    int passed_ = 0;
    int failed_ = 0;
};

void verify_type_registry(VerificationSuite& suite)
{
    TypeRegistry registry;
    registry.add_type(7);
    suite.check(registry.is_type(7), "registered type is found");
    suite.check(!registry.is_type(8), "unregistered type is rejected");
}

void verify_copy_semantics(VerificationSuite& suite)
{
    const CopyableValue original(42);
    const CopyableValue copy = original;
    suite.check(copy.value() == 42, "copy operation preserves object state");
}

void verify_noexcept_contract(VerificationSuite& suite)
{
    const SafeOperation operation;
    suite.check(operation.try_operation(), "noexcept operation reports success without throwing");
}

void verify_value_lifetime(VerificationSuite& suite)
{
    const auto local_value = safe_local_value();
    suite.check(local_value.has_value() && local_value.value() == 1,
                "returned value remains valid after the function exits");
}

void verify_bounds_handling(VerificationSuite& suite)
{
    std::vector<int> values(10, 0);
    suite.check(set_array_value(values, 3, 99) && values[3] == 99,
                "valid vector index is updated");
    suite.check(!set_array_value(values, 1000, 99),
                "out-of-range vector index is rejected");
}

void verify_algorithm_behavior(VerificationSuite& suite)
{
    suite.check(sum_range(0, 999) == 499500,
                "range-sum algorithm returns the expected result");

    bool rejected_invalid_range = false;
    try
    {
        static_cast<void>(sum_range(5, 4));
    }
    catch (const std::invalid_argument&)
    {
        rejected_invalid_range = true;
    }
    suite.check(rejected_invalid_range, "invalid range is rejected with a clear exception");
}

void verify_vector_removal(VerificationSuite& suite)
{
    std::vector<int> items{1, 2, 3, 2};
    const auto removed = remove_value(items, 2);
    suite.check(removed == 2 && items == std::vector<int>({1, 3}),
                "all matching vector values are removed safely");
}

void verify_computation(VerificationSuite& suite)
{
    const int result = compute_value();
    suite.check(result == 3, "calculation is independent of assertion side effects");
}

void verify_token_traversal(VerificationSuite& suite)
{
    Token first;
    Token second;
    first.next_token = &second;

    suite.check(count_tokens(&first) == 2, "linked tokens are counted correctly");
    suite.check(count_tokens(nullptr) == 0, "null token input is handled safely");
}

} // namespace secure_review

int main()
{
    using namespace secure_review;

    std::cout << "Secure Code Review Enhanced Artifact\n\n";

    VerificationSuite suite;
    verify_type_registry(suite);
    verify_copy_semantics(suite);
    verify_noexcept_contract(suite);
    verify_value_lifetime(suite);
    verify_bounds_handling(suite);
    verify_algorithm_behavior(suite);
    verify_vector_removal(suite);
    verify_computation(suite);
    verify_token_traversal(suite);
    suite.print_summary();

    return suite.exit_code();
}
