#include "Tools/SkinDesigner/SkinDesignerMainComponent.h"

#include "GUI/Skins/Dev/DevSkinJson.h"
#include "Tools/SkinDesigner/SkinDesignerAlertWindow.h"
#include "Tools/SkinDesigner/SkinDesignerChromeTokens.h"
#include "Tools/SkinDesigner/SkinDesignerUiHelpers.h"

void SkinDesignerMainComponent::onDuplicateProduct()
{
    if (! duplicateFromSkin(sessionState_.productSkinId))
        return;
    // Compare was cleared in adopt; push only when Standalone Dev is connected.
    if (bridge_.isConnected())
        pushVisible();
}

void SkinDesignerMainComponent::onDuplicateDesigner()
{
    if (sessionState_.designerSkinId.isEmpty())
        return;
    (void) duplicateFromSkin(sessionState_.designerSkinId);
}

bool SkinDesignerMainComponent::duplicateFromSkin(const juce::String& sourceId)
{
    const auto* source = layer_.findSkin(sourceId);
    const auto sourceName = source != nullptr ? source->name : juce::String("My Skin");
    const auto suggested = SkinDesignerUi::suggestUniqueSkinName(layer_, sourceName);
    const auto name = SkinDesignerUi::askText("Duplicate",
                                            "Duplicate \"" + sourceName + "\" as:",
                                            suggested);
    if (name.isEmpty())
        return false;

    juce::Result result = juce::Result::fail("Duplicate failed");
    runWithoutLayerChangeNotify([this, &sourceId, &name, &result]
    {
        result = layer_.duplicateSkin(sourceId, name);
    });
    if (result.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Duplicate failed", result.getErrorMessage());
        return false;
    }

    adoptActiveDesignerSkinAfterCreate();
    return true;
}

void SkinDesignerMainComponent::onNew()
{
    // Product palette + custom Designer pastilles; mappings cleared → full Unbound list.
    const auto name = SkinDesignerUi::askText("New Designer skin", "Name:",
                                            SkinDesignerUi::suggestUniqueSkinName(layer_, "New Skin"));
    if (name.isEmpty())
        return;

    juce::Result result = juce::Result::fail("New failed");
    runWithoutLayerChangeNotify([this, &name, &result]
    {
        result = layer_.createDesignerSkinWithUnboundMappings(
            sessionState_.productSkinId, name, sessionState_.designerSkinId);
    });
    if (result.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "New failed", result.getErrorMessage());
        return;
    }

    adoptActiveDesignerSkinAfterCreate();
}

void SkinDesignerMainComponent::onDelete()
{
    if (! canEditDesignerSkin())
        return;

    const auto* skin = layer_.findSkin(sessionState_.designerSkinId);
    if (skin == nullptr)
        return;

    SkinDesignerChrome::AlertWindow confirm("Delete Designer skin",
                                          "Delete \"" + skin->name + "\"? This cannot be undone.",
                                          juce::MessageBoxIconType::WarningIcon);
    confirm.addButton("Delete", 1);
    confirm.addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
    if (confirm.runModalLoop() != 1)
        return;

    juce::Result result = juce::Result::fail("Delete failed");
    runWithoutLayerChangeNotify([this, &result]
    {
        result = layer_.deleteDevSkin(sessionState_.designerSkinId);
    });
    if (result.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Delete failed", result.getErrorMessage());
        return;
    }
    sessionState_.designerSkinId = {};
    clearMappingModeToIdle();
    // refreshAll (inside) picks the next remaining Designer skin, so record after it.
    finishSilentDocumentEdit();
}

void SkinDesignerMainComponent::onRename()
{
    if (! canEditDesignerSkin())
        return;
    const auto* skin = layer_.findSkin(sessionState_.designerSkinId);
    if (skin == nullptr)
        return;
    const auto name = SkinDesignerUi::askText("Rename Designer skin", "New name:", skin->name);
    if (name.isEmpty())
        return;
    const auto result = layer_.renameDevSkin(skin->id, name);
    if (result.failed())
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Rename failed", result.getErrorMessage());
}

void SkinDesignerMainComponent::syncCompareButtonChrome()
{
    auto& compare = skinsPanel_.compareButton();
    compare.setButtonText("Compare");
    compare.setToggleState(sessionState_.compareOn, juce::dontSendNotification);

    if (sessionState_.compareOn)
    {
        if (! compareBlinkTimer_.isTimerRunning())
        {
            compareBlinkLit_ = true;
            compareBlinkTimer_.startTimerHz(2); // ~500 ms phase
        }
        const auto colour = compareBlinkLit_ ? SkinDesignerChrome::danger()
                                             : SkinDesignerChrome::textMuted();
        compare.setColour(juce::TextButton::textColourOffId, colour);
        compare.setColour(juce::TextButton::textColourOnId, colour);
    }
    else
    {
        compareBlinkTimer_.stopTimer();
        compareBlinkLit_ = true;
        const auto normal = SkinDesignerChrome::textPrimary();
        compare.setColour(juce::TextButton::textColourOffId, normal);
        compare.setColour(juce::TextButton::textColourOnId, normal);
    }
    compare.repaint();
}

