#include "AlgorithmsDataStructuresEnhanced.hpp"

#include <iostream>
#include <vector>

int main()
{
    using namespace algorithms_enhancement;

    TypeRegistry registry;
    const bool added = registry.add_type("decimal");

    std::vector<int> values{2, 1, 2, 3, 2, 4};
    const std::size_t removed = remove_all(values, 2);
    const auto total = sum_inclusive(1U, 100U);

    Token third{3, nullptr};
    Token second{2, &third};
    Token first{1, &second};
    const ChainAnalysis chain = analyze_chain(&first);

    std::cout << "CS 499 reusable algorithms demonstration\n"
              << "decimal registered: " << (added ? "yes" : "no") << '\n'
              << "values removed: " << removed << '\n'
              << "sum from 1 through 100: " << total.value_or(0U) << '\n'
              << "unique nodes in the chain: " << chain.unique_node_count << '\n';

    return 0;
}
