#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinTypes.h"

/** Mapped colour filter PopupMenu custom rows (Palette swatch + name). */
namespace SkinEditorMappingsFilterMenu
{
    void appendMappedColourItems(juce::PopupMenu& menu,
                                 const TSS::DevSkin::NamedSkin& skin,
                                 const juce::StringArray& values,
                                 const juce::String& active);
}
