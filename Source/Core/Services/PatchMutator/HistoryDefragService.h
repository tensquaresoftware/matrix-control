#pragma once

#include <utility>

namespace Core
{

    class MutationHistoryStore;

    struct HistoryDefragResult
    {
        bool success = false;
        int remappedRootIndex = -1;
        int remappedRetryIndex = -1;
    };

    class HistoryDefragService
    {
    public:
        static HistoryDefragResult defrag(MutationHistoryStore& store,
                                          std::pair<int, int> selectedIndices);
    };

} // namespace Core
