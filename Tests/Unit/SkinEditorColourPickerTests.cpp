#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"
#include "Tools/SkinEditor/SkinEditorChromeTokens.h"
#include "Tools/SkinEditor/SkinEditorColourPicker.h"

class SkinEditorColourPickerTests : public juce::UnitTest
{
public:
    SkinEditorColourPickerTests()
        : juce::UnitTest("SkinEditorColourPicker")
    {
    }

    void runTest() override
    {
        SkinEditorChrome::warmFonts();
        saturationThenBrightnessMoveOnlyMatchingAxis();
        preferredHeightLayoutsTwoAxisSlidersBelowRgb();
    }

private:
    struct AxisLayoutProbe
    {
        juce::Label* hashLabel = nullptr;
        juce::Label* satLabel = nullptr;
        juce::Label* brightLabel = nullptr;
        juce::TextEditor* bEditor = nullptr;
        int rgbRowBottom = 0;
        juce::Array<juce::Slider*> sliders;
    };

    static juce::Array<juce::Slider*> linearHorizontalSliders(SkinEditorColourPicker& picker)
    {
        juce::Array<juce::Slider*> out;
        for (auto* child : picker.getChildren())
            if (auto* slider = dynamic_cast<juce::Slider*>(child))
                if (slider->getSliderStyle() == juce::Slider::LinearHorizontal)
                    out.add(slider);
        return out;
    }

    static AxisLayoutProbe probeAxisLayout(SkinEditorColourPicker& picker)
    {
        AxisLayoutProbe probe;
        probe.sliders = linearHorizontalSliders(picker);
        for (auto* child : picker.getChildren())
        {
            if (auto* lab = dynamic_cast<juce::Label*>(child))
            {
                if (lab->getText() == "#")
                    probe.hashLabel = lab;
                else if (lab->getText() == "Saturation:")
                    probe.satLabel = lab;
                else if (lab->getText() == "Brightness:")
                    probe.brightLabel = lab;
            }
            else if (auto* ed = dynamic_cast<juce::TextEditor*>(child))
            {
                probe.rgbRowBottom = juce::jmax(probe.rgbRowBottom, ed->getBottom());
                if (probe.bEditor == nullptr || ed->getRight() >= probe.bEditor->getRight())
                    probe.bEditor = ed;
            }
        }
        return probe;
    }

    void saturationThenBrightnessMoveOnlyMatchingAxis()
    {
        beginTest("Saturation then Brightness sliders move only matching HSV axis");

        SkinEditorColourPicker picker;
        picker.setColour(juce::Colour::fromHSV(0.45f, 0.40f, 0.70f, 1.0f),
                         juce::dontSendNotification);
        picker.setEnabledEditing(true);

        int changeCount = 0;
        juce::Colour last;
        picker.onColourChanged = [&](juce::Colour c)
        {
            ++changeCount;
            last = c;
        };

        auto sliders = linearHorizontalSliders(picker);
        expectEquals(sliders.size(), 2);
        auto* satSlider = sliders[0];
        auto* brightSlider = sliders[1];
        expect(satSlider != nullptr && brightSlider != nullptr);

        float h0 = 0.0f, s0 = 0.0f, v0 = 0.0f;
        picker.getColour().getHSB(h0, s0, v0);

        satSlider->setValue(0.85, juce::sendNotificationSync);
        expect(changeCount >= 1);
        float h1 = 0.0f, s1 = 0.0f, v1 = 0.0f;
        last.getHSB(h1, s1, v1);
        expectWithinAbsoluteError(h1, h0, 0.02f);
        expectWithinAbsoluteError(v1, v0, 0.02f);
        expect(s1 > s0 + 0.2f);
        expectWithinAbsoluteError(s1, 0.85f, 0.02f);

        const int afterSat = changeCount;
        float hSat = h1, sSat = s1, vSat = v1;
        brightSlider->setValue(0.25, juce::sendNotificationSync);
        expect(changeCount > afterSat);
        float h2 = 0.0f, s2 = 0.0f, v2 = 0.0f;
        last.getHSB(h2, s2, v2);
        expectWithinAbsoluteError(h2, hSat, 0.02f);
        expectWithinAbsoluteError(s2, sSat, 0.02f);
        expect(v2 < vSat - 0.2f);
        expectWithinAbsoluteError(v2, 0.25f, 0.02f);
    }

    void preferredHeightLayoutsTwoAxisSlidersBelowRgb()
    {
        beginTest("preferredHeight lays out two LinearHorizontal sliders below #/RGB");

        SkinEditorColourPicker picker;
        const int w = SkinEditorChrome::kColorPanelWidth;
        const int h = SkinEditorColourPicker::preferredHeightForWidth(w);
        picker.setBounds(0, 0, w, h);

        const auto probe = probeAxisLayout(picker);
        expectEquals(probe.sliders.size(), 2);
        expect(probe.hashLabel != nullptr && probe.satLabel != nullptr && probe.brightLabel != nullptr);
        expect(probe.bEditor != nullptr && probe.rgbRowBottom > 0);

        expectEquals(probe.satLabel->getX(), probe.hashLabel->getX());
        expectEquals(probe.brightLabel->getX(), probe.hashLabel->getX());

        const int gap = SkinEditorChrome::kPanelContentPad;
        constexpr int bottomPad = SkinEditorChrome::kPanelContentPad - 5;
        expectEquals(probe.satLabel->getY(), probe.rgbRowBottom + gap);
        expectEquals(probe.brightLabel->getY(), probe.satLabel->getBottom());
        expectEquals(h - probe.brightLabel->getBottom(), bottomPad);
        expectEquals(probe.sliders[0]->getY(), probe.satLabel->getY() + 1);
        expectEquals(probe.sliders[1]->getY(), probe.brightLabel->getY() + 1);

        constexpr int tipPad = juce::jmin(12, 24 / 2); // kFieldRowH / 2, same as layoutAxisRows
        constexpr int trimRight = 3;
        for (auto* slider : probe.sliders)
        {
            expect(slider->getWidth() > 0);
            expect(slider->getHeight() > 0);
            expectEquals(slider->getRight(), probe.bEditor->getRight() + tipPad - trimRight);
            expect(picker.getLocalBounds().contains(slider->getBounds()));
        }
        expect(probe.sliders[0]->getBottom() <= probe.sliders[1]->getY());
    }
};

static SkinEditorColourPickerTests skinEditorColourPickerTests;
