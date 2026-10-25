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

void SkinEditorMainComponent::layoutSessionWell(juce::Rectangle<int> titleBar, int titleRight)
{
    sessionSeparatorXs_.clear();
    sessionWellBounds_ = {};

    const int wellW = SkinEditorChrome::sessionWellPreferredWidth();
    const int wellX = juce::jmax(titleRight + SkinEditorChrome::kButtonGap,
                                 titleBar.getRight() - wellW);
    sessionWellBounds_ = { wellX, titleBar.getY() + 2,
                           juce::jmin(wellW, titleBar.getRight() - wellX),
                           titleBar.getHeight() - 4 };

    const auto well = sessionWellBounds_;
    const int buttonY = well.getY()
                        + (well.getHeight() - SkinEditorChrome::kButtonHeight) / 2;
    auto row = juce::Rectangle<int>(well.getX(), buttonY, well.getWidth(),
                                    SkinEditorChrome::kButtonHeight);
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
}
