#include "GUI/Skins/Dev/DevSkinColourElementList.h"

#include "GUI/Skins/SkinColours.h"

namespace TSS::DevSkin
{
    namespace
    {
        void appendPanels(std::vector<const SkinColours::ColourElement*>& out)
        {
            using namespace SkinColours;
            out.push_back(&Panels::kWindowSurfaceBackground);
            out.push_back(&Panels::kHeaderPanelBackground);
            out.push_back(&Panels::kBodyPanelBackground);
            out.push_back(&Panels::kFooterPanelBackground);
            out.push_back(&Panels::kHeaderPanelBorder);
            out.push_back(&Panels::kFooterPanelBorder);
            out.push_back(&Panels::kFooterMessageErrorBackground);
            out.push_back(&Panels::kDialogBorder);
            out.push_back(&Panels::kDialogTitleBand);
            out.push_back(&Panels::kDialogGlyph);
            out.push_back(&Panels::kDialogText);
            out.push_back(&Panels::kDialogTextOn);
            out.push_back(&Widgets::ChromeText::kText);
            out.push_back(&Widgets::ChromeText::kTextHover);
            out.push_back(&Widgets::ChromeText::kTextEmphasis);
            out.push_back(&Widgets::FooterMessage::kMessageInfo);
            out.push_back(&Widgets::FooterMessage::kMessageWarning);
            out.push_back(&Widgets::FooterMessage::kMessageError);
            out.push_back(&Widgets::FooterMessage::kGlyphHover);
            out.push_back(&Widgets::SectionHeader::kText);
            out.push_back(&Widgets::SectionHeader::kLineBlue);
            out.push_back(&Widgets::SectionHeader::kLineOrange);
            out.push_back(&Widgets::ModuleHeader::kText);
            out.push_back(&Widgets::ModuleHeader::kLineBlue);
            out.push_back(&Widgets::ModuleHeader::kLineOrange);
            out.push_back(&Widgets::ModuleHeader::kTextHover);
            out.push_back(&Widgets::ModuleHeader::kBackgroundEmphasis);
            out.push_back(&Widgets::ModuleHeader::kTextEmphasis);
            out.push_back(&Widgets::GroupLabel::kText);
            out.push_back(&Widgets::GroupLabel::kLine);
            out.push_back(&Widgets::Label::kText);
            out.push_back(&Widgets::Label::kBackgroundClicked);
            out.push_back(&Widgets::Label::kBackgroundHover);
            out.push_back(&Widgets::HorizontalSeparator::kLine);
        }

        void appendButtons(std::vector<const SkinColours::ColourElement*>& out)
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
            out.push_back(&Widgets::Button::kTextEmphasis);
            out.push_back(&Widgets::Button::kBackgroundError);
            out.push_back(&Widgets::Button::kTextError);
            out.push_back(&Widgets::Button::kSecondaryTextHover);
            out.push_back(&Widgets::Toggle::kBorder);
            out.push_back(&Widgets::Toggle::kBackgroundOff);
            out.push_back(&Widgets::Toggle::kTextOff);
            out.push_back(&Widgets::Toggle::kBackgroundOn);
            out.push_back(&Widgets::Toggle::kTextOn);
            out.push_back(&Widgets::Toggle::kBackgroundDisabled);
            out.push_back(&Widgets::Toggle::kBorderDisabled);
            out.push_back(&Widgets::Toggle::kTextDisabled);
            out.push_back(&Widgets::ProgressBar::kTrack);
            out.push_back(&Widgets::ProgressBar::kTrackDisabled);
            out.push_back(&Widgets::ProgressBar::kValueBar);
            out.push_back(&Widgets::ProgressBar::kValueBarDisabled);
            out.push_back(&Widgets::ProgressBar::kText);
            out.push_back(&Widgets::ProgressBar::kTextDisabled);
            out.push_back(&Widgets::ProgressBar::kSecondaryText);
            out.push_back(&Widgets::ProgressBar::kSecondaryTextDisabled);
        }

