#include "Tools/SkinEditor/SkinEditorMainComponent.h"

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "Tools/SkinEditor/SkinEditorMappingModePolicy.h"

void SkinEditorMainComponent::onPastilleSelected(const juce::String& name)
{
    using Intent = SkinEditorMappingPolicy::PastilleIntent;

    auto policyMode = SkinEditorMappingPolicy::Mode::kIdle;
    if (mappingMode_ == MappingMode::kHeldColour)
        policyMode = SkinEditorMappingPolicy::Mode::kHeldColour;
    else if (mappingMode_ == MappingMode::kArmedRow)
        policyMode = SkinEditorMappingPolicy::Mode::kArmedRow;

    const auto intent = SkinEditorMappingPolicy::pastilleClick(policyMode,
                                                              canEditEditorSkin(),
                                                              name.isEmpty());
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
            enterHeldColour(name);
            return;
    }
}

void SkinEditorMainComponent::onMappingRowActivated(TSS::SkinColourKey key, bool fromUnbound)
{
    using Intent = SkinEditorMappingPolicy::RowIntent;

    auto policyMode = SkinEditorMappingPolicy::Mode::kIdle;
    if (mappingMode_ == MappingMode::kHeldColour)
        policyMode = SkinEditorMappingPolicy::Mode::kHeldColour;
    else if (mappingMode_ == MappingMode::kArmedRow)
        policyMode = SkinEditorMappingPolicy::Mode::kArmedRow;

    const auto intent = SkinEditorMappingPolicy::rowClick(policyMode, canEditEditorSkin());
    switch (intent)
    {
        case Intent::kIgnore:
            return;
        case Intent::kBindHeldColour:
            applyMapping(key, selectedPastille_, fromUnbound);
            return;
        case Intent::kEnterOrChangeArmedRow:
            enterArmedRow(key, fromUnbound);
            return;
    }
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
    auto policyMode = SkinEditorMappingPolicy::Mode::kIdle;
    if (mappingMode_ == MappingMode::kHeldColour)
        policyMode = SkinEditorMappingPolicy::Mode::kHeldColour;
    else if (mappingMode_ == MappingMode::kArmedRow)
        policyMode = SkinEditorMappingPolicy::Mode::kArmedRow;

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

void SkinEditorMainComponent::updateMappingWitness()
{
    auto policyMode = SkinEditorMappingPolicy::Mode::kIdle;
    if (mappingMode_ == MappingMode::kHeldColour)
        policyMode = SkinEditorMappingPolicy::Mode::kHeldColour;
    else if (mappingMode_ == MappingMode::kArmedRow)
        policyMode = SkinEditorMappingPolicy::Mode::kArmedRow;

    mappingWitnessActive_ = SkinEditorMappingPolicy::mappingWitnessVisible(policyMode);
    mappingWitnessShowSwatch_ = false;
    mappingWitnessColour_ = juce::Colours::transparentBlack;

    if (mappingMode_ == MappingMode::kHeldColour)
    {
        mappingDetailLabel_.setText("Colour: " + selectedPastille_, juce::dontSendNotification);
        if (const auto* skin = layer_.findSkin(visibleSkinId()))
        {
            for (const auto& pastille : skin->palette)
            {
                if (pastille.name == selectedPastille_)
                {
                    mappingWitnessShowSwatch_ = true;
                    mappingWitnessColour_ = juce::Colour(pastille.argb);
                    break;
                }
            }
        }
    }
    else if (mappingMode_ == MappingMode::kArmedRow)
    {
        const auto target = TSS::DevSkin::toString(armedKey_.object) + " / "
                            + TSS::DevSkin::toString(armedKey_.property);
        mappingDetailLabel_.setText("Target: " + target, juce::dontSendNotification);
    }
    else
    {
        mappingDetailLabel_.setText({}, juce::dontSendNotification);
    }

    mappingDetailLabel_.setVisible(mappingWitnessActive_);
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
