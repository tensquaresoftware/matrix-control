#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"

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
            if (! fits || row.getWidth() < SkinEditorChrome::kButtonGap)
            {
                stop();
                return;
            }
            row.removeFromLeft(SkinEditorChrome::kButtonGap);
        }

        void button(juce::TextButton& b)
        {
            button(b, SkinEditorChrome::measureButtonWidth(b));
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
            b.setBounds(row.removeFromLeft(width).withHeight(SkinEditorChrome::kButtonHeight));
        }

        void separator()
        {
            constexpr int kSep = SkinEditorChrome::kButtonGap * 2 + 1;
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
            label.setBounds(row.removeFromLeft(width).withHeight(SkinEditorChrome::kButtonHeight));
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
            box.setBounds(row.removeFromLeft(width).withHeight(SkinEditorChrome::kButtonHeight));
        }
    };
}

void SkinEditorMainComponent::layoutTitleBar(juce::Rectangle<int> titleBar)
{
    const int wellH = SkinEditorChrome::kSessionWellHeight;
    const int wellY = titleBar.getY() + SkinEditorChrome::kChromeStackGap;
    const int sessionW = SkinEditorChrome::sessionWellPreferredWidth();
    layoutSessionWell({ titleBar.getX(), wellY, sessionW, wellH });

    const int opX = titleBar.getX() + sessionW + SkinEditorChrome::kButtonGap;
    layoutOperationWell({ opX, wellY, juce::jmax(0, titleBar.getRight() - opX), wellH });
}

void SkinEditorMainComponent::comboBoxChanged(juce::ComboBox* box)
{
    if (suppressCombo_)
        return;
    if (box == &skinsPanel_.productCombo())
        applyProductComboChange();
    else if (box == &skinsPanel_.editorCombo())
        applyEditorComboChange();
    else if (box == &jsonSyncCombo_)
        applyJsonSyncComboChange();
}

void SkinEditorMainComponent::setupSessionWell()
{
    SkinEditorChrome::stylePanelTitle(sessionTitleLabel_, "SESSION");
    addAndMakeVisible(sessionTitleLabel_);

    auto wire = [this](juce::TextButton& button, auto handler)
    {
        addAndMakeVisible(button);
        button.onClick = handler;
    };
    wire(undoButton_, [this] { onUndo(); });
    wire(redoButton_, [this] { onRedo(); });
    wire(compareButton_, [this] { onCompareToggle(); });
    wire(pushButton_, [this] { onPush(); });
    wire(exportButton_, [this] { onExportJson(); });
    wire(importButton_, [this] { onImportJson(); });

    jsonSyncLabel_.setText("JSON Sync:", juce::dontSendNotification);
    jsonSyncLabel_.setFont(SkinEditorChrome::fontRegular());
    jsonSyncLabel_.setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
    jsonSyncLabel_.setJustificationType(juce::Justification::centredLeft);
    SkinEditorChrome::preventTextSquish(jsonSyncLabel_);
    jsonSyncLabel_.setBorderSize({});
    jsonSyncLabel_.setTooltip(
        "How the Skin Editor reacts when the last imported JSON changes on disk");
    addAndMakeVisible(jsonSyncLabel_);

    jsonSyncCombo_.addItem("Off", static_cast<int>(TSS::DevSkin::JsonSyncMode::kOff) + 1);
    jsonSyncCombo_.addItem("Ask", static_cast<int>(TSS::DevSkin::JsonSyncMode::kAsk) + 1);
    jsonSyncCombo_.addItem("Auto", static_cast<int>(TSS::DevSkin::JsonSyncMode::kAuto) + 1);
    jsonSyncCombo_.setTooltip(
        "Off: ignore disk changes. Ask: prompt Reload/Discard. Auto: reload when clean.");
    addAndMakeVisible(jsonSyncCombo_);
    jsonSyncCombo_.addListener(this);
    syncJsonSyncComboFromSession();

    compareBlinkTimer_.onTick = [this] { tickCompareBlink(); };
    undoButton_.setEnabled(false);
    redoButton_.setEnabled(false);
}

void SkinEditorMainComponent::setupOperationWell()
{
    SkinEditorChrome::stylePanelTitle(operationTitleLabel_, "OPERATION");
    addAndMakeVisible(operationTitleLabel_);
}

void SkinEditorMainComponent::layoutSessionControls(juce::Rectangle<int> well,
                                                    juce::Rectangle<int> row)
{
    sessionSeparatorY_ = row.getY();
    sessionSeparatorH_ = row.getHeight();

    SessionWellPlacer place { row, true, &sessionSeparatorXs_ };
    const int sessionTitleW = SkinEditorChrome::titleTabWidth("SESSION");
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

    const int undoRedoW = SkinEditorChrome::sessionUndoRedoButtonWidth();
    place.button(undoButton_, undoRedoW);
    place.gap();
    place.button(redoButton_, undoRedoW);
    place.separator();
    place.button(compareButton_);
    place.gap();
    place.button(pushButton_, SkinEditorChrome::sessionPushButtonWidth());
    place.separator();
    place.button(exportButton_);
    place.gap();
    place.button(importButton_);
    place.separator();
    place.label(jsonSyncLabel_, SkinEditorChrome::jsonSyncLabelWidth());
    place.gap();
    place.combo(jsonSyncCombo_, SkinEditorChrome::jsonSyncComboWidth());
    if (place.fits && row.getWidth() >= SkinEditorChrome::kGutter)
        row.removeFromLeft(SkinEditorChrome::kGutter);
}

void SkinEditorMainComponent::layoutSessionWell(juce::Rectangle<int> well)
{
    sessionSeparatorXs_.clear();
    sessionWellBounds_ = {};
    if (well.isEmpty())
        return;

    // Compact well on the left of the title row (OPERATION stretches to the right).
    sessionWellBounds_ = well;
    layoutSessionControls(sessionWellBounds_,
                          { sessionWellBounds_.getX(),
                            sessionWellBounds_.getY() + SkinEditorChrome::kGutter,
                            sessionWellBounds_.getWidth(),
                            SkinEditorChrome::kButtonHeight });
}

void SkinEditorMainComponent::layoutBodyPanels(juce::Rectangle<int> bounds)
{
    auto left = bounds.removeFromLeft(SkinEditorChrome::kColorPanelWidth);
    const int skinsH = SkinEditorSkinsPanel::preferredHeight();
    const int colorH = SkinEditorColourPicker::preferredHeightForWidth(
        SkinEditorChrome::kColorPanelWidth);
    // SKINS↔COLOR: each panel's facing kGutter → kChromeStackGap. COLOR fixed height.
    skinsPanel_.setBounds(left.getX(), left.getY(), left.getWidth(), skinsH);
    colourPicker_.setBounds(left.getX(), left.getY() + skinsH, left.getWidth(), colorH);

    const int paletteGap = SkinEditorChrome::kPaletteSideGap;
    bounds.removeFromLeft(paletteGap);
    // Remainder = Palette + gap + Mappings; both Palette and Mapped colour share growth.
    const int paletteW = SkinEditorChrome::palettePanelWidthFromBodyRemainder(bounds.getWidth());
    const int mappingsW = SkinEditorChrome::mappingsPanelWidthForPalettePanel(paletteW);
    palettePanel_.setBounds(bounds.removeFromLeft(paletteW));
    bounds.removeFromLeft(paletteGap);
    mappingsPanel_.setBounds(bounds.removeFromLeft(mappingsW));
}