        void appendSliders(std::vector<const SkinColours::ColourElement*>& out)
        {
            using namespace SkinColours;
            out.push_back(&Widgets::Slider::kTrack);
            out.push_back(&Widgets::Slider::kTrackDisabled);
            out.push_back(&Widgets::Slider::kBackgroundDisabled);
            out.push_back(&Widgets::Slider::kValueBar);
            out.push_back(&Widgets::Slider::kValueBarDisabled);
            out.push_back(&Widgets::Slider::kText);
            out.push_back(&Widgets::Slider::kTextDisabled);
            out.push_back(&Widgets::Slider::kFocusBorder);
            out.push_back(&Widgets::Slider::kEditorText);
            out.push_back(&Widgets::Slider::kCaret);
            out.push_back(&Widgets::SliderK2::kBackgroundDisabled);
            out.push_back(&Widgets::SliderK2::kFocusBorder);
            out.push_back(&Widgets::SliderK2::kText);
            out.push_back(&Widgets::SliderK2::kTextDisabled);
            out.push_back(&Widgets::SliderK2::kTrack);
            out.push_back(&Widgets::SliderK2::kTrackDisabled);
            out.push_back(&Widgets::SliderK2::kValueBar);
            out.push_back(&Widgets::SliderK2::kValueBarDisabled);
            out.push_back(&Widgets::SliderK2::kEditorText);
            out.push_back(&Widgets::SliderK2::kCaret);
            out.push_back(&Widgets::SliderK3::kBackgroundDisabled);
            out.push_back(&Widgets::SliderK3::kFocusBorder);
            out.push_back(&Widgets::SliderK3::kText);
            out.push_back(&Widgets::SliderK3::kTextDisabled);
            out.push_back(&Widgets::SliderK3::kTrack);
            out.push_back(&Widgets::SliderK3::kTrackDisabled);
            out.push_back(&Widgets::SliderK3::kValueBar);
            out.push_back(&Widgets::SliderK3::kValueBarDisabled);
            out.push_back(&Widgets::SliderK3::kEditorText);
            out.push_back(&Widgets::SliderK3::kCaret);
            out.push_back(&Widgets::SliderK4::kTrack);
            out.push_back(&Widgets::SliderK4::kFocusBorder);
            out.push_back(&Widgets::SliderK4::kValueBar);
            out.push_back(&Widgets::SliderK4::kText);
            out.push_back(&Widgets::SliderK4::kBackgroundDisabled);
            out.push_back(&Widgets::SliderK4::kTextDisabled);
            out.push_back(&Widgets::SliderK4::kTrackDisabled);
            out.push_back(&Widgets::SliderK4::kValueBarDisabled);
            out.push_back(&Widgets::SliderK4::kEditorText);
            out.push_back(&Widgets::SliderK4::kCaret);
        }

