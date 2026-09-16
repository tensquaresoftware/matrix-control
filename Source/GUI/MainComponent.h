#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Helpers/LockDimmingFilm.h"
#include "GUI/Layout/PanelDimensions.h"
#include "Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "Panels/MainComponent/BodyPanel/BodyPanel.h"
#include "Panels/MainComponent/FooterPanel/FooterPanel.h"
#include "GUI/Widgets/ModuleHeader.h"

namespace Core
{
    class PatchFileService;
}

namespace TSS
{
    class Skin;
}

class WidgetFactory;

struct MainComponentConstructionArgs
{
    TSS::Skin& skin;
    const GuiLayoutDimensions& layoutDimensions;
    WidgetFactory& widgetFactory;
    juce::AudioProcessorValueTreeState& apvts;
    const Core::PatchFileService& patchFileService;
};

class MainComponent : public juce::Component,
                      private juce::ValueTree::Listener
{
public:
    explicit MainComponent(const MainComponentConstructionArgs& args);
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(TSS::Skin& skin);
    void setUiScale(float uiScale);

    /** Idempotent — call once from PluginEditor::createUiShell so the film is attached. */
    void attachLockDimmingFilm(juce::AudioProcessorValueTreeState& apvts);

    HeaderPanel& getHeaderPanel() { return headerPanel; }
    BodyPanel& getBodyPanel() { return bodyPanel; }
    FooterPanel& getFooterPanel() { return footerPanel; }

    using BusReorderHandler = std::function<void(int fromBus, int toBus)>;

    void setBusReorderHandler(BusReorderHandler handler);

    void setMasterInitConfirmationGate(TSS::ModuleHeader::InitConfirmationGate gate);

    /** Routes editorial Undo/Redo shortcuts from deep widget focus (Matrix Mod combos, etc.). */
    void setEditorialUndoRedoKeyHandler(std::function<bool(const juce::KeyPress&)> handler);

    bool keyPressed(const juce::KeyPress& key) override;

#if JUCE_DEBUG
    void setUiElementsTestVisible(bool visible);
    juce::Rectangle<int> getUiElementsTestAreaBounds() const;
#endif

private:
    void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override;
    void valueTreeRedirected(juce::ValueTree& tree) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}

    void refreshLockDimmingFilm();
    std::vector<juce::Rectangle<int>> buildLockDimmingFilmHoles(bool includeCompareHole) const;

    TSS::Skin* skin_;
    GuiLayoutDimensions layoutDimensions_;
    float uiScale_ = 1.0f;
    juce::AudioProcessorValueTreeState* apvts_ = nullptr;
    bool compareHoleRetryPending_ = false;
#if JUCE_DEBUG
    bool uiElementsTestVisible_ = false;
    int uiElementsTestAreaY_ = 0;
#endif

    // Footer before Header/Body so panel binders can clear HELP during teardown without UAF.
    FooterPanel footerPanel;
    HeaderPanel headerPanel;
    BodyPanel bodyPanel;
    std::unique_ptr<TSS::LockDimmingFilm> lockDimmingFilm_;

    std::function<bool(const juce::KeyPress&)> editorialUndoRedoKeyHandler_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
