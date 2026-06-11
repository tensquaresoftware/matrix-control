#include "TestSliders.h"

#include <array>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Slider.h"

namespace
{
    struct SliderSetDimensions
    {
        int gap;
        int topSliderWidth;
        int topSliderHeight;
        int bottomSliderWidth;
        int bottomSliderHeight;
        int scaleLabelHeight;
    };
}

class TestSliders::SliderScalePanel : public juce::Component
{
public:
    SliderScalePanel(float scale,
                     const juce::String& scaleLabelText,
                     const TSS::SliderLook& sliderLook,
                     const TSS::SliderLook& sliderLookBoldLarge,
                     const TSS::LabelLook& labelLook,
                     const SliderSetDimensions& dimensions)
        : scale_(scale)
        , dimensions_(dimensions)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            dimensions_.bottomSliderWidth,
            dimensions_.scaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        firstSlider_ = createSlider(dimensions_.topSliderWidth, dimensions_.topSliderHeight, sliderLook,
                                    TSS::SliderConfig{ 0.0, 100.0, 50.0, 1.0, "%", {} });

        secondSlider_ = createSlider(dimensions_.topSliderWidth, dimensions_.topSliderHeight, sliderLook,
                                     TSS::SliderConfig{ -63.0, 63.0, 0.0, 1.0, {}, {} });

        thirdSlider_ = createSlider(dimensions_.bottomSliderWidth, dimensions_.bottomSliderHeight, sliderLook,
                                    TSS::SliderConfig{ 0.0, 127.0, 63.0, 1.0, {}, {} });

        fourthSlider_ = createSlider(dimensions_.bottomSliderWidth, dimensions_.bottomSliderHeight, sliderLookBoldLarge,
                                     TSS::SliderConfig{ 0.0, 127.0, 63.0, 1.0, {}, {} });
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(dimensions_.bottomSliderWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledTopSliderHeight = TestScaleColumns::scaledSize(dimensions_.topSliderHeight, scale_);
        const int scaledBottomSliderHeight = TestScaleColumns::scaledSize(dimensions_.bottomSliderHeight, scale_);
        const int labelToSlidersGap = dimensions_.gap;

        const int rowGap = dimensions_.gap;

        return dimensions_.scaleLabelHeight
         + labelToSlidersGap
         + scaledTopSliderHeight
         + rowGap
         + scaledBottomSliderHeight
         + rowGap
         + scaledBottomSliderHeight;
    }

    void resized() override
    {
        const int scaledGap = TestScaleColumns::scaledSize(dimensions_.gap, scale_);
        const int scaledTopSliderWidth = TestScaleColumns::scaledSize(dimensions_.topSliderWidth, scale_);
        const int scaledTopSliderHeight = TestScaleColumns::scaledSize(dimensions_.topSliderHeight, scale_);
        const int scaledBottomSliderWidth = TestScaleColumns::scaledSize(dimensions_.bottomSliderWidth, scale_);
        const int scaledBottomSliderHeight = TestScaleColumns::scaledSize(dimensions_.bottomSliderHeight, scale_);

        const int labelY = 0;
        const int slidersStartY = dimensions_.scaleLabelHeight + dimensions_.gap;
        const int secondSliderX = scaledTopSliderWidth + scaledGap;
        const int thirdSliderY = slidersStartY + scaledTopSliderHeight + dimensions_.gap;
        const int fourthSliderY = thirdSliderY + scaledBottomSliderHeight + dimensions_.gap;

        scaleLabel_->setBounds(0, labelY, scaledBottomSliderWidth, dimensions_.scaleLabelHeight);
        firstSlider_->setBounds(0, slidersStartY, scaledTopSliderWidth, scaledTopSliderHeight);
        secondSlider_->setBounds(secondSliderX, slidersStartY, scaledTopSliderWidth, scaledTopSliderHeight);
        thirdSlider_->setBounds(0, thirdSliderY, scaledBottomSliderWidth, scaledBottomSliderHeight);
        fourthSlider_->setBounds(0, fourthSliderY, scaledBottomSliderWidth, scaledBottomSliderHeight);
    }

private:
    std::unique_ptr<TSS::Slider> createSlider(int width,
                                              int height,
                                              const TSS::SliderLook& look,
                                              const TSS::SliderConfig& config)
    {
        auto slider = std::make_unique<TSS::Slider>(width, height, look, config);
        slider->setUiScale(scale_);
        addAndMakeVisible(*slider);
        return slider;
    }

    float scale_ { 1.0f };
    SliderSetDimensions dimensions_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::Slider> firstSlider_;
    std::unique_ptr<TSS::Slider> secondSlider_;
    std::unique_ptr<TSS::Slider> thirdSlider_;
    std::unique_ptr<TSS::Slider> fourthSlider_;
};

TestSliders::TestSliders(TSS::ISkin& skin)
{
    setSkin(skin);
}

TestSliders::~TestSliders() = default;

void TestSliders::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    createColumnPanels(skin);
    layoutColumnPanels();
}

void TestSliders::resized()
{
    layoutColumnPanels();
}

int TestSliders::getPreferredWidth() const
{
    const int panelGap = kGap_ * kPanelGapMultiplier_;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const std::unique_ptr<SliderScalePanel>& panel) { return panel->getScaledColumnWidth(); }) + (2 * kPadding_);
}

int TestSliders::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight + (2 * kPadding_);
}

void TestSliders::createColumnPanels(TSS::ISkin& skin)
{
    const SliderSetDimensions dimensions
    {
        kGap_,
        kTopSliderWidth_,
        kTopSliderHeight_,
        kBottomSliderWidth_,
        kBottomSliderHeight_,
        kScaleLabelHeight_
    };

    const auto sliderLook = TSS::sliderLookFromSkin(skin);
    auto sliderLookBoldLarge = sliderLook;
    sliderLookBoldLarge.font = skin.getBaseFontBold().withHeight(kBoldLargeSliderFontHeight_);
    const auto labelLook = TSS::labelLookFromSkin(skin);

    columnPanels_.clear();
    removeAllChildren();
    columnPanels_.reserve(TestScaleColumns::kSpecs.size());

    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<SliderScalePanel>(
            spec.scale,
            spec.label,
            sliderLook,
            sliderLookBoldLarge,
            labelLook,
            dimensions);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }
}

void TestSliders::layoutColumnPanels()
{
    const int panelGap = kGap_ * kPanelGapMultiplier_;
    int nextColumnX = kPadding_;

    for (auto& panel : columnPanels_)
    {
        const int columnWidth = panel->getScaledColumnWidth();
        panel->setBounds(nextColumnX, kPadding_, columnWidth, panel->getPreferredHeight());
        nextColumnX += columnWidth + panelGap;
    }
}
