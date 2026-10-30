#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinEditor/SkinEditorChromeTable.h"
#include "Tools/SkinEditor/SkinEditorMappingModePolicy.h"
#include "Tools/SkinEditor/SkinEditorUiHelpers.h"

SkinEditorMappingPolicy::Mode SkinEditorMainComponent::policyModeFromMappingMode() const noexcept
{
    switch (mappingMode_)
    {
        case MappingMode::kHeldColour:    return SkinEditorMappingPolicy::Mode::kHeldColour;
        case MappingMode::kArmedRow:      return SkinEditorMappingPolicy::Mode::kArmedRow;
        case MappingMode::kEditingColour: return SkinEditorMappingPolicy::Mode::kEditingColour;
        case MappingMode::kIdle:          break;
    }
    return SkinEditorMappingPolicy::Mode::kIdle;
}

void SkinEditorMainComponent::onPastilleSelected(const juce::String& name)
{
    using Intent = SkinEditorMappingPolicy::PastilleIntent;

    const bool bindBlocked = ! SkinEditorMappingPolicy::canBindPastille(
        isPastillePending(name),
        isPastilleSameRgbAlreadyMapped(name));
    const auto intent = SkinEditorMappingPolicy::pastilleClick(policyModeFromMappingMode(),
                                                              canEditEditorSkin(),
                                                              name.isEmpty(),
                                                              bindBlocked);
    switch (intent)
    {
        case Intent::kIgnoreEmpty:
            return;
        case Intent::kSelectReadOnly:
            selectedPastille_ = name;
            syncColourPickerToSelection();
            return;
        case Intent::kBindArmedRow:
            selectedPastille_ = name;
            syncColourPickerToSelection();
            applyMapping(armedKey_, name, armedFromUnbound_);
            return;
        case Intent::kEnterOrChangeHeldColour:
        {
            const bool leftArmedBindBlocked = mappingMode_ == MappingMode::kArmedRow && bindBlocked;
            enterHeldColour(name);
            if (leftArmedBindBlocked)
            {
                if (isPastillePending(name))
                    setStatusMessage(SkinEditorMappingPolicy::kCalibrateBeforeMappingMessage);
                else if (isPastilleSameRgbAlreadyMapped(name))
                    setStatusMessage(SkinEditorMappingPolicy::kSameColourMappedStatusMessage);
            }
            return;
        }
    }
}

bool SkinEditorMainComponent::isKeyUnboundOnEditor(TSS::SkinColourKey key) const
{
    const auto* skin = layer_.findSkin(sessionState_.editorSkinId);
    if (skin == nullptr)
        return false;
    return skin->mappings.find(key) == skin->mappings.end();
}

void SkinEditorMainComponent::onMappingRowActivated(TSS::SkinColourKey key)
{
    using Intent = SkinEditorMappingPolicy::RowIntent;

    const bool fromUnbound = isKeyUnboundOnEditor(key);
    const bool bindBlocked = ! SkinEditorMappingPolicy::canBindPastille(
        isPastillePending(selectedPastille_),
        isPastilleSameRgbAlreadyMapped(selectedPastille_));
    const auto intent = SkinEditorMappingPolicy::rowClick(policyModeFromMappingMode(),
                                                         canEditEditorSkin(),
                                                         bindBlocked);
    switch (intent)
    {
        case Intent::kIgnore:
            if (mappingMode_ == MappingMode::kHeldColour)
            {
                if (isPastillePending(selectedPastille_))
                    setStatusMessage(SkinEditorMappingPolicy::kCalibrateBeforeMappingMessage);
                else if (isPastilleSameRgbAlreadyMapped(selectedPastille_))
                    setStatusMessage(SkinEditorMappingPolicy::kSameColourMappedStatusMessage);
            }
            return;
        case Intent::kBindHeldColour:
            applyMapping(key, selectedPastille_, fromUnbound);
            return;
        case Intent::kEnterOrChangeArmedRow:
            enterArmedRow(key, fromUnbound);
            return;
    }
}

