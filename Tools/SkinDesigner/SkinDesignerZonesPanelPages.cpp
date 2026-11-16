#include "Tools/SkinDesigner/SkinDesignerZonesPanel.h"

#include <algorithm>
#include <vector>

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinDesigner/SkinDesignerChrome.h"

namespace
{
    // Em dash (Skin Designer Inter path — not plugin footer ASCII scope).
    const juce::String kUnusedCellText = juce::String::fromUTF8("\xe2\x80\x94");
    constexpr const char* kMcsIntro =
        "MCS (Multi Colour Set) objects have several colour sets (cs1, cs2, etc.). "
        "Assign which GUI zone reads which drawer.";
    constexpr const char* kUcsIntro =
        "UCS (Unique Colour Set) objects have a single colour set (cs1 only). "
        "The product inventory fixes this list; it is not editable here.";
    constexpr const char* kExceptionsIntro =
        "Exceptions are named product overrides outside the MCS zone map. "
        "Shown here for reference only; you cannot change them yet.";

    juce::String exceptionUsesText(const TSS::ZoneExceptionRow& row)
    {
        const auto cs = TSS::DevSkin::toString(row.colourSet);
        if (row.usesKind == TSS::ZoneExceptionUsesKind::kUcs)
            return "UCS " + cs;
        return cs;
    }

    void styleMutedLabel(juce::Label& label, const juce::String& text, bool header)
    {
        label.setText(text, juce::dontSendNotification);
        label.setFont(SkinDesignerChrome::fontRegular());
        label.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        label.setColour(juce::Label::textColourId,
                        header ? SkinDesignerChrome::textTableHeader()
                               : SkinDesignerChrome::textTableBody());
        label.setJustificationType(juce::Justification::centredLeft);
        SkinDesignerChrome::preventTextSquish(label);
    }

    void styleMatrixCombo(juce::ComboBox& combo)
    {
        const auto badge = SkinDesignerChrome::textMuted();
        const auto ink = SkinDesignerChrome::surfaceTable();
        combo.setColour(juce::ComboBox::backgroundColourId, badge);
        combo.setColour(juce::ComboBox::outlineColourId, badge);
        combo.setColour(juce::ComboBox::arrowColourId, ink);
        combo.setColour(juce::ComboBox::textColourId, ink);
        combo.setColour(juce::ComboBox::buttonColourId, badge);
    }
}

void SkinDesignerZonesPanel::styleSubTab(juce::Label& label, bool active)
{
    label.setFont(SkinDesignerChrome::fontRegular());
    label.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    label.setColour(juce::Label::textColourId,
                    active ? SkinDesignerChrome::textPrimary()
                           : SkinDesignerChrome::textMuted());
    label.setJustificationType(juce::Justification::centredLeft);
    label.setInterceptsMouseClicks(false, false);
    SkinDesignerChrome::preventTextSquish(label);
}

void SkinDesignerZonesPanel::updateIntroText()
{
    const char* text = kMcsIntro;
    switch (subPage_)
    {
        case SubPage::kMatrix:     text = kMcsIntro; break;
        case SubPage::kShared:     text = kUcsIntro; break;
        case SubPage::kExceptions: text = kExceptionsIntro; break;
    }
    introLabel_.setText(text, juce::dontSendNotification);
}

void SkinDesignerZonesPanel::setSubPage(SubPage page)
{
    if (subPage_ == page)
        return;
    subPage_ = page;
    styleSubTab(matrixSubTab_, subPage_ == SubPage::kMatrix);
    styleSubTab(sharedSubTab_, subPage_ == SubPage::kShared);
    styleSubTab(exceptionsSubTab_, subPage_ == SubPage::kExceptions);
    updateIntroText();
    applySubPageVisibility();
    resized();
    repaint();
}

