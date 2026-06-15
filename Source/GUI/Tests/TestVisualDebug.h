#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/GroupLabel.h"
#include "GUI/Widgets/HorizontalSeparator.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/SectionHeader.h"
#include "GUI/Widgets/VerticalSeparator.h"

struct TestVisualDebugSettings
{
    bool gridEnabled = false;
    int gridCellSize = 4;
    juce::Colour gridColour = juce::Colours::red.withAlpha(0.5f);

    bool boundsEnabled = false;
    juce::Colour boundsColour = juce::Colours::cyan.withAlpha(0.5f);

    static TestVisualDebugSettings& get() noexcept;
};

namespace TestVisualDebugPainter
{
    inline constexpr juce::uint32 kWidgetDrawingAreaBackgroundColour = 0xFF222222;

    inline int widgetDrawingAreaTop()
    {
        return TestScaleColumns::kScaleLabelHeight + TestScaleColumns::kGap;
    }

    inline bool isScaleColumnLabel(const juce::Component& child, const juce::Component& parent)
    {
        if (dynamic_cast<const TSS::Label*>(&child) == nullptr)
            return false;

        return parent.getNumChildComponents() > 0 && parent.getChildComponent(0) == &child;
    }

    inline bool isTransparentDebugWidget(const juce::Component& child, const juce::Component& parent)
    {
        if (isScaleColumnLabel(child, parent))
            return false;

        return dynamic_cast<const TSS::Label*>(&child) != nullptr
            || dynamic_cast<const TSS::GroupLabel*>(&child) != nullptr
            || dynamic_cast<const TSS::HorizontalSeparator*>(&child) != nullptr
            || dynamic_cast<const TSS::VerticalSeparator*>(&child) != nullptr
            || dynamic_cast<const TSS::ModuleHeader*>(&child) != nullptr
            || dynamic_cast<const TSS::SectionHeader*>(&child) != nullptr;
    }

    inline juce::Rectangle<int> findWidgetDrawingArea(const juce::Component& contentHost)
    {
        const int areaTop = widgetDrawingAreaTop();

        for (int pageIndex = 0; pageIndex < contentHost.getNumChildComponents(); ++pageIndex)
        {
            auto* testPage = contentHost.getChildComponent(pageIndex);
            if (testPage == nullptr || !testPage->isVisible())
                continue;

            return { 0, areaTop, testPage->getWidth(), juce::jmax(0, testPage->getHeight() - areaTop) };
        }

        return contentHost.getLocalBounds().withTop(areaTop);
    }

    inline juce::Point<int> findFiftyPercentWidgetOrigin(const juce::Component& contentHost)
    {
        for (int pageIndex = 0; pageIndex < contentHost.getNumChildComponents(); ++pageIndex)
        {
            auto* testPage = contentHost.getChildComponent(pageIndex);
            if (testPage == nullptr || !testPage->isVisible())
                continue;

            for (int panelIndex = 0; panelIndex < testPage->getNumChildComponents(); ++panelIndex)
            {
                auto* panel = testPage->getChildComponent(panelIndex);
                if (panel == nullptr || panel->getX() != 0)
                    continue;

                if (panel->getNumChildComponents() < 2)
                    break;

                auto* firstWidget = panel->getChildComponent(1);
                if (firstWidget == nullptr)
                    break;

                return contentHost.getLocalArea(firstWidget, firstWidget->getLocalBounds()).getPosition();
            }

            break;
        }

        return { 0, widgetDrawingAreaTop() };
    }

    inline void paintWidgetDrawingAreaBackground(juce::Graphics& graphics, const juce::Component& contentHost)
    {
        graphics.setColour(juce::Colour(kWidgetDrawingAreaBackgroundColour));
        graphics.fillRect(findWidgetDrawingArea(contentHost));
    }

    inline void paintTransparentWidgetBounds(const juce::Component& rootHost,
                                             const juce::Component& parent,
                                             juce::Graphics& graphics,
                                             juce::Colour colour)
    {
        if (!parent.isVisible())
            return;

        for (int i = 0; i < parent.getNumChildComponents(); ++i)
        {
            auto* child = parent.getChildComponent(i);
            if (child == nullptr || !child->isVisible())
                continue;

            if (isTransparentDebugWidget(*child, parent))
            {
                graphics.setColour(colour);
                graphics.fillRect(rootHost.getLocalArea(child, child->getLocalBounds()));
            }

            paintTransparentWidgetBounds(rootHost, *child, graphics, colour);
        }
    }

    inline int positiveModulo(int value, int modulus)
    {
        const int remainder = value % modulus;
        return remainder < 0 ? remainder + modulus : remainder;
    }

    inline void paintPixelGrid(juce::Graphics& graphics,
                               juce::Rectangle<int> bounds,
                               juce::Point<int> origin,
                               int cellSize,
                               juce::Colour colour)
    {
        if (cellSize <= 0 || bounds.isEmpty())
            return;

        const auto gridArea = bounds.getIntersection(
            juce::Rectangle<int>(bounds.getX(), origin.y, bounds.getWidth(), bounds.getBottom() - origin.y));

        if (gridArea.isEmpty())
            return;

        const float alpha = colour.getFloatAlpha();
        const auto lineColour = colour.withAlpha(1.0f);

        juce::Image layer(juce::Image::ARGB, gridArea.getWidth(), gridArea.getHeight(), true);
        juce::Image::BitmapData pixels(layer, juce::Image::BitmapData::writeOnly);

        for (int localY = 0; localY < gridArea.getHeight(); ++localY)
        {
            const int worldY = gridArea.getY() + localY;

            for (int localX = 0; localX < gridArea.getWidth(); ++localX)
            {
                const int worldX = gridArea.getX() + localX;

                const bool onVertical = positiveModulo(worldX - origin.x, cellSize) == 0;
                const bool onHorizontal = positiveModulo(worldY - origin.y, cellSize) == 0;

                if (onVertical || onHorizontal)
                    pixels.setPixelColour(localX, localY, lineColour);
            }
        }

        graphics.saveState();
        graphics.setOpacity(alpha);
        graphics.drawImageAt(layer, gridArea.getX(), gridArea.getY());
        graphics.restoreState();
    }
}
