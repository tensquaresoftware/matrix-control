#pragma once

#include "GUI/Layout/PanelDimensions.h"

class DimensionFactory
{
public:
    static GuiLayoutDimensions buildGuiLayoutDimensions();
    static PluginEditorDimensions buildPluginEditorDimensions();
};
