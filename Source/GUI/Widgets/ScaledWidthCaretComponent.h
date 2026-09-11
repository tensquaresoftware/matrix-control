#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    /** Caret that keeps JUCE vertical placement but paints width from a scale-aware thickness. */
    class ScaledWidthCaretComponent final : public juce::CaretComponent
    {
    public:
        using ThicknessFn = std::function<float()>;

        ScaledWidthCaretComponent(juce::Component* keyFocusOwner, ThicknessFn thickness)
            : juce::CaretComponent(keyFocusOwner)
            , thickness_(std::move(thickness))
        {
            setPaintingIsUnclipped(true);
        }

        void paint(juce::Graphics& g) override
        {
            g.setColour(findColour(caretColourId, true));

            const float thickness = thickness_ != nullptr
                                        ? juce::jmax(1.0f, thickness_())
                                        : 2.0f;
            const auto bounds = getLocalBounds().toFloat();
            const float x = bounds.getCentreX() - 0.5f * thickness;
            auto caret = juce::Rectangle<float>(x, bounds.getY(), thickness, bounds.getHeight());

            if (auto* parent = getParentComponent())
            {
                const auto parentClip = getLocalArea(parent, parent->getLocalBounds()).toFloat();
                caret = caret.getIntersection(parentClip);
            }

            if (! caret.isEmpty())
                g.fillRect(caret);
        }

    private:
        ThicknessFn thickness_;
    };
}
