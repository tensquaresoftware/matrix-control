#include "HeaderPanel.h"

#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

using tss::SkinColourId;

HeaderPanel::HeaderPanel(tss::ISkin& skin, int width, int height)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , skinLabel_(skin, kSkinLabelWidth_, kControlHeight_, "SKIN :", tss::LabelStyle::HeaderPanel)
    , skinComboBox_(skin, kComboBoxWidth_, kControlHeight_, tss::ComboBox::Style::ButtonLike)
    , zoomLabel_(skin, kZoomLabelWidth_, kControlHeight_, "ZOOM :", tss::LabelStyle::HeaderPanel)
    , zoomComboBox_(skin, kComboBoxWidth_, kControlHeight_, tss::ComboBox::Style::ButtonLike)
{
    setOpaque(true);
    
    addAndMakeVisible(skinLabel_);
    
    skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kBlack, 
                         static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack));
    skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kCream, 
                         static_cast<int>(tss::Skin::SkinComboBoxItemId::kCream));
    skinComboBox_.setSelectedId(static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack), juce::dontSendNotification);
    addAndMakeVisible(skinComboBox_);
    
    addAndMakeVisible(zoomLabel_);
    
    zoomComboBox_.addItem(PluginDisplayNames::ChoiceLists::ZoomLevels::k50, PluginIDs::Settings::ZoomLevels::k50);
    zoomComboBox_.addItem(PluginDisplayNames::ChoiceLists::ZoomLevels::k75, PluginIDs::Settings::ZoomLevels::k75);
    zoomComboBox_.addItem(PluginDisplayNames::ChoiceLists::ZoomLevels::k90, PluginIDs::Settings::ZoomLevels::k90);
    zoomComboBox_.addItem(PluginDisplayNames::ChoiceLists::ZoomLevels::k100, PluginIDs::Settings::ZoomLevels::k100);
    zoomComboBox_.addItem(PluginDisplayNames::ChoiceLists::ZoomLevels::k125, PluginIDs::Settings::ZoomLevels::k125);
    zoomComboBox_.addItem(PluginDisplayNames::ChoiceLists::ZoomLevels::k150, PluginIDs::Settings::ZoomLevels::k150);
    zoomComboBox_.addItem(PluginDisplayNames::ChoiceLists::ZoomLevels::k200, PluginIDs::Settings::ZoomLevels::k200);
    zoomComboBox_.setSelectedId(PluginIDs::Settings::ZoomLevels::k100, juce::dontSendNotification);
    addAndMakeVisible(zoomComboBox_);
}

void HeaderPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kHeaderPanelBackground));
}

void HeaderPanel::resized()
{
    const auto bounds = getLocalBounds();
    const auto spacing = getSpacing();
    const auto controlY = (height_ - kControlHeight_) / 2;
    
    int currentX = kLeftPadding_;
    
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

void HeaderPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    tss::propagateSkin(skin, &skinLabel_, &skinComboBox_, &zoomLabel_, &zoomComboBox_);
}