juce::String SkinEditorMainComponent::boundPastilleNameFor(TSS::SkinColourKey key) const
{
    const auto* skin = layer_.findSkin(visibleSkinId());
    if (skin == nullptr)
        return {};

    const auto mapped = skin->mappings.find(key);
    if (mapped == skin->mappings.end())
        return {};

    for (const auto& pastille : skin->palette)
        if (pastille.name == mapped->second)
            return pastille.name;
    return {};
}

bool SkinEditorMainComponent::isPastillePending(const juce::String& pastilleName) const
{
    if (pastilleName.isEmpty())
        return false;
    const auto* skin = layer_.findSkin(sessionState_.editorSkinId);
    if (skin == nullptr)
        return false;
    for (const auto& pastille : skin->palette)
        if (pastille.name == pastilleName)
            return pastille.pending;
    return false;
}

bool SkinEditorMainComponent::isPastilleSameRgbAlreadyMapped(const juce::String& pastilleName) const
{
    const auto* skin = layer_.findSkin(sessionState_.editorSkinId);
    if (skin == nullptr)
        return false;
    return SkinEditorUi::isPastilleArgbMappedUnderOtherName(*skin, pastilleName);
}

void SkinEditorMainComponent::onMappedColourClicked(TSS::SkinColourKey key)
{
    // Edit only: never enters HeldColour and never writes a mapping.
    const auto name = boundPastilleNameFor(key);
    if (SkinEditorMappingPolicy::mappedColourClick(false, name.isNotEmpty())
        != SkinEditorMappingPolicy::MappedColourIntent::kSelectForEdit)
        return;

    enterEditingColour(name);
}

void SkinEditorMainComponent::enterHeldColour(const juce::String& pastilleName)
{
    mappingMode_ = MappingMode::kHeldColour;
    selectedPastille_ = pastilleName;
    armedKey_ = {};
    armedFromUnbound_ = false;
    syncColourPickerToSelection();
    updateMappingWitness();
    grabKeyboardFocus();
}

void SkinEditorMainComponent::enterArmedRow(TSS::SkinColourKey key, bool fromUnbound)
{
    mappingMode_ = MappingMode::kArmedRow;
    armedKey_ = key;
    armedFromUnbound_ = fromUnbound;
    updateMappingWitness();
    grabKeyboardFocus();
}

void SkinEditorMainComponent::enterEditingColour(const juce::String& pastilleName)
{
    mappingMode_ = MappingMode::kEditingColour;
    selectedPastille_ = pastilleName;
    armedKey_ = {};
    armedFromUnbound_ = false;
    palettePanel_.selectPastille(pastilleName);
    syncColourPickerToSelection();
    updateMappingWitness();
    grabKeyboardFocus();
}

void SkinEditorMainComponent::clearMappingModeToIdle()
{
    // Production Escape / panel onEscapeRequested land here; afterEscape() is the Idle SSOT.
    jassert(SkinEditorMappingPolicy::afterEscape() == SkinEditorMappingPolicy::Mode::kIdle);
    mappingMode_ = MappingMode::kIdle;
    selectedPastille_.clear();
    armedKey_ = {};
    armedFromUnbound_ = false;
    palettePanel_.clearSelection();
    colourPicker_.clearActiveColour();
    colourPicker_.setEnabledEditing(false);
    updateMappingWitness();
}

void SkinEditorMainComponent::retainMappingModeAfterWrite(TSS::SkinColourKey key, bool fromUnbound)
{
    const auto policyMode = policyModeFromMappingMode();
    const auto next = SkinEditorMappingPolicy::afterSuccessfulWrite(policyMode,
                                                                   mappingsPanel_.isHoldColour());
    if (next == SkinEditorMappingPolicy::Mode::kIdle)
    {
        clearMappingModeToIdle();
        return;
    }

    if (next == SkinEditorMappingPolicy::Mode::kArmedRow)
        enterArmedRow(key, fromUnbound);
    else
        updateMappingWitness();
}

