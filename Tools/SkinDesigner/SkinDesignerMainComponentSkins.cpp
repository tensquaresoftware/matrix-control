#include "Tools/SkinDesigner/SkinDesignerMainComponent.h"
#include "Tools/SkinDesigner/SkinDesignerUiHelpers.h"

#include "Tools/SkinDesigner/SkinDesignerChromeFonts.h"

namespace
{
    struct SessionWellPlacer
    {
        juce::Rectangle<int>& row;
        bool fits = true;
        std::vector<int>* separatorXs = nullptr;

        void stop() noexcept { fits = false; }

        void gap()
        {
            if (! fits || row.getWidth() < SkinDesignerChrome::kButtonGap)
            {
                stop();
                return;
            }
            row.removeFromLeft(SkinDesignerChrome::kButtonGap);
        }

        void button(juce::TextButton& b)
        {
            button(b, SkinDesignerChrome::measureButtonWidth(b));
        }

        void button(juce::TextButton& b, int width)
        {
            if (! fits)
            {
                b.setBounds({});
                return;
            }
            if (row.getWidth() < width)
            {
                b.setBounds({});
                stop();
                return;
            }
            b.setBounds(row.removeFromLeft(width).withHeight(SkinDesignerChrome::kButtonHeight));
        }

        void separator()
        {
            constexpr int kSep = SkinDesignerChrome::kButtonGap * 2 + 1;
            if (! fits || row.getWidth() < kSep)
            {
                stop();
                return;
            }
            gap();
            if (separatorXs != nullptr)
                separatorXs->push_back(row.getX());
            row.removeFromLeft(1);
            gap();
        }

        void label(juce::Label& label, int width)
        {
            if (! fits)
            {
                label.setBounds({});
                return;
            }
            if (row.getWidth() < width)
            {
                label.setBounds({});
                stop();
                return;
            }
            label.setBounds(row.removeFromLeft(width).withHeight(SkinDesignerChrome::kButtonHeight));
        }

        void combo(juce::ComboBox& box, int width)
        {
            if (! fits)
            {
                box.setBounds({});
                return;
            }
            if (row.getWidth() < width)
            {
                box.setBounds({});
                stop();
                return;
            }
            box.setBounds(row.removeFromLeft(width).withHeight(SkinDesignerChrome::kButtonHeight));
        }
    };
}

void SkinDesignerMainComponent::layoutTitleBar(juce::Rectangle<int> titleBar)
{
    const int wellH = SkinDesignerChrome::kSessionWellHeight;
    const int wellY = titleBar.getY() + SkinDesignerChrome::kChromeStackGap;
    const int sessionW = SkinDesignerChrome::sessionWellPreferredWidth();
    layoutSessionWell({ titleBar.getX(), wellY, sessionW, wellH });

    const int opX = titleBar.getX() + sessionW + SkinDesignerChrome::kButtonGap;
    layoutOperationWell({ opX, wellY, juce::jmax(0, titleBar.getRight() - opX), wellH });
}

void SkinDesignerMainComponent::comboBoxChanged(juce::ComboBox* box)
{
    if (suppressCombo_)
        return;
    if (box == &skinsPanel_.productCombo())
        applyProductComboChange();
    else if (box == &skinsPanel_.designerCombo())
        applyDesignerComboChange();
    else if (box == &jsonSyncCombo_)
        applyJsonSyncComboChange();
}