void SkinDesignerZonesPanel::applySubPageVisibility()
{
    const bool matrix = subPage_ == SubPage::kMatrix;
    const bool shared = subPage_ == SubPage::kShared;
    const bool exceptions = subPage_ == SubPage::kExceptions;

    resetButton_.setVisible(matrix);
    skinHeader_.setVisible(matrix);
    zonesGroupHeader_.setVisible(matrix);
    for (auto& header : zoneHeaders_)
        header.setVisible(matrix);
    for (auto& row : rowLabels_)
        row.setVisible(matrix);
    for (auto& cell : usedCells_)
        if (cell.combo != nullptr)
            cell.combo->setVisible(matrix);
    for (auto& dash : dashLabels_)
        dash->setVisible(matrix);

    for (auto& label : ucsLabels_)
        label->setVisible(shared);
    for (auto& header : ucsColumnHeaders_)
        header.setVisible(shared);

    exceptionObjectHeader_.setVisible(exceptions);
    locationHeader_.setVisible(exceptions);
    usesHeader_.setVisible(exceptions);
    for (auto& label : exceptionObjectLabels_)
        label->setVisible(exceptions);
    for (auto& label : exceptionLocationLabels_)
        label->setVisible(exceptions);
    for (auto& label : exceptionUsesLabels_)
        label->setVisible(exceptions);
}

void SkinDesignerZonesPanel::rebuildUsedCells()
{
    usedCells_.clear();
    dashLabels_.clear();

    for (const auto object : TSS::kMultiSetSkinObjects)
    {
        for (const auto zone : TSS::kAllSkinZones)
        {
            if (! TSS::isZoneUsed(object, zone))
            {
                auto dash = std::make_unique<juce::Label>();
                styleMutedLabel(*dash, kUnusedCellText, false);
                dash->setJustificationType(juce::Justification::centred);
                content_.addAndMakeVisible(*dash);
                dashLabels_.push_back(std::move(dash));
                continue;
            }

            UsedCell cell;
            cell.object = object;
            cell.zone = zone;
            cell.combo = std::make_unique<juce::ComboBox>();
            styleMatrixCombo(*cell.combo);
            cell.combo->clear(juce::dontSendNotification);
            const int count = static_cast<int>(TSS::colourSetCount(object));
            for (int i = 0; i < count; ++i)
                cell.combo->addItem(TSS::DevSkin::toString(static_cast<TSS::ColourSet>(i)), i + 1);

            const auto index = usedCells_.size();
            cell.combo->onChange = [this, index]
            {
                if (index < usedCells_.size())
                    applyCellChange(usedCells_[index]);
            };
            content_.addAndMakeVisible(*cell.combo);
            usedCells_.push_back(std::move(cell));
        }
    }
}

void SkinDesignerZonesPanel::rebuildUcsLabels()
{
    ucsLabels_.clear();
    std::vector<TSS::SkinObjectType> sorted (TSS::kSingleSetSkinObjects.begin(),
                                             TSS::kSingleSetSkinObjects.end());
    std::sort(sorted.begin(), sorted.end(), [](TSS::SkinObjectType a, TSS::SkinObjectType b)
    {
        return TSS::DevSkin::toString(a).compareIgnoreCase(TSS::DevSkin::toString(b)) < 0;
    });
    for (const auto object : sorted)
    {
        auto label = std::make_unique<juce::Label>();
        styleMutedLabel(*label, TSS::DevSkin::toString(object), false);
        content_.addAndMakeVisible(*label);
        ucsLabels_.push_back(std::move(label));
    }

    const juce::BorderSize<int> headerPad (0, SkinDesignerChrome::kMappingsColumnPad, 0,
                                           SkinDesignerChrome::kMappingsColumnPad);
    for (auto& header : ucsColumnHeaders_)
    {
        styleMutedLabel(header, {}, true);
        header.setFont(SkinDesignerChrome::fontBold());
        header.setJustificationType(juce::Justification::centredLeft);
        header.setBorderSize(headerPad);
        content_.addAndMakeVisible(header);
    }
}

