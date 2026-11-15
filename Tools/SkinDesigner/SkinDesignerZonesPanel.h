#pragma once

#include <array>
#include <functional>
#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/SkinZoneRegistry.h"
#include "GUI/Skins/SkinValues.h"

/** Zone Mappings work surface: matrix, Shared, Exceptions (sub-tabs), Reset. */
class SkinDesignerZonesPanel final : public juce::Component
{
public:
    SkinDesignerZonesPanel();

    void setLayer(TSS::DevSkin::Layer* layer);
    void setActiveSkinId(const juce::String& skinId);
    void setReadOnly(bool readOnly);
    void refresh();

    /** Peer title (COLOUR MAPPINGS) clicked — host switches the right work surface. */
    std::function<void()> onPeerWorkSurfaceRequested;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseUp(const juce::MouseEvent& e) override;

private:
    enum class SubPage
    {
        kMatrix,
        kShared,
        kExceptions
    };

    struct UsedCell
    {
        TSS::SkinObjectType object = TSS::SkinObjectType::kSlider;
        TSS::SkinZone zone = TSS::SkinZone::kPatch;
        std::unique_ptr<juce::ComboBox> combo;
    };

    /** Scroll body: paints MCS table chrome when that sub-page is active. */
    class Content final : public juce::Component
    {
    public:
        explicit Content(SkinDesignerZonesPanel& owner) : owner_(owner) {}
        void paint(juce::Graphics& g) override;

    private:
        SkinDesignerZonesPanel& owner_;
    };

    void buildTitleChrome();
    void buildSubTabs();
    void buildMatrixHeaders();
    void buildReadOnlySections();
    void rebuildUsedCells();
    void rebuildUcsLabels();
    void rebuildExceptionLabels();
    void syncCombosFromMap();
    void setSubPage(SubPage page);
    void styleSubTab(juce::Label& label, bool active);
    void applySubPageVisibility();
    void updateIntroText();
    void layoutContent();
    void paintMatrixChrome(juce::Graphics& g) const;
    void paintSharedChrome(juce::Graphics& g) const;
    void paintExceptionsChrome(juce::Graphics& g) const;
    void paintSubTabUnderline(juce::Graphics& g) const;
    int layoutMatrixPage(int contentW);
    int layoutSharedPage(int contentW);
    int layoutExceptionsPage(int contentW);
    void applyCellChange(UsedCell& cell);
    void resetToDefaults();
    TSS::ColourSet effectiveColourSet(TSS::SkinObjectType object, TSS::SkinZone zone) const;
    const TSS::SkinZoneMap* activeZoneMap() const;
    juce::String activeSkinDisplayName() const;

    TSS::DevSkin::Layer* layer_ = nullptr;
    juce::String skinId_;
    bool readOnly_ = true;
    bool updatingCombos_ = false;
    SubPage subPage_ = SubPage::kMatrix;

    juce::Label titleLabel_;
    juce::Label peerTitleLabel_;
    juce::Rectangle<int> titleTabBounds_;
    juce::Rectangle<int> peerTitleBounds_;

    juce::Label matrixSubTab_ { {}, "MCS objects" };
    juce::Label sharedSubTab_ { {}, "UCS objects" };
    juce::Label exceptionsSubTab_ { {}, "Exceptions" };
    juce::Label introLabel_;

    juce::TextButton resetButton_ { "Reset to product defaults" };

    juce::Label skinHeader_ { {}, "Skin:" };
    juce::Rectangle<int> skinNameBadgeBounds_;
    juce::String skinNameBadgeText_;
    juce::Label objectHeader_ { {}, "Objects" };
    juce::Label zonesGroupHeader_ { {}, "Zones" };
    std::array<juce::Label, TSS::kAllSkinZones.size()> zoneHeaders_;
    std::array<juce::Label, TSS::kMultiSetSkinObjects.size()> rowLabels_;
    std::vector<UsedCell> usedCells_;
    std::vector<std::unique_ptr<juce::Label>> dashLabels_;

    std::vector<std::unique_ptr<juce::Label>> ucsLabels_;
    std::array<juce::Label, 3> ucsColumnHeaders_;

    juce::Label exceptionObjectHeader_ { {}, "Object" };
    juce::Label locationHeader_ { {}, "Location" };
    juce::Label usesHeader_ { {}, "Uses" };
    std::vector<std::unique_ptr<juce::Label>> exceptionObjectLabels_;
    std::vector<std::unique_ptr<juce::Label>> exceptionLocationLabels_;
    std::vector<std::unique_ptr<juce::Label>> exceptionUsesLabels_;

    juce::Viewport viewport_;
    Content content_ { *this };
};