void SkinDesignerMainComponent::setupSessionWell()
{
    SkinDesignerChrome::stylePanelTitle(sessionTitleLabel_, "SESSION");
    addAndMakeVisible(sessionTitleLabel_);

    auto wire = [this](juce::TextButton& button, auto handler)
    {
        addAndMakeVisible(button);
        button.onClick = handler;
    };
    wire(loadButton_, [this] { onLoadJson(); });
    wire(saveButton_, [this] { onSave(); });
    wire(saveAsButton_, [this] { onSaveAs(); });
    wire(undoButton_, [this] { onUndo(); });
    wire(redoButton_, [this] { onRedo(); });
    wire(compareButton_, [this] { onCompareToggle(); });
    wire(pushButton_, [this] { onPush(); });
    loadButton_.setTooltip("Replace the Designer skins with the contents of a skins JSON file");
    saveButton_.setTooltip(
        "Save every Designer skin to the loaded JSON file (asks for a file the first time)");
    saveAsButton_.setTooltip("Save every Designer skin to a new JSON file and watch that file");

    jsonSyncLabel_.setText("JSON Sync:", juce::dontSendNotification);
    jsonSyncLabel_.setFont(SkinDesignerChrome::fontRegular());
    jsonSyncLabel_.setColour(juce::Label::textColourId, SkinDesignerChrome::textMuted());
    jsonSyncLabel_.setJustificationType(juce::Justification::centredLeft);
    SkinDesignerChrome::preventTextSquish(jsonSyncLabel_);
    jsonSyncLabel_.setBorderSize({});
    jsonSyncLabel_.setTooltip(
        "How the Skin Designer reacts when the loaded JSON changes on disk");
    addAndMakeVisible(jsonSyncLabel_);

    jsonSyncCombo_.addItem("Off", static_cast<int>(TSS::DevSkin::JsonSyncMode::kOff) + 1);
    jsonSyncCombo_.addItem("Ask", static_cast<int>(TSS::DevSkin::JsonSyncMode::kAsk) + 1);
    jsonSyncCombo_.addItem("Auto", static_cast<int>(TSS::DevSkin::JsonSyncMode::kAuto) + 1);
    jsonSyncCombo_.setTooltip(
        "Off: ignore disk changes. Ask: prompt Reload/Discard. Auto: reload without asking (disk wins).");
    addAndMakeVisible(jsonSyncCombo_);
    jsonSyncCombo_.addListener(this);
    syncJsonSyncComboFromSession();

    compareBlinkTimer_.onTick = [this] { tickCompareBlink(); };
    undoButton_.setEnabled(false);
    redoButton_.setEnabled(false);
}

void SkinDesignerMainComponent::setupOperationWell()
{
    SkinDesignerChrome::stylePanelTitle(operationTitleLabel_, "OPERATION");
    addAndMakeVisible(operationTitleLabel_);
}

void SkinDesignerMainComponent::layoutSessionControls(juce::Rectangle<int> well,
                                                    juce::Rectangle<int> row)
{
    sessionSeparatorY_ = row.getY();
    sessionSeparatorH_ = row.getHeight();

    SessionWellPlacer place { row, true, &sessionSeparatorXs_ };
    const int sessionTitleW = SkinDesignerChrome::titleTabWidth("SESSION");
    if (row.getWidth() < sessionTitleW)
    {
        sessionTitleLabel_.setBounds({});
        place.stop();
    }
    else
    {
        sessionTitleLabel_.setBounds(row.getX(), well.getY() - 1, sessionTitleW,
                                     well.getHeight() + 2);
        // No gap after tab: kTitleTabPadH already matches left title air.
        row.removeFromLeft(sessionTitleW);
    }

    place.button(loadButton_);
    place.gap();
    place.button(saveButton_);
    place.gap();
    place.button(saveAsButton_);
    place.separator();
    place.label(jsonSyncLabel_, SkinDesignerChrome::jsonSyncLabelWidth());
    place.gap();
    place.combo(jsonSyncCombo_, SkinDesignerChrome::jsonSyncComboWidth());
    place.separator();
    const int undoRedoW = SkinDesignerChrome::sessionUndoRedoButtonWidth();
    place.button(undoButton_, undoRedoW);
    place.gap();
    place.button(redoButton_, undoRedoW);
    place.separator();
    place.button(compareButton_);
    place.gap();
    place.button(pushButton_, SkinDesignerChrome::sessionPushButtonWidth());
    if (place.fits && row.getWidth() >= SkinDesignerChrome::kGutter)
        row.removeFromLeft(SkinDesignerChrome::kGutter);
}

