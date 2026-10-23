#include "GUI/Skins/Dev/DevSkinColourElementList.h"

#include "GUI/Skins/SkinColours.h"

namespace TSS::DevSkin
{
    namespace
    {
        void appendPanels(std::vector<const SkinColours::ColourElement*>& out)
        {
            using namespace SkinColours;
            out.push_back(&Panels::kHeaderPanelBackground);
            out.push_back(&Panels::kBodyPanelBackground);
            out.push_back(&Panels::kFooterPanelBackground);
            out.push_back(&Panels::kHeaderPanelBorder);
            out.push_back(&Panels::kFooterPanelBorder);
            out.push_back(&Panels::kFooterMessageErrorBackground);
            out.push_back(&Panels::kDialogBorder);
            out.push_back(&Panels::kDialogTitleBand);
            out.push_back(&Panels::kWindowCloseGlyph);
            out.push_back(&Widgets::DarkPanel::kText);
            out.push_back(&Widgets::DarkPanel::kTextHover);
            out.push_back(&Widgets::FooterMessage::kMessageInfo);
            out.push_back(&Widgets::FooterMessage::kMessageWarning);
            out.push_back(&Widgets::FooterMessage::kMessageError);
            out.push_back(&Widgets::SectionHeader::kText);
            out.push_back(&Widgets::SectionHeader::kLineBlue);
            out.push_back(&Widgets::SectionHeader::kLineOrange);
            out.push_back(&Widgets::ModuleHeader::kText);
            out.push_back(&Widgets::ModuleHeader::kLineBlue);
            out.push_back(&Widgets::ModuleHeader::kLineOrange);
            out.push_back(&Widgets::GroupLabel::kText);
            out.push_back(&Widgets::GroupLabel::kLine);
            out.push_back(&Widgets::Label::kText);
            out.push_back(&Widgets::HorizontalSeparator::kLine);
        }

        void appendControls(std::vector<const SkinColours::ColourElement*>& out)
        {
            using namespace SkinColours;
            out.push_back(&Widgets::Button::kBackgroundOff);
            out.push_back(&Widgets::Button::kBorderOff);
            out.push_back(&Widgets::Button::kTextOff);
            out.push_back(&Widgets::Button::kBackgroundOn);
            out.push_back(&Widgets::Button::kBorderOn);
            out.push_back(&Widgets::Button::kTextOn);
            out.push_back(&Widgets::Button::kBackgroundHover);
            out.push_back(&Widgets::Button::kTextHover);
            out.push_back(&Widgets::Button::kBackgroundClicked);
            out.push_back(&Widgets::Button::kTextClicked);
            out.push_back(&Widgets::Button::kBackgroundDisabled);
            out.push_back(&Widgets::Button::kBorderDisabled);
            out.push_back(&Widgets::Button::kTextDisabled);
            out.push_back(&Widgets::Toggle::kBorder);
            out.push_back(&Widgets::Toggle::kBackgroundOff);
            out.push_back(&Widgets::Toggle::kTextOff);
            out.push_back(&Widgets::Toggle::kBackgroundOn);
            out.push_back(&Widgets::Toggle::kTextOn);
            out.push_back(&Widgets::Toggle::kBackgroundDisabled);
            out.push_back(&Widgets::Toggle::kBorderDisabled);
            out.push_back(&Widgets::Toggle::kTextDisabled);
            out.push_back(&Widgets::Slider::kTrack);
            out.push_back(&Widgets::Slider::kTrackDisabled);
            out.push_back(&Widgets::Slider::kBackgroundDisabled);
            out.push_back(&Widgets::Slider::kValueBar);
            out.push_back(&Widgets::Slider::kValueBarDisabled);
            out.push_back(&Widgets::Slider::kText);
            out.push_back(&Widgets::Slider::kTextDisabled);
            out.push_back(&Widgets::Slider::kFocusBorder);
            out.push_back(&Widgets::SliderButtonLike::kTrack);
            out.push_back(&Widgets::SliderButtonLike::kFocusBorder);
            out.push_back(&Widgets::SliderButtonLike::kValueBar);
            out.push_back(&Widgets::SliderButtonLike::kText);
        }

