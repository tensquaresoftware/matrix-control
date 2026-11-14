#pragma once

#include <array>
#include <functional>
#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/SkinZoneRegistry.h"
#include "GUI/Skins/SkinValues.h"

/** Zones work surface: MCS matrix, UCS list, Exceptions, Reset. */
class SkinDesignerZonesPanel final : public juce::Component
{
public:
    SkinDesignerZonesPanel();

    void setLayer(TSS::DevSkin::Layer* layer);
    void setActiveSkinId(const juce::String& skinId);
    void setReadOnly(bool readOnly);
    void refresh();

    /** Peer title (MAPPINGS) clicked — host switches the right work surface. */
    std::function<void()> onPeerWorkSurfaceRequested;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseUp(const juce::MouseEvent& e) override;

private:
    struct UsedCell
    {
        TSS::SkinObjectType object = TSS::SkinObjectType::kSlider;
        TSS::SkinZone zone = TSS::SkinZone::kPatch;
        std::unique_ptr<juce::ComboBox> combo;
    };

    void buildTitleChrome();
    void buildMatrixHeaders();
    void buildReadOnlySections();
    void rebuildUsedCells();
    void rebuildUcsLabels();
    void rebuildExceptionLabels();
    void syncCombosFromMap();
    void layoutContent();
    int layoutMatrixBlock(int y);
    int layoutUcsBlock(int y, int contentW);
    int layoutExceptionsBlock(int y, int contentW);
    void applyCellChange(UsedCell& cell);
    void resetToDefaults();
    TSS::ColourSet effectiveColourSet(TSS::SkinObjectType object, TSS::SkinZone zone) const;
    const TSS::SkinZoneMap* activeZoneMap() const;
    juce::String activeSkinDisplayName() const;

    TSS::DevSkin::Layer* layer_ = nullptr;
    juce::String skinId_;
    bool readOnly_ = true;
    bool updatingCombos_ = false;

    juce::Label titleLabel_;
    juce::Label peerTitleLabel_;
    juce::Rectangle<int> titleTabBounds_;
    juce::Rectangle<int> peerTitleBounds_;

    juce::Label skinLabel_;
    juce::TextButton resetButton_ { "Reset to product defaults" };

    juce::Label matrixSectionLabel_ { {}, "Object -> zone matrix" };
    juce::Label objectHeader_ { {}, "Object" };
    std::array<juce::Label, TSS::kAllSkinZones.size()> zoneHeaders_;
    std::array<juce::Label, TSS::kMultiSetSkinObjects.size()> rowLabels_;
    std::vector<UsedCell> usedCells_;
    std::vector<std::unique_ptr<juce::Label>> dashLabels_;

    juce::Label ucsSectionLabel_ { {}, "Shared / single-set objects (UCS)" };
    std::vector<std::unique_ptr<juce::Label>> ucsLabels_;

    juce::Label exceptionsSectionLabel_ { {}, "Exceptions (overrides)" };
    juce::Label whereHeader_ { {}, "Where" };
    juce::Label exceptionObjectHeader_ { {}, "Object" };
    juce::Label usesHeader_ { {}, "Uses" };
    std::vector<std::unique_ptr<juce::Label>> exceptionWhereLabels_;
    std::vector<std::unique_ptr<juce::Label>> exceptionObjectLabels_;
    std::vector<std::unique_ptr<juce::Label>> exceptionUsesLabels_;

    juce::Label hintLabel_ { {}, "Paint drawers in Mappings - Assign geography here" };

    juce::Viewport viewport_;
    juce::Component content_;
};
