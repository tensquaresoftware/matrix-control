#include "AboutPanel.h"

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginVersion.h"

using TSS::SkinColourId;

namespace
{
    constexpr juce::uint32 kTitleAndValueColour = ColourChart::kWhite;
    // Same red as About/Settings modal close cross (AboutWindow / SettingsWindow).
    constexpr juce::uint32 kLinkHoverColour = 0xff9A131D;

    int measureTextWidth(const juce::Font& font, const juce::String& text)
    {
        return juce::GlyphArrangement::getStringWidthInt(font, text);
    }
}

AboutPanel::AboutTextLink::AboutTextLink(const juce::String& text, juce::URL url)
    : text_(text)
    , url_(std::move(url))
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

void AboutPanel::AboutTextLink::setFont(juce::Font font)
{
    font_ = std::move(font);
    repaint();
}

void AboutPanel::AboutTextLink::setIdleColour(juce::Colour colour)
{
    idleColour_ = colour;
    repaint();
}

void AboutPanel::AboutTextLink::setHoverColour(juce::Colour colour)
{
    hoverColour_ = colour;
    repaint();
}

void AboutPanel::AboutTextLink::setLinkText(const juce::String& text)
{
    text_ = text;
    repaint();
}

void AboutPanel::AboutTextLink::setURL(juce::URL url)
{
    url_ = std::move(url);
}

void AboutPanel::AboutTextLink::paint(juce::Graphics& g)
{
    g.setFont(font_);
    g.setColour(hovered_ ? hoverColour_ : idleColour_);
    g.drawText(text_, getLocalBounds(), juce::Justification::centredLeft, false);
}

void AboutPanel::AboutTextLink::mouseEnter(const juce::MouseEvent&)
{
    hovered_ = true;
    repaint();
}

void AboutPanel::AboutTextLink::mouseExit(const juce::MouseEvent&)
{
    hovered_ = false;
    leftPressActive_ = false;
    repaint();
}

void AboutPanel::AboutTextLink::mouseDown(const juce::MouseEvent& event)
{
    leftPressActive_ = event.mods.isLeftButtonDown();
}

void AboutPanel::AboutTextLink::mouseUp(const juce::MouseEvent& event)
{
    const bool wasLeftPress = leftPressActive_;
    leftPressActive_ = false;

    if (! wasLeftPress || ! event.mouseWasClicked() || ! isEnabled() || ! url_.isWellFormed())
        return;

    if (! getLocalBounds().contains(event.getPosition()))
        return;

    url_.launchInDefaultBrowser();
}

AboutPanel::AboutPanel(TSS::ISkin& skin)
    : skin_(&skin)
    , emailLink_(PluginDisplayNames::About::kEmailDisplay,
                 juce::URL(PluginDisplayNames::About::kEmailUrl))
    , githubLink_(PluginDisplayNames::About::kGitHubDisplay,
                  juce::URL(PluginDisplayNames::About::kGitHubUrl))
    , linkedInLink_(PluginDisplayNames::About::kLinkedInDisplay,
                    juce::URL(PluginDisplayNames::About::kLinkedInUrl))
    , bmadLink_(PluginDisplayNames::About::kBmadCreditLinkDisplay,
                juce::URL(PluginDisplayNames::About::kBmadCreditUrl))
{
    setOpaque(true);
    refreshHyperlinkAppearance();

    addAndMakeVisible(emailLink_);
    addAndMakeVisible(githubLink_);
    addAndMakeVisible(linkedInLink_);
    addAndMakeVisible(bmadLink_);
}

void AboutPanel::registerContextualHelp(TSS::ContextualHelpBinder::FooterResolver resolveFooter)
{
    namespace Help = PluginDisplayNames::About::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(std::move(resolveFooter));
    contextualHelpBinder_->setHostShowingPredicate([this] { return isShowing(); });
    contextualHelpBinder_->bind(&emailLink_, Help::kEmail);
    contextualHelpBinder_->bind(&githubLink_, Help::kGitHub);
    contextualHelpBinder_->bind(&linkedInLink_, Help::kLinkedIn);
    contextualHelpBinder_->bind(&bmadLink_, Help::kBmad);
}