        void appendComboBoxes(std::vector<const SkinColours::ColourElement*>& out)
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
            out.push_back(&Widgets::ComboBox::kTextFocus);
            out.push_back(&Widgets::ComboBoxK2::kBackground);
            out.push_back(&Widgets::ComboBoxK2::kBackgroundDisabled);
            out.push_back(&Widgets::ComboBoxK2::kBorder);
            out.push_back(&Widgets::ComboBoxK2::kBorderDisabled);
            out.push_back(&Widgets::ComboBoxK2::kFocusBorder);
            out.push_back(&Widgets::ComboBoxK2::kText);
            out.push_back(&Widgets::ComboBoxK2::kTextDisabled);
            out.push_back(&Widgets::ComboBoxK2::kTextFocus);
            out.push_back(&Widgets::ComboBoxK2::kTriangle);
            out.push_back(&Widgets::ComboBoxK2::kTriangleDisabled);
            out.push_back(&Widgets::ComboBoxK3::kBackground);
            out.push_back(&Widgets::ComboBoxK3::kBackgroundDisabled);
            out.push_back(&Widgets::ComboBoxK3::kBorder);
            out.push_back(&Widgets::ComboBoxK3::kBorderDisabled);
            out.push_back(&Widgets::ComboBoxK3::kFocusBorder);
            out.push_back(&Widgets::ComboBoxK3::kText);
            out.push_back(&Widgets::ComboBoxK3::kTextDisabled);
            out.push_back(&Widgets::ComboBoxK3::kTextFocus);
            out.push_back(&Widgets::ComboBoxK3::kTriangle);
            out.push_back(&Widgets::ComboBoxK3::kTriangleDisabled);
        }

        void appendPopupMenus(std::vector<const SkinColours::ColourElement*>& out)
        {
            using namespace SkinColours;
            out.push_back(&Widgets::PopupMenu::kBackground);
            out.push_back(&Widgets::PopupMenu::kBorder);
            out.push_back(&Widgets::PopupMenu::kSeparator);
            out.push_back(&Widgets::PopupMenu::kText);
            out.push_back(&Widgets::PopupMenu::kTextDisabled);
            out.push_back(&Widgets::PopupMenu::kBackgroundHover);
            out.push_back(&Widgets::PopupMenu::kTextHover);
            out.push_back(&Widgets::PopupMenu::kScrollbar);
            out.push_back(&Widgets::PopupMenuK2::kBackground);
            out.push_back(&Widgets::PopupMenuK2::kBackgroundHover);
            out.push_back(&Widgets::PopupMenuK2::kBorder);
            out.push_back(&Widgets::PopupMenuK2::kScrollbar);
            out.push_back(&Widgets::PopupMenuK2::kSeparator);
            out.push_back(&Widgets::PopupMenuK2::kText);
            out.push_back(&Widgets::PopupMenuK2::kTextDisabled);
            out.push_back(&Widgets::PopupMenuK2::kTextHover);
            out.push_back(&Widgets::PopupMenuK3::kBackground);
            out.push_back(&Widgets::PopupMenuK3::kBackgroundHover);
            out.push_back(&Widgets::PopupMenuK3::kBorder);
            out.push_back(&Widgets::PopupMenuK3::kScrollbar);
            out.push_back(&Widgets::PopupMenuK3::kSeparator);
            out.push_back(&Widgets::PopupMenuK3::kText);
            out.push_back(&Widgets::PopupMenuK3::kTextDisabled);
            out.push_back(&Widgets::PopupMenuK3::kTextHover);
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
            out.push_back(&Widgets::NumberBox::kCaret);
            out.push_back(&Widgets::NumberBox::kBorder);
            out.push_back(&Widgets::NumberBox::kBorderDisabled);
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
            out.push_back(&Widgets::LedK2::kBackgroundOff);
            out.push_back(&Widgets::LedK2::kMaxOn);
            out.push_back(&Widgets::LedK2::kBorderOff);
            out.push_back(&Widgets::PeakIndicator::kBackgroundOff);
            out.push_back(&Widgets::PeakIndicator::kBorderOff);
            out.push_back(&Widgets::PeakIndicator::kMaxOn);
            out.push_back(&Widgets::PeakIndicatorK2::kBackgroundOff);
            out.push_back(&Widgets::PeakIndicatorK2::kMaxOn);
            out.push_back(&Widgets::PeakIndicatorK2::kBorderOff);
        }
    }

    const std::vector<const SkinColours::ColourElement*>& compiledColourElements()
    {
        static const std::vector<const SkinColours::ColourElement*> elements = []
        {
            std::vector<const SkinColours::ColourElement*> out;
            out.reserve(256);
            appendPanels(out);
            appendButtons(out);
            appendSliders(out);
            appendComboBoxes(out);
            appendPopupMenus(out);
            appendDisplays(out);
            return out;
        }();
        return elements;
    }
}
