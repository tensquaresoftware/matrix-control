#include "Tools/SkinDesigner/SkinDesignerMappingsFilterMenu.h"

#include "Tools/SkinDesigner/SkinDesignerChrome.h"
#include "Tools/SkinDesigner/SkinDesignerMappingsColumnFilter.h"
#include "Tools/SkinDesigner/SkinDesignerUiHelpers.h"

namespace SkinDesignerMappingsFilterMenu
{
    namespace
    {
        constexpr int kPaletteSwatchX = 4;
        constexpr int kPaletteNameGap = 8;
        constexpr int kMenuTickSlot = 16;

        struct MappedColourMenuSpec
        {
            juce::String name;
            juce::uint32 argb = 0;
            bool pending = false;
            bool unbound = false;
            bool foundInPalette = false;
            bool ticked = false;
        };

        class MappedColourFilterMenuItem final : public juce::PopupMenu::CustomComponent
        {
        public:
            explicit MappedColourFilterMenuItem(MappedColourMenuSpec spec)
                : spec_(std::move(spec))
            {
            }

            void getIdealSize(int& idealWidth, int& idealHeight) override
            {
                idealHeight = SkinDesignerChrome::kColumnHeaderHeight;
                const int sw = SkinDesignerChrome::kColourSwatchSize;
                const int nameW = SkinDesignerChrome::measureTextWidth(
                    SkinDesignerChrome::fontRegular(),
                    SkinDesignerUi::displayPastilleName(spec_.name));
                idealWidth = kMenuTickSlot + kPaletteSwatchX + sw + kPaletteNameGap + nameW + 12;
            }

            void paint(juce::Graphics& g) override
            {
                auto bounds = getLocalBounds();
                if (isItemHighlighted())
                {
                    g.setColour(SkinDesignerChrome::controlFillHover());
                    g.fillRect(bounds);
                }

                if (spec_.ticked)
                {
                    auto tickArea = bounds.removeFromLeft(kMenuTickSlot).toFloat().reduced(3.0f, 4.0f);
                    auto tick = getLookAndFeel().getTickShape(1.0f);
                    g.setColour(SkinDesignerChrome::textPrimary());
                    g.fillPath(tick, tick.getTransformToScaleToFit(tickArea, true));
                }
                else
                {
                    bounds.removeFromLeft(kMenuTickSlot);
                }

                const int sw = SkinDesignerChrome::kColourSwatchSize;
                const int contentH = bounds.getHeight();
                const int swY = bounds.getY() + (contentH - sw) / 2;
                const int swX = bounds.getX() + kPaletteSwatchX;
                if (! spec_.unbound && spec_.foundInPalette)
                {
                    const auto swatch = juce::Rectangle<int>(swX, swY, sw, sw);
                    if (spec_.pending)
                        SkinDesignerChrome::paintPendingColourSwatch(g, swatch);
                    else
                        SkinDesignerChrome::paintColourSwatch(g, swatch, juce::Colour(spec_.argb));
                }

                const int nameLeft = bounds.getX() + kPaletteSwatchX + sw + kPaletteNameGap;
                g.setFont(SkinDesignerChrome::fontRegular());
                g.setColour(spec_.unbound ? SkinDesignerChrome::attention()
                                          : SkinDesignerChrome::textPrimary());
                g.drawText(SkinDesignerUi::displayPastilleName(spec_.name),
                           nameLeft,
                           bounds.getY(),
                           juce::jmax(0, bounds.getRight() - nameLeft - 4),
                           contentH,
                           juce::Justification::centredLeft,
                           false);
            }

        private:
            MappedColourMenuSpec spec_;
        };

        MappedColourMenuSpec makeMappedMenuSpec(const TSS::DevSkin::NamedSkin& skin,
                                                const juce::String& name,
                                                bool ticked)
        {
            MappedColourMenuSpec spec;
            spec.name = name;
            spec.ticked = ticked;
            spec.unbound = (name == SkinDesignerMappingsColumnFilter::kUnboundMenuValue);
            if (spec.unbound)
                return spec;
            for (const auto& pastille : skin.palette)
            {
                if (pastille.name == name)
                {
                    spec.argb = pastille.argb;
                    spec.pending = pastille.pending;
                    spec.foundInPalette = true;
                    break;
                }
            }
            return spec;
        }
    }

    void appendMappedColourItems(juce::PopupMenu& menu,
                                 const TSS::DevSkin::NamedSkin& skin,
                                 const juce::StringArray& values,
                                 const juce::String& active)
    {
        for (int i = 0; i < values.size(); ++i)
        {
            const auto& name = values[i];
            menu.addCustomItem(i + 1,
                               std::make_unique<MappedColourFilterMenuItem>(
                                   makeMappedMenuSpec(skin, name, name == active)),
                               nullptr,
                               name);
        }
    }
}
