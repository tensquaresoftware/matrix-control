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
    , guiScaleLabel_(skin, kGuiScaleLabelWidth_, kControlHeight_, "GUI :", tss::LabelStyle::HeaderPanel)
    , guiScaleComboBox_(skin, kComboBoxWidth_, kControlHeight_, tss::ComboBox::Style::ButtonLike)
{
    setOpaque(true);
    
    addAndMakeVisible(skinLabel_);
    
    skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kBlack, 
                         static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack));
    skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kCream, 
                         static_cast<int>(tss::Skin::SkinComboBoxItemId::kCream));
    skinComboBox_.setSelectedId(static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack), juce::dontSendNotification);
    addAndMakeVisible(skinComboBox_);
    
    addAndMakeVisible(guiScaleLabel_);
    
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k50, PluginIDs::Settings::ScaleLevels::k50);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k75, PluginIDs::Settings::ScaleLevels::k75);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k90, PluginIDs::Settings::ScaleLevels::k90);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k100, PluginIDs::Settings::ScaleLevels::k100);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k125, PluginIDs::Settings::ScaleLevels::k125);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k150, PluginIDs::Settings::ScaleLevels::k150);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k200, PluginIDs::Settings::ScaleLevels::k200);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k250, PluginIDs::Settings::ScaleLevels::k250);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k300, PluginIDs::Settings::ScaleLevels::k300);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k400, PluginIDs::Settings::ScaleLevels::k400);
    guiScaleComboBox_.setSelectedId(PluginIDs::Settings::ScaleLevels::k100, juce::dontSendNotification);
    addAndMakeVisible(guiScaleComboBox_);
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
    
    guiScaleLabel_.setBounds(
        bounds.getX() + currentX,
        bounds.getY() + controlY,
        kGuiScaleLabelWidth_,
        kControlHeight_
    );
    currentX += kGuiScaleLabelWidth_ + spacing;
    
    guiScaleComboBox_.setBounds(
        bounds.getX() + currentX,
        bounds.getY() + controlY,
        kComboBoxWidth_,
        kControlHeight_
    );
}

void HeaderPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    tss::propagateSkin(skin, &skinLabel_, &skinComboBox_, &guiScaleLabel_, &guiScaleComboBox_);
}