        void appendMenus(std::vector<const SkinColours::ColourElement*>& out)
        {
            using namespace SkinColours;
            out.push_back(&Widgets::ComboBox::kBackground);
            out.push_back(&Widgets::ComboBox::kBackgroundDisabled);
            out.push_back(&Widgets::ComboBox::kBorder);
            out.push_back(&Widgets::ComboBox::kBorderDisabled);
            out.push_back(&Widgets::ComboBox::kFocusBorder);
            out.push_back(&Widgets::ComboBox::kTriangle);
            out.push_back(&Widgets::ComboBox::kTriangleDisabled);
            out.push_back(&Widgets::ComboBox::kText);
            out.push_back(&Widgets::ComboBox::kTextDisabled);
            out.push_back(&Widgets::ComboBoxButtonLike::kBackground);
            out.push_back(&Widgets::ComboBoxButtonLike::kBackgroundDisabled);
            out.push_back(&Widgets::ComboBoxButtonLike::kBorder);
            out.push_back(&Widgets::ComboBoxButtonLike::kBorderDisabled);
            out.push_back(&Widgets::ComboBoxButtonLike::kText);
            out.push_back(&Widgets::ComboBoxButtonLike::kTextDisabled);
            out.push_back(&Widgets::ComboBoxButtonLike::kTriangle);
            out.push_back(&Widgets::ComboBoxButtonLike::kTriangleDisabled);
            out.push_back(&Widgets::PopupMenu::kBackground);
            out.push_back(&Widgets::PopupMenu::kBorder);
            out.push_back(&Widgets::PopupMenu::kSeparator);
            out.push_back(&Widgets::PopupMenu::kText);
            out.push_back(&Widgets::PopupMenu::kTextDisabled);
            out.push_back(&Widgets::PopupMenu::kBackgroundHover);
            out.push_back(&Widgets::PopupMenu::kTextHover);
            out.push_back(&Widgets::PopupMenu::kScrollbar);
            out.push_back(&Widgets::PopupMenuButtonLike::kBackground);
            out.push_back(&Widgets::PopupMenuButtonLike::kBorder);
            out.push_back(&Widgets::PopupMenuButtonLike::kSeparator);
            out.push_back(&Widgets::PopupMenuButtonLike::kText);
            out.push_back(&Widgets::PopupMenuButtonLike::kTextDisabled);
            out.push_back(&Widgets::PopupMenuButtonLike::kBackgroundHover);
            out.push_back(&Widgets::PopupMenuButtonLike::kTextHover);
            out.push_back(&Widgets::PopupMenuButtonLike::kScrollbar);
        }

        void appendDisplays(std::vector<const SkinColours::ColourElement*>& out)
        {
            using namespace SkinColours;
            out.push_back(&Widgets::NumberBox::kBackground);
            out.push_back(&Widgets::NumberBox::kBackgroundDisabled);
            out.push_back(&Widgets::NumberBox::kText);
            out.push_back(&Widgets::NumberBox::kTextFocus);
            out.push_back(&Widgets::NumberBox::kEditorBackground);
            out.push_back(&Widgets::NumberBox::kEditorSelectionBackground);
            out.push_back(&Widgets::NumberBox::kEditorText);
            out.push_back(&Widgets::EnvelopeDisplay::kBackground);
            out.push_back(&Widgets::EnvelopeDisplay::kBorder);
            out.push_back(&Widgets::EnvelopeDisplay::kEnvelope);
            out.push_back(&Widgets::PatchNameDisplay::kBackground);
            out.push_back(&Widgets::PatchNameDisplay::kBorder);
            out.push_back(&Widgets::PatchNameDisplay::kText);
            out.push_back(&Widgets::PatchNameDisplay::kSecondaryText);
            out.push_back(&Widgets::PatchNameDisplay::kCaret);
            out.push_back(&Widgets::TrackGeneratorDisplay::kBackground);
            out.push_back(&Widgets::TrackGeneratorDisplay::kBorder);
            out.push_back(&Widgets::TrackGeneratorDisplay::kShaper);
            out.push_back(&Widgets::ReadOnlyValueField::kBackground);
            out.push_back(&Widgets::ReadOnlyValueField::kText);
            out.push_back(&Widgets::ReadOnlyValueField::kTextError);
            out.push_back(&Widgets::Led::kBackgroundOff);
            out.push_back(&Widgets::Led::kBorderOff);
            out.push_back(&Widgets::Led::kMaxOn);
        }
    }

    const std::vector<const SkinColours::ColourElement*>& compiledColourElements()
    {
        static const std::vector<const SkinColours::ColourElement*> elements = []
        {
            std::vector<const SkinColours::ColourElement*> out;
            out.reserve(128);
            appendPanels(out);
            appendControls(out);
            appendMenus(out);
            appendDisplays(out);
            return out;
        }();
        return elements;
    }
}