void SkinEditorMainComponent::applyMapping(TSS::SkinColourKey key,
                                           const juce::String& pastilleName,
                                           bool fromUnbound)
{
    if (! canEditEditorSkin() || pastilleName.isEmpty())
        return;

    const bool pending = isPastillePending(pastilleName);
    const bool sameRgbMapped = isPastilleSameRgbAlreadyMapped(pastilleName);
    if (! SkinEditorMappingPolicy::canBindPastille(pending, sameRgbMapped))
    {
        setStatusMessage(pending ? SkinEditorMappingPolicy::kCalibrateBeforeMappingMessage
                                 : SkinEditorMappingPolicy::kSameColourMappedStatusMessage);
        return;
    }

    if (const auto* skin = layer_.findSkin(sessionState_.editorSkinId))
    {
        const auto it = skin->mappings.find(key);
        if (it != skin->mappings.end() && it->second == pastilleName)
        {
            retainMappingModeAfterWrite(key, fromUnbound);
            return;
        }
    }

    const auto result = fromUnbound
                            ? layer_.bindUnbound(sessionState_.editorSkinId, key, pastilleName)
                            : layer_.setMapping(sessionState_.editorSkinId, key, pastilleName);
    if (result.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               fromUnbound ? "Bind failed" : "Mapping failed",
                                               result.getErrorMessage());
        return;
    }

    retainMappingModeAfterWrite(key, fromUnbound);
}

void SkinEditorMainComponent::fillWitnessSwatchForSelectedPastille()
{
    if (const auto* skin = layer_.findSkin(visibleSkinId()))
    {
        for (const auto& pastille : skin->palette)
        {
            if (pastille.name == selectedPastille_)
            {
                mappingWitnessShowSwatch_ = true;
                mappingWitnessPending_ = pastille.pending;
                mappingWitnessColour_ = juce::Colour(pastille.argb);
                return;
            }
        }
    }
}

void SkinEditorMainComponent::fillHeldColourWitness()
{
    const bool pending = isPastillePending(selectedPastille_);
    mappingWitnessSameColourCollision_ = ! pending
                                         && isPastilleSameRgbAlreadyMapped(selectedPastille_);
    const auto next = pending
                          ? juce::String(SkinEditorMappingPolicy::kHeldPendingNextStep)
                          : mappingWitnessSameColourCollision_
                                ? juce::String(SkinEditorMappingPolicy::kSameColourMappedNextStep)
                                : SkinEditorMappingPolicy::withHoldColourSuffix(
                                      SkinEditorMappingPolicy::kHeldColourNextStep,
                                      mappingsPanel_.isHoldColour(),
                                      true);
    setWitnessTexts(SkinEditorMappingPolicy::kSelectedColourRole, selectedPastille_, next);
    fillWitnessSwatchForSelectedPastille();
}

void SkinEditorMainComponent::fillArmedRowWitness()
{
    // Slash hierarchy - avoids looking like the OPERATION next-step arrow.
    const auto identity = TSS::DevSkin::toString(armedKey_.object) + " / "
                          + TSS::DevSkin::toString(armedKey_.property) + " / "
                          + TSS::DevSkin::toString(armedKey_.state);
    setWitnessTexts(SkinEditorMappingPolicy::kSelectedTargetRole,
                    identity,
                    SkinEditorMappingPolicy::withHoldColourSuffix(
                        SkinEditorMappingPolicy::kArmedRowNextStep,
                        mappingsPanel_.isHoldColour(),
                        false));
}

void SkinEditorMainComponent::fillEditingColourWitness()
{
    setWitnessTexts(SkinEditorMappingPolicy::kEditingColourRole,
                    selectedPastille_,
                    SkinEditorMappingPolicy::kEditingColourNextStep);
    fillWitnessSwatchForSelectedPastille();
}

void SkinEditorMainComponent::setWitnessTexts(const juce::String& role,
                                              const juce::String& identity,
                                              const juce::String& nextStep)
{
    // Tooltips keep the full text; layoutOperationWitness may elide the visible label.
    mappingPrefixLabel_.setTooltip(role);
    mappingPrefixLabel_.setText(role, juce::dontSendNotification);
    mappingNameLabel_.setTooltip(identity);
    mappingNameLabel_.setText(identity, juce::dontSendNotification);
    mappingDetailLabel_.setTooltip(nextStep);
    mappingDetailLabel_.setText(nextStep, juce::dontSendNotification);
}

