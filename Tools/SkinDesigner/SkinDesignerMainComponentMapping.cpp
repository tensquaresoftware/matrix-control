#include "Tools/SkinDesigner/SkinDesignerMainComponent.h"

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinDesigner/SkinDesignerChromeTable.h"
#include "Tools/SkinDesigner/SkinDesignerMappingModePolicy.h"
#include "Tools/SkinDesigner/SkinDesignerUiHelpers.h"

SkinDesignerMappingPolicy::Mode SkinDesignerMainComponent::policyModeFromMappingMode() const noexcept
{
    switch (mappingMode_)
    {
        case MappingMode::kHeldColour:    return SkinDesignerMappingPolicy::Mode::kHeldColour;
        case MappingMode::kArmedRow:      return SkinDesignerMappingPolicy::Mode::kArmedRow;
        case MappingMode::kEditingColour: return SkinDesignerMappingPolicy::Mode::kEditingColour;
        case MappingMode::kIdle:          break;
    }
    return SkinDesignerMappingPolicy::Mode::kIdle;
}

void SkinDesignerMainComponent::onPastilleSelected(const juce::String& name)
{
    using Intent = SkinDesignerMappingPolicy::PastilleIntent;

    const bool bindBlocked = ! SkinDesignerMappingPolicy::canBindPastille(
        isPastillePending(name),
        isPastilleSameRgbAlreadyMapped(name));
    const auto intent = SkinDesignerMappingPolicy::pastilleClick(policyModeFromMappingMode(),
                                                              canEditDesignerSkin(),
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
                setStatusMessage(isPastillePending(name)
                                     ? SkinDesignerMappingPolicy::kCalibrateBeforeMappingMessage
                                     : SkinDesignerMappingPolicy::kSameColourMappedStatusMessage);
            return;
        }
    }
}

bool SkinDesignerMainComponent::isKeyUnboundOnDesigner(TSS::SkinColourKey key) const
{
    const auto* skin = layer_.findSkin(sessionState_.designerSkinId);
    if (skin == nullptr)
        return false;
    return skin->mappings.find(key) == skin->mappings.end();
}

void SkinDesignerMainComponent::onMappingRowActivated(TSS::SkinColourKey key)
{
    using Intent = SkinDesignerMappingPolicy::RowIntent;

    const bool fromUnbound = isKeyUnboundOnDesigner(key);
    const bool bindBlocked = ! SkinDesignerMappingPolicy::canBindPastille(
        isPastillePending(selectedPastille_),
        isPastilleSameRgbAlreadyMapped(selectedPastille_));
    const auto intent = SkinDesignerMappingPolicy::rowClick(policyModeFromMappingMode(),
                                                         canEditDesignerSkin(),
                                                         bindBlocked);
    switch (intent)
    {
        case Intent::kIgnore:
            if (mappingMode_ == MappingMode::kHeldColour)
                setStatusMessage(isPastillePending(selectedPastille_)
                                     ? SkinDesignerMappingPolicy::kCalibrateBeforeMappingMessage
                                     : SkinDesignerMappingPolicy::kSameColourMappedStatusMessage);
            return;
        case Intent::kBindHeldColour:
            applyMapping(key, selectedPastille_, fromUnbound);
            return;
        case Intent::kEnterOrChangeArmedRow:
            enterArmedRow(key, fromUnbound);
            return;
    }
}

juce::String SkinDesignerMainComponent::boundPastilleNameFor(TSS::SkinColourKey key) const
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

bool SkinDesignerMainComponent::isPastillePending(const juce::String& pastilleName) const
{
    if (pastilleName.isEmpty())
        return false;
    const auto* skin = layer_.findSkin(sessionState_.designerSkinId);
    if (skin == nullptr)
        return false;
    for (const auto& pastille : skin->palette)
        if (pastille.name == pastilleName)
            return pastille.pending;
    return false;
}

bool SkinDesignerMainComponent::isPastilleSameRgbAlreadyMapped(const juce::String& pastilleName) const
{
    const auto* skin = layer_.findSkin(sessionState_.designerSkinId);
    if (skin == nullptr)
        return false;
    return SkinDesignerUi::isPastilleArgbMappedUnderOtherName(*skin, pastilleName);
}

