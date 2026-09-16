// Furtive left-footer contextual help for Patch Mutator controls (display-only overlay).

#include "PatchMutatorPanel.h"

#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/HierarchicalComboBox.h"
#include "GUI/Widgets/Toggle.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace MutatorHelp = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::ContextualHelp;

void PatchMutatorPanel::registerContextualHelp()
{
    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));
    contextualHelpBinder_->setHostShowingPredicate([this] { return isShowing(); });

    contextualHelpBinder_->bind(modeComboBox_.get(), MutatorHelp::kMode);
    contextualHelpBinder_->bind(pitchComboBox_.get(), MutatorHelp::kPitch);
    contextualHelpBinder_->bind(historyComboBox_.get(), MutatorHelp::kHistory);
    contextualHelpBinder_->bind(mutateButton_.get(), MutatorHelp::kMutate);
    contextualHelpBinder_->bind(retryButton_.get(), MutatorHelp::kRetry);
    contextualHelpBinder_->bind(historyPreviousButton_.get(), MutatorHelp::kHistoryPrevious);
    contextualHelpBinder_->bind(historyNextButton_.get(), MutatorHelp::kHistoryNext);
    contextualHelpBinder_->bind(compareButton_.get(), MutatorHelp::kCompare);
    contextualHelpBinder_->bind(deleteButton_.get(), MutatorHelp::kDelete);
    contextualHelpBinder_->bind(clearButton_.get(), MutatorHelp::kFlush);
    contextualHelpBinder_->bind(exportButton_.get(), MutatorHelp::kExport);
    contextualHelpBinder_->bind(dco1Toggle_.get(), MutatorHelp::kEnableDco1);
    contextualHelpBinder_->bind(dco2Toggle_.get(), MutatorHelp::kEnableDco2);
    contextualHelpBinder_->bind(vcfVcaToggle_.get(), MutatorHelp::kEnableVcfVca);
    contextualHelpBinder_->bind(fmTrackToggle_.get(), MutatorHelp::kEnableFmTrack);
    contextualHelpBinder_->bind(rampPortamentoToggle_.get(), MutatorHelp::kEnableRampPortamento);
    contextualHelpBinder_->bind(env1Toggle_.get(), MutatorHelp::kEnableEnvelope1);
    contextualHelpBinder_->bind(env2Toggle_.get(), MutatorHelp::kEnableEnvelope2);
    contextualHelpBinder_->bind(env3Toggle_.get(), MutatorHelp::kEnableEnvelope3);
    contextualHelpBinder_->bind(lfo1Toggle_.get(), MutatorHelp::kEnableLfo1);
    contextualHelpBinder_->bind(lfo2Toggle_.get(), MutatorHelp::kEnableLfo2);
    contextualHelpBinder_->bind(enableMatrixModToggle_.get(), MutatorHelp::kEnableMatrixMod);
}