void SkinDesignerZonesPanel::rebuildExceptionLabels()
{
    exceptionObjectLabels_.clear();
    exceptionLocationLabels_.clear();
    exceptionUsesLabels_.clear();

    for (const auto& row : TSS::kZoneExceptionRows)
    {
        auto object = std::make_unique<juce::Label>();
        auto location = std::make_unique<juce::Label>();
        auto uses = std::make_unique<juce::Label>();
        styleMutedLabel(*object, TSS::DevSkin::toString(row.object), false);
        // Text painted with Operation arrows in paintExceptionsChrome.
        styleMutedLabel(*location, {}, false);
        location->setTooltip(row.where);
        styleMutedLabel(*uses, exceptionUsesText(row), false);
        content_.addAndMakeVisible(*object);
        content_.addAndMakeVisible(*location);
        content_.addAndMakeVisible(*uses);
        exceptionObjectLabels_.push_back(std::move(object));
        exceptionLocationLabels_.push_back(std::move(location));
        exceptionUsesLabels_.push_back(std::move(uses));
    }
}

void SkinDesignerZonesPanel::syncCombosFromMap()
{
    const juce::ScopedValueSetter<bool> scope(updatingCombos_, true);
    for (auto& cell : usedCells_)
    {
        if (cell.combo == nullptr)
            continue;
        const auto cs = effectiveColourSet(cell.object, cell.zone);
        cell.combo->setSelectedId(static_cast<int>(cs) + 1, juce::dontSendNotification);
    }
}

void SkinDesignerZonesPanel::layoutContent()
{
    const int contentW = juce::jmax(1, viewport_.getWidth() - SkinDesignerChrome::kScrollTrackWidth);
    int y = 0;
    switch (subPage_)
    {
        case SubPage::kMatrix:     y = layoutMatrixPage(contentW); break;
        case SubPage::kShared:     y = layoutSharedPage(contentW); break;
        case SubPage::kExceptions: y = layoutExceptionsPage(contentW); break;
    }
    content_.setSize(juce::jmax(1, viewport_.getWidth()), juce::jmax(viewport_.getHeight(), y));
    content_.repaint();
}

void SkinDesignerZonesPanel::applyCellChange(UsedCell& cell)
{
    if (updatingCombos_ || readOnly_ || layer_ == nullptr || skinId_.isEmpty()
        || cell.combo == nullptr)
        return;

    const int selectedId = cell.combo->getSelectedId();
    if (selectedId <= 0)
    {
        syncCombosFromMap();
        return;
    }

    const auto colourSet = static_cast<TSS::ColourSet>(selectedId - 1);
    TSS::SkinZoneMap map;
    if (const auto* existing = activeZoneMap())
        map = *existing;
    map[TSS::SkinZoneMapKey { cell.object, cell.zone }] = colourSet;

    const auto result = layer_->setZoneMap(skinId_, std::move(map));
    if (result.failed())
        syncCombosFromMap();
}

void SkinDesignerZonesPanel::resetToDefaults()
{
    if (readOnly_ || layer_ == nullptr || skinId_.isEmpty())
        return;

    const auto result = layer_->setZoneMap(skinId_, {});
    if (result.failed())
        return;
    syncCombosFromMap();
}

TSS::ColourSet SkinDesignerZonesPanel::effectiveColourSet(TSS::SkinObjectType object,
                                                          TSS::SkinZone zone) const
{
    if (const auto* map = activeZoneMap())
    {
        const auto it = map->find(TSS::SkinZoneMapKey { object, zone });
        if (it != map->end())
            return it->second;
    }
    return TSS::defaultColourSet(object, zone);
}

const TSS::SkinZoneMap* SkinDesignerZonesPanel::activeZoneMap() const
{
    if (layer_ == nullptr || skinId_.isEmpty())
        return nullptr;
    const auto* skin = layer_->findSkin(skinId_);
    return skin != nullptr ? &skin->zoneMap : nullptr;
}

juce::String SkinDesignerZonesPanel::activeSkinDisplayName() const
{
    if (skinId_.isEmpty())
        return "-";
    if (layer_ == nullptr)
        return skinId_;
    const auto* skin = layer_->findSkin(skinId_);
    if (skin == nullptr || skin->name.isEmpty())
        return skinId_;
    return skin->name;
}