void SkinDesignerMainComponent::onMappedColourClicked(TSS::SkinColourKey key)
{
    // Edit only: never enters HeldColour and never writes a mapping.
    const auto name = boundPastilleNameFor(key);
    if (SkinDesignerMappingPolicy::mappedColourClick(false, name.isNotEmpty())
        != SkinDesignerMappingPolicy::MappedColourIntent::kSelectForEdit)
        return;

    enterEditingColour(name);
}

void SkinDesignerMainComponent::enterHeldColour(const juce::String& pastilleName)
{
    mappingMode_ = MappingMode::kHeldColour;
    selectedPastille_ = pastilleName;
    armedKey_ = {};
    armedFromUnbound_ = false;
    syncColourPickerToSelection();
    updateMappingWitness();
    grabKeyboardFocus();
}

void SkinDesignerMainComponent::enterArmedRow(TSS::SkinColourKey key, bool fromUnbound)
{
    mappingMode_ = MappingMode::kArmedRow;
    armedKey_ = key;
    armedFromUnbound_ = fromUnbound;
    mappingsPanel_.setPropertyBindHoverEnabled(false);
    updateMappingWitness();
    grabKeyboardFocus();
}

void SkinDesignerMainComponent::enterEditingColour(const juce::String& pastilleName)
{
    mappingMode_ = MappingMode::kEditingColour;
    selectedPastille_ = pastilleName;
    armedKey_ = {};
    armedFromUnbound_ = false;
    palettePanel_.selectPastille(pastilleName);
    syncColourPickerToSelection();
    mappingsPanel_.setPropertyBindHoverEnabled(false);
    updateMappingWitness();
    grabKeyboardFocus();
}

void SkinDesignerMainComponent::clearMappingModeToIdle()
{
    // Production Escape / panel onEscapeRequested land here; afterEscape() is the Idle SSOT.
    jassert(SkinDesignerMappingPolicy::afterEscape() == SkinDesignerMappingPolicy::Mode::kIdle);
    mappingMode_ = MappingMode::kIdle;
    selectedPastille_.clear();
    armedKey_ = {};
    armedFromUnbound_ = false;
    palettePanel_.clearSelection();
    colourPicker_.clearActiveColour();
    colourPicker_.setEnabledEditing(false);
    mappingsPanel_.setPropertyBindHoverEnabled(false);
    updateMappingWitness();
}

void SkinDesignerMainComponent::retainMappingModeAfterWrite(TSS::SkinColourKey key, bool fromUnbound)
{
    const auto policyMode = policyModeFromMappingMode();
    const auto next = SkinDesignerMappingPolicy::afterSuccessfulWrite(policyMode,
                                                                   mappingsPanel_.isHoldColour());
    if (next == SkinDesignerMappingPolicy::Mode::kIdle)
    {
        clearMappingModeToIdle();
        return;
    }

    if (next == SkinDesignerMappingPolicy::Mode::kArmedRow)
        enterArmedRow(key, fromUnbound);
    else
        updateMappingWitness();
}

void SkinDesignerMainComponent::applyMapping(TSS::SkinColourKey key,
                                           const juce::String& pastilleName,
                                           bool fromUnbound)
{
    if (! canEditDesignerSkin() || pastilleName.isEmpty())
        return;

    const bool pending = isPastillePending(pastilleName);
    const bool sameRgbMapped = isPastilleSameRgbAlreadyMapped(pastilleName);
    if (! SkinDesignerMappingPolicy::canBindPastille(pending, sameRgbMapped))
    {
        setStatusMessage(pending ? SkinDesignerMappingPolicy::kCalibrateBeforeMappingMessage
                                 : SkinDesignerMappingPolicy::kSameColourMappedStatusMessage);
        return;
    }

    bool writerAlreadyMapped = false;
    if (const auto* skin = layer_.findSkin(sessionState_.designerSkinId))
    {
        const auto it = skin->mappings.find(key);
        writerAlreadyMapped = (it != skin->mappings.end() && it->second == pastilleName);
    }

    if (! writerAlreadyMapped)
    {
        const auto result = fromUnbound
                                ? layer_.bindUnbound(sessionState_.designerSkinId, key, pastilleName)
                                : layer_.setMapping(sessionState_.designerSkinId, key, pastilleName);
        if (result.failed())
        {
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                                   fromUnbound ? "Bind failed" : "Mapping failed",
                                                   result.getErrorMessage());
            return;
        }
    }

    const auto peerResult = layer_.propagateMappingToLinkPeers(sessionState_.designerSkinId,
                                                               key,
                                                               pastilleName);
    if (peerResult.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Link mapping",
                                               peerResult.getErrorMessage());
        return;
    }

    retainMappingModeAfterWrite(key, fromUnbound);
}

