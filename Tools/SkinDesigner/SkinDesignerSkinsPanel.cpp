#include "Tools/SkinDesigner/SkinDesignerSkinsPanel.h"

#include "Tools/SkinDesigner/SkinDesignerChromeFonts.h"
#include "Tools/SkinDesigner/SkinDesignerChromePanel.h"

namespace
{
    int separatorGap() noexcept
    {
        return SkinDesignerChrome::kButtonGap * 2;
    }

    int separatorBlockHeight() noexcept
    {
        return separatorGap() + 1 + separatorGap();
    }

    /** Product combo + Dup/Compare row + separator + Designer combo + actions row. */
    int contentRowsHeight() noexcept
    {
        return SkinDesignerChrome::kButtonHeight // Product combo + stepper
               + SkinDesignerChrome::kButtonGap
               + SkinDesignerChrome::kButtonHeight // Product Duplicate + Compare
               + separatorBlockHeight()
               + SkinDesignerChrome::kButtonHeight // Designer combo + stepper
               + SkinDesignerChrome::kButtonGap
               + SkinDesignerChrome::kButtonHeight; // Dup / New / Ren / Del
    }
}

SkinDesignerSkinsPanel::SkinDesignerSkinsPanel()
{
    SkinDesignerChrome::stylePanelTitle(titleLabel_, "SKINS");
    addAndMakeVisible(titleLabel_);

    for (auto* label : { &productLabel_, &designerLabel_ })
    {
        label->setFont(SkinDesignerChrome::fontRegular());
        label->setColour(juce::Label::textColourId, SkinDesignerChrome::textMuted());
        SkinDesignerChrome::preventTextSquish(*label);
        addAndMakeVisible(*label);
    }

    addAndMakeVisible(productCombo_);
    addAndMakeVisible(designerCombo_);
    addAndMakeVisible(productStepper_);
    addAndMakeVisible(designerStepper_);

    auto wire = [this](juce::TextButton& button, std::function<void()>& handler)
    {
        addAndMakeVisible(button);
        button.onClick = [&handler]
        {
            if (handler != nullptr)
                handler();
        };
    };
    wire(productDuplicateButton_, onDuplicateProduct);
    wire(compareButton_, onCompare);
    wire(designerDuplicateButton_, onDuplicateDesigner);
    wire(newButton_, onNew);
    wire(renameButton_, onRename);
    wire(deleteButton_, onDelete);

    compareButton_.setTooltip(
        "Show the selected Product skin on Matrix-Control (read-only Designer while on)");
    designerDuplicateButton_.setTooltip("Duplicate");
    newButton_.setTooltip("New");
    renameButton_.setTooltip("Rename");
    deleteButton_.setTooltip("Delete");

    productStepper_.onDecrement = [this] { cycleCombo(productCombo_, -1); };
    productStepper_.onIncrement = [this] { cycleCombo(productCombo_, +1); };
    productStepper_.setDecrementTooltip("Previous Product Skin");
    productStepper_.setIncrementTooltip("Next Product Skin");

    designerStepper_.onDecrement = [this] { cycleCombo(designerCombo_, -1); };
    designerStepper_.onIncrement = [this] { cycleCombo(designerCombo_, +1); };
    designerStepper_.setDecrementTooltip("Previous Designer Skin");
    designerStepper_.setIncrementTooltip("Next Designer Skin");
}

void SkinDesignerSkinsPanel::cycleCombo(juce::ComboBox& combo, int delta)
{
    const int count = combo.getNumItems();
    if (count <= 0)
        return;

    const int index = combo.getSelectedItemIndex();
    if (index < 0)
        return;

    const int next = index + delta;
    if (next < 0 || next >= count)
        return;

    combo.setSelectedItemIndex(next, juce::sendNotificationSync);
}

void SkinDesignerSkinsPanel::setProductDuplicateEnabled(bool enabled)
{
    productDuplicateButton_.setEnabled(enabled);
}

void SkinDesignerSkinsPanel::syncProductNavButtons()
{
    const int count = productCombo_.getNumItems();
    const int index = productCombo_.getSelectedItemIndex();
    productStepper_.setDecrementEnabled(count > 0 && index > 0);
    productStepper_.setIncrementEnabled(count > 0 && index >= 0 && index < count - 1);
}

void SkinDesignerSkinsPanel::syncDesignerNavButtons()
{
    const int count = designerCombo_.getNumItems();
    const int index = designerCombo_.getSelectedItemIndex();
    designerStepper_.setDecrementEnabled(count > 0 && index > 0);
    designerStepper_.setIncrementEnabled(count > 0 && index >= 0 && index < count - 1);
}

void SkinDesignerSkinsPanel::setDesignerActionsEnabled(bool canDuplicate,
                                                   bool canRenameDelete,
                                                   bool canNew)
{
    designerDuplicateButton_.setEnabled(canDuplicate);
    renameButton_.setEnabled(canRenameDelete);
    deleteButton_.setEnabled(canRenameDelete);
    newButton_.setEnabled(canNew);
}

int SkinDesignerSkinsPanel::preferredHeight() noexcept
{
    return SkinDesignerChrome::kGutter + SkinDesignerChrome::kPanelTitleHeight
           + SkinDesignerChrome::kPanelContentPad + contentRowsHeight()
           + SkinDesignerChrome::kPanelContentPad + SkinDesignerChrome::kGutter;
}

int SkinDesignerSkinsPanel::labelColumnWidth() noexcept
{
    const auto font = SkinDesignerChrome::fontRegular();
    return juce::jmax(SkinDesignerChrome::measureLabelWidth("Product:", font),
                      SkinDesignerChrome::measureLabelWidth("Designer:", font));
}

