#include "Tools/SkinDesigner/SkinDesignerAlertWindow.h"

#include <algorithm>
#include <set>
#include <vector>

namespace SkinDesignerChrome
{
    namespace
    {
        std::vector<int> suggestedButtonOrder(const juce::MessageBoxOptions& options)
        {
            const auto& buttons = options.getButtons();
            std::vector<int> order;
            order.reserve(static_cast<size_t>(options.getNumButtons()));
            for (int i = 0; i < options.getNumButtons(); ++i)
                order.push_back(i);

            for (int index = 0; index < options.getNumButtons(); ++index)
            {
                const auto& button = buttons[static_cast<size_t>(index)];
                if (button.isPreferredAction())
                {
                    order.erase(std::find(order.begin(), order.end(), index));
                    order.insert(order.begin(), index);
                }
                else if (button.getRole() == juce::MessageBoxButtonRole::cancel)
                {
                    order.erase(std::find(order.begin(), order.end(), index));
                    order.push_back(index);
                }
            }
            return order;
        }

        juce::KeyPress actionKeyForButton(const juce::MessageBoxOptions& options,
                                          const juce::MessageBoxButtonOptions& button,
                                          size_t visualIndex)
        {
            if (button.isPreferredAction()
                || (options.getNumButtons() == 2 && visualIndex == 0))
                return juce::KeyPress(juce::KeyPress::returnKey);

            if (button.getRole() == juce::MessageBoxButtonRole::cancel
                || static_cast<int>(visualIndex) == options.getNumButtons() - 1)
                return juce::KeyPress(juce::KeyPress::escapeKey);

            return {};
        }

        void addOrderedButtons(AlertWindow& alert, const juce::MessageBoxOptions& options)
        {
            const auto& buttons = options.getButtons();
            const auto order = suggestedButtonOrder(options);
            std::set<juce::juce_wchar> uniqueShortcuts;

            for (size_t visualIndex = 0; visualIndex < order.size(); ++visualIndex)
            {
                const auto& button = buttons[static_cast<size_t>(order[visualIndex])];
                const auto shortcut = juce::CharacterFunctions::toLowerCase(button.getText()[0]);
                const auto mnemonic = uniqueShortcuts.insert(shortcut).second
                                          ? juce::KeyPress(static_cast<int>(shortcut), 0, 0)
                                          : juce::KeyPress();
                alert.addButton(button.getText(),
                                button.getCode(),
                                mnemonic,
                                actionKeyForButton(options, button, visualIndex));
            }
        }
    }

    std::unique_ptr<juce::AlertWindow> makeAlertWindowForOptions(
        const juce::MessageBoxOptions& options)
    {
        auto aw = std::make_unique<AlertWindow>(options.getTitle(),
                                                options.getMessage(),
                                                options.getIconType(),
                                                options.getAssociatedComponent());
        aw->setEscapeKeyCancels(options.getNumButtons() == 0);

        if (options.getNumButtons() == 0)
            return aw;

        jassert(options.areAllResultCodesUnique());

        if (options.getNumButtons() == 1)
        {
            const auto& button = options.getButtons().front();
            aw->addButton(button.getText(),
                          button.getCode(),
                          juce::KeyPress(juce::KeyPress::escapeKey),
                          juce::KeyPress(juce::KeyPress::returnKey));
            return aw;
        }

        addOrderedButtons(*aw, options);
        return aw;
    }
}
