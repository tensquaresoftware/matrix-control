#include "TypographyStyles.h"

namespace TSS
{
    namespace
    {
        namespace TypographyHeights
        {
            constexpr float kDefault_ = 14.0f;
            constexpr float kSmall_ = 12.0f;
            constexpr float kModuleHeader_ = 16.0f;
            constexpr float kSectionHeader_ = 20.0f;
            constexpr float kPatchName_ = 28.0f;
        }
    }

    TypographyStyle getTypographyStyle(TypographyStyleId styleId)
    {
        switch (styleId)
        {
            case TypographyStyleId::kSmall:
                return TypographyStyle { TypographyHeights::kSmall_, false };

            case TypographyStyleId::kModuleHeader:
                return TypographyStyle { TypographyHeights::kModuleHeader_, false };

            case TypographyStyleId::kSectionHeader:
                return TypographyStyle { TypographyHeights::kSectionHeader_, false };

            case TypographyStyleId::kPatchName:
                return TypographyStyle { TypographyHeights::kPatchName_, false };

            case TypographyStyleId::kDefault:
            default:
                return TypographyStyle { TypographyHeights::kDefault_, false };
        }
    }
}