void AboutPanel::setOnEscapePressed(std::function<void()> callback)
{
    onEscapePressed_ = std::move(callback);
}

void AboutPanel::refreshHyperlinkAppearance()
{
    const auto valueColour = juce::Colour(kTitleAndValueColour);
    const auto labelColour = skin_->getColour(SkinColourId::kLabelText);
    const auto hoverColour = juce::Colour(kLinkHoverColour);

    emailLink_.setLinkText(PluginDisplayNames::About::kEmailDisplay);
    emailLink_.setURL(juce::URL(PluginDisplayNames::About::kEmailUrl));
    emailLink_.setIdleColour(valueColour);
    emailLink_.setHoverColour(hoverColour);

    githubLink_.setLinkText(PluginDisplayNames::About::kGitHubDisplay);
    githubLink_.setURL(juce::URL(PluginDisplayNames::About::kGitHubUrl));
    githubLink_.setIdleColour(valueColour);
    githubLink_.setHoverColour(hoverColour);

    linkedInLink_.setLinkText(PluginDisplayNames::About::kLinkedInDisplay);
    linkedInLink_.setURL(juce::URL(PluginDisplayNames::About::kLinkedInUrl));
    linkedInLink_.setIdleColour(valueColour);
    linkedInLink_.setHoverColour(hoverColour);

    bmadLink_.setLinkText(PluginDisplayNames::About::kBmadCreditLinkDisplay);
    bmadLink_.setURL(juce::URL(PluginDisplayNames::About::kBmadCreditUrl));
    bmadLink_.setIdleColour(labelColour);
    bmadLink_.setHoverColour(hoverColour);

    layoutHyperlinkButtons();
}

juce::String AboutPanel::getSpecLabel(int rowIndex) const
{
    switch (rowIndex)
    {
        case 0: return PluginDisplayNames::About::kOrganizationLabel;
        case 1: return PluginDisplayNames::About::kAuthorLabel;
        case 2: return PluginDisplayNames::About::kEmailLabel;
        case 3: return PluginDisplayNames::About::kGitHubLabel;
        case 4: return PluginDisplayNames::About::kLinkedInLabel;
        case 5: return PluginDisplayNames::About::kVersionLabel;
        case 6: return PluginDisplayNames::About::kReleaseDateLabel;
        default: return {};
    }
}

juce::String AboutPanel::getSpecValue(int rowIndex) const
{
    switch (rowIndex)
    {
        case 0: return PluginDisplayNames::About::kOrganizationValue;
        case 1: return PluginDisplayNames::About::kAuthorValue;
        case 2: return PluginDisplayNames::About::kEmailDisplay;
        case 3: return PluginDisplayNames::About::kGitHubDisplay;
        case 4: return PluginDisplayNames::About::kLinkedInDisplay;
        case 5: return PluginVersion::getVersionString();
        case 6: return PluginVersion::getReleaseDateString();
        default: return {};
    }
}

AboutPanel::SpecGridLayout AboutPanel::getSpecGridLayout() const
{
    SpecGridLayout layout;
    const float sf = uiScale_;
    const int padding = juce::roundToInt(static_cast<float>(kPadding_) * sf);
    const int titleBlock = juce::roundToInt(static_cast<float>(kTitleHeight_ + kGapAfterTitle_) * sf);
    const int taglineBlock = juce::roundToInt(static_cast<float>(kTaglineHeight_ + kGapAfterTagline_) * sf);
    layout.rowHeight = juce::roundToInt(static_cast<float>(kSpecRowHeight_) * sf);
    layout.firstRowY = padding + titleBlock + taglineBlock;

    const auto baseFont = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * sf);
    const int columnGap = juce::roundToInt(static_cast<float>(kColumnGapDesign_) * sf);

    int maxLabelWidth = 0;
    int maxValueWidth = 0;
    for (int row = 0; row < kSpecRowCount_; ++row)
    {
        maxLabelWidth = juce::jmax(maxLabelWidth, measureTextWidth(baseFont, getSpecLabel(row)));
        maxValueWidth = juce::jmax(maxValueWidth, measureTextWidth(baseFont, getSpecValue(row)));
    }

    const int gridWidth = maxLabelWidth + columnGap + maxValueWidth;
    const int contentWidth = getWidth() - padding * 2;
    const int gridX = padding + juce::jmax(0, (contentWidth - gridWidth) / 2);

    layout.labelColumn = { gridX, layout.firstRowY, maxLabelWidth, layout.rowHeight * kSpecRowCount_ };
    layout.valueColumn = { gridX + maxLabelWidth + columnGap, layout.firstRowY, maxValueWidth, layout.rowHeight * kSpecRowCount_ };
    return layout;
}

