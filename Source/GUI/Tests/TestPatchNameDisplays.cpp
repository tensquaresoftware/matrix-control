#include "TestPatchNameDisplays.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/PatchNameDisplay.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace
{
    constexpr int kModeRowHeight = 28;
    constexpr int kOutcomeRowHeight = 22;
    constexpr int kModeRowGap = 8;
    constexpr const char* kDemoPatchName = "WARMPAD ";
    constexpr const char* kDemoMutatorSecondary = "M00";
    constexpr const char* kOutcomeNone = "Name-required outcome: none";
}

class TestPatchNameDisplays::PatchNameDisplayScalePanel : public juce::Component
{
public:
    struct Config
    {
        float scale = 1.0f;
        juce::String scaleLabelText;
        const TSS::PatchNameDisplayLook& displayLook;
        const TSS::LabelLook& labelLook;
        const PatchNameDisplayDimensions& dimensions;
    };

    explicit PatchNameDisplayScalePanel(const Config& config)
        : scale_(config.scale)
        , dimensions_(config.dimensions)
        , displayLook_(config.displayLook)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            dimensions_.width,
            TestScaleColumns::kScaleLabelHeight,
            config.labelLook,
            config.scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        display_ = std::make_unique<TSS::PatchNameDisplay>(dimensions_.width, dimensions_.height, displayLook_);
        display_->setUiScale(scale_);
        display_->setPatchName(kDemoPatchName);
        addAndMakeVisible(*display_);
    }

    TSS::PatchNameDisplay& getDisplay() noexcept { return *display_; }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(dimensions_.width, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledDisplayHeight = TestScaleColumns::scaledSize(dimensions_.height, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight + rowGap + scaledDisplayHeight;
    }

    void resized() override
    {
        const int scaledWidth = TestScaleColumns::scaledSize(dimensions_.width, scale_);
        const int scaledHeight = TestScaleColumns::scaledSize(dimensions_.height, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int displayY = TestScaleColumns::kScaleLabelHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledWidth, TestScaleColumns::kScaleLabelHeight);
        display_->setBounds(0, displayY, scaledWidth, scaledHeight);
    }

    void applyDemoMode(DemoMode mode, std::function<void(bool)> outcomeCallback)
    {
        namespace Overlay = PluginDisplayNames::PatchEditSection::PatchNameModule::DragDropOverlay;

        // Silence abort noise from demo resets before re-arming.
        display_->onNameRequiredOutcome(nullptr);
        display_->clearDragOverlay();
        display_->clearNameRequired();
        if (display_->isEditing())
            display_->cancelEdit();

        display_->setPatchName(kDemoPatchName);
        display_->setSecondaryLabel({});
        display_->setEditable(false);

        const bool tracksOutcome = mode == DemoMode::kNameRequired
            || mode == DemoMode::kNameRequiredDragRestore;
        if (tracksOutcome && outcomeCallback)
            display_->onNameRequiredOutcome(std::move(outcomeCallback));

        switch (mode)
        {
            case DemoMode::kIdle:
                break;

            case DemoMode::kInlineEdit:
                display_->setEditable(true);
                display_->beginEdit();
                break;

            case DemoMode::kMutatorSecondary:
                display_->setSecondaryLabel(kDemoMutatorSecondary);
                break;

            case DemoMode::kDragOverlay:
                display_->showDragOverlay(Overlay::kBadPrimary, Overlay::kDropToLoad);
                break;

            case DemoMode::kNameRequired:
                display_->setSecondaryLabel(kDemoMutatorSecondary); // suppressed while armed
                display_->armNameRequired();
                break;

            case DemoMode::kNameRequiredDragRestore:
                display_->armNameRequired();
                display_->showDragOverlay(Overlay::kBadPrimary, Overlay::kDropToLoad);
                display_->clearDragOverlay(); // name-required restores if still armed
                break;
        }
    }

private:
    float scale_ { 1.0f };
    PatchNameDisplayDimensions dimensions_ {};
    TSS::PatchNameDisplayLook displayLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::PatchNameDisplay> display_;
};

int TestPatchNameDisplays::modeControlsHeight()
{
    return kModeRowHeight + kModeRowGap + kOutcomeRowHeight;
}