void SkinDesignerMainComponent::fillWitnessSwatchForSelectedPastille()
{
    juce::Colour hoverPreview;
    bool foundInPalette = false;
    if (const auto* skin = layer_.findSkin(visibleSkinId()))
        for (const auto& pastille : skin->palette)
            if (pastille.name == selectedPastille_)
            {
                mappingWitnessShowSwatch_ = true;
                mappingWitnessPending_ = pastille.pending;
                mappingWitnessColour_ = juce::Colour(pastille.argb);
                foundInPalette = true;
                if (! pastille.pending)
                    hoverPreview = mappingWitnessColour_;
                break;
            }
    if (mappingMode_ != MappingMode::kHeldColour)
        return;
    const bool canBind = foundInPalette
                         && SkinDesignerMappingPolicy::canBindPastille(
                             isPastillePending(selectedPastille_),
                             isPastilleSameRgbAlreadyMapped(selectedPastille_));
    mappingsPanel_.setPropertyBindHoverEnabled(canBind, hoverPreview);
}

void SkinDesignerMainComponent::fillHeldColourWitness()
{
    const bool pending = isPastillePending(selectedPastille_);
    mappingWitnessSameColourCollision_ = ! pending
                                         && isPastilleSameRgbAlreadyMapped(selectedPastille_);
    juce::String next = SkinDesignerMappingPolicy::kHeldPendingNextStep;
    if (! pending)
        next = mappingWitnessSameColourCollision_
                   ? juce::String(SkinDesignerMappingPolicy::kSameColourMappedNextStep)
                   : SkinDesignerMappingPolicy::withHoldColourSuffix(
                         SkinDesignerMappingPolicy::kHeldColourNextStep,
                         mappingsPanel_.isHoldColour(), true);
    setWitnessTexts(SkinDesignerMappingPolicy::kSelectedColourRole, selectedPastille_, next);
    fillWitnessSwatchForSelectedPastille();
}

void SkinDesignerMainComponent::fillArmedRowWitness()
{
    // Slash hierarchy - avoids looking like the OPERATION next-step arrow.
    const auto identity = TSS::DevSkin::toString(armedKey_.object) + " / "
                          + TSS::DevSkin::toString(armedKey_.property) + " / "
                          + TSS::DevSkin::toString(armedKey_.state);
    setWitnessTexts(SkinDesignerMappingPolicy::kSelectedTargetRole,
                    identity,
                    SkinDesignerMappingPolicy::withHoldColourSuffix(
                        SkinDesignerMappingPolicy::kArmedRowNextStep,
                        mappingsPanel_.isHoldColour(),
                        false));
}

void SkinDesignerMainComponent::fillEditingColourWitness()
{
    setWitnessTexts(SkinDesignerMappingPolicy::kEditingColourRole,
                    selectedPastille_,
                    SkinDesignerMappingPolicy::kEditingColourNextStep);
    fillWitnessSwatchForSelectedPastille();
}

void SkinDesignerMainComponent::setWitnessTexts(const juce::String& role,
                                              const juce::String& identity,
                                              const juce::String& nextStep)
{
    mappingWitnessRoleFull_ = role;
    mappingWitnessIdentityFull_ = identity;
    mappingWitnessDetailFull_ = nextStep;
    mappingPrefixLabel_.setText(role, juce::dontSendNotification);
    mappingNameLabel_.setText(identity, juce::dontSendNotification);
    mappingDetailLabel_.setText(nextStep, juce::dontSendNotification);
}