void SkinDesignerMainComponent::tickCompareBlink()
{
    if (! sessionState_.compareOn)
        return;

    compareBlinkLit_ = ! compareBlinkLit_;
    const auto colour = compareBlinkLit_ ? SkinDesignerChrome::danger()
                                         : SkinDesignerChrome::textMuted();
    auto& compare = skinsPanel_.compareButton();
    compare.setColour(juce::TextButton::textColourOffId, colour);
    compare.setColour(juce::TextButton::textColourOnId, colour);
    compare.repaint();
}

void SkinDesignerMainComponent::onCompareToggle()
{
    sessionState_.compareOn = ! sessionState_.compareOn;
    refreshAll();
    if (bridge_.isConnected())
        pushVisible();
}

void SkinDesignerMainComponent::onAddPastille()
{
    if (! canEditDesignerSkin())
        return;

    const auto* skin = layer_.findSkin(sessionState_.designerSkinId);
    if (skin == nullptr)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Add failed", "Designer skin not found");
        return;
    }

    const auto name = SkinDesignerUi::suggestUniqueTempPastilleName(*skin);
    const auto argb = SkinDesignerUi::suggestUniqueOpaqueArgb(*skin);
    // Select before add so onChanged refresh highlights the new row without mapping click.
    const auto previousSelection = selectedPastille_;
    selectedPastille_ = name;
    const auto result = layer_.addPastille(sessionState_.designerSkinId, name, argb, true);
    if (result.failed())
    {
        selectedPastille_ = previousSelection;
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Add failed", result.getErrorMessage());
        return;
    }
    // refreshAll already quiet-selected; keep picker in sync if mapping mode was idle.
    syncColourPickerToSelection();
}

void SkinDesignerMainComponent::onDuplicatePastille()
{
    if (! canEditDesignerSkin())
        return;

    const auto sourceName = palettePanel_.getSelectedPastilleName();
    if (sourceName.isEmpty())
        return;

    const auto* skin = layer_.findSkin(sessionState_.designerSkinId);
    if (skin == nullptr)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Duplicate failed", "Designer skin not found");
        return;
    }

    juce::uint32 sourceArgb = 0;
    bool found = false;
    for (const auto& pastille : skin->palette)
    {
        if (pastille.name == sourceName)
        {
            sourceArgb = pastille.argb;
            found = true;
            break;
        }
    }
    if (! found)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Duplicate failed", "Colour not found");
        return;
    }

    const auto name = SkinDesignerUi::suggestUniquePastilleName(*skin, sourceName);
    if (name.isEmpty())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Duplicate failed", "Could not name the copy");
        return;
    }

    // Select before add so onChanged refresh highlights the new row without mapping click.
    const auto previousSelection = selectedPastille_;
    selectedPastille_ = name;
    const auto result = layer_.addPastille(sessionState_.designerSkinId, name, sourceArgb, false);
    if (result.failed())
    {
        selectedPastille_ = previousSelection;
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Duplicate failed", result.getErrorMessage());
        return;
    }
    syncColourPickerToSelection();
}

void SkinDesignerMainComponent::onRenamePastille(const juce::String& pastilleName)
{
    if (! canEditDesignerSkin() || pastilleName.isEmpty())
        return;
    if (layer_.isProductPastilleName(pastilleName))
        return;

    const auto name = SkinDesignerUi::askText("Rename colour", "New name:", pastilleName);
    if (name.isEmpty() || name == pastilleName)
        return;

    // Update selection before rename so onChanged refresh finds the new name.
    const bool wasSelected = selectedPastille_ == pastilleName;
    if (wasSelected)
        selectedPastille_ = name;

    const auto result = layer_.renamePastille(sessionState_.designerSkinId, pastilleName, name);
    if (result.failed())
    {
        if (wasSelected)
            selectedPastille_ = pastilleName;
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Rename failed", result.getErrorMessage());
    }
}

void SkinDesignerMainComponent::onDeletePastille()
{
    if (! canEditDesignerSkin())
        return;
    const auto name = palettePanel_.getSelectedPastilleName();
    if (name.isEmpty() || layer_.isProductPastilleName(name))
        return;

    bool removeRefs = false;
    const int refs = layer_.countMappingsReferencing(sessionState_.designerSkinId, name);
    if (refs > 0)
    {
        SkinDesignerChrome::AlertWindow window("Delete colour",
                                             name + " is used by " + juce::String(refs)
                                                 + " mapping(s). Delete and clear those mappings?",
                                             juce::MessageBoxIconType::WarningIcon);
        window.addButton("Delete", 1);
        window.addButton("Cancel", 0);
        if (window.runModalLoop() != 1)
            return;
        removeRefs = true;
    }

    const auto result = layer_.deletePastille(sessionState_.designerSkinId, name, removeRefs);
    if (result.failed())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                               "Delete failed", result.getErrorMessage());
        return;
    }
    if (selectedPastille_ == name)
    {
        if (mappingMode_ == MappingMode::kHeldColour
            || mappingMode_ == MappingMode::kEditingColour)
            clearMappingModeToIdle();
        else
            selectedPastille_.clear();
    }
}

void SkinDesignerMainComponent::onColourChanged(juce::Colour colour)
{
    if (! canEditDesignerSkin() || selectedPastille_.isEmpty())
        return;
    const auto result = layer_.setPastilleArgb(sessionState_.designerSkinId,
                                               selectedPastille_,
                                               colour.getARGB());
    if (result.failed())
        setStatusMessage(result.getErrorMessage());
    else if (mappingMode_ == MappingMode::kHeldColour
             || mappingMode_ == MappingMode::kEditingColour)
        updateMappingWitness();
}
