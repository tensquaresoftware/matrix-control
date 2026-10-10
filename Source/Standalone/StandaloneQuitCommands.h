#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace MatrixStandalone
{
inline bool isWindowsAltF4QuitKey (const juce::KeyPress& key) noexcept
{
    return key == juce::KeyPress (juce::KeyPress::F4Key, juce::ModifierKeys::altModifier, 0);
}

inline bool isStandardApplicationQuitKeypress (const juce::KeyPress& key) noexcept
{
    return key == juce::KeyPress ('q', juce::ModifierKeys::commandModifier, 0);
}

/** Registers JUCEApplication Quit on the manager and listens for its default keypresses on keyFocusRoot.
    ComponentType is templated because juce_IncludeModuleHeaders.h may #define Component as juce::Component. */
template <typename ComponentType>
void bindStandaloneQuitCommands (juce::ApplicationCommandManager& commandManager,
                                 juce::ApplicationCommandTarget& target,
                                 ComponentType& keyFocusRoot)
{
    commandManager.registerAllCommandsForTarget (&target);
    commandManager.setFirstCommandTarget (&target);
    keyFocusRoot.addKeyListener (commandManager.getKeyMappings());
}

inline bool quitCommandHasStandardKeypress (const juce::ApplicationCommandManager& commandManager)
{
    if (commandManager.getKeyMappings() == nullptr)
        return false;

    const auto keys = commandManager.getKeyMappings()->getKeyPressesAssignedToCommand (
        juce::StandardApplicationCommandIDs::quit);

    for (const auto& key : keys)
        if (isStandardApplicationQuitKeypress (key))
            return true;

    return false;
}
} // namespace MatrixStandalone
