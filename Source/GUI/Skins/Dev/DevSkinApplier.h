#pragma once

#include "GUI/Skins/Dev/DevSkinTypes.h"
#include "GUI/Skins/Skin.h"

namespace TSS::DevSkin
{
    /** Apply a push payload onto a mutable Skin (Dev Standalone path only). */
    ApplyResult applyPushToSkin(const PushPayload& payload, Skin& targetSkin);

    Skin::ColourVariant compiledBaseVariant(const PushPayload& payload);
}
