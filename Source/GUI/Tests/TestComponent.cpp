#include "TestComponent.h"

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
#include "GUI/Tests/TestTrackGeneratorDisplays.h"
#include "GUI/Tests/TestVisualDebug.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    constexpr int kFirstComboItemId = 1;
    constexpr int kLastComboItemId = 21;

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
}

TestComponent::TestComponent(Options options)
    : settingsState_(options.settingsState)
    , layoutDimensions_(DimensionFactory::buildGuiLayoutDimensions())
{
    setOpaque(true);
    setWantsKeyboardFocus(true);
    setFocusContainerType(juce::Component::FocusContainerType::focusContainer);

    skin_ = &options.skin;
    widgetFactory_ = std::make_unique<WidgetFactory>(options.apvts);
    TSS::ComboBox::setPopupLayoutDimensions(layoutDimensions_.popupMenu);

    createHeaderControls();
    createVisualDebugControls();

    testContentHost_ = std::make_unique<TestContentHost>();
    testViewport_.setViewedComponent(testContentHost_.get(), false);
    testViewport_.setScrollBarsShown(true, false);
    addAndMakeVisible(testViewport_);

    createTestPages(options.apvts);

    restorePersistedWidgetSelection();
    setSize(options.width, options.height);
    updateVisibleTests();
}

TestComponent::~TestComponent() = default;

void TestComponent::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    applySkinToTestPages(skin);

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

    gridToggle_.setButtonText("GRID:");
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

    boundsToggle_.setButtonText("BOUNDS:");
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
        widgetLabel_ = std::make_unique<TSS::Label>(widgetLabelWidth_, kDebugControlHeight_, labelLook, "WIDGET:");
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
            "WIDGET:")) + kGap_
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

void TestComponent::restorePersistedWidgetSelection()
{
    const int defaultSelectionId = getDefaultWidgetSelectionId();
    const int savedSelectionId = static_cast<int>(settingsState_.getProperty(PluginIDs::Settings::kTestWidget, defaultSelectionId));
    const int boundedSelectionId = juce::jlimit(kFirstComboItemId, kLastComboItemId, savedSelectionId);

    widgetSelector_.setSelectedId(boundedSelectionId, juce::dontSendNotification);
    if (widgetSelector_.getSelectedId() == 0)
        widgetSelector_.setSelectedId(defaultSelectionId, juce::dontSendNotification);
}

void TestComponent::persistWidgetSelection(int selectedId)
{
    if (selectedId == 0)
        return;

    settingsState_.setProperty(PluginIDs::Settings::kTestWidget, selectedId, nullptr);
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
