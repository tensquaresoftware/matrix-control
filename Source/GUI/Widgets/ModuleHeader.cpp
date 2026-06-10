#include "ModuleHeader.h"

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Widgets/Button.h"

namespace TSS
{
    namespace
    {
        ModuleHeader::ColourVariant colourVariantForLayout(ModuleHeader::ColumnLayout layout)
        {
            return (layout == ModuleHeader::ColumnLayout::PatchEdit)
                ? ModuleHeader::ColourVariant::Blue
                : ModuleHeader::ColourVariant::Orange;
        }

        int titleBandWidthForLayout(ModuleHeader::ColumnLayout layout, const ModuleHeaderDimensions& dimensions)
        {
            return (layout == ModuleHeader::ColumnLayout::PatchEdit)
                ? dimensions.patchEditTitleBandWidth
                : dimensions.masterEditTitleBandWidth;
        }
    }

    ModuleHeader::ModuleHeader(int width, int height, const ModuleHeaderLook& look, ColourVariant variant,
                               const juce::String& text, const ModuleHeaderDimensions& dimensions)
        : presentation_(Presentation::TitleOnly)
        , columnLayout_(ColumnLayout::PatchEdit)
        , buttonSet_(ButtonSet::None)
        , look_(look)
        , dimensions_(dimensions)
        , width_(width)
        , height_(height)
        , text_(text)
        , colourVariant_(variant)
    {
        setOpaque(false);
        setSize(width_, height_);
    }

    ModuleHeader::ModuleHeader(const WithActionsSpec& spec)
        : presentation_(Presentation::TitleWithActions)
        , columnLayout_(spec.columnLayout)
        , buttonSet_(spec.buttonSet)
        , look_(moduleHeaderLookFromSkin(spec.skin))
        , dimensions_(spec.dimensions)
        , titleBandWidthDesign_(titleBandWidthForLayout(spec.columnLayout, spec.dimensions))
        , text_(spec.widgetFactory.getGroupDisplayName(spec.moduleId))
        , colourVariant_(colourVariantForLayout(spec.columnLayout))
        , apvts_(&spec.apvts)
    {
        setOpaque(false);
        jassert(spec.buttonSet == ButtonSet::InitOnly || spec.buttonSet == ButtonSet::InitCopyPaste);

        createInitButton(spec);
        if (spec.buttonSet == ButtonSet::InitCopyPaste)
            createCopyPasteButtons(spec);
    }

    ModuleHeader::~ModuleHeader() = default;

    int ModuleHeader::getTitleBandWidthDesign() const
    {
        if (presentation_ == Presentation::TitleOnly)
            return width_;

        return titleBandWidthDesign_;
    }

    juce::Rectangle<float> ModuleHeader::getTitlePaintBounds() const
    {
        auto full = getLocalBounds().toFloat();

        if (presentation_ == Presentation::TitleOnly)
            return full;

        const float tw = static_cast<float>(
            ScaledLayout::scaledInt(static_cast<float>(getTitleBandWidthDesign()), uiScale_));
        full.setWidth(juce::jmin(tw, full.getWidth()));
        return full;
    }

    void ModuleHeader::setLook(const ModuleHeaderLook& look)
    {
        look_ = look;
        repaint();
    }

    void ModuleHeader::setSkin(ISkin& skin)
    {
        setLook(moduleHeaderLookFromSkin(skin));
        applyButtonLooksFromSkin(skin);
    }