void SkinDesignerMainComponent::paintMappingWitnessDecorations(juce::Graphics& g) const
{
    if (operationStartHintActive_)
    {
        if (! mappingBadgeBounds_.isEmpty())
            SkinDesignerChrome::drawChromeBadge(g, mappingBadgeBounds_,
                                              SkinDesignerUi::kNoDesignerSkinBadge,
                                              SkinDesignerChrome::attention());
        return;
    }

    if (! mappingWitnessActive_)
        return;
    if (! mappingBadgeBounds_.isEmpty())
    {
        if (mappingWitnessSameColourCollision_)
            SkinDesignerChrome::drawChromeBadge(g,
                                              mappingBadgeBounds_,
                                              SkinDesignerMappingPolicy::kSameColourBadge,
                                              SkinDesignerChrome::danger());
        else if (mappingWitnessPending_)
            // Pending Temp: same danger fill as BRIDGE/SAME COLOUR; label stays MAPPING.
            SkinDesignerChrome::drawChromeBadge(g,
                                              mappingBadgeBounds_,
                                              "MAPPING",
                                              SkinDesignerChrome::danger());
        else
            SkinDesignerChrome::drawMappingModeBadge(g, mappingBadgeBounds_);
    }
    if (mappingWitnessShowSwatch_ && ! mappingSwatchBounds_.isEmpty())
    {
        if (mappingWitnessPending_)
            SkinDesignerChrome::paintPendingColourSwatch(g, mappingSwatchBounds_);
        else
            SkinDesignerChrome::paintColourSwatch(g, mappingSwatchBounds_, mappingWitnessColour_);
    }
    if (! mappingArrowBounds_.isEmpty())
        SkinDesignerChrome::drawOperationNextArrow(g, mappingArrowBounds_,
                                                 SkinDesignerChrome::textMuted());
}

void SkinDesignerMainComponent::updateMappingWitness()
{
    const auto policyMode = policyModeFromMappingMode();
    operationStartHintActive_ = sessionState_.designerSkinId.isEmpty()
                                && mappingMode_ == MappingMode::kIdle;
    mappingWitnessActive_ = ! operationStartHintActive_
                            && SkinDesignerMappingPolicy::mappingWitnessVisible(policyMode);
    mappingWitnessShowSwatch_ = false;
    mappingWitnessPending_ = false;
    mappingWitnessSameColourCollision_ = false;
    mappingWitnessColour_ = juce::Colours::transparentBlack;

    if (operationStartHintActive_)
    {
        mappingWitnessRoleFull_.clear();
        mappingWitnessIdentityFull_.clear();
        mappingWitnessDetailFull_ = SkinDesignerUi::kNoDesignerSkinHint;
        mappingPrefixLabel_.setText({}, juce::dontSendNotification);
        mappingNameLabel_.setText({}, juce::dontSendNotification);
        mappingDetailLabel_.setText(SkinDesignerUi::kNoDesignerSkinHint, juce::dontSendNotification);
    }
    else if (mappingMode_ == MappingMode::kHeldColour)
        fillHeldColourWitness();
    else if (mappingMode_ == MappingMode::kArmedRow)
        fillArmedRowWitness();
    else if (mappingMode_ == MappingMode::kEditingColour)
        fillEditingColourWitness();
    else
    {
        mappingWitnessRoleFull_.clear();
        mappingWitnessIdentityFull_.clear();
        mappingWitnessDetailFull_.clear();
        mappingPrefixLabel_.setText({}, juce::dontSendNotification);
        mappingNameLabel_.setText({}, juce::dontSendNotification);
        mappingDetailLabel_.setText({}, juce::dontSendNotification);
    }

    mappingPrefixLabel_.setVisible(mappingWitnessActive_);
    mappingNameLabel_.setVisible(mappingWitnessActive_);
    mappingDetailLabel_.setVisible(mappingWitnessActive_ || operationStartHintActive_);
    resized();
    repaint();
}

void SkinDesignerMainComponent::syncColourPickerToSelection()
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
                colourPicker_.setEnabledEditing(canEditDesignerSkin());
                return;
            }
        }
    }

    colourPicker_.clearActiveColour();
    colourPicker_.setEnabledEditing(false);
}
