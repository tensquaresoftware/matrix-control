#include "MainComponent.h"

#include "Core/Services/DeviceTypeRegistry.h"
#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Helpers/LockDimmingFilmPolicy.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/SharedPanel/SharedPanel.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginIDs.h"

using TSS::SkinColourId;

namespace
{
    namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

    const juce::Identifier kDeviceDetectedId("deviceDetected");
    const juce::Identifier kDeviceTypeId(MatrixDeviceTypes::kApvtsPropertyName);
    const juce::Identifier kDeviceMidiUnresponsiveId(Core::kDeviceMidiUnresponsiveProperty);
}

MainComponent::MainComponent(const MainComponentConstructionArgs& args)
    : skin_(&args.skin)
    , layoutDimensions_(args.layoutDimensions)
    , footerPanel(args.skin, layoutDimensions_.footer, args.apvts)
    , headerPanel(args.skin, layoutDimensions_.header)
    , bodyPanel(args.skin, layoutDimensions_, args.widgetFactory, args.apvts, args.patchFileService)
{
    setOpaque(true);
    setSize(layoutDimensions_.editor.width, layoutDimensions_.editor.height);
    
    addAndMakeVisible(headerPanel);
    addAndMakeVisible(bodyPanel);
    addAndMakeVisible(footerPanel);
}

MainComponent::~MainComponent()
{
    if (apvts_ != nullptr)
        apvts_->state.removeListener(this);
}

void MainComponent::attachLockDimmingFilm(juce::AudioProcessorValueTreeState& apvts)
{
    if (lockDimmingFilm_ != nullptr)
        return;

    apvts_ = &apvts;
    lockDimmingFilm_ = std::make_unique<TSS::LockDimmingFilm>();
    addChildComponent(*lockDimmingFilm_);
    apvts_->state.addListener(this);
    refreshLockDimmingFilm();
}

void MainComponent::paint(juce::Graphics& g)
{
    if (skin_ != nullptr)
        g.fillAll(skin_->getColour(SkinColourId::kHeaderPanelBackground));
}

void MainComponent::resized()
{
    const auto bounds = getLocalBounds();
    const float sf = uiScale_;

    const int headerHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(layoutDimensions_.header.height), sf);
    const int bodyHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(layoutDimensions_.body.height), sf);
    const int footerHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(layoutDimensions_.footer.height), sf);
    const int footerY = headerHeight + bodyHeight;

    headerPanel.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), headerHeight);
#if JUCE_DEBUG
    uiElementsTestAreaY_ = headerHeight;

    if (uiElementsTestVisible_)
    {
        bodyPanel.setVisible(false);
        footerPanel.setVisible(false);
        if (lockDimmingFilm_ != nullptr)
            lockDimmingFilm_->setVisible(false);
        return;
    }
#endif

    bodyPanel.setVisible(true);
    footerPanel.setVisible(true);
    bodyPanel.setBounds(bounds.getX(), bounds.getY() + headerHeight, bounds.getWidth(), bodyHeight);
    footerPanel.setBounds(bounds.getX(), bounds.getY() + footerY, bounds.getWidth(), footerHeight);

    if (lockDimmingFilm_ != nullptr)
        lockDimmingFilm_->setBounds(bounds);

    refreshLockDimmingFilm();
}

#if JUCE_DEBUG
void MainComponent::setUiElementsTestVisible(bool visible)
{
    if (uiElementsTestVisible_ == visible)
        return;

    uiElementsTestVisible_ = visible;
    resized();
}

juce::Rectangle<int> MainComponent::getUiElementsTestAreaBounds() const
{
    const auto bounds = getLocalBounds();
    return bounds.withTrimmedTop(uiElementsTestAreaY_);
}
#endif

void MainComponent::setSkin(TSS::Skin& skin)
{
    skin_ = &skin;
    headerPanel.setSkin(skin);
    bodyPanel.setSkin(skin);
    footerPanel.setSkin(skin);
}

void MainComponent::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    headerPanel.setUiScale(uiScale_);
    bodyPanel.setUiScale(uiScale_);
    footerPanel.setUiScale(uiScale_);
    resized();
    repaint();
}

void MainComponent::setBusReorderHandler(BusReorderHandler handler)
{
    bodyPanel.setBusReorderHandler(std::move(handler));
}

