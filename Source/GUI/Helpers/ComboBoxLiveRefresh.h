#pragma once

#include <functional>
#include <vector>

#include <juce_core/juce_core.h>

namespace TSS::ComboBoxLiveRefresh
{
    /** Pure decision for OS-driven combo refresh while a popup may be open. */
    enum class Action
    {
        kSkipRebuild,
        kRebuildOnly,
        kDismissRebuildReopen
    };

    [[nodiscard]] inline Action planRefresh(bool popupOpen, bool itemSetUnchanged) noexcept
    {
        if (itemSetUnchanged)
            return Action::kSkipRebuild;

        return popupOpen ? Action::kDismissRebuildReopen : Action::kRebuildOnly;
    }

    [[nodiscard]] inline bool identifiersEqual(const std::vector<juce::String>& left,
                                               const std::vector<juce::String>& right) noexcept
    {
        if (left.size() != right.size())
            return false;

        for (size_t i = 0; i < left.size(); ++i)
        {
            if (left[i] != right[i])
                return false;
        }

        return true;
    }

    /** Pure dismiss → rebuild → show sequence for an open popup; rebuild-only when closed. */
    inline void rebuildPreservingOpenPopup(bool wasOpen,
                                           const std::function<void()>& dismiss,
                                           const std::function<void()>& rebuild,
                                           const std::function<void()>& show)
    {
        if (wasOpen)
            dismiss();

        rebuild();

        if (wasOpen)
            show();
    }
}

#include "GUI/Widgets/ComboBox.h"

namespace TSS::ComboBoxLiveRefresh
{
    /** Dismiss an open popup before rebuild, then reopen when it was showing. */
    inline void rebuildPreservingOpenPopup(ComboBox& combo, const std::function<void()>& rebuild)
    {
        rebuildPreservingOpenPopup(
            combo.isPopupOpen(),
            [&combo]() { combo.dismissPopup(); },
            rebuild,
            [&combo]() { combo.showPopup(); });
    }
}
