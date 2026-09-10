#include "PatchNameDisplayPanel.h"

#include "GUI/Helpers/GrayedControlHelper.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/PatchNameDisplay.h"
#include "Core/Services/PatchMutator/MutationNaming.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    using PluginIDs::PatchEditSection::PatchNameModule::kPatchName;
    using PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kDefaultPatchName;
    using PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kCompareSecondaryLabel;
    using PluginDisplayNames::PatchEditSection::PatchNameModule::Messages::kInvalidCharacterFooter;

    namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;
    namespace InternalPatches = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;
}

PatchNameDisplayPanel::PatchNameDisplayPanel(TSS::ISkin& skin,
                                             int width,
                                             int height,
                                             const PatchNameDisplayDimensions& patchNameDims,
                                             const ModuleHeaderDimensions& moduleHeaderDims,
                                             juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , patchNameDims_(patchNameDims)
    , moduleHeaderDims_(moduleHeaderDims)
    , apvts_(apvts)
    , moduleHeader_(std::make_unique<TSS::ModuleHeader>(
          patchNameDims_.width,
          moduleHeaderDims_.height,
          TSS::moduleHeaderLookFromSkin(skin),
          TSS::ModuleHeader::ColourVariant::Blue,
          PluginDisplayNames::PatchEditSection::PatchNameModule::kName,
          moduleHeaderDims_))
    , patchNameDisplay_(std::make_unique<TSS::PatchNameDisplay>(
          patchNameDims_.width,
          patchNameDims_.height,
          TSS::patchNameDisplayLookFromSkin(skin)))
{
    setOpaque(false);
    setSize(width_, height_);
    addAndMakeVisible(*moduleHeader_);
    addAndMakeVisible(*patchNameDisplay_);

    patchNameDisplay_->onCommit([this](juce::String newName)
    {
        if (renameCommitHandler_)
            renameCommitHandler_(newName);
    });

    patchNameDisplay_->onIllegalCharacter([this]()
    {
        TSS::GrayedControlHelper::setFooterErrorMessage(apvts_, kInvalidCharacterFooter);
    });

    patchNameDisplay_->onIllegalCharacterCleared([this]()
    {
        clearInvalidCharacterFooterIfPresent();
    });

    patchNameDisplay_->onEditEnded([this]()
    {
        clearInvalidCharacterFooterIfPresent();
    });

    apvts_.state.addListener(this);
    syncFromApvtsState();
}

PatchNameDisplayPanel::~PatchNameDisplayPanel()
{
    apvts_.state.removeListener(this);
}

TSS::PatchNameDisplay& PatchNameDisplayPanel::getPatchNameDisplay()
{
    return *patchNameDisplay_;
}

void PatchNameDisplayPanel::applyDragOverlay(DragOverlayKind kind,
                                             const juce::String& previewPrimaryName)
{
    if (patchNameDisplay_ == nullptr)
        return;

    namespace Overlay = PluginDisplayNames::PatchEditSection::PatchNameModule::DragDropOverlay;

    switch (kind)
    {
        case DragOverlayKind::kValidSingle:
            patchNameDisplay_->showDragOverlay(
                previewPrimaryName.isNotEmpty() ? previewPrimaryName : juce::String(Overlay::kBadPrimary),
                Overlay::kDropToLoad);
            break;

        case DragOverlayKind::kValidSelection:
            patchNameDisplay_->showDragOverlay(Overlay::kPatchesEllipsis, Overlay::kDropToLoad);
            break;

        case DragOverlayKind::kInvalid:
            patchNameDisplay_->showDragOverlay(Overlay::kBadPrimary, Overlay::kBadFile);
            break;

        case DragOverlayKind::kInvalidPlural:
            patchNameDisplay_->showDragOverlay(Overlay::kBadPrimary, Overlay::kBadFiles);
            break;
    }
}

void PatchNameDisplayPanel::clearDragOverlay()
{
    if (patchNameDisplay_ == nullptr)
        return;

    patchNameDisplay_->clearDragOverlay();
    syncFromApvtsState();
}

void PatchNameDisplayPanel::armNameRequired()
{
    if (patchNameDisplay_ == nullptr)
        return;

    ensureNameRequiredOutcomeForwarder();
    patchNameDisplay_->armNameRequired();
}

void PatchNameDisplayPanel::clearNameRequired()
{
    if (patchNameDisplay_ == nullptr)
        return;

    patchNameDisplay_->clearNameRequired();
    syncFromApvtsState();
}

bool PatchNameDisplayPanel::isNameRequiredArmed() const
{
    return patchNameDisplay_ != nullptr && patchNameDisplay_->isNameRequiredArmed();
}

void PatchNameDisplayPanel::setCanEditProvider(CanEditProvider provider)
{
    canEditProvider_ = std::move(provider);
    syncFromApvtsState();
}

void PatchNameDisplayPanel::setRenameCommitHandler(RenameCommitHandler handler)
{
    renameCommitHandler_ = std::move(handler);
}

void PatchNameDisplayPanel::setNameRequiredOutcomeHandler(NameRequiredOutcomeHandler handler)
{
    nameRequiredOutcomeHandler_ = std::move(handler);
    ensureNameRequiredOutcomeForwarder();
}

void PatchNameDisplayPanel::ensureNameRequiredOutcomeForwarder()
{
    if (patchNameDisplay_ == nullptr)
        return;

    patchNameDisplay_->onNameRequiredOutcome([this](bool success)
    {
        if (nameRequiredOutcomeHandler_)
            nameRequiredOutcomeHandler_(success);

        // Mode cleared in the widget — refresh editable/secondary that sync skipped while armed.
        syncFromApvtsState();
    });
}

void PatchNameDisplayPanel::resized()
{
    auto area = getLocalBounds();
    const float sf = uiScale_;

    area.removeFromTop(TSS::ScaledLayout::scaledInt(static_cast<float>(patchNameDims_.topPadding), sf));

    if (moduleHeader_ != nullptr)
        moduleHeader_->setBounds(area.removeFromTop(
            TSS::ScaledLayout::scaledInt(static_cast<float>(moduleHeaderDims_.height), sf)));

    area.removeFromTop(TSS::ScaledLayout::scaledInt(
        static_cast<float>(patchNameDims_.moduleHeaderToDisplayGap), sf));

    area.removeFromBottom(TSS::ScaledLayout::scaledInt(
        static_cast<float>(patchNameDims_.bottomPadding), sf));

    if (patchNameDisplay_ != nullptr)
        patchNameDisplay_->setBounds(area.removeFromTop(
            TSS::ScaledLayout::scaledInt(static_cast<float>(patchNameDims_.height), sf)));
}

void PatchNameDisplayPanel::setSkin(TSS::ISkin& skin)
{
    if (moduleHeader_ != nullptr)
        moduleHeader_->setLook(TSS::moduleHeaderLookFromSkin(skin));
    if (patchNameDisplay_ != nullptr)
        patchNameDisplay_->setLook(TSS::patchNameDisplayLookFromSkin(skin));
}

void PatchNameDisplayPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;

    if (moduleHeader_ != nullptr)
        moduleHeader_->setUiScale(uiScale_);
    if (patchNameDisplay_ != nullptr)
        patchNameDisplay_->setUiScale(uiScale_);

    resized();
    repaint();
}

