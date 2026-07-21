#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    /** Scroll metrics + mutation for popup custom scrollbars (viewport or offset-backed). */
    class IPopupMenuScrollModel
    {
    public:
        virtual ~IPopupMenuScrollModel() = default;

        virtual int getContentHeight() const = 0;
        virtual int getViewportHeight() const = 0;
        virtual int getViewY() const = 0;
        virtual void setViewY(int y) = 0;
    };

    class ViewportPopupMenuScrollModel : public IPopupMenuScrollModel
    {
    public:
        explicit ViewportPopupMenuScrollModel(juce::Viewport& viewport);

        int getContentHeight() const override;
        int getViewportHeight() const override;
        int getViewY() const override;
        void setViewY(int y) override;

    private:
        juce::Viewport& viewport_;
    };

    class CallbackPopupMenuScrollModel : public IPopupMenuScrollModel
    {
    public:
        CallbackPopupMenuScrollModel(std::function<int()> getContentHeight,
                                     std::function<int()> getViewportHeight,
                                     std::function<int()> getViewY,
                                     std::function<void(int)> setViewY);

        int getContentHeight() const override;
        int getViewportHeight() const override;
        int getViewY() const override;
        void setViewY(int y) override;

    private:
        std::function<int()> getContentHeight_;
        std::function<int()> getViewportHeight_;
        std::function<int()> getViewY_;
        std::function<void(int)> setViewY_;
    };

    /** Shared popup scrollbar paint / drag / wheel — event-driven repaint only (no idle timer). */
    class PopupMenuCustomScrollBar : public juce::Component
    {
    public:
        PopupMenuCustomScrollBar(IPopupMenuScrollModel& model,
                                 const juce::Colour& scrollbarColour,
                                 float thumbInset,
                                 int minThumbHeightDesign,
                                 float uiScale);

        void paint(juce::Graphics& g) override;
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

    private:
        void scrollToMousePosition(int mouseY);
        int computeThumbHeight(int trackHeight, int contentHeight, int viewportHeight) const;

        inline constexpr static float kThumbHighlightBrighter_ = 0.2f;
        inline constexpr static float kWheelScrollFactorScrollbar_ = 0.5f;

        IPopupMenuScrollModel& model_;
        juce::Colour scrollbarColour_;
        float thumbInset_;
        int minThumbHeightDesign_;
        float uiScale_;
    };
}
