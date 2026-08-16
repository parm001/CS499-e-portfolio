#include "AlgorithmsDataStructuresEnhanced.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace
{

[[nodiscard]] bool recursive_contains(
    const std::vector<std::string>& values,
    const std::string& target,
    std::size_t index = 0U)
{
    if (index >= values.size())
    {
        return false;
    }
    if (values[index] == target)
    {
        return true;
    }
    return recursive_contains(values, target, index + 1U);
}

[[nodiscard]] std::optional<std::uint64_t> iterative_sum_inclusive(
    std::uint64_t start,
    std::uint64_t end) noexcept
{
    if (end < start)
    {
        return std::nullopt;
    }

    // Volatile prevents the benchmark build from replacing this legacy loop
    // with the same closed-form calculation used by the enhanced function.
    volatile std::uint64_t total = 0U;
    for (std::uint64_t value = start;; ++value)
    {
        total += value;
        if (value == end)
        {
            break;
        }
    }
    return static_cast<std::uint64_t>(total);
}

[[nodiscard]] double milliseconds(std::chrono::nanoseconds duration)
{
    return static_cast<double>(duration.count()) / 1'000'000.0;
}

} // namespace

int main()
{
    using algorithms_enhancement::TypeRegistry;
    using algorithms_enhancement::sum_inclusive;
    using Clock = std::chrono::steady_clock;

    constexpr std::size_t type_count = 5'000U;
    constexpr std::size_t lookup_repetitions = 250U;
    const std::string target = "type_4999";

    std::vector<std::string> type_names;
    type_names.reserve(type_count);
    TypeRegistry registry;
    for (std::size_t index = 0; index < type_count; ++index)
    {
        std::string type_name = "type_" + std::to_string(index);
        type_names.push_back(type_name);
        static_cast<void>(registry.add_type(std::move(type_name)));
    }

    std::size_t recursive_hits = 0U;
    const auto recursive_start = Clock::now();
    for (std::size_t attempt = 0; attempt < lookup_repetitions; ++attempt)
    {
        recursive_hits += recursive_contains(type_names, target) ? 1U : 0U;
    }
    const auto recursive_duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - recursive_start);

    std::size_t hash_hits = 0U;
    const auto hash_start = Clock::now();
    for (std::size_t attempt = 0; attempt < lookup_repetitions; ++attempt)
    {
        hash_hits += registry.is_type(target) ? 1U : 0U;
    }
    const auto hash_duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - hash_start);

    constexpr std::uint64_t range_end = 50'000'000U;
    const auto iterative_start = Clock::now();
    const auto iterative_result = iterative_sum_inclusive(0U, range_end);
    const auto iterative_duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - iterative_start);

    constexpr std::size_t formula_repetitions = 100'000U;
    std::uint64_t formula_checksum = 0U;
    std::optional<std::uint64_t> formula_result;
    const auto formula_start = Clock::now();
    for (std::size_t attempt = 0; attempt < formula_repetitions; ++attempt)
    {
        formula_result = sum_inclusive(0U, range_end);
        formula_checksum += formula_result.value_or(0U) ^
            static_cast<std::uint64_t>(attempt);
    }
    const auto formula_duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - formula_start);

    if (recursive_hits != lookup_repetitions || hash_hits != lookup_repetitions ||
        !iterative_result.has_value() || !formula_result.has_value() ||
        iterative_result != formula_result || formula_checksum == 0U)
    {
        std::cerr << "Performance comparison validation failed.\n";
        return 1;
    }

    const double recursive_ms = milliseconds(recursive_duration);
    const double hash_ms = milliseconds(hash_duration);
    const double membership_speedup =
        hash_duration.count() == 0 ? 0.0 :
        static_cast<double>(recursive_duration.count()) /
            static_cast<double>(hash_duration.count());

    const double iterative_ms = milliseconds(iterative_duration);
    const double formula_average_ns =
        static_cast<double>(formula_duration.count()) /
        static_cast<double>(formula_repetitions);
    const double range_speedup = formula_average_ns == 0.0 ? 0.0 :
        static_cast<double>(iterative_duration.count()) / formula_average_ns;

    std::cout << std::fixed << std::setprecision(3)
              << "CS 499 large-input performance comparison\n\n"
              << "Membership lookup: " << type_count << " type names, "
              << lookup_repetitions << " searches for the last name\n"
              << "  Recursive linear search: " << recursive_ms << " ms\n"
              << "  Hash-based lookup:       " << hash_ms << " ms\n"
              << "  Observed speedup:        " << membership_speedup << "x\n\n"
              << "Inclusive range sum: 0 through " << range_end << '\n'
              << "  Loop-based calculation:  " << iterative_ms << " ms\n"
              << "  Formula average:          " << formula_average_ns << " ns per call\n"
              << "  Estimated speedup:        " << range_speedup << "x\n\n"
              << "Plain-language result: the hash lookup avoids checking thousands of\n"
              << "names one by one, and the formula calculates the range total without\n"
              << "visiting all 50,000,001 numbers.\n";

    return 0;
}
