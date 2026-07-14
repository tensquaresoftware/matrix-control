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
#include "GUI/Tests/TestHierarchicalComboBoxes.h"
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
#include "GUI/Tests/TestVisualDebug.h"
#include "GUI/Tests/TestTrackGeneratorDisplays.h"
#include "GUI/Tests/TestVerticalSeparators.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    constexpr int kFirstComboItemId_ = 1;
    constexpr int kLastComboItemId_ = 21;

    class TestContentHost : public juce::Component
    {
    public:
        void paint(juce::Graphics& g) override
        {
            TestVisualDebugPainter::paintWidgetDrawingAreaBackground(g, *this);
        }

        void paintOverChildren(juce::Graphics& g) override
        {
            const auto& settings = TestVisualDebugSettings::get();
            const auto widgetArea = TestVisualDebugPainter::findWidgetDrawingArea(*this);
            const auto gridOrigin = TestVisualDebugPainter::findFiftyPercentWidgetOrigin(*this);

            if (settings.boundsEnabled)
            {
                TestVisualDebugPainter::paintTransparentWidgetBounds(
                    *this,
                    *this,
                    g,
                    settings.boundsColour);
            }

            if (settings.gridEnabled)
            {
                TestVisualDebugPainter::paintPixelGrid(
                    g,
                    widgetArea,
                    gridOrigin,
                    settings.gridCellSize,
                    settings.gridColour);
            }
        }
    };

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
    setWantsKeyboardFocus(true);
    setFocusContainerType(juce::Component::FocusContainerType::focusContainer);

    skin_ = &skin;
    widgetFactory_ = std::make_unique<WidgetFactory>(apvts);
    TSS::ComboBox::setPopupLayoutDimensions(layoutDimensions_.popupMenu);

    createHeaderControls();
    createVisualDebugControls();

    testContentHost_ = std::make_unique<TestContentHost>();
    testViewport_.setViewedComponent(testContentHost_.get(), false);
    testViewport_.setScrollBarsShown(true, false);
    addAndMakeVisible(testViewport_);

    createTestPages(apvts);

    restorePersistedWidgetSelection();
    setSize(width, height);
    updateVisibleTests();
}

TestComponent::~TestComponent() = default;

void TestComponent::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;

    if (testButtons_ != nullptr)
        testButtons_->setSkin(skin);
    if (testToggles_ != nullptr)
        testToggles_->setSkin(skin);
    if (testSliders_ != nullptr)
        testSliders_->setSkin(skin);
    if (testComboBoxes_ != nullptr)
        testComboBoxes_->setSkin(skin);
    if (testHierarchicalComboBoxes_ != nullptr)
        testHierarchicalComboBoxes_->setSkin(skin);
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

    updateWidgetLabelLook();
    gridColourPicker_.repaint();
    boundsColourPicker_.repaint();

    layoutTestContentHost();
}

void TestComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(kBackgroundColour_));
}

void TestComponent::resized()
{
    const int topRowHeight = juce::jmax(kWidgetLabelHeight_, kWidgetSelectorHeight_);
    const int testsY = kPadding_ + topRowHeight + kGap_;

    layoutHeaderControls(topRowHeight);

    testViewport_.setBounds(kPadding_, testsY, getWidth() - (2 * kPadding_), getHeight() - testsY - kPadding_);
    layoutTestContentHost();
}