int SkinDesignerSkinsPanel::comboWidthForContent(int contentW, int labelColW) noexcept
{
    constexpr int kMinCombo = 72;
    const int trailing = SkinDesignerChrome::kButtonGap + SkinDesignerStepper::kPreferredWidth;
    const int fixed = labelColW + trailing;
    return juce::jmax(kMinCombo, contentW - fixed);
}

void SkinDesignerSkinsPanel::paint(juce::Graphics& g)
{
    SkinDesignerChrome::paintPanelFrame(g, getLocalBounds(), titleTabBounds_);
    if (separatorW_ > 0)
    {
        g.setColour(SkinDesignerChrome::separatorLine().withAlpha(0.85f));
        g.fillRect(separatorX_, separatorY_, separatorW_, 1);
    }
}

SkinDesignerSkinsPanel::SelectorMetrics SkinDesignerSkinsPanel::makeSelectorMetrics(
    int contentWidth) const noexcept
{
    SelectorMetrics metrics;
    metrics.labelColW = labelColumnWidth();
    metrics.stepperW = SkinDesignerStepper::kPreferredWidth;
    metrics.comboW = comboWidthForContent(contentWidth, metrics.labelColW);
    metrics.selectorSpanW = metrics.comboW + SkinDesignerChrome::kButtonGap + metrics.stepperW;
    return metrics;
}

void SkinDesignerSkinsPanel::layoutComboNavRow(juce::Rectangle<int>& content,
                                             const SelectorMetrics& metrics,
                                             const ComboNavRowRefs& rowRefs)
{
    auto row = content.removeFromTop(SkinDesignerChrome::kButtonHeight);
    rowRefs.label.setBounds(row.removeFromLeft(metrics.labelColW));
    rowRefs.combo.setBounds(row.removeFromLeft(metrics.comboW));
    row.removeFromLeft(SkinDesignerChrome::kButtonGap);
    rowRefs.stepper.setBounds(row.removeFromLeft(metrics.stepperW)
                                  .withHeight(SkinDesignerChrome::kButtonHeight));
}

void SkinDesignerSkinsPanel::layoutProductDuplicateRow(juce::Rectangle<int>& content,
                                                       const SelectorMetrics& metrics)
{
    // Duplicate + Compare: same span as Product combo.
    content.removeFromTop(SkinDesignerChrome::kButtonGap);
    auto row = content.removeFromTop(SkinDesignerChrome::kButtonHeight);
    row.removeFromLeft(metrics.labelColW);
    auto span = row.removeFromLeft(metrics.comboW);
    const int gaps = SkinDesignerChrome::kButtonGap;
    const int avail = juce::jmax(4, metrics.comboW - gaps);
    const int dupW = avail / 2;
    const int compareW = avail - dupW;
    productDuplicateButton_.setBounds(
        span.removeFromLeft(dupW).withHeight(SkinDesignerChrome::kButtonHeight));
    span.removeFromLeft(SkinDesignerChrome::kButtonGap);
    compareButton_.setBounds(
        span.removeFromLeft(compareW).withHeight(SkinDesignerChrome::kButtonHeight));
}

void SkinDesignerSkinsPanel::layoutDesignerActionRow(juce::Rectangle<int>& content,
                                                 const SelectorMetrics& metrics)
{
    // Dup / New / Ren = combo width; Del = stepper width (same gap as combo→stepper).
    content.removeFromTop(SkinDesignerChrome::kButtonGap);
    auto actions = content.removeFromTop(SkinDesignerChrome::kButtonHeight);
    actions.removeFromLeft(metrics.labelColW);
    auto span = actions.removeFromLeft(metrics.selectorSpanW);
    const int trioGaps = 2 * SkinDesignerChrome::kButtonGap;
    const int trioAvail = juce::jmax(4, metrics.comboW - trioGaps);
    const int w0 = trioAvail / 3;
    const int w1 = trioAvail / 3;
    const int w2 = trioAvail - w0 - w1;

    auto placeCell = [&](juce::TextButton& button, int width, bool addGap)
    {
        button.setBounds(span.removeFromLeft(width).withHeight(SkinDesignerChrome::kButtonHeight));
        if (addGap)
            span.removeFromLeft(SkinDesignerChrome::kButtonGap);
    };
    placeCell(designerDuplicateButton_, w0, true);
    placeCell(newButton_, w1, true);
    placeCell(renameButton_, w2, true);
    placeCell(deleteButton_, metrics.stepperW, false);
}

void SkinDesignerSkinsPanel::resized()
{
    separatorX_ = 0;
    separatorW_ = 0;
    const auto chrome = SkinDesignerChrome::layoutPanel(getLocalBounds(),
                                                      SkinDesignerChrome::titleTabWidth("SKINS"));
    titleTabBounds_ = chrome.tab;
    titleLabel_.setBounds(chrome.tab);
    auto content = chrome.content;
    const auto metrics = makeSelectorMetrics(content.getWidth());

    layoutComboNavRow(content, metrics, { productLabel_, productCombo_, productStepper_ });
    layoutProductDuplicateRow(content, metrics);

    content.removeFromTop(separatorGap());
    separatorY_ = content.getY();
    separatorX_ = content.getX();
    separatorW_ = content.getWidth();
    content.removeFromTop(1);
    content.removeFromTop(separatorGap());

    layoutComboNavRow(content, metrics, { designerLabel_, designerCombo_, designerStepper_ });
    layoutDesignerActionRow(content, metrics);
}
