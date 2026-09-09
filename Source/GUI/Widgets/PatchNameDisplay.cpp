#include "PatchNameDisplay.h"

#include "Core/Services/PatchNameEditRules.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace TSS
{
    PatchNameDisplay::PatchNameDisplay(int width, int height, const PatchNameDisplayLook& look)
        : look_(look)
        , width_(width)
        , height_(height)
        , patchName_(PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kDefaultPatchName)
    {
        setOpaque(false);
        setSize(width_, height_);
        setWantsKeyboardFocus(true);
    }

    PatchNameDisplay::~PatchNameDisplay()
    {
        stopTimer();
        detachOutsideClickListener();
    }

    void PatchNameDisplay::setLook(const PatchNameDisplayLook& look)
    {
        look_ = look;
        repaint();
    }

    void PatchNameDisplay::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        repaint();
    }

    void PatchNameDisplay::setPatchName(const juce::String& patchName)
    {
        if (dragOverlayActive_)
        {
            patchName_ = patchName;
            return;
        }

        patchName_ = patchName;
        repaint();
    }

    void PatchNameDisplay::setSecondaryLabel(const juce::String& secondaryLabel)
    {
        if (dragOverlayActive_ || nameRequiredArmed_)
        {
            secondaryLabel_ = secondaryLabel;
            return;
        }

        if (secondaryLabel_ == secondaryLabel)
            return;

        secondaryLabel_ = secondaryLabel;
        repaint();
    }

    void PatchNameDisplay::setEditable(bool editable)
    {
        if (editable_ == editable)
            return;

        editable_ = editable;

        if (! editable_ && ! nameRequiredArmed_)
        {
            hoveredPrimary_ = false;
            if (editing_)
                cancelEdit();
        }

        repaint();
    }

    void PatchNameDisplay::onCommit(std::function<void(juce::String)> callback)
    {
        onCommit_ = std::move(callback);
    }

    void PatchNameDisplay::onIllegalCharacter(std::function<void()> callback)
    {
        onIllegalCharacter_ = std::move(callback);
    }

    void PatchNameDisplay::onIllegalCharacterCleared(std::function<void()> callback)
    {
        onIllegalCharacterCleared_ = std::move(callback);
    }

    void PatchNameDisplay::onEditEnded(std::function<void()> callback)
    {
        onEditEnded_ = std::move(callback);
    }

    void PatchNameDisplay::onNameRequiredOutcome(std::function<void(bool success)> callback)
    {
        onNameRequiredOutcome_ = std::move(callback);
    }

    void PatchNameDisplay::armNameRequired()
    {
        if (dragOverlayActive_)
        {
            nameRequiredArmed_ = true;
            return;
        }

        if (nameRequiredArmed_ && editing_)
            return;

        // End a normal rename cleanly (onEditEnded) before entering name-required.
        if (editing_ && ! nameRequiredArmed_)
            cancelEdit();

        nameRequiredArmed_ = true;
        enterNameRequiredEditSession();
    }

    void PatchNameDisplay::clearNameRequired()
    {
        if (! nameRequiredArmed_)
            return;

        nameRequiredArmed_ = false;

        if (dragOverlayActive_)
        {
            // Armed under drag overlay — abort without touching the overlay timer.
            notifyNameRequiredOutcome(false);
            return;
        }

        if (editing_)
        {
            endEditSessionVisuals();
            editing_ = false;
            illegalCharPending_ = false;
            if (onEditEnded_)
                onEditEnded_();
        }

        repaint();
        notifyNameRequiredOutcome(false);
    }

    void PatchNameDisplay::enterNameRequiredEditSession()
    {
        editBuffer_.clear();
        caretIndex_ = 0;
        caretOn_ = true;
        blinkSecondaryVisible_ = true;
        illegalCharPending_ = false;
        editing_ = true;
        hoveredPrimary_ = false;

        grabKeyboardFocus();
        attachOutsideClickListener();
        startTimerHz(kDragSecondaryBlinkHz_);
        repaint();
    }

    void PatchNameDisplay::beginEdit()
    {
        if (! editable_ || editing_ || dragOverlayActive_ || nameRequiredArmed_)
            return;

        // Fresh empty field — do not preload the current patch name.
        editBuffer_.clear();
        caretIndex_ = 0;
        caretOn_ = true;
        illegalCharPending_ = false;
        editing_ = true;

        grabKeyboardFocus();
        attachOutsideClickListener();
        startTimer(kCaretBlinkIntervalMs_);
        repaint();
    }

    void PatchNameDisplay::notifyNameRequiredOutcome(bool success)
    {
        if (onNameRequiredOutcome_)
            onNameRequiredOutcome_(success);
    }

    void PatchNameDisplay::endEditSessionVisuals()
    {
        stopTimer();
        detachOutsideClickListener();
    }

    void PatchNameDisplay::cancelEdit()
    {
        if (! editing_ && ! nameRequiredArmed_)
            return;

        const bool wasNameRequired = nameRequiredArmed_;

        // Name-required only suspended under drag: disarm + abort, keep overlay blink alive.
        if (dragOverlayActive_ && wasNameRequired)
        {
            nameRequiredArmed_ = false;
            editing_ = false;
            illegalCharPending_ = false;
            notifyNameRequiredOutcome(false);
            return;
        }

        nameRequiredArmed_ = false;

        endEditSessionVisuals();
        editing_ = false;
        illegalCharPending_ = false;
        blinkSecondaryVisible_ = true;
        repaint();

        if (onEditEnded_)
            onEditEnded_();

        if (wasNameRequired)
            notifyNameRequiredOutcome(false);
    }

    void PatchNameDisplay::commitEdit()
    {
        if (! editing_)
            return;

        const bool wasNameRequired = nameRequiredArmed_;
        const auto nameRequiredEnd = wasNameRequired
            ? Core::PatchNameEditRules::resolveNameRequiredCommitEndState(editBuffer_, patchName_)
            : Core::PatchNameEditRules::NameRequiredCommitEndState {};
        const auto resolvedName = wasNameRequired
            ? nameRequiredEnd.resolvedName
            : Core::PatchNameEditRules::resolveCommittedPatchName(editBuffer_, patchName_);

        nameRequiredArmed_ = false;
        endEditSessionVisuals();
        editing_ = false;
        illegalCharPending_ = false;
        blinkSecondaryVisible_ = true;
        repaint();

        if (! wasNameRequired || nameRequiredEnd.shouldInvokeRenameCommit)
        {
            if (onCommit_)
                onCommit_(resolvedName);
        }

        if (onEditEnded_)
            onEditEnded_();

        if (wasNameRequired)
            notifyNameRequiredOutcome(nameRequiredEnd.success);
    }

    void PatchNameDisplay::suspendEditForDragOverlay()
    {
        // Keep nameRequiredArmed_ so clearDragOverlay can restore the session.
        endEditSessionVisuals();
        editing_ = false;
        illegalCharPending_ = false;
        caretOn_ = true;
    }

    void PatchNameDisplay::attachOutsideClickListener()
    {
        if (listeningForOutsideClicks_)
            return;

        juce::Desktop::getInstance().addGlobalMouseListener(this);
        listeningForOutsideClicks_ = true;
    }

    void PatchNameDisplay::detachOutsideClickListener()
    {
        if (! listeningForOutsideClicks_)
            return;

        juce::Desktop::getInstance().removeGlobalMouseListener(this);
        listeningForOutsideClicks_ = false;
    }

    int PatchNameDisplay::maxCaretIndex() const noexcept
    {
        if (editBuffer_.isEmpty())
            return 0;

        if (editBuffer_.length() >= kNameLength_)
            return kNameLength_ - 1;

        return editBuffer_.length();
    }
}
