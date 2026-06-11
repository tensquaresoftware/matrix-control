#include "TestComponent.h"

#include <functional>

#include <juce_graphics/juce_graphics.h>

#include "GUI/Factories/DimensionFactory.h"
#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestActivityLeds.h"
#include "GUI/Tests/TestButtons.h"
#include "GUI/Tests/TestComboBoxes.h"
#include "GUI/Tests/TestEnvelopeDisplays.h"
#include "GUI/Tests/TestGroupLabels.h"
#include "GUI/Tests/TestHorizontalSeparators.h"
#include "GUI/Tests/TestLabels.h"
#include "GUI/Tests/TestModulationBusCells.h"
#include "GUI/Tests/TestModulationBusHeaders.h"
#include "GUI/Tests/TestModuleHeaders.h"
#include "GUI/Tests/TestNumberBoxes.h"
#include "GUI/Tests/TestParameterCells.h"
#include "GUI/Tests/TestPatchNameDisplays.h"
#include "GUI/Tests/TestPeakIndicators.h"
#include "GUI/Tests/TestPopupMenus.h"
#include "GUI/Tests/TestSectionHeaders.h"
#include "GUI/Tests/TestSliders.h"
#include "GUI/Tests/TestToggles.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Tests/TestTrackGeneratorDisplays.h"
#include "GUI/Tests/TestVerticalSeparators.h"
#include "GUI/Widgets/ComboBox.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    constexpr int kFirstComboItemId_ = 1;
    constexpr int kLastComboItemId_ = 20;

    struct TestPageEntry
    {
        TestComponent::TestWidgetType type;
        juce::Component* component;
        std::function<int()> preferredWidth;
        std::function<int()> preferredHeight;
    };
}

TestComponent::TestComponent(TSS::ISkin& skin,
                             juce::AudioProcessorValueTreeState& apvts,
                             juce::ValueTree& settingsState,
                             int width,
                             int height)
    : settingsState_(settingsState)
    , layoutDimensions_(DimensionFactory::buildGuiLayoutDimensions())
{
    setOpaque(true);
    setSize(width, height);

    skin_ = &skin;
    widgetFactory_ = std::make_unique<WidgetFactory>(apvts);
    TSS::ComboBox::setPopupLayoutDimensions(layoutDimensions_.popupMenu);

    createHeaderControls();

    testContentHost_ = std::make_unique<juce::Component>();
    testViewport_.setViewedComponent(testContentHost_.get(), false);
    testViewport_.setScrollBarsShown(true, false);
    addAndMakeVisible(testViewport_);

    createTestPages(apvts);

    restorePersistedWidgetSelection();
    updateVisibleTests();
}

TestComponent::~TestComponent() = default;

void TestComponent::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;

    if (widgetLabel_ != nullptr)
        widgetLabel_->setLook(TSS::labelLookFromSkin(skin));

    if (testButtons_ != nullptr)
        testButtons_->setSkin(skin);
    if (testToggles_ != nullptr)
        testToggles_->setSkin(skin);
    if (testSliders_ != nullptr)
        testSliders_->setSkin(skin);
    if (testComboBoxes_ != nullptr)
        testComboBoxes_->setSkin(skin);
    if (testNumberBoxes_ != nullptr)
        testNumberBoxes_->setSkin(skin);
    if (testLabels_ != nullptr)
        testLabels_->setSkin(skin);
    if (testGroupLabels_ != nullptr)
        testGroupLabels_->setSkin(skin);
    if (testModuleHeaders_ != nullptr)
        testModuleHeaders_->setSkin(skin);
    if (testSectionHeaders_ != nullptr)
        testSectionHeaders_->setSkin(skin);
    if (testEnvelopeDisplays_ != nullptr)
        testEnvelopeDisplays_->setSkin(skin);
    if (testTrackGeneratorDisplays_ != nullptr)
        testTrackGeneratorDisplays_->setSkin(skin);
    if (testPatchNameDisplays_ != nullptr)
        testPatchNameDisplays_->setSkin(skin);
    if (testHorizontalSeparators_ != nullptr)
        testHorizontalSeparators_->setSkin(skin);
    if (testVerticalSeparators_ != nullptr)
        testVerticalSeparators_->setSkin(skin);
    if (testActivityLeds_ != nullptr)
        testActivityLeds_->setSkin(skin);
    if (testPeakIndicators_ != nullptr)
        testPeakIndicators_->setSkin(skin);
    if (testParameterCells_ != nullptr)
        testParameterCells_->setSkin(skin);
    if (testModulationBusCells_ != nullptr)
        testModulationBusCells_->setSkin(skin);
    if (testModulationBusHeaders_ != nullptr)
        testModulationBusHeaders_->setSkin(skin);
    if (testPopupMenus_ != nullptr)
        testPopupMenus_->setSkin(skin);

    layoutTestContentHost();
}

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

    testViewport_.setBounds(kPadding_, testsY, getWidth() - (2 * kPadding_), getHeight() - testsY - kPadding_);
    layoutTestContentHost();
}

