#include "PatchInitService.h"

#include "Core/Models/PatchModel.h"

namespace Core
{

    PatchInitService::PatchInitService(PatchModel& patchModel,
                                       InitTemplateLoader& initTemplateLoader,
                                       TemplatesFolderSupplier templatesFolder)
        : patchModel_(patchModel)
        , initTemplateLoader_(initTemplateLoader)
        , templatesFolder_(std::move(templatesFolder))
    {
    }

    InitTemplateLoadResult PatchInitService::initFullPatch()
    {
        return initTemplateLoader_.loadPatch(patchModel_, templatesFolder_());
    }

} // namespace Core