AboutPanel::BmadCreditLayout AboutPanel::getBmadCreditLayout() const
{
    BmadCreditLayout layout;
    const float sf = uiScale_;
    const int padding = juce::roundToInt(static_cast<float>(kPadding_) * sf);
    const auto grid = getSpecGridLayout();
    const int specsBottom = grid.firstRowY + grid.rowHeight * kSpecRowCount_;
    const int gapBeforeSep = juce::roundToInt(static_cast<float>(kGapBeforeCreditSeparator_) * sf);
    const int sepBand = juce::roundToInt(static_cast<float>(kCreditSeparatorBand_) * sf);
    const int gapAfterSep = juce::roundToInt(static_cast<float>(kGapAfterCreditSeparator_) * sf);
    const int creditHeight = juce::roundToInt(static_cast<float>(kCreditLineHeight_) * sf);

    layout.separatorBounds = {
        padding,
        specsBottom + gapBeforeSep,
        juce::jmax(0, getWidth() - padding * 2),
        sepBand
    };

    const auto italicFont = skin_->getBaseFont()
                                .withHeight(skin_->getBaseFont().getHeight() * sf)
                                .italicised();
    const auto prefix = juce::String(PluginDisplayNames::About::kBmadCreditPrefix);
    const auto linkText = juce::String(PluginDisplayNames::About::kBmadCreditLinkDisplay);
    const auto suffix = juce::String(PluginDisplayNames::About::kBmadCreditSuffix);
    const int prefixWidth = measureTextWidth(italicFont, prefix);
    const int linkWidth = measureTextWidth(italicFont, linkText);
    const int suffixWidth = measureTextWidth(italicFont, suffix);
    const int totalWidth = prefixWidth + linkWidth + suffixWidth;
    const int contentWidth = juce::jmax(0, getWidth() - padding * 2);
    const int startX = padding + juce::jmax(0, (contentWidth - totalWidth) / 2);
    const int creditY = layout.separatorBounds.getBottom() + gapAfterSep;

    layout.prefixBounds = { startX, creditY, prefixWidth, creditHeight };
    layout.linkBounds = { startX + prefixWidth, creditY, linkWidth, creditHeight };
    layout.suffixBounds = { startX + prefixWidth + linkWidth, creditY, suffixWidth, creditHeight };
    return layout;
}

juce::Rectangle<int> AboutPanel::getSpecRowBounds(int rowIndex, bool labelColumn) const
{
    const auto grid = getSpecGridLayout();
    const auto column = labelColumn ? grid.labelColumn : grid.valueColumn;
    return { column.getX(), column.getY() + rowIndex * grid.rowHeight, column.getWidth(), grid.rowHeight };
}

juce::Rectangle<int> AboutPanel::getSpecValueRowBounds(int rowIndex) const
{
    const auto grid = getSpecGridLayout();
    const float sf = uiScale_;
    const auto baseFont = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * sf);
    const int textWidth = measureTextWidth(baseFont, getSpecValue(rowIndex));
    const int slack = juce::roundToInt(2.0f * sf);
    const int y = grid.firstRowY + rowIndex * grid.rowHeight;

    return { grid.valueColumn.getX(), y, textWidth + slack, grid.rowHeight };
}

void AboutPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kBodyPanelBackground));

    const float sf = uiScale_;
    const int padding = juce::roundToInt(static_cast<float>(kPadding_) * sf);
    auto bounds = getLocalBounds().reduced(padding);

    const auto labelColour = skin_->getColour(SkinColourId::kLabelText);
    const auto valueColour = juce::Colour(kTitleAndValueColour);
    const auto baseFont = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * sf);
    const auto titleFont = skin_->getBrandFontBold().withHeight(skin_->getBrandFontBold().getHeight() * sf);

    auto titleArea = bounds.removeFromTop(juce::roundToInt(static_cast<float>(kTitleHeight_) * sf));
    g.setColour(valueColour);
    g.setFont(titleFont);
    g.drawText(PluginDisplayNames::About::kProductTitle,
               titleArea,
               juce::Justification::centred,
               false);

    bounds.removeFromTop(juce::roundToInt(static_cast<float>(kGapAfterTitle_) * sf));

    auto taglineArea = bounds.removeFromTop(juce::roundToInt(static_cast<float>(kTaglineHeight_) * sf));
    g.setColour(labelColour);
    g.setFont(baseFont);
    g.drawFittedText(PluginDisplayNames::About::kTagline,
                     taglineArea,
                     juce::Justification::centred,
                     2);

    for (int row = 0; row < kSpecRowCount_; ++row)
    {
        const auto labelBounds = getSpecRowBounds(row, true);
        g.setColour(labelColour);
        g.setFont(baseFont);
        g.drawText(getSpecLabel(row), labelBounds, juce::Justification::centredRight, false);

        if (row >= 2 && row <= 4)
            continue;

        const auto valueBounds = getSpecRowBounds(row, false);
        g.setColour(valueColour);
        g.drawText(getSpecValue(row), valueBounds, juce::Justification::centredLeft, false);
    }

    paintBmadCredit(g);
}

void AboutPanel::paintBmadCredit(juce::Graphics& g)
{
    const float sf = uiScale_;
    const auto labelColour = skin_->getColour(SkinColourId::kLabelText);
    const auto baseFont = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * sf);
    const auto credit = getBmadCreditLayout();

    constexpr float kSeparatorDesignThickness = 1.0f;
    const float lineThickness = TSS::ScaledDrawing::snappedStrokeThicknessFromDesign(
        kSeparatorDesignThickness,
        sf,
        TSS::ScaledDrawing::systemDisplayScaleForComponent(*this),
        TSS::ScaledDrawing::StrokeSnapPolicy::kRound);
    auto separatorLine = credit.separatorBounds.toFloat();
    separatorLine.setHeight(lineThickness);
    separatorLine.setY(static_cast<float>(credit.separatorBounds.getCentreY()) - lineThickness * 0.5f);
    g.setColour(skin_->getColour(SkinColourId::kHorizontalSeparatorLine));
    g.fillRect(separatorLine);

    const auto italicFont = baseFont.italicised();
    g.setFont(italicFont);
    g.setColour(labelColour);
    g.drawText(PluginDisplayNames::About::kBmadCreditPrefix,
               credit.prefixBounds,
               juce::Justification::centredLeft,
               false);
    g.drawText(PluginDisplayNames::About::kBmadCreditSuffix,
               credit.suffixBounds,
               juce::Justification::centredLeft,
               false);
}

void AboutPanel::layoutHyperlinkButtons()
{
    emailLink_.setBounds(getSpecValueRowBounds(2));
    githubLink_.setBounds(getSpecValueRowBounds(3));
    linkedInLink_.setBounds(getSpecValueRowBounds(4));

    const float sf = uiScale_;
    const auto linkFont = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * sf);
    emailLink_.setFont(linkFont);
    githubLink_.setFont(linkFont);
    linkedInLink_.setFont(linkFont);

    const auto credit = getBmadCreditLayout();
    bmadLink_.setFont(linkFont.italicised());
    bmadLink_.setBounds(credit.linkBounds);
}

void AboutPanel::resized()
{
    layoutHyperlinkButtons();
}

bool AboutPanel::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey && onEscapePressed_)
    {
        onEscapePressed_();
        return true;
    }

    return Component::keyPressed(key);
}

void AboutPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    refreshHyperlinkAppearance();
    repaint();
}

void AboutPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    layoutHyperlinkButtons();
    repaint();
}
