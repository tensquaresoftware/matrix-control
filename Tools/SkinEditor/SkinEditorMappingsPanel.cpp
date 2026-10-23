#include "Tools/SkinEditor/SkinEditorMappingsPanel.h"

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinEditor/SkinEditorChrome.h"

namespace
{
    juce::String askPastille(const juce::StringArray& names, const juce::String& current)
    {
        juce::AlertWindow window("Bind mapping", "Choose a Palette pastille id:",
                                 juce::MessageBoxIconType::QuestionIcon);
        window.addComboBox("pastille", names, "Pastille");
        if (auto* box = window.getComboBoxComponent("pastille"))
        {
            const int idx = names.indexOf(current);
            if (idx >= 0)
                box->setSelectedItemIndex(idx, juce::dontSendNotification);
        }
        window.addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
        window.addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
        if (window.runModalLoop() != 1)
            return {};
        if (auto* box = window.getComboBoxComponent("pastille"))
            return box->getText();
        return {};
    }
}

SkinEditorMappingsPanel::SkinEditorMappingsPanel()
{
    objectFilter_.addItem("All objects", 1);
    int id = 2;
    for (const auto& name : TSS::DevSkin::allObjectTypeNames())
        objectFilter_.addItem(name, id++);
    objectFilter_.setSelectedId(1, juce::dontSendNotification);
    objectFilter_.addListener(this);
    addAndMakeVisible(objectFilter_);

    stateFilter_.addItem("All states", 1);
    id = 2;
    for (const auto& name : TSS::DevSkin::allColourStateNames())
        stateFilter_.addItem(name, id++);
    stateFilter_.setSelectedId(1, juce::dontSendNotification);
    stateFilter_.addListener(this);
    addAndMakeVisible(stateFilter_);

    list_.setRowHeight(22);
    list_.setColour(juce::ListBox::backgroundColourId, SkinEditorChrome::surfacePanel());
    addAndMakeVisible(list_);
    addAndMakeVisible(bindButton_);
    bindButton_.onClick = [this] { promptBind(list_.getSelectedRow()); };
}

void SkinEditorMappingsPanel::setLayer(TSS::DevSkin::Layer* layer)
{
    layer_ = layer;
}

void SkinEditorMappingsPanel::setActiveSkinId(const juce::String& skinId)
{
    skinId_ = skinId;
}

void SkinEditorMappingsPanel::setReadOnly(bool readOnly)
{
    readOnly_ = readOnly;
    bindButton_.setEnabled(! readOnly_);
}

void SkinEditorMappingsPanel::refresh()
{
    rebuildRows();
    list_.deselectAllRows();
    list_.updateContent();
    list_.repaint();
    bindButton_.setEnabled(false);
}

void SkinEditorMappingsPanel::paint(juce::Graphics& g)
{
    g.fillAll(SkinEditorChrome::surfacePanel());
    g.setColour(SkinEditorChrome::textPrimary());
    g.drawText("Mappings", getLocalBounds().removeFromTop(22).reduced(SkinEditorChrome::kPanelPad, 0),
               juce::Justification::centredLeft, false);
}

void SkinEditorMappingsPanel::resized()
{
    auto bounds = getLocalBounds().reduced(SkinEditorChrome::kPanelPad);
    bounds.removeFromTop(24);
    auto filters = bounds.removeFromTop(28);
    objectFilter_.setBounds(filters.removeFromLeft(filters.getWidth() / 2).reduced(1));
    stateFilter_.setBounds(filters.reduced(1));
    auto actions = bounds.removeFromBottom(28);
    bindButton_.setBounds(actions.removeFromLeft(80).reduced(1));
    list_.setBounds(bounds);
}

int SkinEditorMappingsPanel::getNumRows()
{
    return static_cast<int>(rows_.size());
}

void SkinEditorMappingsPanel::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected)
{
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;
    if (selected)
        g.fillAll(juce::Colour(0xff303030));

    const auto& item = rows_[static_cast<size_t>(row)];
    g.setColour(SkinEditorChrome::textPrimary());
    g.drawText(item.propertyLabel + "  ->  " + item.pastilleName,
               6, 0, width - 12, height, juce::Justification::centredLeft);
}

void SkinEditorMappingsPanel::selectedRowsChanged(int)
{
    bindButton_.setEnabled(! readOnly_ && list_.getSelectedRow() >= 0);
}

void SkinEditorMappingsPanel::listBoxItemDoubleClicked(int row, const juce::MouseEvent&)
{
    promptBind(row);
}

void SkinEditorMappingsPanel::comboBoxChanged(juce::ComboBox*)
{
    refresh();
}

void SkinEditorMappingsPanel::rebuildRows()
{
    rows_.clear();
    if (layer_ == nullptr)
        return;
    const auto* skin = layer_->findSkin(skinId_);
    if (skin == nullptr)
        return;

    TSS::SkinObjectType objectFilter {};
    const bool filterObject = objectFilter_.getSelectedId() > 1
                              && TSS::DevSkin::parseObjectType(objectFilter_.getText(), objectFilter);
    TSS::SkinColourState stateFilter {};
    const bool filterState = stateFilter_.getSelectedId() > 1
                             && TSS::DevSkin::parseState(stateFilter_.getText(), stateFilter);

    for (const auto& [key, pastille] : skin->mappings)
    {
        if (filterObject && key.object != objectFilter)
            continue;
        if (filterState && key.state != stateFilter)
            continue;

        Row row;
        row.key = key;
        row.propertyLabel = TSS::DevSkin::toString(key.property)
                            + " [" + TSS::DevSkin::toString(key.object)
                            + " / " + TSS::DevSkin::toString(key.state) + "]";
        row.pastilleName = pastille;
        rows_.push_back(row);
    }
}

void SkinEditorMappingsPanel::promptBind(int row)
{
    if (readOnly_ || layer_ == nullptr || row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    const auto* skin = layer_->findSkin(skinId_);
    if (skin == nullptr)
        return;

    juce::StringArray names;
    for (const auto& pastille : skin->palette)
        names.add(pastille.name);
    if (names.isEmpty())
        return;

    const auto chosen = askPastille(names, rows_[static_cast<size_t>(row)].pastilleName);
    if (chosen.isEmpty() || onBindRequested == nullptr)
        return;
    onBindRequested(rows_[static_cast<size_t>(row)].key, chosen);
}
