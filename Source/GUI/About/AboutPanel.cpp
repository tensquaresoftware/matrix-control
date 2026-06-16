#include "AboutPanel.h"

#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginVersion.h"

using TSS::SkinColourId;

namespace
{
    constexpr juce::uint32 kTitleAndValueColour = ColourChart::kWhite;

    void configureHyperlink(juce::HyperlinkButton& link, const juce::String& text, const juce::URL& url)
    {
        link.setButtonText(text);
        link.setURL(url);
        link.setJustificationType(juce::Justification::centredLeft);
        link.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        link.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        link.setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
        link.setColour(juce::TextButton::textColourOnId, juce::Colour(kTitleAndValueColour));
        link.setColour(juce::TextButton::textColourOffId, juce::Colour(kTitleAndValueColour));
    }

    int measureTextWidth(const juce::Font& font, const juce::String& text)
    {
        juce::GlyphArrangement glyphs;
        glyphs.addLineOfText(font, text, 0.0f, 0.0f);
        return juce::roundToInt(glyphs.getBoundingBox(0, -1, true).getWidth());
    }
}

AboutPanel::AboutPanel(TSS::ISkin& skin)
    : skin_(&skin)
    , emailLink_(PluginDisplayNames::About::kEmailDisplay,
                 juce::URL(PluginDisplayNames::About::kEmailUrl))
    , githubLink_(PluginDisplayNames::About::kGitHubDisplay,
                  juce::URL(PluginDisplayNames::About::kGitHubUrl))
    , linkedInLink_(PluginDisplayNames::About::kLinkedInDisplay,
                    juce::URL(PluginDisplayNames::About::kLinkedInUrl))
{
    setOpaque(true);

    configureHyperlink(emailLink_, PluginDisplayNames::About::kEmailDisplay,
                       juce::URL(PluginDisplayNames::About::kEmailUrl));
    configureHyperlink(githubLink_, PluginDisplayNames::About::kGitHubDisplay,
                       juce::URL(PluginDisplayNames::About::kGitHubUrl));
    configureHyperlink(linkedInLink_, PluginDisplayNames::About::kLinkedInDisplay,
                       juce::URL(PluginDisplayNames::About::kLinkedInUrl));

    addAndMakeVisible(emailLink_);
    addAndMakeVisible(githubLink_);
    addAndMakeVisible(linkedInLink_);
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
}

void AboutPanel::layoutHyperlinkButtons()
{
    emailLink_.setBounds(getSpecValueRowBounds(2));
    githubLink_.setBounds(getSpecValueRowBounds(3));
    linkedInLink_.setBounds(getSpecValueRowBounds(4));

    const float sf = uiScale_;
    const auto linkFont = skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * sf);
    emailLink_.setFont(linkFont, false);
    githubLink_.setFont(linkFont, false);
    linkedInLink_.setFont(linkFont, false);
}

void AboutPanel::resized()
{
    layoutHyperlinkButtons();
}

void AboutPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
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
