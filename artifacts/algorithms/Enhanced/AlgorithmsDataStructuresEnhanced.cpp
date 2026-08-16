#include "AlgorithmsDataStructuresEnhanced.hpp"

#include <algorithm>
#include <limits>
#include <utility>

namespace algorithms_enhancement
{

TypeRegistry::TypeRegistry()
    : known_types_{"int", "string", "double", "bool"}
{
}

bool TypeRegistry::add_type(std::string type_name)
{
    return known_types_.insert(std::move(type_name)).second;
}

bool TypeRegistry::is_type(const std::string& type_name) const
{
    return known_types_.find(type_name) != known_types_.end();
}

std::size_t TypeRegistry::size() const noexcept
{
    return known_types_.size();
}

bool update_at(
    std::vector<int>& values,
    std::size_t index,
    int new_value) noexcept
{
    if (index >= values.size())
    {
        return false;
    }

    values[index] = new_value;
    return true;
}

std::size_t remove_all(std::vector<int>& values, int target)
{
    const auto original_size = values.size();
    const auto new_end = std::remove(values.begin(), values.end(), target);
    values.erase(new_end, values.end());
    return original_size - values.size();
}

std::optional<std::uint64_t> sum_inclusive(
    std::uint64_t start,
    std::uint64_t end) noexcept
{
    if (end < start)
    {
        return std::nullopt;
    }

    const std::uint64_t distance = end - start;
    if (distance == std::numeric_limits<std::uint64_t>::max())
    {
        return std::nullopt;
    }
    std::uint64_t count = distance + 1U;

    if (start > std::numeric_limits<std::uint64_t>::max() - end)
    {
        return std::nullopt;
    }
    std::uint64_t endpoint_sum = start + end;

    // Divide before multiplying. In an arithmetic sequence, either the count
    // or the sum of the endpoints is even.
    if ((count % 2U) == 0U)
    {
        count /= 2U;
    }
    else
    {
        endpoint_sum /= 2U;
    }

    if (endpoint_sum != 0U &&
        count > std::numeric_limits<std::uint64_t>::max() / endpoint_sum)
    {
        return std::nullopt;
    }

    return count * endpoint_sum;
}

ChainAnalysis analyze_chain(const Token* head) noexcept
{
    if (head == nullptr)
    {
        return {};
    }

    const Token* slow = head;
    const Token* fast = head;
    const Token* meeting_point = nullptr;

    while (fast != nullptr && fast->next != nullptr)
    {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast)
        {
            meeting_point = slow;
            break;
        }
    }

    if (meeting_point == nullptr)
    {
        std::size_t count = 0;
        for (const Token* current = head; current != nullptr; current = current->next)
        {
            ++count;
        }
        return {ChainStatus::acyclic, count, std::nullopt};
    }

    const Token* entry_from_head = head;
    const Token* entry_from_meeting = meeting_point;
    std::size_t prefix_length = 0;
    while (entry_from_head != entry_from_meeting)
    {
        entry_from_head = entry_from_head->next;
        entry_from_meeting = entry_from_meeting->next;
        ++prefix_length;
    }

    std::size_t cycle_length = 1;
    for (const Token* current = entry_from_head->next;
         current != entry_from_head;
         current = current->next)
    {
        ++cycle_length;
    }

    return {
        ChainStatus::cycle_detected,
        prefix_length + cycle_length,
        entry_from_head->id};
}

} // namespace algorithms_enhancement