TestPatchNameDisplays::TestPatchNameDisplays(TSS::ISkin& skin, const PatchNameDisplayDimensions& dimensions)
    : dimensions_(dimensions)
{
    modeLabel_ = std::make_unique<juce::Label>("modeLabel", "Presentation mode:");
    modeLabel_->setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(*modeLabel_);

    modeSelector_ = std::make_unique<juce::ComboBox>("patchNameDemoMode");
    modeSelector_->addItem("1 Idle (1-line)", static_cast<int>(DemoMode::kIdle));
    modeSelector_->addItem("2 Inline edit", static_cast<int>(DemoMode::kInlineEdit));
    modeSelector_->addItem("3 Mutator secondary", static_cast<int>(DemoMode::kMutatorSecondary));
    modeSelector_->addItem("4 Drag overlay", static_cast<int>(DemoMode::kDragOverlay));
    modeSelector_->addItem("5 Name required", static_cast<int>(DemoMode::kNameRequired));
    modeSelector_->addItem("6 Name required → drag → restore",
                            static_cast<int>(DemoMode::kNameRequiredDragRestore));
    modeSelector_->setSelectedId(static_cast<int>(DemoMode::kIdle), juce::dontSendNotification);
    modeSelector_->onChange = [this]()
    {
        selectedMode_ = static_cast<DemoMode>(modeSelector_->getSelectedId());
        applySelectedModeToPanels();
    };
    addAndMakeVisible(*modeSelector_);

    outcomeLabel_ = std::make_unique<juce::Label>("outcomeLabel", kOutcomeNone);
    outcomeLabel_->setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(*outcomeLabel_);

    setSkin(skin);
}

TestPatchNameDisplays::~TestPatchNameDisplays() = default;

void TestPatchNameDisplays::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestPatchNameDisplays::setOutcomeReadout(const juce::String& outcome)
{
    if (outcomeLabel_ == nullptr)
        return;

    outcomeLabel_->setText("Name-required outcome: " + outcome, juce::dontSendNotification);
}

void TestPatchNameDisplays::resized()
{
    const int width = getWidth();
    modeLabel_->setBounds(0, 0, juce::jmin(160, width / 3), kModeRowHeight);
    modeSelector_->setBounds(modeLabel_->getRight() + 8,
                             0,
                             juce::jmax(180, width - modeLabel_->getWidth() - 8),
                             kModeRowHeight);
    outcomeLabel_->setBounds(0, kModeRowHeight + 4, width, kOutcomeRowHeight);
    layoutColumnPanels();
}

int TestPatchNameDisplays::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestPatchNameDisplays::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return modeControlsHeight() + kModeRowGap + maxHeight;
}

void TestPatchNameDisplays::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();

    // Keep the mode controls; only recreate scale columns.
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        auto* child = getChildComponent(i);
        if (child != modeLabel_.get()
            && child != modeSelector_.get()
            && child != outcomeLabel_.get())
        {
            removeChildComponent(child);
        }
    }

    const auto displayLook = TSS::patchNameDisplayLookFromSkin(*skin_);
    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<PatchNameDisplayScalePanel>(
            PatchNameDisplayScalePanel::Config {
                .scale = spec.scale,
                .scaleLabelText = spec.label,
                .displayLook = displayLook,
                .labelLook = labelLook,
                .dimensions = dimensions_
            });
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    applySelectedModeToPanels();
    layoutColumnPanels();
}

void TestPatchNameDisplays::applySelectedModeToPanels()
{
    setOutcomeReadout("none");

    auto outcomeCallback = [this](bool success)
    {
        setOutcomeReadout(success ? "success" : "cancel");
    };

    for (auto& panel : columnPanels_)
        panel->applyDemoMode(selectedMode_, outcomeCallback);
}

void TestPatchNameDisplays::layoutColumnPanels()
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    const int columnsY = modeControlsHeight() + kModeRowGap;
    int nextColumnX = 0;

    for (auto& panel : columnPanels_)
    {
        const int columnWidth = panel->getScaledColumnWidth();
        panel->setBounds(nextColumnX, columnsY, columnWidth, panel->getPreferredHeight());
        nextColumnX += columnWidth + panelGap;
    }
}
