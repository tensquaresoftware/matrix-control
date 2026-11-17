#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"
#include "GUI/Layout/Design/DesignPanels.h"
#include "GUI/Skins/Skin.h"

namespace TSS
{
    class ISkin;
    class Logo;
    class PopupMenuRenderer;
    class ContextualHelpBinder;
    class PopupMenuOutsideDismissWatcher;

    class HeaderLogoPopupMenu : public juce::Component
    {
    public:
        struct Config
        {
            float uiScale = 1.0f;
            int currentSkinItemId = 0;
            int currentUiScaleId = 0;
            ContextualHelpBinder* contextualHelpBinder = nullptr;
            std::function<void(int skinItemId)> onSkinSelected;
            std::function<void(int scaleId)> onUiScaleSelected;
            std::function<void()> onSettingsRequested;
            std::function<void()> onAboutRequested;
        };

        HeaderLogoPopupMenu(ISkin& skin, Config config);
        ~HeaderLogoPopupMenu() override;

        void paint(juce::Graphics& g) override;
        void resized() override;
        bool hitTest(int x, int y) override;
        void mouseMove(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;
        void mouseExit(const juce::MouseEvent& e) override;
        bool keyPressed(const juce::KeyPress& key) override;
        void inputAttemptWhenModal() override;

        void detachContextualHelpBinder();

        static void show(Logo& logo, ISkin& skin, Config config);

    private:
        enum class ItemKind
        {
            SectionHeader,
            SkinFamily,
            UiScale,
            Settings,
            About,
            Spacer,
            HorizontalRule
        };

        struct MenuItem
        {
            ItemKind kind = ItemKind::SectionHeader;
            int valueId = 0;
            juce::String text;
            juce::String shortcut;
            int column = 0;
            int row = 0;
            bool enabled = true;
        };

        struct FamilyMember
        {
            int skinItemId = 0;
            juce::String text;
            juce::String shortcut;
        };

        class FamilyFlyout : public juce::Component
        {
        public:
            FamilyFlyout(HeaderLogoPopupMenu& owner,
                         Skin::SkinFamily family,
                         std::vector<FamilyMember> members);

            void paint(juce::Graphics& g) override;
            void mouseMove(const juce::MouseEvent& e) override;
            void mouseUp(const juce::MouseEvent& e) override;
            void mouseExit(const juce::MouseEvent& e) override;
            void mouseWheelMove(const juce::MouseEvent& e,
                                const juce::MouseWheelDetails& wheel) override;

            Skin::SkinFamily getFamily() const noexcept { return family_; }
            int preferredWidth() const;
            int preferredHeight() const;
            void ignoreClicksForMs(int ms) noexcept;

        private:
            HeaderLogoPopupMenu& owner_;
            Skin::SkinFamily family_;
            std::vector<FamilyMember> members_;
            int highlightedIndex_ = -1;
            int scrollPixels_ = 0;
            juce::uint32 ignoreClicksUntilMs_ = 0;

            int contentHeightPx() const;
            int maxScrollPixels() const;
            juce::Rectangle<float> memberBounds(int index) const;
            int memberIndexAt(int x, int y) const;
            void updateHighlight(int index);
            void setScrollPixels(int pixels);
        };

        inline constexpr static float kBorderThicknessDesign_ = 1.0f;
        inline constexpr static float kHighlightGap_ = 1.0f;
        inline constexpr static int kColumnCount_ = 2;
        inline constexpr static int kColumnWidthDesign_ = TSS::Design::Panels::Header::kLogoPopupColumnWidth;
        inline constexpr static int kActionColumnWidthDesign_ =
            TSS::Design::Panels::Header::kLogoPopupActionColumnWidth;

        float uiScale_ = 1.0f;
        int currentSkinItemId_ = 0;
        int currentUiScaleId_ = 0;
        int highlightedFlatIndex_ = -1;
        int mainPopupWidth_ = 0;
        int mainPopupHeight_ = 0;
        juce::uint32 ignoreModalDismissUntilMs_ = 0;

        std::function<void(int skinItemId)> onSkinSelected_;
        std::function<void(int scaleId)> onUiScaleSelected_;
        std::function<void()> onSettingsRequested_;
        std::function<void()> onAboutRequested_;
        ContextualHelpBinder* contextualHelpBinder_ = nullptr;

        PopupMenuLook look_{};
        std::unique_ptr<PopupMenuRenderer> renderer_;
        juce::Font cachedFont_;
        std::vector<MenuItem> items_;
        std::unique_ptr<PopupMenuOutsideDismissWatcher> outsideDismissWatcher_;
        std::unique_ptr<FamilyFlyout> familyFlyout_;

        void buildItems();
        void appendUiScaleColumnItems();
        void appendSkinAndActionColumnItems();
        int getItemHeightPx() const;
        float getRowHeightPx(const MenuItem& item) const;
        float columnContentHeightForDisplayScale(int column, float systemDisplayScale) const;
        float getSeparatorWidth() const;
        float getColumnWidth(int column) const;
        juce::Colour sectionHeaderTextColour() const;
        juce::Rectangle<float> getItemBounds(int flatIndex) const;
        juce::Rectangle<int> preferredBoundsOver(Logo& logo) const;
        int getFlatIndexAt(int x, int y) const;
        bool isSelectableItem(int flatIndex) const;
        bool isCurrentSelection(int flatIndex) const;
        bool familyContainsCurrentSkin(Skin::SkinFamily family) const;
        void updateHighlightedItem(int flatIndex);
        void updateContextualHelpForItem(int flatIndex);
        const char* contextualHelpTextForItem(int flatIndex) const;
        void selectItem(int flatIndex);
        void selectSkinItemId(int skinItemId);
        void openFamilyFlyout(Skin::SkinFamily family);
        void closeFamilyFlyout();
        void layoutFamilyFlyout();
        std::vector<FamilyMember> membersForFamily(Skin::SkinFamily family) const;
        void closePopup();
        struct ItemDrawMetrics
        {
            float textPadding = 0.0f;
            float highlightGap = 0.0f;
            float ruleThickness = 0.0f;
        };
        void drawItems(juce::Graphics& g, const juce::Rectangle<float>& contentBounds);
        void drawMenuItem(juce::Graphics& g, int flatIndex, const ItemDrawMetrics& metrics);
    };
}
