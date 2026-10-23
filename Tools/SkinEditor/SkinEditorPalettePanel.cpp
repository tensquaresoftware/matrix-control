#include "Tools/SkinEditor/SkinEditorPalettePanel.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"

SkinEditorPalettePanel::SkinEditorPalettePanel()
{
    list_.setRowHeight(22);
    list_.setColour(juce::ListBox::backgroundColourId, SkinEditorChrome::surfacePanel());
    addAndMakeVisible(list_);
    addAndMakeVisible(addButton_);
    addAndMakeVisible(deleteButton_);
    addButton_.onClick = [this]
    {
        if (onAddRequested)
            onAddRequested();
    };
    deleteButton_.onClick = [this]
    {
        if (onDeleteRequested)
            onDeleteRequested();
    };
}

void SkinEditorPalettePanel::setLayer(TSS::DevSkin::Layer* layer)
{
    layer_ = layer;
}

void SkinEditorPalettePanel::setActiveSkinId(const juce::String& skinId)
{
    skinId_ = skinId;
}

void SkinEditorPalettePanel::setReadOnly(bool readOnly)
{
    readOnly_ = readOnly;
    addButton_.setEnabled(! readOnly_);
    deleteButton_.setEnabled(! readOnly_);
}

void SkinEditorPalettePanel::refresh()
{
    rebuildRows();
    list_.updateContent();
    list_.repaint();

    const auto selected = getSelectedPastilleName();
    deleteButton_.setEnabled(! readOnly_ && selected.isNotEmpty()
                             && layer_ != nullptr
                             && ! layer_->isProductPastilleName(selected));
}

juce::String SkinEditorPalettePanel::getSelectedPastilleName() const
{
    const int row = list_.getSelectedRow();
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return {};
    if (rows_[static_cast<size_t>(row)].sectionHeader)
        return {};
    return rows_[static_cast<size_t>(row)].name;
}

void SkinEditorPalettePanel::selectPastille(const juce::String& name)
{
    for (size_t i = 0; i < rows_.size(); ++i)
    {
        if (! rows_[i].sectionHeader && rows_[i].name == name)
        {
            list_.selectRow(static_cast<int>(i));
            return;
        }
    }
}

void SkinEditorPalettePanel::paint(juce::Graphics& g)
{
    g.fillAll(SkinEditorChrome::surfacePanel());
    g.setColour(SkinEditorChrome::textPrimary());
    g.drawText("Palette", getLocalBounds().removeFromTop(22).reduced(SkinEditorChrome::kPanelPad, 0),
               juce::Justification::centredLeft, false);
}

void SkinEditorPalettePanel::resized()
{
    auto bounds = getLocalBounds().reduced(SkinEditorChrome::kPanelPad);
    bounds.removeFromTop(24);
    auto actions = bounds.removeFromBottom(28);
    addButton_.setBounds(actions.removeFromLeft(70).reduced(1));
    deleteButton_.setBounds(actions.removeFromLeft(90).reduced(1));
    list_.setBounds(bounds);
}

int SkinEditorPalettePanel::getNumRows()
{
    return static_cast<int>(rows_.size());
}

void SkinEditorPalettePanel::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected)
{
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    const auto& item = rows_[static_cast<size_t>(row)];
    if (item.sectionHeader)
    {
        g.setColour(SkinEditorChrome::surfaceApp());
        g.fillRect(0, 0, width, height);
        g.setColour(SkinEditorChrome::textMuted());
        g.drawText(item.headerText, 6, 0, width - 12, height, juce::Justification::centredLeft);
        return;
    }

    if (selected)
        g.fillAll(juce::Colour(0xff303030));

    g.setColour(juce::Colour(item.argb));
    g.fillRect(4, 4, height - 8, height - 8);
    g.setColour(SkinEditorChrome::textPrimary());
    g.drawText(item.name, height + 4, 0, width - height - 8, height, juce::Justification::centredLeft);
}

void SkinEditorPalettePanel::selectedRowsChanged(int)
{
    const auto name = getSelectedPastilleName();
    deleteButton_.setEnabled(! readOnly_ && name.isNotEmpty()
                             && layer_ != nullptr
                             && ! layer_->isProductPastilleName(name));
    if (name.isNotEmpty() && onPastilleSelected)
        onPastilleSelected(name);
}

void SkinEditorPalettePanel::rebuildRows()
{
    rows_.clear();
    if (layer_ == nullptr)
        return;

    const auto* skin = layer_->findSkin(skinId_);
    if (skin == nullptr)
        return;

    std::vector<Row> product;
    std::vector<Row> editor;
    for (const auto& pastille : skin->palette)
    {
        Row row;
        row.name = pastille.name;
        row.argb = pastille.argb;
        row.product = layer_->isProductPastilleName(pastille.name);
        if (row.product)
            product.push_back(row);
        else
            editor.push_back(row);
    }

    if (! product.empty())
    {
        rows_.push_back({ {}, 0, false, true, "Product colours" });
        rows_.insert(rows_.end(), product.begin(), product.end());
    }
    if (! editor.empty() || ! readOnly_)
    {
        rows_.push_back({ {}, 0, false, true, "Editor colours" });
        rows_.insert(rows_.end(), editor.begin(), editor.end());
    }
}
