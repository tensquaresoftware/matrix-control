#include "MatrixModulationPanel.h"

#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ModulationBusCell.h"
#include "GUI/Widgets/Slider.h"
#include "Shared/Definitions/PluginDisplayNames.h"

void MatrixModulationPanel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::MatrixModulationSection::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    contextualHelpBinder_->bind(initButton_.get(), Help::kSectionInit);
    contextualHelpBinder_->bind(copyButton_.get(), Help::kSectionCopy);
    contextualHelpBinder_->bind(pasteButton_.get(), Help::kSectionPaste);

    for (auto& bus : modulationBuses_)
    {
        if (bus == nullptr)
            continue;

        contextualHelpBinder_->bind(bus->getBusNumberLabel(), Help::kBusHandle);
        contextualHelpBinder_->bind(bus->getSourceComboBox(), Help::kSource);
        contextualHelpBinder_->bind(bus->getAmountSlider(), Help::kAmount);
        contextualHelpBinder_->bind(bus->getDestinationComboBox(), Help::kDestination);
        contextualHelpBinder_->bind(bus->getInitButton(), Help::kBusInit);
    }
}
