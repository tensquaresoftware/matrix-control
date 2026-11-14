#include "Tools/SkinDesigner/SkinDesignerZonesPanel.h"

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinDesigner/SkinDesignerChrome.h"

namespace
{
    constexpr int kRowH = SkinDesignerChrome::kColumnHeaderHeight;
    constexpr int kSectionGap = SkinDesignerChrome::kPanelContentPad;
    constexpr int kObjectColW = 120;
    constexpr int kZoneColW = 88;
    constexpr int kWhereColW = 280;
    constexpr int kExceptionObjectColW = 100;
    constexpr int kUsesColW = 72;
    constexpr const char* kUnusedCellText = "-";

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

    void styleSectionTitle(juce::Label& label)
    {
        label.setFont(SkinDesignerChrome::fontRegular());
        label.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        label.setColour(juce::Label::textColourId, SkinDesignerChrome::textMuted());
        label.setJustificationType(juce::Justification::centredLeft);
        SkinDesignerChrome::preventTextSquish(label);
    }

    void styleZoneCombo(juce::ComboBox& combo)
    {
        combo.setColour(juce::ComboBox::backgroundColourId, SkinDesignerChrome::controlFill());
        combo.setColour(juce::ComboBox::outlineColourId, SkinDesignerChrome::controlOutline());
        combo.setColour(juce::ComboBox::arrowColourId, SkinDesignerChrome::textMuted());
        combo.setColour(juce::ComboBox::textColourId, SkinDesignerChrome::textPrimary());
        combo.setColour(juce::ComboBox::buttonColourId, SkinDesignerChrome::controlFill());
    }
}

void SkinDesignerZonesPanel::buildTitleChrome()
{
    SkinDesignerChrome::stylePanelTitle(titleLabel_, SkinDesignerChrome::kZonesWorkSurfaceTitle);
    addAndMakeVisible(titleLabel_);
    SkinDesignerChrome::stylePeerWorkSurfaceTitle(peerTitleLabel_,
                                                  SkinDesignerChrome::kMappingsWorkSurfaceTitle);
    addAndMakeVisible(peerTitleLabel_);

    styleMutedLabel(skinLabel_, "Skin: -", false);
    addAndMakeVisible(skinLabel_);
    resetButton_.onClick = [this] { resetToDefaults(); };
    addAndMakeVisible(resetButton_);
    styleSectionTitle(matrixSectionLabel_);
    addAndMakeVisible(matrixSectionLabel_);
}

void SkinDesignerZonesPanel::buildMatrixHeaders()
{
    styleMutedLabel(objectHeader_, "Object", true);
    content_.addAndMakeVisible(objectHeader_);
    for (size_t i = 0; i < TSS::kAllSkinZones.size(); ++i)
    {
        styleMutedLabel(zoneHeaders_[i], TSS::DevSkin::toString(TSS::kAllSkinZones[i]), true);
        content_.addAndMakeVisible(zoneHeaders_[i]);
    }
    for (size_t i = 0; i < TSS::kMultiSetSkinObjects.size(); ++i)
    {
        styleMutedLabel(rowLabels_[i], TSS::DevSkin::toString(TSS::kMultiSetSkinObjects[i]), false);
        content_.addAndMakeVisible(rowLabels_[i]);
    }
}

void SkinDesignerZonesPanel::buildReadOnlySections()
{
    styleSectionTitle(ucsSectionLabel_);
    content_.addAndMakeVisible(ucsSectionLabel_);
    styleSectionTitle(exceptionsSectionLabel_);
    content_.addAndMakeVisible(exceptionsSectionLabel_);
    styleMutedLabel(whereHeader_, "Where", true);
    styleMutedLabel(exceptionObjectHeader_, "Object", true);
    styleMutedLabel(usesHeader_, "Uses", true);
    content_.addAndMakeVisible(whereHeader_);
    content_.addAndMakeVisible(exceptionObjectHeader_);
    content_.addAndMakeVisible(usesHeader_);
    styleMutedLabel(hintLabel_, hintLabel_.getText(), false);
    hintLabel_.setColour(juce::Label::textColourId, SkinDesignerChrome::textMuted());
    content_.addAndMakeVisible(hintLabel_);
}

