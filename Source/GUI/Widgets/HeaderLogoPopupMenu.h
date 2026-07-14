#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"
#include "GUI/Layout/Design/DesignPanels.h"

namespace TSS
{
    class ISkin;
    class Logo;
    class PopupMenuRenderer;

    class HeaderLogoPopupMenu : public juce::Component
    {
    public:
        HeaderLogoPopupMenu(ISkin& skin,
                            float uiScale,
                            int currentSkinItemId,
                            int currentUiScaleId,
                            std::function<void(int skinItemId)> onSkinSelected,
                            std::function<void(int scaleId)> onUiScaleSelected,
                            std::function<void()> onAudioMidiSettingsRequested,
                            std::function<void()> onSettingsRequested,
                            std::function<void()> onAboutRequested);
        ~HeaderLogoPopupMenu() override;

        void paint(juce::Graphics& g) override;
        void mouseMove(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;
        void mouseExit(const juce::MouseEvent& e) override;
        bool keyPressed(const juce::KeyPress& key) override;
        void inputAttemptWhenModal() override;

        static void show(Logo& logo,
                         ISkin& skin,
                         float uiScale,
                         int currentSkinItemId,
                         int currentUiScaleId,
                         std::function<void(int skinItemId)> onSkinSelected,
                         std::function<void(int scaleId)> onUiScaleSelected,
                         std::function<void()> onAudioMidiSettingsRequested,
                         std::function<void()> onSettingsRequested,
                         std::function<void()> onAboutRequested);

    private:
        enum class ItemKind
        {
            SectionHeader,
            Skin,
            UiScale,
            Settings,
            AudioMidiDevices,
            About,
            Spacer,
            HorizontalRule
        };

        struct MenuItem
        {
            ItemKind kind = ItemKind::SectionHeader;
            int valueId = 0;
            juce::String text;
            int column = 0;
            int row = 0;
        };

        inline constexpr static float kBorderThicknessDesign_ = 1.0f;
        inline constexpr static float kHighlightGap_ = 1.0f;
        inline constexpr static int kColumnCount_ = 2;
        inline constexpr static int kColumnWidthDesign_ = TSS::Design::Panels::Header::kLogoPopupColumnWidth;

        float uiScale_ = 1.0f;
        int currentSkinItemId_ = 0;
        int currentUiScaleId_ = 0;
        int highlightedFlatIndex_ = -1;

        std::function<void(int skinItemId)> onSkinSelected_;
        std::function<void(int scaleId)> onUiScaleSelected_;
        std::function<void()> onAudioMidiSettingsRequested_;
        std::function<void()> onSettingsRequested_;
        std::function<void()> onAboutRequested_;
        bool showAudioMidiDevices_ = false;

        PopupMenuLook look_{};
        std::unique_ptr<PopupMenuRenderer> renderer_;
        juce::Font cachedFont_;
        std::vector<MenuItem> items_;

        void buildItems();
        int getItemHeightPx() const;
        float getSeparatorWidth() const;
        float getColumnWidth(int column) const;
        juce::Rectangle<float> getItemBounds(int flatIndex) const;
        int getFlatIndexAt(int x, int y) const;
        bool isSelectableItem(int flatIndex) const;
        bool isCurrentSelection(int flatIndex) const;
        void updateHighlightedItem(int flatIndex);
        void selectItem(int flatIndex);
        void closePopup();
        void drawItems(juce::Graphics& g, const juce::Rectangle<float>& contentBounds);
    };
}