void SkinDesignerMainComponent::layoutSessionWell(juce::Rectangle<int> well)
{
    sessionSeparatorXs_.clear();
    sessionWellBounds_ = {};
    if (well.isEmpty())
        return;

    // Compact well on the left of the title row (OPERATION stretches to the right).
    sessionWellBounds_ = well;
    layoutSessionControls(sessionWellBounds_,
                          { sessionWellBounds_.getX(),
                            sessionWellBounds_.getY() + SkinDesignerChrome::kGutter,
                            sessionWellBounds_.getWidth(),
                            SkinDesignerChrome::kButtonHeight });
}

void SkinDesignerMainComponent::layoutBodyPanels(juce::Rectangle<int> bounds)
{
    auto left = bounds.removeFromLeft(SkinDesignerChrome::kColorPanelWidth);
    const int skinsH = SkinDesignerSkinsPanel::preferredHeight();
    const int colorH = SkinDesignerColourPicker::preferredHeightForWidth(
        SkinDesignerChrome::kColorPanelWidth);
    // SKINS↔COLOR: each panel's facing kGutter → kChromeStackGap. COLOR fixed height.
    skinsPanel_.setBounds(left.getX(), left.getY(), left.getWidth(), skinsH);
    colourPicker_.setBounds(left.getX(), left.getY() + skinsH, left.getWidth(), colorH);

    const int paletteGap = SkinDesignerChrome::kPaletteSideGap;
    bounds.removeFromLeft(paletteGap);
    // Remainder = Palette + gap + Mappings; both Palette and Mapped colour share growth.
    const int paletteW = SkinDesignerChrome::palettePanelWidthFromBodyRemainder(bounds.getWidth());
    const int mappingsW = SkinDesignerChrome::mappingsPanelWidthForPalettePanel(paletteW);
    palettePanel_.setBounds(bounds.removeFromLeft(paletteW));
    bounds.removeFromLeft(paletteGap);
    mappingsPanel_.setBounds(bounds.removeFromLeft(mappingsW));
}

void SkinDesignerMainComponent::layoutStatusPushOrNoticeStrip(int badgeY,
                                                            int badgeH,
                                                            int gap,
                                                            const juce::Font& statusFont)
{
    if (jsonSyncReloadNoticeVisible_)
    {
        // Bounds for [X][2px][WARNING] already set in layoutStatusBar (X at LAST PUSH column).
        const int labelX = lastPushBadgeBounds_.getRight() + gap;
        const int labelMaxW = juce::jmax(0, appBadgeBounds_.getX() - gap - labelX);
        lastPushLabel_.setText(SkinDesignerChrome::elideToWidth(statusFont,
                                                              jsonSyncReloadNoticeText_,
                                                              labelMaxW),
                               juce::dontSendNotification);
        lastPushLabel_.setTooltip(jsonSyncReloadNoticeText_);
        lastPushLabel_.setBounds(labelX, badgeY, labelMaxW, badgeH);
        lastPushBadgeHit_.setBounds({});
        lastPushBadgeHit_.setTooltip({});
        jsonSyncNoticeCloseHit_.setBounds(jsonSyncNoticeCloseBounds_);
        jsonSyncNoticeCloseHit_.setVisible(true);
        return;
    }

    jsonSyncNoticeCloseBounds_ = {};

    const int labelX = lastPushBadgeBounds_.getRight() + gap;
    const int labelMaxW = juce::jmax(0, appBadgeBounds_.getX() - gap - labelX);
    const auto full = statusDetail_.isNotEmpty() ? statusDetail_ : "None";
    const auto shown = SkinDesignerChrome::elidePathToWidth(statusFont, full, labelMaxW);
    const bool isFilePathStatus = SkinDesignerUi::isFilePathStatus(full);
    const auto tip = (isFilePathStatus && shown != full) ? full : juce::String {};
    lastPushLabel_.setText(shown, juce::dontSendNotification);
    lastPushLabel_.setTooltip(tip);
    lastPushLabel_.setBounds(labelX, badgeY, labelMaxW, badgeH);
    lastPushBadgeHit_.setBounds(lastPushBadgeBounds_);
    lastPushBadgeHit_.setTooltip(tip);
    jsonSyncNoticeCloseHit_.setBounds({});
    jsonSyncNoticeCloseHit_.setVisible(false);
}
