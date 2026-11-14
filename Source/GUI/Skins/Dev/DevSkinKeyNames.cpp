#include "GUI/Skins/Dev/DevSkinKeyNames.h"

namespace TSS::DevSkin
{
    namespace
    {
        template <typename Enum>
        struct Named
        {
            Enum value;
            const char* name;
        };

        constexpr Named<SkinObjectType> kObjects[] = {
            { SkinObjectType::kWindowSurface, "WindowSurface" },
            { SkinObjectType::kHeaderPanel, "HeaderPanel" },
            { SkinObjectType::kBodyPanel, "BodyPanel" },
            { SkinObjectType::kFooterPanel, "FooterPanel" },
            { SkinObjectType::kChromeText, "ChromeText" },
            { SkinObjectType::kFooterMessage, "FooterMessage" },
            { SkinObjectType::kSectionHeader, "SectionHeader" },
            { SkinObjectType::kModuleHeader, "ModuleHeader" },
            { SkinObjectType::kGroupLabel, "GroupLabel" },
            { SkinObjectType::kLabel, "Label" },
            { SkinObjectType::kHorizontalSeparator, "HorizontalSeparator" },
            { SkinObjectType::kButton, "Button" },
            { SkinObjectType::kToggle, "Toggle" },
            { SkinObjectType::kSlider, "Slider" },
            { SkinObjectType::kProgressBar, "ProgressBar" },
            { SkinObjectType::kComboBox, "ComboBox" },
            { SkinObjectType::kPopupMenu, "PopupMenu" },
            { SkinObjectType::kNumberBox, "NumberBox" },
            { SkinObjectType::kEnvelopeDisplay, "EnvelopeDisplay" },
            { SkinObjectType::kTrackGeneratorDisplay, "TrackGeneratorDisplay" },
            { SkinObjectType::kPatchNameDisplay, "PatchNameDisplay" },
            { SkinObjectType::kDialog, "Dialog" },
            { SkinObjectType::kReadOnlyValueField, "ReadOnlyValueField" },
            { SkinObjectType::kLed, "Led" },
            { SkinObjectType::kPeakIndicator, "PeakIndicator" },
        };

        constexpr Named<SkinColourProperty> kProperties[] = {
            { SkinColourProperty::kBackground, "Background" },
            { SkinColourProperty::kBorder, "Border" },
            { SkinColourProperty::kText, "Text" },
            { SkinColourProperty::kSecondaryText, "SecondaryText" },
            { SkinColourProperty::kLine, "Line" },
            { SkinColourProperty::kLineBlue, "LineBlue" },
            { SkinColourProperty::kLineOrange, "LineOrange" },
            { SkinColourProperty::kTrack, "Track" },
            { SkinColourProperty::kValueBar, "ValueBar" },
            { SkinColourProperty::kFocusBorder, "FocusBorder" },
            { SkinColourProperty::kTriangle, "Triangle" },
            { SkinColourProperty::kSeparator, "Separator" },
            { SkinColourProperty::kScrollbar, "Scrollbar" },
            { SkinColourProperty::kEnvelope, "Envelope" },
            { SkinColourProperty::kShaper, "Shaper" },
            { SkinColourProperty::kEditorBackground, "EditorBackground" },
            { SkinColourProperty::kEditorSelectionBackground, "EditorSelectionBackground" },
            { SkinColourProperty::kEditorText, "EditorText" },
            { SkinColourProperty::kTitleBand, "TitleBand" },
            { SkinColourProperty::kGlyph, "Glyph" },
            { SkinColourProperty::kCaret, "Caret" },
        };

        constexpr Named<SkinColourState> kStates[] = {
            { SkinColourState::kDefault, "Default" },
            { SkinColourState::kOff, "Off" },
            { SkinColourState::kOn, "On" },
            { SkinColourState::kHover, "Hover" },
            { SkinColourState::kClicked, "Clicked" },
            { SkinColourState::kDisabled, "Disabled" },
            { SkinColourState::kFocus, "Focus" },
            { SkinColourState::kEmphasis, "Emphasis" },
            { SkinColourState::kInfo, "Info" },
            { SkinColourState::kWarning, "Warning" },
            { SkinColourState::kError, "Error" },
        };

