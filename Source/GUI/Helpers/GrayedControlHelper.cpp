#include "GrayedControlHelper.h"

#include <memory>
#include <unordered_map>

namespace TSS::GrayedControlHelper
{
    namespace
    {
        class GrayedClickForwarder;

        std::unordered_map<juce::Component*, std::unique_ptr<GrayedClickForwarder>>& forwarders()
        {
            static std::unordered_map<juce::Component*, std::unique_ptr<GrayedClickForwarder>> map;
            return map;
        }

        class GrayedClickForwarder : public juce::MouseListener,
                                     public juce::ComponentListener
        {
        public:
            GrayedClickForwarder(juce::Component& component, std::function<void()> showFooter)
                : owner_(&component)
                , showFooter_(std::move(showFooter))
            {
                owner_->addMouseListener(this, false);
                owner_->addComponentListener(this);
            }

            ~GrayedClickForwarder() override
            {
                detachFromOwner();
            }

            void setGrayed(bool grayed) { grayed_ = grayed; }

            void setShowFooter(std::function<void()> showFooter)
            {
                showFooter_ = std::move(showFooter);
            }

            void mouseDown(const juce::MouseEvent&) override
            {
                if (grayed_ && showFooter_)
                    showFooter_();
            }

            void componentBeingDeleted(juce::Component& component) override
            {
                if (owner_ != &component)
                    return;

                owner_ = nullptr;
                forwarders().erase(&component);
            }

        private:
            void detachFromOwner()
            {
                if (owner_ == nullptr)
                    return;

                owner_->removeMouseListener(this);
                owner_->removeComponentListener(this);
                owner_ = nullptr;
            }

            juce::Component* owner_ = nullptr;
            bool grayed_ = false;
            std::function<void()> showFooter_;
        };
    }

    void applyGrayedAppearance(juce::Component& component, bool grayed)
    {
        component.setAlpha(grayed ? 0.5f : 1.0f);
    }

    void setFooterInfoMessage(juce::AudioProcessorValueTreeState& apvts, const juce::String& message)
    {
        apvts.state.setProperty("uiMessageText", message, nullptr);
        apvts.state.setProperty("uiMessageSeverity", "info", nullptr);
    }

    void setFooterWarningMessage(juce::AudioProcessorValueTreeState& apvts, const juce::String& message)
    {
        apvts.state.setProperty("uiMessageText", message, nullptr);
        apvts.state.setProperty("uiMessageSeverity", "warning", nullptr);
    }

    void setGrayedClickHandler(juce::Component& component,
                               bool grayed,
                               std::function<void()> showFooter)
    {
        if (! grayed)
        {
            clearGrayedClickHandler(component);
            return;
        }

        auto& map = forwarders();
        auto it = map.find(&component);

        if (it == map.end())
        {
            it = map.emplace(&component,
                             std::make_unique<GrayedClickForwarder>(component, std::move(showFooter))).first;
        }
        else
        {
            it->second->setShowFooter(std::move(showFooter));
        }

        it->second->setGrayed(true);
        component.setInterceptsMouseClicks(true, false);
    }

    void clearGrayedClickHandler(juce::Component& component)
    {
        forwarders().erase(&component);
        component.setInterceptsMouseClicks(true, true);
    }
}
