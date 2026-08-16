#ifndef ALGORITHMS_DATA_STRUCTURES_ENHANCED_HPP
#define ALGORITHMS_DATA_STRUCTURES_ENHANCED_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace algorithms_enhancement
{

// Stores recognized type names for average O(1) membership lookup.
class TypeRegistry
{
public:
    TypeRegistry();

    [[nodiscard]] bool add_type(std::string type_name);
    [[nodiscard]] bool is_type(const std::string& type_name) const;
    [[nodiscard]] std::size_t size() const noexcept;

private:
    std::unordered_set<std::string> known_types_;
};

// Updates one vector element only when the requested index is valid. O(1).
[[nodiscard]] bool update_at(
    std::vector<int>& values,
    std::size_t index,
    int new_value) noexcept;

// Removes every matching value while preserving the order of retained items.
// Time complexity is O(n), with O(1) auxiliary space.
[[nodiscard]] std::size_t remove_all(std::vector<int>& values, int target);

// Computes an inclusive arithmetic-series sum in O(1) time. std::nullopt
// represents an invalid range or a result that cannot fit in uint64_t.
[[nodiscard]] std::optional<std::uint64_t> sum_inclusive(
    std::uint64_t start,
    std::uint64_t end) noexcept;

struct Token
{
    int id = 0;
    const Token* next = nullptr;
};

enum class ChainStatus
{
    empty,
    acyclic,
    cycle_detected
};

struct ChainAnalysis
{
    ChainStatus status = ChainStatus::empty;
    std::size_t unique_node_count = 0;
    std::optional<int> cycle_entry_id;
};

// Uses Floyd's cycle-detection algorithm to inspect a linked chain in O(n)
// time and O(1) auxiliary space.
[[nodiscard]] ChainAnalysis analyze_chain(const Token* head) noexcept;

} // namespace algorithms_enhancement

#endif
