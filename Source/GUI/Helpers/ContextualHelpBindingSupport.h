#pragma once

#include "GUI/Helpers/ContextualHelpBinder.h"
#include "GUI/MainComponent.h"
#include "GUI/Panels/MainComponent/FooterPanel/FooterPanel.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "GUI/Widgets/ModuleHeader.h"

namespace TSS
{
    inline ContextualHelpBinder::FooterResolver makeMainComponentFooterResolver(juce::Component& host)
    {
        return [&host]() -> FooterPanel*
        {
            if (auto* main = host.findParentComponentOfClass<MainComponent>())
                return &main->getFooterPanel();

            return nullptr;
        };
    }

    inline void bindModuleHeaderInitOnly(ContextualHelpBinder& binder,
                                         ModuleHeader* header,
                                         const char* initHelp)
    {
        if (header == nullptr)
            return;

        binder.bind(header->getInitButton(), initHelp);
    }

    struct ModuleHeaderIcpHelp
    {
        const char* init = nullptr;
        const char* copy = nullptr;
        const char* paste = nullptr;
    };

    inline void bindModuleHeaderInitCopyPaste(ContextualHelpBinder& binder,
                                              ModuleHeader* header,
                                              const ModuleHeaderIcpHelp& helps)
    {
        if (header == nullptr)
            return;

        binder.bind(header->getInitButton(), helps.init);
        binder.bind(header->getCopyButton(), helps.copy);
        binder.bind(header->getPasteButton(), helps.paste);
    }

    inline void bindParameterCellHelps(ContextualHelpBinder& binder,
                                       BaseModulePanel& panel,
                                       const char* const* helps,
                                       size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            if (helps[i] == nullptr)
                continue;

            if (auto* cell = panel.getParameterCellAt(i))
                binder.bind(cell, helps[i]);
        }
    }
}