void TestComponent::createHeaderControls()
{
    updateWidgetLabelLook();
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

void TestComponent::createVisualDebugControls()
{
    auto& settings = TestVisualDebugSettings::get();

    gridToggle_.setButtonText("GRID :");
    gridToggle_.setToggleState(settings.gridEnabled, juce::dontSendNotification);
    gridToggle_.onClick = [this]
    {
        TestVisualDebugSettings::get().gridEnabled = gridToggle_.getToggleState();
        repaintVisualDebugLayers();
    };
    addAndMakeVisible(gridToggle_);

    gridSizeSelector_.addItem("4 x 4", 4);
    gridSizeSelector_.addItem("8 x 8", 8);
    gridSizeSelector_.addItem("12 x 12", 12);
    gridSizeSelector_.addItem("16 x 16", 16);
    gridSizeSelector_.setSelectedId(settings.gridCellSize, juce::dontSendNotification);
    gridSizeSelector_.onChange = [this]
    {
        const int selectedId = gridSizeSelector_.getSelectedId();
        if (selectedId == 0)
            return;

        TestVisualDebugSettings::get().gridCellSize = selectedId;
        repaintVisualDebugLayers();
    };
    addAndMakeVisible(gridSizeSelector_);

    gridColourPicker_.setCurrentColour(settings.gridColour, juce::dontSendNotification);
    gridColourPicker_.onColourChanged = [this](juce::Colour colour)
    {
        TestVisualDebugSettings::get().gridColour = colour;
        repaintVisualDebugLayers();
    };
    addAndMakeVisible(gridColourPicker_);

    boundsToggle_.setButtonText("BOUNDS :");
    boundsToggle_.setToggleState(settings.boundsEnabled, juce::dontSendNotification);
    boundsToggle_.onClick = [this]
    {
        TestVisualDebugSettings::get().boundsEnabled = boundsToggle_.getToggleState();
        repaintVisualDebugLayers();
    };
    addAndMakeVisible(boundsToggle_);

    boundsColourPicker_.setCurrentColour(settings.boundsColour, juce::dontSendNotification);
    boundsColourPicker_.onColourChanged = [this](juce::Colour colour)
    {
        TestVisualDebugSettings::get().boundsColour = colour;
        repaintVisualDebugLayers();
    };
    addAndMakeVisible(boundsColourPicker_);
}

void TestComponent::updateWidgetLabelLook()
{
    if (skin_ == nullptr)
        return;

    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    if (widgetLabel_ == nullptr)
        widgetLabel_ = std::make_unique<TSS::Label>(widgetLabelWidth_, kDebugControlHeight_, labelLook, "WIDGET :");
    else
        widgetLabel_->setLook(labelLook);
}

void TestComponent::layoutHeaderControls(int topRowHeight)
{
    const int controlY = kPadding_ + (topRowHeight - kDebugControlHeight_) / 2;
    const int selectorY = kPadding_ + (topRowHeight - kWidgetSelectorHeight_) / 2;

    const int rawLabelWidth = (skin_ != nullptr)
        ? juce::roundToInt(juce::GlyphArrangement::getStringWidth(
            TSS::labelLookFromSkin(*skin_).font,
            "WIDGET :")) + kGap_
        : kGap_;
    widgetLabelWidth_ = juce::jmax(4, alignToMultipleOf4(rawLabelWidth) - kWidgetLabelWidthReduction_ + kWidgetLabelExtraWidth_);

    int x = kPadding_;
    const int maxX = getWidth() - kPadding_;

    if (widgetLabel_ != nullptr && widgetLabel_->isVisible())
    {
        widgetLabel_->setBounds(x, controlY, widgetLabelWidth_, kDebugControlHeight_);
        x += widgetLabelWidth_ + kGap_;
    }

    widgetSelector_.setBounds(x, selectorY, kWidgetSelectorWidth_, kWidgetSelectorHeight_);
    x += kWidgetSelectorWidth_ + kDebugSectionLeadingGap_;

    const int debugControlsWidth = kGridToggleWidth_ + kDebugControlGap_ + kGridSizeSelectorWidth_ + kDebugControlGap_
        + kColourPickerSize_ + kDebugSectionLeadingGap_ + kBoundsToggleWidth_ + kDebugControlGap_ + kColourPickerSize_;
    const bool showDebugControls = x + debugControlsWidth <= maxX;

    gridToggle_.setVisible(showDebugControls);
    gridSizeSelector_.setVisible(showDebugControls);
    gridColourPicker_.setVisible(showDebugControls);
    boundsToggle_.setVisible(showDebugControls);
    boundsColourPicker_.setVisible(showDebugControls);

    if (!showDebugControls)
        return;

    gridToggle_.setBounds(x, controlY, kGridToggleWidth_, kDebugControlHeight_);
    x += kGridToggleWidth_ + kDebugControlGap_;

    gridSizeSelector_.setBounds(x, controlY, kGridSizeSelectorWidth_, kDebugControlHeight_);
    x += kGridSizeSelectorWidth_ + kDebugControlGap_;

    gridColourPicker_.setBounds(x, controlY, kColourPickerSize_, kColourPickerSize_);
    x += kColourPickerSize_ + kDebugSectionLeadingGap_;

    boundsToggle_.setBounds(x, controlY, kBoundsToggleWidth_, kDebugControlHeight_);
    x += kBoundsToggleWidth_ + kDebugControlGap_;

    if (x + kColourPickerSize_ <= maxX)
        boundsColourPicker_.setBounds(x, controlY, kColourPickerSize_, kColourPickerSize_);
}

bool TestComponent::keyPressed(const juce::KeyPress& key)
{
    const auto commandModifier = juce::ModifierKeys::commandModifier;

    if (key == juce::KeyPress('+', commandModifier, 0)
        || key == juce::KeyPress('=', commandModifier, 0)
        || key == juce::KeyPress(juce::KeyPress::numberPadAdd, commandModifier, 0))
    {
        zoomContentIn();
        return true;
    }

    if (key == juce::KeyPress('-', commandModifier, 0)
        || key == juce::KeyPress(juce::KeyPress::numberPadSubtract, commandModifier, 0))
    {
        zoomContentOut();
        return true;
    }

    if (isZoomResetKey(key))
    {
        resetContentZoom();
        return true;
    }

    return false;
}

bool TestComponent::isZoomResetKey(const juce::KeyPress& key)
{
    if (!key.getModifiers().isCommandDown())
        return false;

    const auto keyCode = key.getKeyCode();
    return keyCode == '0'
        || keyCode == juce::KeyPress::numberPad0
        || key.getTextCharacter() == '0';
}

void TestComponent::zoomContentIn()
{
    setContentZoom(contentZoom_ * kContentZoomStepFactor_);
}

void TestComponent::zoomContentOut()
{
    setContentZoom(contentZoom_ / kContentZoomStepFactor_);
}

void TestComponent::resetContentZoom()
{
    setContentZoom(1.0f);
}

void TestComponent::setContentZoom(float zoom)
{
    contentZoom_ = juce::jlimit(kMinContentZoom_, kMaxContentZoom_, zoom);
    applyContentZoom();
}

void TestComponent::applyContentZoom()
{
    if (testContentHost_ == nullptr || baseContentWidth_ <= 0 || baseContentHeight_ <= 0)
        return;

    testContentHost_->setTransform(juce::AffineTransform());

    const int zoomedWidth = juce::jmax(1, juce::roundToInt(static_cast<float>(baseContentWidth_) * contentZoom_));
    const int zoomedHeight = juce::jmax(1, juce::roundToInt(static_cast<float>(baseContentHeight_) * contentZoom_));
    testContentHost_->setSize(zoomedWidth, zoomedHeight);

    for (auto* child : testContentHost_->getChildren())
    {
        if (child == nullptr || !child->isVisible())
            continue;

        const int childWidth = juce::jmax(1, juce::roundToInt(static_cast<float>(baseContentWidth_) * contentZoom_));
        const int childHeight = juce::jmax(1, juce::roundToInt(static_cast<float>(baseContentHeight_) * contentZoom_));
        child->setBounds(0, 0, childWidth, childHeight);
        break;
    }

    repaintVisualDebugLayers();
}

void TestComponent::repaintVisualDebugLayers()
{
    if (testContentHost_ != nullptr)
        testContentHost_->repaint();
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

    testHierarchicalComboBoxes_ = std::make_unique<TestHierarchicalComboBoxes>(
        *skin_,
        patchManager.patchMutator.comboBoxes.patchMutatorHistoryWidth,
        patchManager.patchMutator.comboBoxes.standardHeight);
    testContentHost_->addAndMakeVisible(*testHierarchicalComboBoxes_);

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
    widgetSelector_.addItem("HierarchicalComboBox", static_cast<int>(TestWidgetType::HierarchicalComboBox));
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
    setVisibleForType(TestWidgetType::HierarchicalComboBox, testHierarchicalComboBoxes_.get());
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
    repaintVisualDebugLayers();
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
        { TestWidgetType::HierarchicalComboBox, testHierarchicalComboBoxes_.get(),
          [this] { return testHierarchicalComboBoxes_ != nullptr ? testHierarchicalComboBoxes_->getPreferredWidth() : 0; },
          [this] { return testHierarchicalComboBoxes_ != nullptr ? testHierarchicalComboBoxes_->getPreferredHeight() : 0; } },
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

        const int zoomedWidth = juce::jmax(1, juce::roundToInt(static_cast<float>(page.preferredWidth()) * contentZoom_));
        const int zoomedHeight = juce::jmax(1, juce::roundToInt(static_cast<float>(page.preferredHeight()) * contentZoom_));
        page.component->setBounds(0, 0, zoomedWidth, zoomedHeight);
    }

    if (contentWidth == 0 || contentHeight == 0)
    {
        for (const auto& page : pages)
        {
            if (static_cast<int>(page.type) != selectedId || page.component == nullptr)
                continue;

            contentWidth = page.preferredWidth();
            contentHeight = page.preferredHeight();

            const int zoomedWidth = juce::jmax(1, juce::roundToInt(static_cast<float>(contentWidth) * contentZoom_));
            const int zoomedHeight = juce::jmax(1, juce::roundToInt(static_cast<float>(contentHeight) * contentZoom_));
            page.component->setBounds(0, 0, zoomedWidth, zoomedHeight);
            break;
        }
    }

    const int viewportHeight = juce::jmax(1, testViewport_.getHeight());
    baseContentWidth_ = juce::jmax(contentWidth, 1);
    baseContentHeight_ = juce::jmax(contentHeight, viewportHeight);
    applyContentZoom();
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
