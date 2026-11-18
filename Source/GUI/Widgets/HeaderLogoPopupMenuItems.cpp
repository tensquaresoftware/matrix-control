#include "HeaderLogoPopupMenu.h"

#include "GUI/Helpers/EditorChromeShortcuts.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace TSS
{
    void HeaderLogoPopupMenu::buildItems()
    {
        items_.clear();
        appendUiScaleColumnItems();
        appendSkinAndActionColumnItems();
    }

    void HeaderLogoPopupMenu::appendUiScaleColumnItems()
    {
        items_.push_back({ ItemKind::SectionHeader,
                           0,
                           PluginDisplayNames::HeaderPanel::kLogoUiScaleSection,
                           TSS::EditorChromeShortcutLabels::uiScaleStep(),
                           0,
                           0 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k50,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k50, {}, 0, 1 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k75,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k75, {}, 0, 2 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k100,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k100,
                           TSS::EditorChromeShortcutLabels::uiScaleReset(), 0, 3 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k125,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k125, {}, 0, 4 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k150,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k150, {}, 0, 5 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k175,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k175, {}, 0, 6 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k200,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k200, {}, 0, 7 });
    }

    void HeaderLogoPopupMenu::appendSkinAndActionColumnItems()
    {
        items_.push_back({ ItemKind::SectionHeader, 0, PluginDisplayNames::HeaderPanel::kLogoSkinSection,
                           TSS::EditorChromeShortcutLabels::skinFamilyCycle(), 1, 0 });
        items_.push_back({ ItemKind::SkinFamily,
                           static_cast<int>(Skin::SkinFamily::Black),
                           PluginDisplayNames::ChoiceLists::SkinFamilies::kBlack,
                           {},
                           1, 1 });
        items_.push_back({ ItemKind::SkinFamily,
                           static_cast<int>(Skin::SkinFamily::Cream),
                           PluginDisplayNames::ChoiceLists::SkinFamilies::kCream,
                           {},
                           1, 2 });
        items_.push_back({ ItemKind::SkinFamily,
                           static_cast<int>(Skin::SkinFamily::Tones),
                           PluginDisplayNames::ChoiceLists::SkinFamilies::kTones,
                           {},
                           1, 3 });
        items_.push_back({ ItemKind::Spacer, 0, {}, {}, 1, 4 });
        // Thin rule: gaps around the stroke match the natural space under About.
        items_.push_back({ ItemKind::HorizontalRule, 0, {}, {}, 1, 5 });

        items_.push_back({ ItemKind::Settings,
                           0,
                           PluginDisplayNames::HeaderPanel::kSettingsButton,
                           TSS::EditorChromeShortcutLabels::openSettings(),
                           1, 6 });
        items_.push_back({ ItemKind::About,
                           0,
                           PluginDisplayNames::HeaderPanel::kAboutButton,
                           {},
                           1, 7 });
    }
}
