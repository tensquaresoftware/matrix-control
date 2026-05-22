#include "TestComponent.h"

#include <juce_graphics/juce_graphics.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestButtons.h"
#include "GUI/Tests/TestSliders.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    constexpr int kFirstComboItemId_ = 1;
    constexpr int kLastComboItemId_ = 12;
}

TestComponent::TestComponent(tss::ISkin& skin, juce::ValueTree& settingsState, int width, int height)
    : skin_(skin)
    , settingsState_(settingsState)
{
    setOpaque(true);
    setSize(width, height);

    createHeaderControls();

    testButtons_ = std::make_unique<TestButtons>(skin_);
    addAndMakeVisible(*testButtons_);
    testSliders_ = std::make_unique<TestSliders>(skin_);
    addAndMakeVisible(*testSliders_);

    restorePersistedWidgetSelection();
    updateVisibleTests();
}

TestComponent::~TestComponent() = default;

void TestComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(kBackgroundColour_));
}

void TestComponent::resized()
{
    const int labelY = kPadding_;
    const int comboX = kPadding_ + widgetLabelWidth_ + kGap_;
    const int comboY = kPadding_ + (kWidgetLabelHeight_ - kWidgetSelectorHeight_) / 2;
    const int topRowHeight = juce::jmax(kWidgetLabelHeight_, kWidgetSelectorHeight_);
    const int testsY = kPadding_ + topRowHeight + kGap_;

    if (widgetLabel_ != nullptr)
        widgetLabel_->setBounds(kPadding_, labelY, widgetLabelWidth_, kWidgetLabelHeight_);

    widgetSelector_.setBounds(comboX, comboY, kWidgetSelectorWidth_, kWidgetSelectorHeight_);

    if (testButtons_ != nullptr)
        testButtons_->setBounds(kPadding_, testsY, getWidth() - (2 * kPadding_), getHeight() - testsY - kPadding_);
    if (testSliders_ != nullptr)
        testSliders_->setBounds(kPadding_, testsY, getWidth() - (2 * kPadding_), getHeight() - testsY - kPadding_);
}

void TestComponent::createHeaderControls()
{
    const auto labelLook = tss::labelLookFromSkin(skin_);

    const int rawLabelWidth = juce::roundToInt(juce::TextLayout::getStringWidth(labelLook.font, "WIDGET :")) + kGap_;
    widgetLabelWidth_ = alignToMultipleOf4(rawLabelWidth);

    widgetLabel_ = std::make_unique<tss::Label>(
        widgetLabelWidth_,
        kWidgetLabelHeight_,
        labelLook,
        "WIDGET :");
    addAndMakeVisible(*widgetLabel_);

    widgetSelector_.setName("ButtonStyle");
    populateWidgetSelector();
    widgetSelector_.onChange = [this]
    {
        const int selectedId = widgetSelector_.getSelectedId();
        persistWidgetSelection(selectedId);
        updateVisibleTests();
    };
    addAndMakeVisible(widgetSelector_);
}

void TestComponent::populateWidgetSelector()
{
    widgetSelector_.addItem("Button", static_cast<int>(TestWidgetType::Button));
    widgetSelector_.addItem("Toggle", static_cast<int>(TestWidgetType::Toggle));
    widgetSelector_.addItem("Slider", static_cast<int>(TestWidgetType::Slider));
    widgetSelector_.addItem("ComboBox", static_cast<int>(TestWidgetType::ComboBox));
    widgetSelector_.addItem("NumberBox", static_cast<int>(TestWidgetType::NumberBox));
    widgetSelector_.addItem("Label", static_cast<int>(TestWidgetType::Label));
    widgetSelector_.addItem("GroupLabel", static_cast<int>(TestWidgetType::GroupLabel));
    widgetSelector_.addItem("ModuleHeader", static_cast<int>(TestWidgetType::ModuleHeader));
    widgetSelector_.addItem("SectionHeader", static_cast<int>(TestWidgetType::SectionHeader));
    widgetSelector_.addItem("EnvelopeDisplay", static_cast<int>(TestWidgetType::EnvelopeDisplay));
    widgetSelector_.addItem("TrackGeneratorDisplay", static_cast<int>(TestWidgetType::TrackGeneratorDisplay));
    widgetSelector_.addItem("PatchNameDisplay", static_cast<int>(TestWidgetType::PatchNameDisplay));
}

void TestComponent::restorePersistedWidgetSelection()
{
    const int defaultSelectionId = getDefaultWidgetSelectionId();
    const int savedSelectionId = static_cast<int>(settingsState_.getProperty(PluginIDs::Settings::kTestWidgetId, defaultSelectionId));
    const int boundedSelectionId = juce::jlimit(kFirstComboItemId_, kLastComboItemId_, savedSelectionId);

    widgetSelector_.setSelectedId(boundedSelectionId, juce::dontSendNotification);
}

void TestComponent::persistWidgetSelection(int selectedId)
{
    if (selectedId == 0)
        return;

    settingsState_.setProperty(PluginIDs::Settings::kTestWidgetId, selectedId, nullptr);
}

void TestComponent::updateVisibleTests()
{
    if (testButtons_ == nullptr || testSliders_ == nullptr)
        return;

    const int selectedId = widgetSelector_.getSelectedId();
    const bool shouldShowButtons = selectedId == static_cast<int>(TestWidgetType::Button);
    const bool shouldShowSliders = selectedId == static_cast<int>(TestWidgetType::Slider);

    testButtons_->setVisible(shouldShowButtons);
    testSliders_->setVisible(shouldShowSliders);
}

int TestComponent::alignToMultipleOf4(int value)
{
    if (value <= 0)
        return 4;

    const int remainder = value % 4;
    if (remainder == 0)
        return value;

    return value + (4 - remainder);
}

int TestComponent::getDefaultWidgetSelectionId()
{
    return static_cast<int>(TestWidgetType::Button);
}
