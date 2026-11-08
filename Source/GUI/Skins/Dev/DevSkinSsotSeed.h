#pragma once

#include "GUI/Skins/Dev/DevSkinTypes.h"

namespace TSS::DevSkin
{
    /** Build a read-only compiled Black or Cream NamedSkin from Skin SSOT tables. */
    NamedSkin makeCompiledSkin(SkinKind kind);

    /** Seed mappings + palette for a Dev skin from a compiled Black/Cream base. */
    void seedFromCompiled(NamedSkin& skin, SkinKind compiledBase);
}