bool PatchNameDisplayPanel::isTrackedProperty(const juce::String& propertyName)
{
    return propertyName == kPatchName
        || propertyName == MutatorState::kCompareActive
        || propertyName == MutatorState::kInitialSelected
        || propertyName == MutatorState::kHistoryMutateList
        || propertyName == MutatorState::kSelectedMutateRootIndex
        || propertyName == MutatorState::kSelectedRetryIndex
        || propertyName == InternalPatches::kCurrentBankNumber
        || propertyName == InternalPatches::kCurrentPatchNumber
        || propertyName == PluginIDs::Settings::kMatrix1000PatchesNamesMode;
}

void PatchNameDisplayPanel::valueTreePropertyChanged(juce::ValueTree&,
                                                     const juce::Identifier& property)
{
    if (! isTrackedProperty(property.toString()))
        return;

    // Any of these properties changing while name edit / name-required is open is an interrupt
    // (Mutate/Retry/Compare/patch nav/load) — abandon before refreshing the display.
    if (patchNameDisplay_ != nullptr
        && (patchNameDisplay_->isEditing() || patchNameDisplay_->isNameRequiredArmed()))
    {
        patchNameDisplay_->cancelEdit();
    }

    syncFromApvtsState();
}

void PatchNameDisplayPanel::valueTreeRedirected(juce::ValueTree&)
{
    if (patchNameDisplay_ != nullptr
        && (patchNameDisplay_->isEditing() || patchNameDisplay_->isNameRequiredArmed()))
    {
        patchNameDisplay_->cancelEdit();
    }
    syncFromApvtsState();
}

juce::String PatchNameDisplayPanel::computeSecondaryLabel() const
{
    const bool compareActive = static_cast<bool>(
        apvts_.state.getProperty(MutatorState::kCompareActive, false));

    if (compareActive)
        return kCompareSecondaryLabel;

    const bool historyListEmpty =
        apvts_.state.getProperty(MutatorState::kHistoryMutateList).toString().isEmpty();
    const int selectedRootIndex = static_cast<int>(
        apvts_.state.getProperty(MutatorState::kSelectedMutateRootIndex, -1));

    if (historyListEmpty || selectedRootIndex < 0)
        return {};

    // HISTORY INITIAL auditions the same origin snapshot as Compare, without the lock.
    if (static_cast<bool>(apvts_.state.getProperty(MutatorState::kInitialSelected, false)))
        return kCompareSecondaryLabel;

    const int selectedRetryIndex = static_cast<int>(apvts_.state.getProperty(
        MutatorState::kSelectedRetryIndex, MutatorState::kSelectedRetryRootOnly));

    return Core::MutationNaming::formatPatchName(selectedRootIndex, selectedRetryIndex);
}

void PatchNameDisplayPanel::clearInvalidCharacterFooterIfPresent()
{
    if (apvts_.state.getProperty("uiMessageText").toString() != juce::String(kInvalidCharacterFooter))
        return;

    apvts_.state.setProperty("uiMessageText", juce::String(), nullptr);
    apvts_.state.setProperty("uiMessageSeverity", juce::String(), nullptr);
}

void PatchNameDisplayPanel::syncFromApvtsState()
{
    if (patchNameDisplay_ == nullptr)
        return;

    if (patchNameDisplay_->isDragOverlayActive())
        return;

    auto name = apvts_.state.getProperty(kPatchName, juce::String(kDefaultPatchName)).toString();
    if (name.isEmpty())
        name = kDefaultPatchName;

    patchNameDisplay_->setPatchName(name);
    patchNameDisplay_->setSecondaryLabel(computeSecondaryLabel());

    // Name-required owns the edit session; do not flip editable mid-session via APVTS noise.
    if (! patchNameDisplay_->isNameRequiredArmed())
        patchNameDisplay_->setEditable(canEditProvider_ ? canEditProvider_() : false);
}