void SkinEditorMainComponent::paintMappingWitnessDecorations(juce::Graphics& g) const
{
    if (operationStartHintActive_)
    {
        if (! mappingBadgeBounds_.isEmpty())
            SkinEditorChrome::drawChromeBadge(g, mappingBadgeBounds_,
                                              SkinEditorUi::kNoEditorSkinBadge,
                                              SkinEditorChrome::attention());
        return;
    }

    if (! mappingWitnessActive_)
        return;
    if (! mappingBadgeBounds_.isEmpty())
    {
        if (mappingWitnessSameColourCollision_)
            SkinEditorChrome::drawChromeBadge(g,
                                              mappingBadgeBounds_,
                                              SkinEditorMappingPolicy::kSameColourBadge,
                                              SkinEditorChrome::danger());
        else
            SkinEditorChrome::drawMappingModeBadge(g, mappingBadgeBounds_);
    }
    if (mappingWitnessShowSwatch_ && ! mappingSwatchBounds_.isEmpty())
    {
        if (mappingWitnessPending_)
            SkinEditorChrome::paintPendingColourSwatch(g, mappingSwatchBounds_);
        else
            SkinEditorChrome::paintColourSwatch(g, mappingSwatchBounds_, mappingWitnessColour_);
    }
    if (! mappingArrowBounds_.isEmpty())
        SkinEditorChrome::drawOperationNextArrow(g, mappingArrowBounds_,
                                                 SkinEditorChrome::textMuted());
}

void SkinEditorMainComponent::updateMappingWitness()
{
    const auto policyMode = policyModeFromMappingMode();
    operationStartHintActive_ = sessionState_.editorSkinId.isEmpty()
                                && mappingMode_ == MappingMode::kIdle;
    mappingWitnessActive_ = ! operationStartHintActive_
                            && SkinEditorMappingPolicy::mappingWitnessVisible(policyMode);
    mappingWitnessShowSwatch_ = false;
    mappingWitnessPending_ = false;
    mappingWitnessSameColourCollision_ = false;
    mappingWitnessColour_ = juce::Colours::transparentBlack;

    if (operationStartHintActive_)
    {
        mappingPrefixLabel_.setTooltip({});
        mappingPrefixLabel_.setText({}, juce::dontSendNotification);
        mappingNameLabel_.setTooltip({});
        mappingNameLabel_.setText({}, juce::dontSendNotification);
        mappingDetailLabel_.setTooltip(SkinEditorUi::kNoEditorSkinHint);
        mappingDetailLabel_.setText(SkinEditorUi::kNoEditorSkinHint, juce::dontSendNotification);
    }
    else if (mappingMode_ == MappingMode::kHeldColour)
        fillHeldColourWitness();
    else if (mappingMode_ == MappingMode::kArmedRow)
        fillArmedRowWitness();
    else if (mappingMode_ == MappingMode::kEditingColour)
        fillEditingColourWitness();
    else
    {
        mappingPrefixLabel_.setTooltip({});
        mappingPrefixLabel_.setText({}, juce::dontSendNotification);
        mappingNameLabel_.setTooltip({});
        mappingNameLabel_.setText({}, juce::dontSendNotification);
        mappingDetailLabel_.setTooltip({});
        mappingDetailLabel_.setText({}, juce::dontSendNotification);
    }

    mappingPrefixLabel_.setVisible(mappingWitnessActive_);
    mappingNameLabel_.setVisible(mappingWitnessActive_);
    mappingDetailLabel_.setVisible(mappingWitnessActive_ || operationStartHintActive_);
    resized();
    repaint();
}

void SkinEditorMainComponent::syncColourPickerToSelection()
{
    if (selectedPastille_.isEmpty())
    {
        colourPicker_.clearActiveColour();
        colourPicker_.setEnabledEditing(false);
        return;
    }

    if (const auto* skin = layer_.findSkin(visibleSkinId()))
    {
        for (const auto& pastille : skin->palette)
        {
            if (pastille.name == selectedPastille_)
            {
                colourPicker_.setColour(juce::Colour(pastille.argb), juce::dontSendNotification);
                colourPicker_.setEnabledEditing(canEditEditorSkin());
                return;
            }
        }
    }

    colourPicker_.clearActiveColour();
    colourPicker_.setEnabledEditing(false);
}
