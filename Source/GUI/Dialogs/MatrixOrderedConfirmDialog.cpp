#include "MatrixOrderedConfirmDialog.h"

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

namespace
{
    constexpr float kLabelValueGapEm = 0.5f;
    constexpr int kLabelWidthSlack = 4;

    int scaled(int designValue, float uiScale)
    {
        return juce::roundToInt(static_cast<float>(designValue) * uiScale);
    }
}

MatrixOrderedConfirmDialog::MatrixOrderedConfirmDialog(
    TSS::ISkin& skin,
    float uiScale,
    const PluginEditorInternal::OrderedConfirmAlertOptions& options)
    : skin_(&skin)
    , uiScale_(uiScale)
    , title_(options.title)
    , message_(options.message)
    , valueRows_(options.valueRows)
    , hasMiddle_(options.middleLabel.isNotEmpty())
    , designWidth_(options.designWidth > 0 ? options.designWidth : kDefaultDesignWidth_)
    , alignBodyToCancel_(options.alignBodyToCancel)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    cancelButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, options.cancelLabel);
    primaryButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, options.primaryLabel);

    cancelButton_->setWantsKeyboardFocus(false);
    cancelButton_->onClick = [this] { finish(0); };
    primaryButton_->onClick = [this] { finish(1); };

    addAndMakeVisible(*cancelButton_);
    addAndMakeVisible(*primaryButton_);

    if (hasMiddle_)
    {
        middleButton_ = DialogMatrixHelpers::makeButton(
            skin, DialogMatrixHelpers::kDefaultButtonWidth, options.middleLabel);
        middleButton_->setWantsKeyboardFocus(false);
        middleButton_->onClick = [this] { finish(2); };
        addAndMakeVisible(*middleButton_);
    }

    DialogMatrixHelpers::applyButtonUiScale(*cancelButton_, uiScale_);
    DialogMatrixHelpers::applyButtonUiScale(*primaryButton_, uiScale_);
    if (middleButton_ != nullptr)
        DialogMatrixHelpers::applyButtonUiScale(*middleButton_, uiScale_);
}

juce::String MatrixOrderedConfirmDialog::joinedRowColumn(bool labels) const
{
    juce::StringArray lines;
    for (const auto& row : valueRows_)
        lines.add(labels ? row.label : row.value);

    // One blank line between rows.
    return lines.joinIntoString("\n\n");
}

MatrixOrderedConfirmDialog::BodyLayout MatrixOrderedConfirmDialog::computeBodyLayout() const
{
    namespace Helpers = DialogMatrixHelpers;

    BodyLayout layout;
    layout.bodyFont = Helpers::scaledModalBodyFont(*skin_, uiScale_);

    const int contentWidth = Helpers::contentWidthFor(designWidth_, uiScale_);
    int leftInset = -1;
    int rightInset = -1;
    int textWidth = Helpers::bodyTextWidthFor(contentWidth);

    // Patch name mismatch only: body left edge matches CANCEL. Other confirms keep ~10% inset.
    if (alignBodyToCancel_)
    {
        std::vector<int> buttonWidths;
        buttonWidths.push_back(
            Helpers::estimateButtonWidth(*skin_, cancelButton_->getButtonText(), uiScale_));
        if (hasMiddle_ && middleButton_ != nullptr)
            buttonWidths.push_back(
                Helpers::estimateButtonWidth(*skin_, middleButton_->getButtonText(), uiScale_));
        buttonWidths.push_back(
            Helpers::estimateButtonWidth(*skin_, primaryButton_->getButtonText(), uiScale_));

        const auto pack = Helpers::measureCentredButtonPack(contentWidth, uiScale_, buttonWidths);
        leftInset = pack.leftInset;
        rightInset = scaled(Helpers::kButtonSideMargin, uiScale_);
        textWidth = Helpers::bodyTextWidthFor(contentWidth, leftInset, rightInset);
    }

    int bodyHeight = Helpers::measureBodyHeight(layout.bodyFont, message_, textWidth);

    if (! valueRows_.empty())
    {
        for (const auto& row : valueRows_)
            layout.labelColumnWidth = juce::jmax(
                layout.labelColumnWidth,
                juce::GlyphArrangement::getStringWidthInt(layout.bodyFont, row.label) + kLabelWidthSlack);

        layout.labelColumnWidth += juce::roundToInt(layout.bodyFont.getHeight() * kLabelValueGapEm);
        // Keep at least 1 px for the value column.
        layout.labelColumnWidth = juce::jmin(layout.labelColumnWidth, textWidth - 1);

        // Long values may wrap more than the labels: the taller column decides the block height.
        const int valueWidth = textWidth - layout.labelColumnWidth;
        layout.rowsTextHeight = juce::jmax(
            Helpers::measureBodyHeight(layout.bodyFont, joinedRowColumn(true), layout.labelColumnWidth),
            Helpers::measureBodyHeight(layout.bodyFont, joinedRowColumn(false), valueWidth));
        layout.rowsBlockHeight = layout.rowsTextHeight + Helpers::measureLineStep(layout.bodyFont, textWidth);
        bodyHeight += layout.rowsBlockHeight;
    }

    layout.geometry = Helpers::computeModalGeometry({
        .hostBounds = getLocalBounds(),
        .designWidth = designWidth_,
        .uiScale = uiScale_,
        .systemDisplayScale = TSS::ScaledDrawing::systemDisplayScaleForComponent(*this),
        .bodyHeight = bodyHeight,
        .bodyLeftInset = leftInset,
        .bodyRightInset = rightInset,
    });
    return layout;
}

