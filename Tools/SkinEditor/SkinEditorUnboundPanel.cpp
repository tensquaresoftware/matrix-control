#include "Tools/SkinEditor/SkinEditorUnboundPanel.h"

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinEditor/SkinEditorChrome.h"

namespace
{
    juce::String askPastille(const juce::StringArray& names)
    {
        juce::AlertWindow window("Bind Unbound", "Choose a Palette pastille id:",
                                 juce::MessageBoxIconType::QuestionIcon);
        window.addComboBox("pastille", names, "Pastille");
        window.addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
        window.addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
        if (window.runModalLoop() != 1)
            return {};
        if (auto* box = window.getComboBoxComponent("pastille"))
            return box->getText();
        return {};
    }
}

SkinEditorUnboundPanel::SkinEditorUnboundPanel()
{
    list_.setRowHeight(22);
    list_.setColour(juce::ListBox::backgroundColourId, SkinEditorChrome::surfacePanel());
    addAndMakeVisible(list_);
    addAndMakeVisible(bindButton_);
    bindButton_.onClick = [this] { promptBind(list_.getSelectedRow()); };
}

void SkinEditorUnboundPanel::setLayer(TSS::DevSkin::Layer* layer)
{
    layer_ = layer;
}

void SkinEditorUnboundPanel::setActiveSkinId(const juce::String& skinId)
{
    skinId_ = skinId;
}

void SkinEditorUnboundPanel::setReadOnly(bool readOnly)
{
    readOnly_ = readOnly;
    bindButton_.setEnabled(! readOnly_ && ! forcedEmpty_);
}

void SkinEditorUnboundPanel::setForcedEmpty(bool forcedEmpty)
{
    forcedEmpty_ = forcedEmpty;
    bindButton_.setEnabled(! readOnly_ && ! forcedEmpty_);
}

void SkinEditorUnboundPanel::refresh()
{
    keys_.clear();
    if (! forcedEmpty_ && layer_ != nullptr)
        keys_ = layer_->unboundKeys(skinId_);

    list_.deselectAllRows();
    list_.updateContent();
    list_.repaint();
    bindButton_.setEnabled(false);
    repaint();
}

void SkinEditorUnboundPanel::paint(juce::Graphics& g)
{
    g.fillAll(SkinEditorChrome::surfacePanel());
    g.setColour(SkinEditorChrome::textPrimary());
    const auto count = forcedEmpty_ ? 0 : static_cast<int>(keys_.size());
    g.drawText("Unbound (" + juce::String(count) + ")",
               getLocalBounds().removeFromTop(22).reduced(SkinEditorChrome::kPanelPad, 0),
               juce::Justification::centredLeft, false);
}

void SkinEditorUnboundPanel::resized()
{
    auto bounds = getLocalBounds().reduced(SkinEditorChrome::kPanelPad);
    bounds.removeFromTop(24);
    auto actions = bounds.removeFromBottom(28);
    bindButton_.setBounds(actions.removeFromLeft(80).reduced(1));
    list_.setBounds(bounds);
}

int SkinEditorUnboundPanel::getNumRows()
{
    return static_cast<int>(keys_.size());
}

void SkinEditorUnboundPanel::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected)
{
    if (row < 0 || row >= static_cast<int>(keys_.size()))
        return;
    if (selected)
        g.fillAll(juce::Colour(0xff303030));
    g.setColour(SkinEditorChrome::textPrimary());
    g.drawText(TSS::DevSkin::keyLabel(keys_[static_cast<size_t>(row)]),
               6, 0, width - 12, height, juce::Justification::centredLeft);
}

void SkinEditorUnboundPanel::selectedRowsChanged(int)
{
    bindButton_.setEnabled(! readOnly_ && ! forcedEmpty_ && list_.getSelectedRow() >= 0);
}

void SkinEditorUnboundPanel::listBoxItemDoubleClicked(int row, const juce::MouseEvent&)
{
    promptBind(row);
}

void SkinEditorUnboundPanel::promptBind(int row)
{
    if (readOnly_ || forcedEmpty_ || layer_ == nullptr || row < 0 || row >= static_cast<int>(keys_.size()))
        return;

    const auto* skin = layer_->findSkin(skinId_);
    if (skin == nullptr)
        return;

    juce::StringArray names;
    for (const auto& pastille : skin->palette)
        names.add(pastille.name);
    if (names.isEmpty())
        return;

    const auto chosen = askPastille(names);
    if (chosen.isEmpty() || onBindRequested == nullptr)
        return;
    onBindRequested(keys_[static_cast<size_t>(row)], chosen);
}
