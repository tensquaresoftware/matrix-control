#include "HeaderPanel.h"

#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

using tss::SkinColourId;

HeaderPanel::HeaderPanel(tss::ISkin& skin, int width, int height)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , skinLabel_(kSkinLabelWidth_, kControlHeight_, tss::headerPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kSkinLabel, tss::LabelStyle::HeaderPanel)
    , skinComboBox_(kComboBoxWidth_, kControlHeight_, tss::comboBoxLookFromSkin(skin), tss::ComboBox::Style::ButtonLike)
    , uiScaleLabel_(kUiScaleLabelWidth_, kControlHeight_, tss::headerPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kUiScaleLabel, tss::LabelStyle::HeaderPanel)
    , uiScaleComboBox_(kComboBoxWidth_, kControlHeight_, tss::comboBoxLookFromSkin(skin), tss::ComboBox::Style::ButtonLike)
    , uiElementsButton_(kUiElementsButtonWidth_, kControlHeight_, tss::buttonLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kUiElementsButton)
{
    setOpaque(true);

    addAndMakeVisible(uiScaleLabel_);

    uiScaleComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k50, PluginIDs::Settings::ScaleLevels::k50);
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k75, PluginIDs::Settings::ScaleLevels::k75);
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k100, PluginIDs::Settings::ScaleLevels::k100);
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k125, PluginIDs::Settings::ScaleLevels::k125);
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k150, PluginIDs::Settings::ScaleLevels::k150);
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k175, PluginIDs::Settings::ScaleLevels::k175);
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k200, PluginIDs::Settings::ScaleLevels::k200);
    uiScaleComboBox_.setSelectedId(PluginIDs::Settings::ScaleLevels::k100, juce::dontSendNotification);
    addAndMakeVisible(uiScaleComboBox_);

    addAndMakeVisible(skinLabel_);

    skinComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kBlack,
                         static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack));
    skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kCream,
                         static_cast<int>(tss::Skin::SkinComboBoxItemId::kCream));
    skinComboBox_.setSelectedId(static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack), juce::dontSendNotification);
    addAndMakeVisible(skinComboBox_);

    uiElementsButton_.setClickingTogglesState(true);
    addAndMakeVisible(uiElementsButton_);
}

void HeaderPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kHeaderPanelBackground));
}

void HeaderPanel::resized()
{
    const auto bounds = getLocalBounds();
    const float sf = uiScale_;
    const float gap = static_cast<float>(kGap_) * sf;
    const float controlHeight = static_cast<float>(kControlHeight_) * sf;
    const float scaledHeight = static_cast<float>(height_) * sf;
    const float controlY = (scaledHeight - controlHeight) * 0.5f;

    const float leftPadding = static_cast<float>(kLeftPadding_) * sf;
    const float uiScaleLabelWidth = static_cast<float>(kUiScaleLabelWidth_) * sf;
    const float comboBoxWidth = static_cast<float>(kComboBoxWidth_) * sf;
    const float skinLabelWidth = static_cast<float>(kSkinLabelWidth_) * sf;
    const float uiElementsButtonWidth = static_cast<float>(kUiElementsButtonWidth_) * sf;
    const float rightPadding = static_cast<float>(kRightPadding_) * sf;

    const float originX = static_cast<float>(bounds.getX()) + leftPadding;
    const float x0 = originX;
    const float x1 = originX + uiScaleLabelWidth + gap;
    const float x2 = originX + uiScaleLabelWidth + gap + comboBoxWidth + gap * 2.0f;
    const float x3 = originX + uiScaleLabelWidth + gap + comboBoxWidth + gap * 2.0f + skinLabelWidth + gap;
    const float uiElementsButtonX = static_cast<float>(bounds.getRight()) - rightPadding - uiElementsButtonWidth;

    const int uiScaleLabelX = juce::roundToInt(x0);
    const int uiScaleComboBoxX = juce::roundToInt(x1);
    const int skinLabelX = juce::roundToInt(x2);
    const int skinComboBoxX = juce::roundToInt(x3);
    const int uiElementsButtonBoundsX = juce::roundToInt(uiElementsButtonX);

    const int y = juce::roundToInt(static_cast<float>(bounds.getY()) + controlY);
    const int h = juce::roundToInt(controlHeight);

    uiScaleLabel_.setBounds(uiScaleLabelX, y, juce::roundToInt(uiScaleLabelWidth), h);
    uiScaleLabel_.setUiScale(uiScale_);

    uiScaleComboBox_.setBounds(uiScaleComboBoxX, y, juce::roundToInt(comboBoxWidth), h);
    uiScaleComboBox_.setUiScale(uiScale_);

    skinLabel_.setBounds(skinLabelX, y, juce::roundToInt(skinLabelWidth), h);
    skinLabel_.setUiScale(uiScale_);

    skinComboBox_.setBounds(skinComboBoxX, y, juce::roundToInt(comboBoxWidth), h);
    skinComboBox_.setUiScale(uiScale_);

    uiElementsButton_.setBounds(uiElementsButtonBoundsX, y, juce::roundToInt(uiElementsButtonWidth), h);
    uiElementsButton_.setUiScale(uiScale_);
}

void HeaderPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    skinLabel_.setLook(tss::headerPanelLabelLookFromSkin(skin));
    skinComboBox_.setLook(tss::comboBoxLookFromSkin(skin));
    skinComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    uiScaleLabel_.setLook(tss::headerPanelLabelLookFromSkin(skin));
    uiScaleComboBox_.setLook(tss::comboBoxLookFromSkin(skin));
    uiScaleComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    uiElementsButton_.setLook(tss::buttonLookFromSkin(skin));
}

void HeaderPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    repaint();
}