void MatrixOrderedConfirmDialog::finish(int code)
{
    if (isCurrentlyModal())
        exitModalState(code);
}

void MatrixOrderedConfirmDialog::paintValueRows(juce::Graphics& g, const BodyLayout& layout) const
{
    const auto& textArea = layout.geometry.textArea;
    const juce::Rectangle<int> labelArea { textArea.getX(),
                                           textArea.getY(),
                                           layout.labelColumnWidth,
                                           layout.rowsTextHeight };
    const juce::Rectangle<int> valueArea { textArea.getX() + layout.labelColumnWidth,
                                           textArea.getY(),
                                           juce::jmax(1, textArea.getWidth() - layout.labelColumnWidth),
                                           layout.rowsTextHeight };

    DialogMatrixHelpers::paintBodyText(g, layout.bodyFont, joinedRowColumn(true), labelArea);
    DialogMatrixHelpers::paintBodyText(g, layout.bodyFont, joinedRowColumn(false), valueArea);
}

void MatrixOrderedConfirmDialog::paint(juce::Graphics& g)
{
    const auto layout = computeBodyLayout();
    const auto& geometry = layout.geometry;

    DialogMatrixHelpers::paintMatrixOverlayChrome({
        .g = g,
        .skin = *skin_,
        .dialogBounds = geometry.dialogBounds,
        .borderThickness = geometry.border,
        .titleBarHeight = geometry.titleBarHeight,
        .title = title_,
        .uiScale = uiScale_ });

    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));

    if (! valueRows_.empty())
        paintValueRows(g, layout);

    DialogMatrixHelpers::paintBodyText(g,
                                       layout.bodyFont,
                                       message_,
                                       geometry.textArea.withTrimmedTop(layout.rowsBlockHeight));
    DialogMatrixHelpers::paintActionFooterSeparator({
        .g = g,
        .skin = *skin_,
        .geometry = geometry,
        .uiScale = uiScale_,
        .systemDisplayScale = TSS::ScaledDrawing::systemDisplayScaleForComponent(*this),
    });
}

void MatrixOrderedConfirmDialog::resized()
{
    const auto layout = computeBodyLayout();

    // LTR: Cancel, [middle], primary - pack is centred in the bottom row.
    std::vector<DialogMatrixHelpers::ButtonPlacement> buttons;
    buttons.push_back({ cancelButton_.get(),
                        DialogMatrixHelpers::estimateButtonWidth(*skin_, cancelButton_->getButtonText(), uiScale_) });
    if (hasMiddle_ && middleButton_ != nullptr)
        buttons.push_back({ middleButton_.get(),
                            DialogMatrixHelpers::estimateButtonWidth(*skin_, middleButton_->getButtonText(), uiScale_) });
    buttons.push_back({ primaryButton_.get(),
                        DialogMatrixHelpers::estimateButtonWidth(*skin_, primaryButton_->getButtonText(), uiScale_) });

    DialogMatrixHelpers::layoutCentredButtonRow(layout.geometry.buttonRow, uiScale_, buttons);
}

void MatrixOrderedConfirmDialog::mouseDown(const juce::MouseEvent& e)
{
    if (! computeBodyLayout().geometry.dialogBounds.contains(e.getPosition()))
        finish(0);
}

bool MatrixOrderedConfirmDialog::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        finish(0);
        return true;
    }

    if (key == juce::KeyPress::returnKey)
    {
        finish(1);
        return true;
    }

    return Component::keyPressed(key);
}

