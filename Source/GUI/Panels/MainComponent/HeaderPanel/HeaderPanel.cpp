#include "HeaderPanel.h"

#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Skins/Skin.h"

HeaderPanel::HeaderPanel(tss::Skin& skin, int width, int height)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , skinLabel_(skin, kSkinLabelWidth_, kControlHeight_, "SKIN :")
    , skinComboBox_(skin, kComboBoxWidth_, kControlHeight_, tss::ComboBox::Style::ButtonLike)
    , zoomLabel_(skin, kZoomLabelWidth_, kControlHeight_, "ZOOM :")
    , zoomComboBox_(skin, kComboBoxWidth_, kControlHeight_, tss::ComboBox::Style::ButtonLike)
{
    setOpaque(true);
    
    addAndMakeVisible(skinLabel_);
    
    skinComboBox_.addItem("BLACK", 1);
    skinComboBox_.addItem("CREAM", 2);
    skinComboBox_.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(skinComboBox_);
    
    addAndMakeVisible(zoomLabel_);
    
    zoomComboBox_.addItem("50%", 1);
    zoomComboBox_.addItem("75%", 2);
    zoomComboBox_.addItem("90%", 3);
    zoomComboBox_.addItem("100%", 4);
    zoomComboBox_.addItem("125%", 5);
    zoomComboBox_.addItem("150%", 6);
    zoomComboBox_.addItem("200%", 7);
    zoomComboBox_.setSelectedId(4, juce::dontSendNotification);
    addAndMakeVisible(zoomComboBox_);
}

void HeaderPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getHeaderPanelBackgroundColour());
}

void HeaderPanel::resized()
{
    const auto bounds = getLocalBounds();
    const auto spacing = getSpacing();
    const auto controlY = (height_ - kControlHeight_) / 2;
    
    int currentX = 15; //spacing;
    
    skinLabel_.setBounds(
        bounds.getX() + currentX,
        bounds.getY() + controlY,
        kSkinLabelWidth_,
        kControlHeight_
    );
    currentX += kSkinLabelWidth_ + spacing;
    
    skinComboBox_.setBounds(
        bounds.getX() + currentX,
        bounds.getY() + controlY,
        kComboBoxWidth_,
        kControlHeight_
    );
    currentX += kComboBoxWidth_ + spacing * 2;
    
    zoomLabel_.setBounds(
        bounds.getX() + currentX,
        bounds.getY() + controlY,
        kZoomLabelWidth_,
        kControlHeight_
    );
    currentX += kZoomLabelWidth_ + spacing;
    
    zoomComboBox_.setBounds(
        bounds.getX() + currentX,
        bounds.getY() + controlY,
        kComboBoxWidth_,
        kControlHeight_
    );
}

void HeaderPanel::setSkin(tss::Skin& skin)
{
    skin_ = &skin;
    skinLabel_.setSkin(skin);
    skinComboBox_.setSkin(skin);
    zoomLabel_.setSkin(skin);
    zoomComboBox_.setSkin(skin);
}

