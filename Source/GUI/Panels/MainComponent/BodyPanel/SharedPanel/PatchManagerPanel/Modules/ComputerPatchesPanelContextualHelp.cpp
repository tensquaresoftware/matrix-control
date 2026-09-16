#include "ComputerPatchesPanel.h"

#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/GroupLabel.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "Shared/Definitions/PluginDisplayNames.h"

void ComputerPatchesPanel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    contextualHelpBinder_->bind(moduleHeader_.get(), Help::kTitle);
    contextualHelpBinder_->bind(browserGroupLabel.get(), Help::kBrowser);
    contextualHelpBinder_->bind(storageGroupLabel.get(), Help::kStorage);
    contextualHelpBinder_->bind(loadPreviousPatchFileButton_.get(), Help::kPrevious);
    contextualHelpBinder_->bind(loadNextPatchFileButton_.get(), Help::kNext);
    contextualHelpBinder_->bind(selectPatchFileComboBox_.get(), Help::kSelectPatch);
    contextualHelpBinder_->bind(openPatchFolderButton_.get(), Help::kOpen);
    contextualHelpBinder_->bind(savePatchFileAsButton_.get(), Help::kSaveAs);
    contextualHelpBinder_->bind(savePatchFileButton_.get(), Help::kSave);
}
