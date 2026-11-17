#include "HeaderLogoPopupMenu.h"

#include "Logo.h"
#include "PopupMenuOutsideDismissWatcher.h"
#include "PopupMenuPositioner.h"
#include "PopupMenuRenderer.h"
#include "ComboBox.h"

#include "GUI/Helpers/ContextualHelpBinder.h"
#include "GUI/Helpers/EditorChromeShortcuts.h"
#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/SkinCatalog.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace TSS
{
    namespace
    {
        int shortcutDigitForSkinId(int skinItemId) noexcept
        {
            using namespace PluginIDs::Settings::SkinVariants;
            switch (skinItemId)
            {
                case kBlack: return 1;
                case kBlackChroma: return 2;
                case kBlackMinimal: return 3;
                case kCream: return 4;
                case kCreamChroma: return 5;
                case kCreamMinimal: return 6;
                default: return 0;
            }
        }
    }

    HeaderLogoPopupMenu::HeaderLogoPopupMenu(ISkin& skin, Config config)
        : uiScale_(config.uiScale)
        , currentSkinItemId_(config.currentSkinItemId)
        , currentUiScaleId_(config.currentUiScaleId)
        , onSkinSelected_(std::move(config.onSkinSelected))
        , onUiScaleSelected_(std::move(config.onUiScaleSelected))
        , onSettingsRequested_(std::move(config.onSettingsRequested))
        , onAboutRequested_(std::move(config.onAboutRequested))
        , contextualHelpBinder_(config.contextualHelpBinder)
        , look_(popupMenuLookFromSkin(skin,
                                       zoneColourSet(SkinObjectType::kPopupMenu, kMonoMenuExceptionZone)))
        , renderer_(std::make_unique<PopupMenuRenderer>(uiScale_))
        , cachedFont_(look_.font.withHeight(look_.font.getHeight() * uiScale_))
    {
        renderer_->setLook(look_);
        buildItems();
        setWantsKeyboardFocus(true);
        setAlwaysOnTop(true);
        setInterceptsMouseClicks(true, true);
        // Main panel + flyout paint their own chrome; expanded host stays transparent in the gap.
        setOpaque(false);

        outsideDismissWatcher_ = std::make_unique<PopupMenuOutsideDismissWatcher>(*this, [this] { closePopup(); });
    }

    HeaderLogoPopupMenu::~HeaderLogoPopupMenu() = default;

    bool HeaderLogoPopupMenu::isSelectableItem(int flatIndex) const
    {
        if (! juce::isPositiveAndBelow(flatIndex, static_cast<int>(items_.size())))
            return false;

        const auto& item = items_[static_cast<size_t>(flatIndex)];
        if (! item.enabled)
            return false;

        return item.kind == ItemKind::SkinFamily || item.kind == ItemKind::UiScale
            || item.kind == ItemKind::Settings || item.kind == ItemKind::About;
    }

    bool HeaderLogoPopupMenu::familyContainsCurrentSkin(Skin::SkinFamily family) const
    {
        return SkinCatalog::familyForItemId(currentSkinItemId_) == family;
    }

    bool HeaderLogoPopupMenu::isCurrentSelection(int flatIndex) const
    {
        if (! isSelectableItem(flatIndex))
            return false;

        const auto& item = items_[static_cast<size_t>(flatIndex)];
        if (item.kind == ItemKind::Settings || item.kind == ItemKind::About)
            return false;

        if (item.kind == ItemKind::SkinFamily)
            return familyContainsCurrentSkin(static_cast<Skin::SkinFamily>(item.valueId));

        return item.valueId == currentUiScaleId_;
    }

    void HeaderLogoPopupMenu::updateHighlightedItem(int flatIndex)
    {
        if (flatIndex >= 0 && ! isSelectableItem(flatIndex))
            flatIndex = -1;

        if (highlightedFlatIndex_ == flatIndex)
            return;

        highlightedFlatIndex_ = flatIndex;
        repaint(0, 0, mainPopupWidth_, mainPopupHeight_);

        if (flatIndex >= 0)
        {
            const auto& item = items_[static_cast<size_t>(flatIndex)];
            if (item.kind == ItemKind::SkinFamily)
                openFamilyFlyout(static_cast<Skin::SkinFamily>(item.valueId));
            else
                closeFamilyFlyout();
        }
    }

    const char* HeaderLogoPopupMenu::contextualHelpTextForItem(int flatIndex) const
    {
        if (! juce::isPositiveAndBelow(flatIndex, static_cast<int>(items_.size())))
            return nullptr;

        namespace Help = PluginDisplayNames::HeaderPanel::ContextualHelp;
        switch (items_[static_cast<size_t>(flatIndex)].kind)
        {
            case ItemKind::Settings:         return Help::kSettings;
            case ItemKind::About:            return Help::kAbout;
            case ItemKind::SkinFamily:       return Help::kSkin;
            case ItemKind::UiScale:          return Help::kUiScale;
            case ItemKind::SectionHeader:
                if (items_[static_cast<size_t>(flatIndex)].text
                    == PluginDisplayNames::HeaderPanel::kLogoSkinSection)
                    return Help::kSkin;
                if (items_[static_cast<size_t>(flatIndex)].text
                    == PluginDisplayNames::HeaderPanel::kLogoUiScaleSection)
                    return Help::kUiScale;
                return nullptr;
            case ItemKind::Spacer:
            case ItemKind::HorizontalRule:
                return nullptr;
        }

        return nullptr;
    }

    void HeaderLogoPopupMenu::updateContextualHelpForItem(int flatIndex)
    {
        if (contextualHelpBinder_ == nullptr)
            return;

        if (const auto* helpText = contextualHelpTextForItem(flatIndex))
            contextualHelpBinder_->showHelpText(helpText);
        else
            contextualHelpBinder_->scheduleClear();
    }

    std::vector<HeaderLogoPopupMenu::FamilyMember> HeaderLogoPopupMenu::membersForFamily(
        Skin::SkinFamily family) const
    {
        const int* ids = nullptr;
        int count = 0;
        switch (family)
        {
            case Skin::SkinFamily::Cream:
                ids = SkinCatalog::kCreamFamilyIds;
                count = static_cast<int>(sizeof(SkinCatalog::kCreamFamilyIds)
                                        / sizeof(SkinCatalog::kCreamFamilyIds[0]));
                break;
            case Skin::SkinFamily::Tones:
                ids = SkinCatalog::kTonesFamilyIds;
                count = static_cast<int>(sizeof(SkinCatalog::kTonesFamilyIds)
                                        / sizeof(SkinCatalog::kTonesFamilyIds[0]));
                break;
            case Skin::SkinFamily::Black:
            default:
                ids = SkinCatalog::kBlackFamilyIds;
                count = static_cast<int>(sizeof(SkinCatalog::kBlackFamilyIds)
                                        / sizeof(SkinCatalog::kBlackFamilyIds[0]));
                break;
        }

        std::vector<FamilyMember> members;
        members.reserve(static_cast<size_t>(count));
        for (int i = 0; i < count; ++i)
        {
            const int skinId = ids[i];
            FamilyMember member;
            member.skinItemId = skinId;
            member.text = SkinCatalog::displayNameForItemId(skinId);
            if (const int digit = shortcutDigitForSkinId(skinId); digit > 0)
                member.shortcut = EditorChromeShortcutLabels::skinVariant(digit);
            members.push_back(std::move(member));
        }
        return members;
    }

    void HeaderLogoPopupMenu::openFamilyFlyout(Skin::SkinFamily family)
    {
        if (familyFlyout_ != nullptr && familyFlyout_->getFamily() == family)
            return;

        familyFlyout_.reset();
        familyFlyout_ = std::make_unique<FamilyFlyout>(*this, family, membersForFamily(family));
        addAndMakeVisible(*familyFlyout_);
        layoutFamilyFlyout();
    }

    void HeaderLogoPopupMenu::closeFamilyFlyout()
    {
        if (familyFlyout_ == nullptr)
            return;

        familyFlyout_.reset();
        setSize(mainPopupWidth_, mainPopupHeight_);
    }

    void HeaderLogoPopupMenu::layoutFamilyFlyout()
    {
        if (familyFlyout_ == nullptr)
            return;

        const int flyoutW = familyFlyout_->preferredWidth();
        const int flyoutH = familyFlyout_->preferredHeight();

        int familyRowY = 0;
        for (int i = 0; i < static_cast<int>(items_.size()); ++i)
        {
            const auto& item = items_[static_cast<size_t>(i)];
            if (item.kind == ItemKind::SkinFamily
                && static_cast<Skin::SkinFamily>(item.valueId) == familyFlyout_->getFamily())
            {
                familyRowY = juce::roundToInt(getItemBounds(i).getY());
                break;
            }
        }

        // Overlap the shared vertical border so main + flyout do not double the stroke.
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const int borderOverlap = juce::jmax(1, juce::roundToInt(
            ScaledDrawing::snappedStrokeThicknessFromDesign(
                kBorderThicknessDesign_,
                uiScale_,
                systemDisplayScale,
                ScaledDrawing::StrokeSnapPolicy::kRound)));
        const int flyoutX = juce::jmax(0, mainPopupWidth_ - borderOverlap);
        setSize(flyoutX + flyoutW, juce::jmax(mainPopupHeight_, flyoutH));

        const int maxY = juce::jmax(0, getHeight() - flyoutH);
        familyFlyout_->setBounds(flyoutX, juce::jlimit(0, maxY, familyRowY),
                                 flyoutW, flyoutH);

        if (auto* parent = getParentComponent())
        {
            const auto parentBounds = parent->getLocalBounds();
            auto bounds = getBounds();
            const int maxTop = juce::jmax(parentBounds.getY(),
                                          parentBounds.getBottom() - bounds.getHeight());
            bounds.setY(juce::jlimit(parentBounds.getY(), maxTop, bounds.getY()));
            setBounds(bounds);
        }
    }

    void HeaderLogoPopupMenu::selectSkinItemId(int skinItemId)
    {
        if (onSkinSelected_)
            onSkinSelected_(skinItemId);
        closePopup();
    }

    void HeaderLogoPopupMenu::selectItem(int flatIndex)
    {
        if (! isSelectableItem(flatIndex))
            return;

        const auto& item = items_[static_cast<size_t>(flatIndex)];
        if (item.kind == ItemKind::SkinFamily)
        {
            openFamilyFlyout(static_cast<Skin::SkinFamily>(item.valueId));
            return;
        }
        if (item.kind == ItemKind::UiScale)
        {
            if (onUiScaleSelected_)
                onUiScaleSelected_(item.valueId);
        }
        else if (item.kind == ItemKind::Settings)
        {
            if (onSettingsRequested_)
                onSettingsRequested_();
        }
        else if (item.kind == ItemKind::About)
        {
            if (onAboutRequested_)
                onAboutRequested_();
        }

        closePopup();
    }

    void HeaderLogoPopupMenu::closePopup()
    {
        if (contextualHelpBinder_ != nullptr)
            contextualHelpBinder_->scheduleClear();

        if (auto* modal = getCurrentlyModalComponent())
        {
            if (modal == this)
                exitModalState(0);
        }
    }

    void HeaderLogoPopupMenu::detachContextualHelpBinder()
    {
        contextualHelpBinder_ = nullptr;
    }

    void HeaderLogoPopupMenu::resized()
    {
        layoutFamilyFlyout();
    }

    bool HeaderLogoPopupMenu::hitTest(int x, int y)
    {
        if (x >= 0 && y >= 0 && x < mainPopupWidth_ && y < mainPopupHeight_)
            return true;

        if (familyFlyout_ != nullptr && familyFlyout_->getBounds().contains(x, y))
            return true;

        return false;
    }

    void HeaderLogoPopupMenu::mouseMove(const juce::MouseEvent& e)
    {
        const int flatIndex = getFlatIndexAt(e.getPosition().x, e.getPosition().y);
        updateHighlightedItem(flatIndex);
        updateContextualHelpForItem(flatIndex);
    }

    void HeaderLogoPopupMenu::mouseUp(const juce::MouseEvent& e)
    {
        const auto flatIndex = getFlatIndexAt(e.getPosition().x, e.getPosition().y);
        if (flatIndex >= 0)
            selectItem(flatIndex);
    }

    void HeaderLogoPopupMenu::mouseExit(const juce::MouseEvent& e)
    {
        if (familyFlyout_ != nullptr && familyFlyout_->getBounds().contains(e.getEventRelativeTo(this).getPosition()))
            return;

        updateHighlightedItem(-1);
        updateContextualHelpForItem(-1);
    }

    bool HeaderLogoPopupMenu::keyPressed(const juce::KeyPress& key)
    {
        if (key.getKeyCode() == juce::KeyPress::escapeKey)
        {
            closePopup();
            return true;
        }

        return false;
    }

    void HeaderLogoPopupMenu::inputAttemptWhenModal()
    {
        closePopup();
    }

    void HeaderLogoPopupMenu::show(Logo& logo, ISkin& skin, Config config)
    {
        auto* topLevelComponent = logo.getTopLevelComponent();
        if (topLevelComponent == nullptr)
            return;

        auto popupMenu = std::make_unique<HeaderLogoPopupMenu>(skin, std::move(config));
        auto* rawPtr = popupMenu.get();
        const auto bounds = rawPtr->preferredBoundsOver(logo);
        rawPtr->mainPopupWidth_ = bounds.getWidth();
        rawPtr->mainPopupHeight_ = bounds.getHeight();

        topLevelComponent->addAndMakeVisible(popupMenu.release());
        rawPtr->setBounds(bounds);
        rawPtr->toFront(false);
        rawPtr->grabKeyboardFocus();
        rawPtr->enterModalState(false, nullptr, true);
        rawPtr->outsideDismissWatcher_->arm();

        // Open the family that owns the current skin so the side popup is discoverable.
        rawPtr->openFamilyFlyout(SkinCatalog::familyForItemId(rawPtr->currentSkinItemId_));
    }
}