SkinDesignerZonesPanel::SkinDesignerZonesPanel()
{
    buildTitleChrome();
    buildMatrixHeaders();
    buildReadOnlySections();
    rebuildUsedCells();
    rebuildUcsLabels();
    rebuildExceptionLabels();

    viewport_.setViewedComponent(&content_, false);
    viewport_.setScrollBarsShown(true, false);
    viewport_.setScrollBarThickness(SkinDesignerChrome::kScrollTrackWidth);
    viewport_.setColour(juce::ScrollBar::backgroundColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(viewport_);
}

void SkinDesignerZonesPanel::setLayer(TSS::DevSkin::Layer* layer)
{
    layer_ = layer;
}

void SkinDesignerZonesPanel::setActiveSkinId(const juce::String& skinId)
{
    if (skinId_ == skinId)
        return;
    skinId_ = skinId;
}

void SkinDesignerZonesPanel::setReadOnly(bool readOnly)
{
    readOnly_ = readOnly;
    resetButton_.setEnabled(! readOnly_ && skinId_.isNotEmpty());
    for (auto& cell : usedCells_)
        if (cell.combo != nullptr)
            cell.combo->setEnabled(! readOnly_ && skinId_.isNotEmpty());
}

void SkinDesignerZonesPanel::refresh()
{
    skinLabel_.setText("Skin: " + activeSkinDisplayName(), juce::dontSendNotification);
    setReadOnly(readOnly_);
    syncCombosFromMap();
    resized();
    repaint();
}

void SkinDesignerZonesPanel::paint(juce::Graphics& g)
{
    SkinDesignerChrome::paintPanelFrame(g, getLocalBounds(), titleTabBounds_, 0);
    SkinDesignerChrome::paintRoundedWell(g, viewport_.getBounds(), SkinDesignerChrome::surfacePanel());
}

void SkinDesignerZonesPanel::resized()
{
    const int activeTabW = SkinDesignerChrome::titleTabWidth(
        SkinDesignerChrome::kZonesWorkSurfaceTitle);
    const auto chrome = SkinDesignerChrome::layoutPanel(getLocalBounds(), activeTabW, 0);
    titleTabBounds_ = chrome.tab;
    titleLabel_.setBounds(chrome.tab);
    peerTitleBounds_ = SkinDesignerChrome::layoutPeerWorkSurfaceTitle(
        peerTitleLabel_, chrome.tab, SkinDesignerChrome::kMappingsWorkSurfaceTitle);

    auto content = chrome.content;
    auto topRow = content.removeFromTop(SkinDesignerChrome::kButtonHeight);
    const int resetW = SkinDesignerChrome::measureButtonWidth(resetButton_);
    resetButton_.setBounds(topRow.removeFromRight(resetW));
    topRow.removeFromRight(SkinDesignerChrome::kButtonGap);
    skinLabel_.setBounds(topRow);

    content.removeFromTop(SkinDesignerChrome::kPanelContentPad);
    matrixSectionLabel_.setBounds(content.removeFromTop(kRowH));
    content.removeFromTop(SkinDesignerChrome::kPanelContentPad);

    auto listBounds = content;
    listBounds.setRight(listBounds.getRight() + SkinDesignerChrome::kPanelContentPad);
    viewport_.setBounds(listBounds);
    layoutContent();
}

void SkinDesignerZonesPanel::mouseUp(const juce::MouseEvent& e)
{
    if (! e.mouseWasClicked())
        return;
    if (peerTitleBounds_.contains(e.getPosition()) && onPeerWorkSurfaceRequested != nullptr)
        onPeerWorkSurfaceRequested();
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
            styleZoneCombo(*cell.combo);
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
    for (const auto object : TSS::kSingleSetSkinObjects)
    {
        auto label = std::make_unique<juce::Label>();
        styleMutedLabel(*label, TSS::DevSkin::toString(object), false);
        content_.addAndMakeVisible(*label);
        ucsLabels_.push_back(std::move(label));
    }
}

void SkinDesignerZonesPanel::rebuildExceptionLabels()
{
    exceptionWhereLabels_.clear();
    exceptionObjectLabels_.clear();
    exceptionUsesLabels_.clear();

    for (const auto& row : TSS::kZoneExceptionRows)
    {
        auto where = std::make_unique<juce::Label>();
        auto object = std::make_unique<juce::Label>();
        auto uses = std::make_unique<juce::Label>();
        styleMutedLabel(*where, row.where, false);
        where->setTooltip(row.where);
        styleMutedLabel(*object, TSS::DevSkin::toString(row.object), false);
        styleMutedLabel(*uses, exceptionUsesText(row), false);
        content_.addAndMakeVisible(*where);
        content_.addAndMakeVisible(*object);
        content_.addAndMakeVisible(*uses);
        exceptionWhereLabels_.push_back(std::move(where));
        exceptionObjectLabels_.push_back(std::move(object));
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

int SkinDesignerZonesPanel::layoutMatrixBlock(int y)
{
    objectHeader_.setBounds(0, y, kObjectColW, kRowH);
    for (size_t i = 0; i < TSS::kAllSkinZones.size(); ++i)
        zoneHeaders_[i].setBounds(kObjectColW + static_cast<int>(i) * kZoneColW, y, kZoneColW, kRowH);
    y += kRowH;

    size_t dashIndex = 0;
    size_t usedIndex = 0;
    for (size_t row = 0; row < TSS::kMultiSetSkinObjects.size(); ++row)
    {
        const auto object = TSS::kMultiSetSkinObjects[row];
        rowLabels_[row].setBounds(0, y, kObjectColW, kRowH);
        for (size_t col = 0; col < TSS::kAllSkinZones.size(); ++col)
        {
            const auto zone = TSS::kAllSkinZones[col];
            const auto cellBounds = juce::Rectangle<int>(
                kObjectColW + static_cast<int>(col) * kZoneColW, y, kZoneColW, kRowH);
            if (! TSS::isZoneUsed(object, zone))
            {
                if (dashIndex < dashLabels_.size())
                    dashLabels_[dashIndex++]->setBounds(cellBounds.reduced(2, 1));
                continue;
            }
            if (usedIndex < usedCells_.size() && usedCells_[usedIndex].combo != nullptr)
                usedCells_[usedIndex++].combo->setBounds(cellBounds.reduced(2, 1));
        }
        y += kRowH;
    }
    return y;
}

int SkinDesignerZonesPanel::layoutUcsBlock(int y, int contentW)
{
    y += kSectionGap;
    ucsSectionLabel_.setBounds(0, y, contentW, kRowH);
    y += kRowH;
    for (auto& label : ucsLabels_)
    {
        label->setBounds(0, y, contentW, kRowH);
        y += kRowH;
    }
    return y;
}

int SkinDesignerZonesPanel::layoutExceptionsBlock(int y, int contentW)
{
    y += kSectionGap;
    exceptionsSectionLabel_.setBounds(0, y, contentW, kRowH);
    y += kRowH;
    whereHeader_.setBounds(0, y, kWhereColW, kRowH);
    exceptionObjectHeader_.setBounds(kWhereColW, y, kExceptionObjectColW, kRowH);
    usesHeader_.setBounds(kWhereColW + kExceptionObjectColW, y, kUsesColW, kRowH);
    y += kRowH;

    for (size_t i = 0; i < exceptionWhereLabels_.size(); ++i)
    {
        exceptionWhereLabels_[i]->setBounds(0, y, kWhereColW, kRowH);
        exceptionObjectLabels_[i]->setBounds(kWhereColW, y, kExceptionObjectColW, kRowH);
        exceptionUsesLabels_[i]->setBounds(kWhereColW + kExceptionObjectColW, y, kUsesColW, kRowH);
        y += kRowH;
    }

    y += kSectionGap;
    hintLabel_.setBounds(0, y, contentW, kRowH);
    return y + kRowH + kSectionGap;
}

void SkinDesignerZonesPanel::layoutContent()
{
    const int contentW = juce::jmax(1, viewport_.getWidth() - SkinDesignerChrome::kScrollTrackWidth);
    int y = layoutMatrixBlock(0);
    y = layoutUcsBlock(y, contentW);
    y = layoutExceptionsBlock(y, contentW);
    content_.setSize(juce::jmax(1, viewport_.getWidth()), juce::jmax(viewport_.getHeight(), y));
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