MatrixMutatorDeleteConfirmDialog::MatrixMutatorDeleteConfirmDialog(TSS::ISkin& skin, float uiScale)
    : skin_(&skin)
    , uiScale_(uiScale)
    , dontAskAgain_(PluginDisplayNames::Dialogs::MutatorDeleteConfirm::kDontAskAgain)
{
    namespace Dialog = PluginDisplayNames::Dialogs::MutatorDeleteConfirm;

    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    const auto textColour = skin.getColour(SkinColourId::kDarkPanelText);
    toggleLook_.setUiScale(uiScale);
    toggleLook_.setLabelFont(DialogMatrixHelpers::scaledModalBodyFont(skin, uiScale));
    dontAskAgain_.setLookAndFeel(&toggleLook_);
    dontAskAgain_.setColour(juce::ToggleButton::textColourId, textColour);
    dontAskAgain_.setColour(juce::ToggleButton::tickColourId, textColour);
    dontAskAgain_.setColour(juce::ToggleButton::tickDisabledColourId, textColour);
    dontAskAgain_.setName({});
    dontAskAgain_.setWantsKeyboardFocus(false);
    dontAskAgain_.setMouseClickGrabsKeyboardFocus(false);
    addAndMakeVisible(dontAskAgain_);

    cancelButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, Dialog::kCancel);
    deleteButton_ = DialogMatrixHelpers::makeButton(
        skin, DialogMatrixHelpers::kDefaultButtonWidth, Dialog::kDelete);

    cancelButton_->setWantsKeyboardFocus(false);
    cancelButton_->onClick = [this] { finish(false); };
    deleteButton_->onClick = [this] { finish(true); };

    addAndMakeVisible(*cancelButton_);
    addAndMakeVisible(*deleteButton_);

    DialogMatrixHelpers::applyButtonUiScale(*cancelButton_, uiScale_);
    DialogMatrixHelpers::applyButtonUiScale(*deleteButton_, uiScale_);
}

MatrixMutatorDeleteConfirmDialog::~MatrixMutatorDeleteConfirmDialog()
{
    dontAskAgain_.setLookAndFeel(nullptr);
}

DialogMatrixHelpers::TextModalLayout MatrixMutatorDeleteConfirmDialog::computeBodyLayout() const
{
    const juce::String body(PluginDisplayNames::Dialogs::MutatorDeleteConfirm::kBody);
    return DialogMatrixHelpers::computeTextModalLayout({
        .skin = *skin_,
        .bodyText = body,
        .hostBounds = getLocalBounds(),
        .designWidth = kDesignWidth_,
        .uiScale = uiScale_,
        .systemDisplayScale = TSS::ScaledDrawing::systemDisplayScaleForComponent(*this),
        .extraBandHeight = scaled(DialogMatrixHelpers::kCheckboxHeight, uiScale_),
    });
}

void MatrixMutatorDeleteConfirmDialog::finish(bool confirmed)
{
    result_ = { confirmed, dontAskAgain_.getToggleState() };
    if (isCurrentlyModal())
        exitModalState(confirmed ? 1 : 0);
}

void MatrixMutatorDeleteConfirmDialog::paint(juce::Graphics& g)
{
    namespace Dialog = PluginDisplayNames::Dialogs::MutatorDeleteConfirm;

    const auto layout = computeBodyLayout();
    const auto& geometry = layout.geometry;

    DialogMatrixHelpers::paintMatrixOverlayChrome({
        .g = g,
        .skin = *skin_,
        .dialogBounds = geometry.dialogBounds,
        .borderThickness = geometry.border,
        .titleBarHeight = geometry.titleBarHeight,
        .title = Dialog::kTitle,
        .uiScale = uiScale_ });

    g.setColour(skin_->getColour(SkinColourId::kDarkPanelText));
    DialogMatrixHelpers::paintBodyText(g, layout.bodyFont, Dialog::kBody, geometry.textArea);
    DialogMatrixHelpers::paintActionFooterSeparator({
        .g = g,
        .skin = *skin_,
        .geometry = geometry,
        .uiScale = uiScale_,
        .systemDisplayScale = TSS::ScaledDrawing::systemDisplayScaleForComponent(*this),
    });
}

void MatrixMutatorDeleteConfirmDialog::resized()
{
    const auto layout = computeBodyLayout();
    const auto& geometry = layout.geometry;

    // Don't ask again: vertically centred in the control band (above the action-footer rule).
    const auto controlBand = DialogMatrixHelpers::controlBandArea(geometry);
    const int checkHeight = scaled(DialogMatrixHelpers::kCheckboxHeight, uiScale_);
    const int checkWidth = toggleLook_.getPreferredWidth(dontAskAgain_.getButtonText(), checkHeight);
    dontAskAgain_.setBounds(geometry.textArea.getX(),
                            controlBand.getCentreY() - checkHeight / 2,
                            checkWidth,
                            checkHeight);

    DialogMatrixHelpers::layoutCentredButtonRow(
        geometry.buttonRow,
        uiScale_,
        { { cancelButton_.get(),
            DialogMatrixHelpers::estimateButtonWidth(*skin_, cancelButton_->getButtonText(), uiScale_) },
          { deleteButton_.get(),
            DialogMatrixHelpers::estimateButtonWidth(*skin_, deleteButton_->getButtonText(), uiScale_) } });
}

void MatrixMutatorDeleteConfirmDialog::mouseDown(const juce::MouseEvent& e)
{
    if (! computeBodyLayout().geometry.dialogBounds.contains(e.getPosition()))
        finish(false);
}

bool MatrixMutatorDeleteConfirmDialog::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        finish(false);
        return true;
    }

    if (key == juce::KeyPress::returnKey)
    {
        finish(true);
        return true;
    }

    return Component::keyPressed(key);
}