    void ModuleHeader::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        repaint();
    }

    void ModuleHeader::setText(const juce::String& text)
    {
        if (text_ == text)
            return;

        text_ = text;
        repaint();
    }

    void ModuleHeader::paint(juce::Graphics& g)
    {
        if (text_.isEmpty())
            return;

        const auto bounds = getTitlePaintBounds();
        drawText(g, bounds);
        drawLine(g, bounds);
    }

    void ModuleHeader::resized()
    {
        if (presentation_ == Presentation::TitleWithActions)
        {
            if (buttonSet_ == ButtonSet::InitCopyPaste)
                layoutInitCopyPasteButtons();
            else
                layoutInitOnlyButtons();
        }

        if (initButton_)
            initButton_->setUiScale(uiScale_);
        if (copyButton_)
            copyButton_->setUiScale(uiScale_);
        if (pasteButton_)
            pasteButton_->setUiScale(uiScale_);

        repaint();
    }

    void ModuleHeader::createInitButton(const WithActionsSpec& spec)
    {
        initButton_ = spec.widgetFactory.createStandaloneButton(
            spec.initWidgetId,
            spec.skin,
            dimensions_.buttonHeight);
        initButton_->onClick = [this, id = spec.initWidgetId]
        {
            apvts_->state.setProperty(id, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
        };
        addAndMakeVisible(*initButton_);
    }

    void ModuleHeader::createCopyPasteButtons(const WithActionsSpec& spec)
    {
        copyButton_ = spec.widgetFactory.createStandaloneButton(
            spec.copyWidgetId,
            spec.skin,
            dimensions_.buttonHeight);
        copyButton_->onClick = [this, id = spec.copyWidgetId]
        {
            apvts_->state.setProperty(id, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
        };
        addAndMakeVisible(*copyButton_);

        pasteButton_ = spec.widgetFactory.createStandaloneButton(
            spec.pasteWidgetId,
            spec.skin,
            dimensions_.buttonHeight);
        pasteButton_->onClick = [this, id = spec.pasteWidgetId]
        {
            apvts_->state.setProperty(id, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
        };
        addAndMakeVisible(*pasteButton_);
    }

    void ModuleHeader::layoutInitOnlyButtons()
    {
        const float sf = uiScale_;
        const int initButtonWidth = ScaledLayout::scaledInt(static_cast<float>(dimensions_.initWidth), sf);
        const int buttonHeight = ScaledLayout::scaledInt(static_cast<float>(dimensions_.buttonHeight), sf);
        const int panelWidth = getWidth();

        if (auto* button = initButton_.get())
            button->setBounds(panelWidth - initButtonWidth, 0, initButtonWidth, buttonHeight);
    }

    void ModuleHeader::layoutInitCopyPasteButtons()
    {
        const float sf = uiScale_;
        const int buttonHeight = ScaledLayout::scaledInt(static_cast<float>(dimensions_.buttonHeight), sf);
        const int panelWidth = getWidth();

        const int pasteButtonWidth = ScaledLayout::scaledInt(static_cast<float>(dimensions_.pasteWidth), sf);
        const int copyButtonWidth = ScaledLayout::scaledInt(static_cast<float>(dimensions_.copyWidth), sf);
        const int initButtonWidth = ScaledLayout::scaledInt(static_cast<float>(dimensions_.initWidth), sf);

        const int pasteX = panelWidth - ScaledLayout::scaledInt(static_cast<float>(dimensions_.pasteWidth), sf);
        const int copyX = panelWidth - ScaledLayout::scaledInt(
            static_cast<float>(dimensions_.pasteWidth + dimensions_.copyWidth), sf);
        const int initX = panelWidth - ScaledLayout::scaledInt(
            static_cast<float>(dimensions_.pasteWidth + dimensions_.copyWidth + dimensions_.initWidth), sf);

        if (auto* button = pasteButton_.get())
            button->setBounds(pasteX, 0, pasteButtonWidth, buttonHeight);
        if (auto* button = copyButton_.get())
            button->setBounds(copyX, 0, copyButtonWidth, buttonHeight);
        if (auto* button = initButton_.get())
            button->setBounds(initX, 0, initButtonWidth, buttonHeight);
    }

    void ModuleHeader::applyButtonLooksFromSkin(ISkin& skin)
    {
        const auto bl = buttonLookFromSkin(skin);
        if (initButton_)
            initButton_->setLook(bl);
        if (copyButton_)
            copyButton_->setLook(bl);
        if (pasteButton_)
            pasteButton_->setLook(bl);
    }

    void ModuleHeader::drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        if (text_.isEmpty())
            return;

        const float textAreaHeight = static_cast<float>(dimensions_.textAreaHeight) * uiScale_;
        const float textLeftPadding = static_cast<float>(dimensions_.textLeftPadding) * uiScale_;

        auto textBounds = bounds;
        textBounds.setHeight(textAreaHeight);
        textBounds.removeFromLeft(textLeftPadding);

        g.setColour(look_.text);
        g.setFont(look_.font.withHeight(look_.font.getHeight() * uiScale_));
        g.drawText(text_, textBounds, juce::Justification::centredLeft, false);
    }

    void ModuleHeader::drawLine(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float textAreaHeight = static_cast<float>(dimensions_.textAreaHeight) * uiScale_;
        const float lineThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            static_cast<float>(dimensions_.lineThickness),
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);
        const auto lineAreaHeight = bounds.getHeight() - textAreaHeight;
        const auto verticalOffset = textAreaHeight + (lineAreaHeight - lineThickness) * 0.5f;

        auto lineBounds = bounds;
        lineBounds.setHeight(lineThickness);
        lineBounds.translate(0.0f, verticalOffset);

        g.setColour(getLineColour());
        g.fillRect(lineBounds);
    }

    juce::Colour ModuleHeader::getLineColour() const
    {
        return (colourVariant_ == ColourVariant::Blue) ? look_.lineBlue : look_.lineOrange;
    }
}