void TestComponent::createHeaderControls()
{
    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    const int rawLabelWidth = juce::roundToInt(juce::TextLayout::getStringWidth(labelLook.font, "WIDGET :")) + kGap_;
    widgetLabelWidth_ = alignToMultipleOf4(rawLabelWidth);

    widgetLabel_ = std::make_unique<TSS::Label>(
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

void TestComponent::createTestPages(juce::AudioProcessorValueTreeState& apvts)
{
    const auto& body = layoutDimensions_.body;
    const auto& patchEdit = body.patchEdit;
    const auto& shared = body.shared;
    const auto& matrixMod = shared.matrixModulation;
    const auto& patchManager = shared.patchManager;

    testButtons_ = std::make_unique<TestButtons>(*skin_);
    testContentHost_->addAndMakeVisible(*testButtons_);

    testToggles_ = std::make_unique<TestToggles>(*skin_, patchManager.patchMutator.toggles);
    testContentHost_->addAndMakeVisible(*testToggles_);

    testSliders_ = std::make_unique<TestSliders>(*skin_);
    testContentHost_->addAndMakeVisible(*testSliders_);

    testComboBoxes_ = std::make_unique<TestComboBoxes>(*skin_);
    testContentHost_->addAndMakeVisible(*testComboBoxes_);

    testNumberBoxes_ = std::make_unique<TestNumberBoxes>(*skin_, patchManager.internalPatches.numberBoxes);
    testContentHost_->addAndMakeVisible(*testNumberBoxes_);

    testLabels_ = std::make_unique<TestLabels>(*skin_);
    testContentHost_->addAndMakeVisible(*testLabels_);

    testGroupLabels_ = std::make_unique<TestGroupLabels>(*skin_, patchManager.internalPatches.groupLabels);
    testContentHost_->addAndMakeVisible(*testGroupLabels_);

    const int testRowWidth = layoutDimensions_.editor.width - (2 * kPadding_);

    auto moduleHeaderDims = layoutDimensions_.patchEditModuleHeader;
    moduleHeaderDims.patchEditTitleBandWidth = TestScaleColumns::bandWidthForTestRow(testRowWidth);
    testModuleHeaders_ = std::make_unique<TestModuleHeaders>(*skin_, moduleHeaderDims);
    testContentHost_->addAndMakeVisible(*testModuleHeaders_);

    constexpr int kSectionHeaderLineStartDesign = 86;
    const int sectionHeaderTestWidthAtFive = patchEdit.sectionHeaderWidth / 5;
    const int sectionHeaderTestWidth = kSectionHeaderLineStartDesign
        + (sectionHeaderTestWidthAtFive - kSectionHeaderLineStartDesign) / 2;
    testSectionHeaders_ = std::make_unique<TestSectionHeaders>(
        *skin_,
        sectionHeaderTestWidth,
        patchEdit.sectionHeaderHeight);
    testContentHost_->addAndMakeVisible(*testSectionHeaders_);

    testEnvelopeDisplays_ = std::make_unique<TestEnvelopeDisplays>(*skin_, patchEdit.displays.childBand);
    testContentHost_->addAndMakeVisible(*testEnvelopeDisplays_);

    testTrackGeneratorDisplays_ = std::make_unique<TestTrackGeneratorDisplays>(*skin_, patchEdit.displays.trackGeneratorBand);
    testContentHost_->addAndMakeVisible(*testTrackGeneratorDisplays_);

    testPatchNameDisplays_ = std::make_unique<TestPatchNameDisplays>(*skin_, patchEdit.displays.patchName);
    testContentHost_->addAndMakeVisible(*testPatchNameDisplays_);

    testHorizontalSeparators_ = std::make_unique<TestHorizontalSeparators>(
        *skin_,
        layoutDimensions_.patchEditParameterCell.separatorWidth,
        layoutDimensions_.patchEditParameterCell.horizontalSeparatorHeight);
    testContentHost_->addAndMakeVisible(*testHorizontalSeparators_);

    testVerticalSeparators_ = std::make_unique<TestVerticalSeparators>(*skin_, body.separators);
    testContentHost_->addAndMakeVisible(*testVerticalSeparators_);

    testActivityLeds_ = std::make_unique<TestActivityLeds>(*skin_, layoutDimensions_.activityLed.size);
    testContentHost_->addAndMakeVisible(*testActivityLeds_);

    testPeakIndicators_ = std::make_unique<TestPeakIndicators>(
        *skin_,
        layoutDimensions_.activityLed.size,
        layoutDimensions_.patchEditParameterCell.comboBoxHeight);
    testContentHost_->addAndMakeVisible(*testPeakIndicators_);

    testParameterCells_ = std::make_unique<TestParameterCells>(
        *skin_,
        *widgetFactory_,
        apvts,
        layoutDimensions_.patchEditParameterCell);
    testContentHost_->addAndMakeVisible(*testParameterCells_);

    testModulationBusCells_ = std::make_unique<TestModulationBusCells>(
        *skin_,
        *widgetFactory_,
        apvts,
        matrixMod.busCell,
        matrixMod.width,
        matrixMod.modulationBusRowHeight);
    testContentHost_->addAndMakeVisible(*testModulationBusCells_);

    testModulationBusHeaders_ = std::make_unique<TestModulationBusHeaders>(
        *skin_,
        matrixMod.busHeaderWidth,
        matrixMod.busHeaderHeight,
        matrixMod.busHeader);
    testContentHost_->addAndMakeVisible(*testModulationBusHeaders_);

    testPopupMenus_ = std::make_unique<TestPopupMenus>(
        *skin_,
        patchManager.computerPatches.comboBoxes.patchManagerComputerPatchesWidth,
        patchManager.computerPatches.comboBoxes.standardHeight);
    testContentHost_->addAndMakeVisible(*testPopupMenus_);
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
    widgetSelector_.addItem("HorizontalSeparator", static_cast<int>(TestWidgetType::HorizontalSeparator));
    widgetSelector_.addItem("VerticalSeparator", static_cast<int>(TestWidgetType::VerticalSeparator));
    widgetSelector_.addItem("Led", static_cast<int>(TestWidgetType::ActivityLed));
    widgetSelector_.addItem("PeakIndicator", static_cast<int>(TestWidgetType::PeakIndicator));
    widgetSelector_.addItem("ParameterCell", static_cast<int>(TestWidgetType::ParameterCell));
    widgetSelector_.addItem("ModulationBusCell", static_cast<int>(TestWidgetType::ModulationBusCell));
    widgetSelector_.addItem("ModulationBusHeader", static_cast<int>(TestWidgetType::ModulationBusHeader));
    widgetSelector_.addItem("PopupMenu", static_cast<int>(TestWidgetType::PopupMenu));
}

void TestComponent::restorePersistedWidgetSelection()
{
    const int defaultSelectionId = getDefaultWidgetSelectionId();
    const int savedSelectionId = static_cast<int>(settingsState_.getProperty(PluginIDs::Settings::kTestWidget, defaultSelectionId));
    const int boundedSelectionId = juce::jlimit(kFirstComboItemId_, kLastComboItemId_, savedSelectionId);

    widgetSelector_.setSelectedId(boundedSelectionId, juce::dontSendNotification);
}

void TestComponent::persistWidgetSelection(int selectedId)
{
    if (selectedId == 0)
        return;

    settingsState_.setProperty(PluginIDs::Settings::kTestWidget, selectedId, nullptr);
}

void TestComponent::updateVisibleTests()
{
    const int selectedId = widgetSelector_.getSelectedId();

    const auto setVisibleForType = [&](TestWidgetType type, juce::Component* component)
    {
        if (component != nullptr)
            component->setVisible(selectedId == static_cast<int>(type));
    };

    setVisibleForType(TestWidgetType::Button, testButtons_.get());
    setVisibleForType(TestWidgetType::Toggle, testToggles_.get());
    setVisibleForType(TestWidgetType::Slider, testSliders_.get());
    setVisibleForType(TestWidgetType::ComboBox, testComboBoxes_.get());
    setVisibleForType(TestWidgetType::NumberBox, testNumberBoxes_.get());
    setVisibleForType(TestWidgetType::Label, testLabels_.get());
    setVisibleForType(TestWidgetType::GroupLabel, testGroupLabels_.get());
    setVisibleForType(TestWidgetType::ModuleHeader, testModuleHeaders_.get());
    setVisibleForType(TestWidgetType::SectionHeader, testSectionHeaders_.get());
    setVisibleForType(TestWidgetType::EnvelopeDisplay, testEnvelopeDisplays_.get());
    setVisibleForType(TestWidgetType::TrackGeneratorDisplay, testTrackGeneratorDisplays_.get());
    setVisibleForType(TestWidgetType::PatchNameDisplay, testPatchNameDisplays_.get());
    setVisibleForType(TestWidgetType::HorizontalSeparator, testHorizontalSeparators_.get());
    setVisibleForType(TestWidgetType::VerticalSeparator, testVerticalSeparators_.get());
    setVisibleForType(TestWidgetType::ActivityLed, testActivityLeds_.get());
    setVisibleForType(TestWidgetType::PeakIndicator, testPeakIndicators_.get());
    setVisibleForType(TestWidgetType::ParameterCell, testParameterCells_.get());
    setVisibleForType(TestWidgetType::ModulationBusCell, testModulationBusCells_.get());
    setVisibleForType(TestWidgetType::ModulationBusHeader, testModulationBusHeaders_.get());
    setVisibleForType(TestWidgetType::PopupMenu, testPopupMenus_.get());

    layoutTestContentHost();
}

void TestComponent::layoutTestContentHost()
{
    if (testContentHost_ == nullptr)
        return;

    const TestPageEntry pages[] =
    {
        { TestWidgetType::Button, testButtons_.get(),
          [this] { return testButtons_ != nullptr ? testButtons_->getPreferredWidth() : 0; },
          [this] { return testButtons_ != nullptr ? testButtons_->getPreferredHeight() : 0; } },
        { TestWidgetType::Toggle, testToggles_.get(),
          [this] { return testToggles_ != nullptr ? testToggles_->getPreferredWidth() : 0; },
          [this] { return testToggles_ != nullptr ? testToggles_->getPreferredHeight() : 0; } },
        { TestWidgetType::Slider, testSliders_.get(),
          [this] { return testSliders_ != nullptr ? testSliders_->getPreferredWidth() : 0; },
          [this] { return testSliders_ != nullptr ? testSliders_->getPreferredHeight() : 0; } },
        { TestWidgetType::ComboBox, testComboBoxes_.get(),
          [this] { return testComboBoxes_ != nullptr ? testComboBoxes_->getPreferredWidth() : 0; },
          [this] { return testComboBoxes_ != nullptr ? testComboBoxes_->getPreferredHeight() : 0; } },
        { TestWidgetType::NumberBox, testNumberBoxes_.get(),
          [this] { return testNumberBoxes_ != nullptr ? testNumberBoxes_->getPreferredWidth() : 0; },
          [this] { return testNumberBoxes_ != nullptr ? testNumberBoxes_->getPreferredHeight() : 0; } },
        { TestWidgetType::Label, testLabels_.get(),
          [this] { return testLabels_ != nullptr ? testLabels_->getPreferredWidth() : 0; },
          [this] { return testLabels_ != nullptr ? testLabels_->getPreferredHeight() : 0; } },
        { TestWidgetType::GroupLabel, testGroupLabels_.get(),
          [this] { return testGroupLabels_ != nullptr ? testGroupLabels_->getPreferredWidth() : 0; },
          [this] { return testGroupLabels_ != nullptr ? testGroupLabels_->getPreferredHeight() : 0; } },
        { TestWidgetType::ModuleHeader, testModuleHeaders_.get(),
          [this] { return testModuleHeaders_ != nullptr ? testModuleHeaders_->getPreferredWidth() : 0; },
          [this] { return testModuleHeaders_ != nullptr ? testModuleHeaders_->getPreferredHeight() : 0; } },
        { TestWidgetType::SectionHeader, testSectionHeaders_.get(),
          [this] { return testSectionHeaders_ != nullptr ? testSectionHeaders_->getPreferredWidth() : 0; },
          [this] { return testSectionHeaders_ != nullptr ? testSectionHeaders_->getPreferredHeight() : 0; } },
        { TestWidgetType::EnvelopeDisplay, testEnvelopeDisplays_.get(),
          [this] { return testEnvelopeDisplays_ != nullptr ? testEnvelopeDisplays_->getPreferredWidth() : 0; },
          [this] { return testEnvelopeDisplays_ != nullptr ? testEnvelopeDisplays_->getPreferredHeight() : 0; } },
        { TestWidgetType::TrackGeneratorDisplay, testTrackGeneratorDisplays_.get(),
          [this] { return testTrackGeneratorDisplays_ != nullptr ? testTrackGeneratorDisplays_->getPreferredWidth() : 0; },
          [this] { return testTrackGeneratorDisplays_ != nullptr ? testTrackGeneratorDisplays_->getPreferredHeight() : 0; } },
        { TestWidgetType::PatchNameDisplay, testPatchNameDisplays_.get(),
          [this] { return testPatchNameDisplays_ != nullptr ? testPatchNameDisplays_->getPreferredWidth() : 0; },
          [this] { return testPatchNameDisplays_ != nullptr ? testPatchNameDisplays_->getPreferredHeight() : 0; } },
        { TestWidgetType::HorizontalSeparator, testHorizontalSeparators_.get(),
          [this] { return testHorizontalSeparators_ != nullptr ? testHorizontalSeparators_->getPreferredWidth() : 0; },
          [this] { return testHorizontalSeparators_ != nullptr ? testHorizontalSeparators_->getPreferredHeight() : 0; } },
        { TestWidgetType::VerticalSeparator, testVerticalSeparators_.get(),
          [this] { return testVerticalSeparators_ != nullptr ? testVerticalSeparators_->getPreferredWidth() : 0; },
          [this] { return testVerticalSeparators_ != nullptr ? testVerticalSeparators_->getPreferredHeight() : 0; } },
        { TestWidgetType::ActivityLed, testActivityLeds_.get(),
          [this] { return testActivityLeds_ != nullptr ? testActivityLeds_->getPreferredWidth() : 0; },
          [this] { return testActivityLeds_ != nullptr ? testActivityLeds_->getPreferredHeight() : 0; } },
        { TestWidgetType::PeakIndicator, testPeakIndicators_.get(),
          [this] { return testPeakIndicators_ != nullptr ? testPeakIndicators_->getPreferredWidth() : 0; },
          [this] { return testPeakIndicators_ != nullptr ? testPeakIndicators_->getPreferredHeight() : 0; } },
        { TestWidgetType::ParameterCell, testParameterCells_.get(),
          [this] { return testParameterCells_ != nullptr ? testParameterCells_->getPreferredWidth() : 0; },
          [this] { return testParameterCells_ != nullptr ? testParameterCells_->getPreferredHeight() : 0; } },
        { TestWidgetType::ModulationBusCell, testModulationBusCells_.get(),
          [this] { return testModulationBusCells_ != nullptr ? testModulationBusCells_->getPreferredWidth() : 0; },
          [this] { return testModulationBusCells_ != nullptr ? testModulationBusCells_->getPreferredHeight() : 0; } },
        { TestWidgetType::ModulationBusHeader, testModulationBusHeaders_.get(),
          [this] { return testModulationBusHeaders_ != nullptr ? testModulationBusHeaders_->getPreferredWidth() : 0; },
          [this] { return testModulationBusHeaders_ != nullptr ? testModulationBusHeaders_->getPreferredHeight() : 0; } },
        { TestWidgetType::PopupMenu, testPopupMenus_.get(),
          [this] { return testPopupMenus_ != nullptr ? testPopupMenus_->getPreferredWidth() : 0; },
          [this] { return testPopupMenus_ != nullptr ? testPopupMenus_->getPreferredHeight() : 0; } },
    };

    const int selectedId = widgetSelector_.getSelectedId();
    int contentWidth = 0;
    int contentHeight = 0;

    for (const auto& page : pages)
    {
        if (page.component == nullptr || !page.component->isVisible())
            continue;

        contentWidth = juce::jmax(contentWidth, page.preferredWidth());
        contentHeight = juce::jmax(contentHeight, page.preferredHeight());
        page.component->setBounds(0, 0, page.preferredWidth(), page.preferredHeight());
        page.component->resized();
    }

    if (contentWidth == 0 || contentHeight == 0)
    {
        for (const auto& page : pages)
        {
            if (static_cast<int>(page.type) != selectedId || page.component == nullptr)
                continue;

            contentWidth = page.preferredWidth();
            contentHeight = page.preferredHeight();
            page.component->setBounds(0, 0, contentWidth, contentHeight);
            page.component->resized();
            break;
        }
    }

    const int viewportHeight = testViewport_.getHeight();
    testContentHost_->setSize(juce::jmax(contentWidth, testViewport_.getWidth()),
                              juce::jmax(contentHeight, viewportHeight));
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