void MainComponent::setMasterInitConfirmationGate(TSS::ModuleHeader::InitConfirmationGate gate)
{
    bodyPanel.setMasterInitConfirmationGate(std::move(gate));
}

void MainComponent::setEditorialUndoRedoKeyHandler(std::function<bool(const juce::KeyPress&)> handler)
{
    editorialUndoRedoKeyHandler_ = std::move(handler);
}

bool MainComponent::keyPressed(const juce::KeyPress& key)
{
    if (editorialUndoRedoKeyHandler_ != nullptr && editorialUndoRedoKeyHandler_(key))
        return true;

    return juce::Component::keyPressed(key);
}

void MainComponent::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if (property.toString() == MutatorState::kCompareActive
        || property == kDeviceDetectedId
        || property == kDeviceTypeId
        || property == kDeviceMidiUnresponsiveId)
    {
        refreshLockDimmingFilm();
    }
}

void MainComponent::valueTreeRedirected(juce::ValueTree&)
{
    refreshLockDimmingFilm();
}

std::vector<juce::Rectangle<int>> MainComponent::buildLockDimmingFilmHoles(bool includeCompareHole) const
{
    std::vector<juce::Rectangle<int>> holes;
    holes.reserve(includeCompareHole ? 3u : 2u);
    holes.push_back(headerPanel.getBounds());
    holes.push_back(footerPanel.getBounds());

    if (! includeCompareHole)
        return holes;

    auto& mutator = bodyPanel.getSharedPanel().getPatchManagerPanel().getPatchMutatorPanel();
    const auto compareLocal = mutator.getCompareButtonBounds();
    if (! compareLocal.isEmpty())
        holes.push_back(getLocalArea(&mutator, compareLocal));

    return holes;
}

void MainComponent::refreshLockDimmingFilm()
{
    if (lockDimmingFilm_ == nullptr || apvts_ == nullptr)
        return;

#if JUCE_DEBUG
    if (uiElementsTestVisible_)
    {
        lockDimmingFilm_->setHoles({});
        lockDimmingFilm_->setVisible(false);
        return;
    }
#endif

    const bool compareActive = static_cast<bool>(
        apvts_->state.getProperty(MutatorState::kCompareActive, false));
    const bool deviceDetected = static_cast<bool>(
        apvts_->state.getProperty(kDeviceDetectedId, false));
    const bool deviceMidiUnresponsive = static_cast<bool>(
        apvts_->state.getProperty(kDeviceMidiUnresponsiveId, false));
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts_->state.getProperty(kDeviceTypeId));

    const auto mode = TSS::resolveLockDimmingFilmMode(
        deviceDetected, deviceType, compareActive, deviceMidiUnresponsive);

    if (! TSS::lockDimmingFilmIsActive(mode))
    {
        lockDimmingFilm_->setHoles({});
        lockDimmingFilm_->setVisible(false);
        compareHoleRetryPending_ = false;
        return;
    }

    const bool includeCompareHole = TSS::lockDimmingFilmIncludesCompareHole(mode);
    auto holes = buildLockDimmingFilmHoles(includeCompareHole);
    const bool missingCompareHole = includeCompareHole && holes.size() < 3u;

    const bool becomingActive = ! lockDimmingFilm_->isVisible();

    lockDimmingFilm_->setBounds(getLocalBounds());
    lockDimmingFilm_->setHoles(std::move(holes));
    lockDimmingFilm_->setVisible(true);
    lockDimmingFilm_->toFront(false);

    // Steal focus only when the film turns on — not on every resize/scale refresh,
    // which would yank keyboard focus off the COMPARE hole.
    if (becomingActive)
        bodyPanel.giveAwayKeyboardFocus();

    // Mutator may finish laying out COMPARE after the lock property callback.
    if (missingCompareHole && ! compareHoleRetryPending_)
    {
        compareHoleRetryPending_ = true;
        juce::Component::SafePointer<MainComponent> safeThis(this);
        juce::MessageManager::callAsync([safeThis]
        {
            if (safeThis == nullptr)
                return;

            safeThis->compareHoleRetryPending_ = false;
            safeThis->refreshLockDimmingFilm();
        });
    }
}