        constexpr Named<ColourSet> kColourSets[] = {
            { ColourSet::k1, "cs1" },
            { ColourSet::k2, "cs2" },
            { ColourSet::k3, "cs3" },
            { ColourSet::k4, "cs4" },
        };

        constexpr Named<SkinZone> kZones[] = {
            { SkinZone::kPatch, "Patch" },
            { SkinZone::kMaster, "Master" },
            { SkinZone::kHeader, "Header" },
            { SkinZone::kModales, "Modales" },
        };

        template <typename Enum, size_t N>
        juce::String nameOf(const Named<Enum> (&table)[N], Enum value)
        {
            for (const auto& entry : table)
                if (entry.value == value)
                    return entry.name;
            return {};
        }

        template <typename Enum, size_t N>
        bool parseOf(const Named<Enum> (&table)[N], const juce::String& text, Enum& out)
        {
            for (const auto& entry : table)
            {
                if (text.equalsIgnoreCase(entry.name))
                {
                    out = entry.value;
                    return true;
                }
            }
            return false;
        }
    }

    juce::String toString(SkinObjectType object)
    {
        return nameOf(kObjects, object);
    }

    juce::String toString(SkinColourProperty property)
    {
        return nameOf(kProperties, property);
    }

    juce::String toString(SkinColourState state)
    {
        return nameOf(kStates, state);
    }

    juce::String toString(ColourSet colourSet)
    {
        return nameOf(kColourSets, colourSet);
    }

    juce::String toString(SkinZone zone)
    {
        return nameOf(kZones, zone);
    }

    bool parseObjectType(const juce::String& text, SkinObjectType& out)
    {
        // Legacy Dev JSON keys from pre-unify ButtonLike object types.
        if (text.equalsIgnoreCase("SliderButtonLike"))
        {
            out = SkinObjectType::kSlider;
            return true;
        }
        if (text.equalsIgnoreCase("ComboBoxButtonLike"))
        {
            out = SkinObjectType::kComboBox;
            return true;
        }
        if (text.equalsIgnoreCase("PopupMenuButtonLike"))
        {
            out = SkinObjectType::kPopupMenu;
            return true;
        }

        // Legacy chrome object names (pre skin-chrome decorrelation).
        if (text.equalsIgnoreCase("DarkPanel"))
        {
            out = SkinObjectType::kChromeText;
            return true;
        }
        if (text.equalsIgnoreCase("WindowClose"))
        {
            out = SkinObjectType::kDialog;
            return true;
        }

        return parseOf(kObjects, text, out);
    }

    bool parseProperty(const juce::String& text, SkinColourProperty& out)
    {
        return parseOf(kProperties, text, out);
    }

    bool parseState(const juce::String& text, SkinColourState& out)
    {
        return parseOf(kStates, text, out);
    }

    bool parseColourSet(const juce::String& text, ColourSet& out)
    {
        return parseOf(kColourSets, text, out);
    }

    bool parseSkinZone(const juce::String& text, SkinZone& out)
    {
        return parseOf(kZones, text, out);
    }

    juce::String keyLabel(const SkinColourKey& key)
    {
        return toString(key.object) + " / " + toString(key.property) + " / "
             + toString(key.state) + " / " + toString(key.colourSet);
    }

    juce::StringArray allObjectTypeNames()
    {
        juce::StringArray names;
        for (const auto& entry : kObjects)
            names.add(entry.name);
        return names;
    }

    juce::StringArray allColourStateNames()
    {
        juce::StringArray names;
        for (const auto& entry : kStates)
            names.add(entry.name);
        return names;
    }
}
