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
            if (! fits)
            {
                b.setBounds({});
                return;
            }
            const int w = SkinEditorChrome::measureButtonWidth(b);
            if (row.getWidth() < w)
            {
                b.setBounds({});
                stop();
                return;
            }
            b.setBounds(row.removeFromLeft(w).withHeight(SkinEditorChrome::kButtonHeight));
        }

        void separator()
        {
            constexpr int kSep = SkinEditorChrome::kButtonGap * 4 + 1;
            if (! fits || row.getWidth() < kSep)
            {
                stop();
                return;
            }
            gap();
            gap();
            if (separatorXs != nullptr)
                separatorXs->push_back(row.getX());
            row.removeFromLeft(1);
            gap();
            gap();
        }
    };
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
        row.removeFromLeft(sessionTitleW);
        place.gap();
    }

    place.button(undoButton_);
    place.gap();
    place.button(redoButton_);
    place.separator();
    place.button(compareButton_);
    place.gap();
    place.button(pushButton_);
    place.separator();
    place.button(exportButton_);
    place.gap();
    place.button(importButton_);
    if (place.fits && row.getWidth() >= SkinEditorChrome::kGutter)
        row.removeFromLeft(SkinEditorChrome::kGutter);
}

void SkinEditorMainComponent::layoutSessionWell(juce::Rectangle<int> titleBar, int titleRight)
{
    sessionSeparatorXs_.clear();
    sessionWellBounds_ = {};

    // Compact well on the right (logo stays outside on dark fill). titleBar is
    // already kGutter-inset so the well's right edge matches OPERATION / SKINS column.
    const int wellH = SkinEditorChrome::kSessionWellHeight;
    const int wellY = titleBar.getY() + SkinEditorChrome::kChromeStackGap;
    const int wellW = SkinEditorChrome::sessionWellPreferredWidth();
    const int wellX = juce::jmax(titleRight + SkinEditorChrome::kButtonGap,
                                 titleBar.getRight() - wellW);
    sessionWellBounds_ = { wellX, wellY,
                           juce::jmin(wellW, titleBar.getRight() - wellX),
                           wellH };
    if (sessionWellBounds_.isEmpty())
        return;

    layoutSessionControls(sessionWellBounds_,
                          { sessionWellBounds_.getX(),
                            sessionWellBounds_.getY() + SkinEditorChrome::kGutter,
                            sessionWellBounds_.getWidth(),
                            SkinEditorChrome::kButtonHeight });
}
