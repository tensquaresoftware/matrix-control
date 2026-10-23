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
            { SkinObjectType::kHeaderPanel, "HeaderPanel" },
            { SkinObjectType::kBodyPanel, "BodyPanel" },
            { SkinObjectType::kFooterPanel, "FooterPanel" },
            { SkinObjectType::kDarkPanel, "DarkPanel" },
            { SkinObjectType::kFooterMessage, "FooterMessage" },
            { SkinObjectType::kSectionHeader, "SectionHeader" },
            { SkinObjectType::kModuleHeader, "ModuleHeader" },
            { SkinObjectType::kGroupLabel, "GroupLabel" },
            { SkinObjectType::kLabel, "Label" },
            { SkinObjectType::kHorizontalSeparator, "HorizontalSeparator" },
            { SkinObjectType::kButton, "Button" },
            { SkinObjectType::kToggle, "Toggle" },
            { SkinObjectType::kSlider, "Slider" },
            { SkinObjectType::kSliderButtonLike, "SliderButtonLike" },
            { SkinObjectType::kComboBox, "ComboBox" },
            { SkinObjectType::kComboBoxButtonLike, "ComboBoxButtonLike" },
            { SkinObjectType::kPopupMenu, "PopupMenu" },
            { SkinObjectType::kPopupMenuButtonLike, "PopupMenuButtonLike" },
            { SkinObjectType::kNumberBox, "NumberBox" },
            { SkinObjectType::kEnvelopeDisplay, "EnvelopeDisplay" },
            { SkinObjectType::kTrackGeneratorDisplay, "TrackGeneratorDisplay" },
            { SkinObjectType::kPatchNameDisplay, "PatchNameDisplay" },
            { SkinObjectType::kDialog, "Dialog" },
            { SkinObjectType::kWindowClose, "WindowClose" },
            { SkinObjectType::kReadOnlyValueField, "ReadOnlyValueField" },
            { SkinObjectType::kLed, "Led" },
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
            { SkinColourState::kInfo, "Info" },
            { SkinColourState::kWarning, "Warning" },
            { SkinColourState::kError, "Error" },
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

    bool parseObjectType(const juce::String& text, SkinObjectType& out)
    {
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

    juce::String keyLabel(const SkinColourKey& key)
    {
        return toString(key.object) + " / " + toString(key.property) + " / " + toString(key.state);
    }
}
