#include "AlgorithmsDataStructuresEnhanced.hpp"

#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace
{

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

    void print_summary() const
    {
        std::cout << "\nVerification summary: " << passed_ << " passed, "
                  << failed_ << " failed.\n";
    }

    [[nodiscard]] int exit_code() const noexcept
    {
        return failed_ == 0 ? 0 : 1;
    }

private:
    int passed_ = 0;
    int failed_ = 0;
};

void verify_type_registry(VerificationSuite& suite)
{
    using algorithms_enhancement::TypeRegistry;

    TypeRegistry registry;
    suite.check(registry.is_type("int"), "known type is found by direct hash lookup");
    suite.check(!registry.is_type("decimal"), "unknown type is rejected");
    suite.check(registry.add_type("decimal") && registry.is_type("decimal"),
                "new type can be added and retrieved");
    suite.check(!registry.add_type("decimal") && registry.size() == 5U,
                "duplicate insertion is rejected without changing the set");
}

void verify_bounds_checked_update(VerificationSuite& suite)
{
    using algorithms_enhancement::update_at;

    std::vector<int> values{10, 20, 30};
    suite.check(update_at(values, 1U, 99) && values == std::vector<int>({10, 99, 30}),
                "valid vector index is updated in place");
    suite.check(!update_at(values, 3U, 77) && values == std::vector<int>({10, 99, 30}),
                "out-of-range index is rejected without mutation");

    std::vector<int> empty;
    suite.check(!update_at(empty, 0U, 1), "empty vector rejects every index");
}

void verify_erase_remove(VerificationSuite& suite)
{
    using algorithms_enhancement::remove_all;

    std::vector<int> values{2, 1, 2, 3, 2, 4};
    suite.check(remove_all(values, 2) == 3U &&
                    values == std::vector<int>({1, 3, 4}),
                "erase-remove deletes all matches and preserves retained order");

    const auto unchanged = values;
    suite.check(remove_all(values, 9) == 0U && values == unchanged,
                "removing a missing value leaves the vector unchanged");

    std::vector<int> all_matches{5, 5, 5};
    suite.check(remove_all(all_matches, 5) == 3U && all_matches.empty(),
                "erase-remove handles a vector containing only matches");
}

void verify_constant_time_sum(VerificationSuite& suite)
{
    using algorithms_enhancement::sum_inclusive;

    const auto common_range = sum_inclusive(0U, 999U);
    suite.check(common_range.has_value() && common_range.value() == 499500U,
                "constant-time range sum returns the expected value");

    const auto single_value = sum_inclusive(42U, 42U);
    suite.check(single_value.has_value() && single_value.value() == 42U,
                "single-value range is handled correctly");

    suite.check(!sum_inclusive(10U, 9U).has_value(),
                "reversed range is rejected");

    suite.check(!sum_inclusive(0U, std::numeric_limits<std::uint64_t>::max()).has_value(),
                "range whose count overflows is rejected");

    const auto max_pair = sum_inclusive(
        std::numeric_limits<std::uint64_t>::max() - 1U,
        std::numeric_limits<std::uint64_t>::max());
    suite.check(!max_pair.has_value(), "range whose endpoint sum overflows is rejected");
}

void verify_chain_analysis(VerificationSuite& suite)
{
    using algorithms_enhancement::ChainAnalysis;
    using algorithms_enhancement::ChainStatus;
    using algorithms_enhancement::Token;
    using algorithms_enhancement::analyze_chain;

    const ChainAnalysis empty = analyze_chain(nullptr);
    suite.check(empty.status == ChainStatus::empty && empty.unique_node_count == 0U,
                "null chain is reported as empty");

    Token third{3, nullptr};
    Token second{2, &third};
    Token first{1, &second};
    const ChainAnalysis acyclic = analyze_chain(&first);
    suite.check(acyclic.status == ChainStatus::acyclic &&
                    acyclic.unique_node_count == 3U &&
                    !acyclic.cycle_entry_id.has_value(),
                "acyclic chain is counted without false cycle detection");

    Token cycle_third{30, nullptr};
    Token cycle_second{20, &cycle_third};
    Token cycle_first{10, &cycle_second};
    cycle_third.next = &cycle_second;
    const ChainAnalysis cyclic = analyze_chain(&cycle_first);
    suite.check(cyclic.status == ChainStatus::cycle_detected &&
                    cyclic.unique_node_count == 3U &&
                    cyclic.cycle_entry_id == std::optional<int>(20),
                "cycle is detected and its entry node is identified");

    Token self_cycle{99, nullptr};
    self_cycle.next = &self_cycle;
    const ChainAnalysis self = analyze_chain(&self_cycle);
    suite.check(self.status == ChainStatus::cycle_detected &&
                    self.unique_node_count == 1U &&
                    self.cycle_entry_id == std::optional<int>(99),
                "single-node self-cycle is handled safely");
}

} // namespace

int main()
{
    std::cout << "CS 499 Algorithms and Data Structures Verification\n\n";

    VerificationSuite suite;
    verify_type_registry(suite);
    verify_bounds_checked_update(suite);
    verify_erase_remove(suite);
    verify_constant_time_sum(suite);
    verify_chain_analysis(suite);
    suite.print_summary();

    return suite.exit_code();
}
