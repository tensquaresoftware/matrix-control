#include "TestSliders.h"

#include <array>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
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

    struct ColumnSpec
    {
        float scale;
        const char* label;
    };

    int scaledSize(int value, float scale)
    {
        return juce::roundToInt(static_cast<float>(value) * scale);
    }

    constexpr auto kColumnSpecs_ = std::array<ColumnSpec, 7>
    {{
        { 0.5f, "50%" },
        { 0.75f, "75%" },
        { 1.0f, "100%" },
        { 1.25f, "125%" },
        { 1.5f, "150%" },
        { 1.75f, "175%" },
        { 2.0f, "200%" }
    }};
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
                                    TSS::SliderConfig{ 0.0, 100.0, 50.0, 1.0, "%" });

        secondSlider_ = createSlider(dimensions_.topSliderWidth, dimensions_.topSliderHeight, sliderLook,
                                     TSS::SliderConfig{ -63.0, 63.0, 0.0, 1.0 });

        thirdSlider_ = createSlider(dimensions_.bottomSliderWidth, dimensions_.bottomSliderHeight, sliderLook,
                                    TSS::SliderConfig{ 0.0, 127.0, 63.0, 1.0 });

        fourthSlider_ = createSlider(dimensions_.bottomSliderWidth, dimensions_.bottomSliderHeight, sliderLookBoldLarge,
                                     TSS::SliderConfig{ 0.0, 127.0, 63.0, 1.0 });
    }

    int getScaledColumnWidth() const
    {
        return scaledSize(dimensions_.bottomSliderWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledTopSliderHeight = scaledSize(dimensions_.topSliderHeight, scale_);
        const int scaledBottomSliderHeight = scaledSize(dimensions_.bottomSliderHeight, scale_);
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
        const int scaledGap = scaledSize(dimensions_.gap, scale_);
        const int scaledTopSliderWidth = scaledSize(dimensions_.topSliderWidth, scale_);
        const int scaledTopSliderHeight = scaledSize(dimensions_.topSliderHeight, scale_);
        const int scaledBottomSliderWidth = scaledSize(dimensions_.bottomSliderWidth, scale_);
        const int scaledBottomSliderHeight = scaledSize(dimensions_.bottomSliderHeight, scale_);

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
    createColumnPanels(skin);
    layoutColumnPanels();
}

TestSliders::~TestSliders() = default;

void TestSliders::resized()
{
    layoutColumnPanels();
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
    columnPanels_.reserve(kColumnSpecs_.size());

    for (const auto& spec : kColumnSpecs_)
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
