#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "ColourChart.h"

namespace tss
{
    namespace SkinColours
    {
        struct ColourElement
        {
            const char* elementName;
            juce::uint32 blackVariant;
            juce::uint32 creamVariant;
        };

        namespace Common
        {
            static constexpr juce::uint32 kBackgroundDisabled = ColourChart::kDarkGrey3;
            static constexpr juce::uint32 kBorderDisabled = ColourChart::kDarkGrey4;
            static constexpr juce::uint32 kDecorationDisabled = ColourChart::kDarkGrey5;
            static constexpr juce::uint32 kContentDisabled = ColourChart::kDarkGrey5;
            static constexpr juce::uint32 kContentHoover = ColourChart::kDarkGrey1;
        }

        namespace Panels
        {
            inline constexpr ColourElement kHeaderPanelBackground = {
                "HeaderPanelBackground",
                ColourChart::kLightGrey1,
                ColourChart::kLightGrey1
            };

            inline constexpr ColourElement kBodyPanelBackground = {
                "BodyPanelBackground",
                ColourChart::kDarkGrey2,
                ColourChart::kCream
            };

            inline constexpr ColourElement kFooterPanelBackground = {
                "FooterPanelBackground",
                ColourChart::kLightGrey1,
                ColourChart::kLightGrey1
            };
        }

        namespace Widgets
        {
            namespace SectionHeader
            {
                inline constexpr ColourElement kText = {
                    "SectionHeaderText",
                    ColourChart::kLightGrey2,
                    ColourChart::kLightGrey2
                };

                inline constexpr ColourElement kLineBlue = {
                    "SectionHeaderLineBlue",
                    ColourChart::kBlue,
                    ColourChart::kBlue
                };

                inline constexpr ColourElement kLineOrange = {
                    "SectionHeaderLineOrange",
                    ColourChart::kOrange,
                    ColourChart::kOrange
                };
            }

            namespace ModuleHeader
            {
                inline constexpr ColourElement kText = {
                    "ModuleHeaderText",
                    ColourChart::kLightGrey2,
                    ColourChart::kLightGrey2
                };

                inline constexpr ColourElement kLineBlue = {
                    "ModuleHeaderLineBlue",
                    ColourChart::kBlue,
                    ColourChart::kBlue
                };

                inline constexpr ColourElement kLineOrange = {
                    "ModuleHeaderLineOrange",
                    ColourChart::kOrange,
                    ColourChart::kOrange
                };
            }

            namespace GroupLabel
            {
                inline constexpr ColourElement kText = {
                    "GroupLabelText",
                    ColourChart::kLightGrey2,
                    ColourChart::kLightGrey2
                };

                inline constexpr ColourElement kLine = {
                    "GroupLabelLine",
                    ColourChart::kDarkGrey5,
                    ColourChart::kDarkGrey5
                };
            }

            namespace Label
            {
                inline constexpr ColourElement kText = {
                    "LabelText",
                    ColourChart::kLightGrey2,
                    ColourChart::kDarkGrey1
                };
            }

            namespace VerticalSeparator
            {
                inline constexpr ColourElement kLine = {
                    "VerticalSeparatorLine",
                    ColourChart::kDarkGrey5,
                    ColourChart::kDarkGrey5
                };
            }

            namespace HorizontalSeparator
            {
                inline constexpr ColourElement kLine = {
                    "HorizontalSeparatorLine",
                    ColourChart::kDarkGrey5,
                    ColourChart::kDarkGrey5
                };
            }

            namespace Button
            {
                inline constexpr ColourElement kBackground = {
                    "ButtonBackground",
                    ColourChart::kBlack,
                    ColourChart::kLightGrey2
                };

                inline constexpr ColourElement kBorder = {
                    "ButtonBorder",
                    ColourChart::kDarkGrey3,
                    ColourChart::kDarkGrey4
                };

                inline constexpr ColourElement kText = {
                    "ButtonText",
                    ColourChart::kLightGrey2,
                    ColourChart::kDarkGrey1,
                };

                inline constexpr ColourElement kBackgroundHoover = {
                    "ButtonBackgroundHoover",
                    ColourChart::kBlack,
                    ColourChart::kLightGrey2
                };

                inline constexpr ColourElement kTextHoover = {
                    "ButtonTextHoover",
                    ColourChart::kWhite,
                    ColourChart::kDarkGrey1,
                };

                inline constexpr ColourElement kBackgroundClicked = {
                    "ButtonBackgroundClicked",
                    ColourChart::kBlack,
                    ColourChart::kLightGrey2
                };

                inline constexpr ColourElement kTextClicked = {
                    "ButtonTextClicked",
                    ColourChart::kRed,
                    ColourChart::kRed,
                };

                inline constexpr ColourElement kBackgroundDisabled = {
                    "ButtonBackgroundDisabled",
                    Common::kBackgroundDisabled,
                    Common::kBackgroundDisabled,
                };

                inline constexpr ColourElement kBorderDisabled = {
                    "ButtonBorderDisabled",
                    Common::kBorderDisabled,
                    Common::kBorderDisabled,
                };

                inline constexpr ColourElement kTextDisabled = {
                    "ButtonTextDisabled",
                    Common::kContentDisabled,
                    Common::kContentDisabled,
                };
            }

            namespace Toggle
            {
                inline constexpr ColourElement kBorder = {
                    "ToggleBorder",
                    ColourChart::kDarkGrey3,
                    ColourChart::kDarkGrey4
                };

                inline constexpr ColourElement kBackgroundOff = {
                    "ToggleBackgroundOff",
                    ColourChart::kGreen1,
                    ColourChart::kGreen1
                };

                inline constexpr ColourElement kTextOff = {
                    "ToggleTextOff",
                    ColourChart::kGreen3,
                    ColourChart::kGreen3
                };

                inline constexpr ColourElement kBackgroundOn = {
                    "ToggleBackgroundOn",
                    ColourChart::kGreen2,
                    ColourChart::kGreen2
                };

                inline constexpr ColourElement kTextOn = {
                    "ToggleTextOn",
                    ColourChart::kGreen4,
                    ColourChart::kGreen4
                };
            }

            namespace Slider
            {
                inline constexpr ColourElement kTrack = {
                    "SliderTrack",
                    ColourChart::kGreen1,
                    ColourChart::kGreen1
                };

                inline constexpr ColourElement kValueBar = {
                    "SliderValueBar",
                    ColourChart::kGreen3,
                    ColourChart::kGreen3
                };

                inline constexpr ColourElement kText = {
                    "SliderText",
                    ColourChart::kGreen4,
                    ColourChart::kGreen4,
                };

                inline constexpr ColourElement kFocusBorder = {
                    "SliderFocusBorder",
                    ColourChart::kGreen2,
                    ColourChart::kGreen2
                };

                inline constexpr ColourElement kTrackDisabled = {
                    "SliderTrackDisabled",
                    Common::kDecorationDisabled,
                    Common::kDecorationDisabled
                };

                inline constexpr ColourElement kValueBarDisabled = {
                    "SliderValueBarDisabled",
                    Common::kDecorationDisabled,
                    Common::kDecorationDisabled
                };

                inline constexpr ColourElement kTextDisabled = {
                    "SliderTextDisabled",
                    Common::kContentDisabled,
                    Common::kContentDisabled,
                };
            }

            namespace ComboBox
            {
                namespace Standard
                {
                    inline constexpr ColourElement kBackground = {
                        "ComboBoxStandardBackground",
                        ColourChart::kGreen1,
                        ColourChart::kGreen1
                    };

                    inline constexpr ColourElement kTriangle = {
                        "ComboBoxStandardTriangle",
                        ColourChart::kGreen3,
                        ColourChart::kGreen3
                    };

                    inline constexpr ColourElement kText = {
                        "ComboBoxStandardText",
                        ColourChart::kGreen4,
                        ColourChart::kGreen4,
                    };

                    inline constexpr ColourElement kFocusBorder = {
                        "ComboBoxStandardFocusBorder",
                        ColourChart::kGreen2,
                        ColourChart::kGreen2
                    };

                    inline constexpr ColourElement kBackgroundDisabled = {
                        "ComboBoxStandardBackgroundDisabled",
                        Common::kBackgroundDisabled,
                        Common::kBackgroundDisabled
                    };

                    inline constexpr ColourElement kTriangleDisabled = {
                        "ComboBoxStandardTriangleDisabled",
                        Common::kDecorationDisabled,
                        Common::kDecorationDisabled
                    };

                    inline constexpr ColourElement kTextDisabled = {
                        "ComboBoxStandardTextDisabled",
                        Common::kContentDisabled,
                        Common::kContentDisabled,
                    };
                }

                namespace ButtonLike
                {
                    inline constexpr ColourElement kBackground = {
                        "ComboBoxButtonLikeBackground",
                        ColourChart::kBlack,
                        ColourChart::kLightGrey2
                    };

                    inline constexpr ColourElement kBorder = {
                        "ComboBoxButtonLikeBorder",
                        ColourChart::kDarkGrey3,
                        ColourChart::kDarkGrey4
                    };

                    inline constexpr ColourElement kText = {
                        "ComboBoxButtonLikeText",
                        ColourChart::kLightGrey2,
                        ColourChart::kDarkGrey1,
                    };

                    inline constexpr ColourElement kTriangle = {
                        "ComboBoxButtonLikeTriangle",
                        ColourChart::kDarkGrey5,
                        ColourChart::kDarkGrey1
                    };

                    inline constexpr ColourElement kBackgroundDisabled = {
                        "ComboBoxButtonLikeBackgroundDisabled",
                        Common::kBackgroundDisabled,
                        Common::kBackgroundDisabled,
                    };

                    inline constexpr ColourElement kBorderDisabled = {
                        "ComboBoxButtonLikeBorderDisabled",
                        Common::kBorderDisabled,
                        Common::kBorderDisabled,
                    };

                    inline constexpr ColourElement kTextDisabled = {
                        "ComboBoxButtonLikeTextDisabled",
                        Common::kContentDisabled,
                        Common::kContentDisabled,
                    };

                    inline constexpr ColourElement kTriangleDisabled = {
                        "ComboBoxButtonLikeTriangleDisabled",
                        Common::kContentDisabled,
                        Common::kContentDisabled
                    };
                }
            }

            namespace PopupMenu
            {
                namespace Standard
                {
                    inline constexpr ColourElement kBackground = {
                        "PopupMenuStandardBackground",
                        ColourChart::kGreen1,
                        ColourChart::kGreen1
                    };

                    inline constexpr ColourElement kBorder = {
                        "PopupMenuStandardBorder",
                        ColourChart::kGreen4,
                        ColourChart::kGreen4
                    };

                    inline constexpr ColourElement kSeparator = {
                        "PopupMenuStandardSeparator",
                        ColourChart::kGreen4,
                        ColourChart::kGreen4
                    };

                    inline constexpr ColourElement kText = {
                        "PopupMenuStandardText",
                        ColourChart::kGreen4,
                        ColourChart::kGreen4,
                    };

                    inline constexpr ColourElement kBackgroundHoover = {
                        "PopupMenuStandardBackgroundHoover",
                        ColourChart::kGreen4,
                        ColourChart::kGreen4,
                    };

                    inline constexpr ColourElement kTextHoover = {
                        "PopupMenuStandardTextHoover",
                        Common::kContentHoover,
                        Common::kContentHoover,
                    };
                }

                namespace ButtonLike
                {
                    inline constexpr ColourElement kBackground = {
                        "PopupMenuButtonLikeBackground",
                        ColourChart::kBlack,
                        ColourChart::kLightGrey2
                    };

                    inline constexpr ColourElement kBorder = {
                        "PopupMenuButtonLikeBorder",
                        ColourChart::kDarkGrey5,
                        ColourChart::kDarkGrey4
                    };

                    inline constexpr ColourElement kSeparator = {
                        "PopupMenuButtonLikeSeparator",
                        ColourChart::kDarkGrey3,
                        ColourChart::kDarkGrey4
                    };

                    inline constexpr ColourElement kText = {
                        "PopupMenuButtonLikeText",
                        ColourChart::kLightGrey2,
                        ColourChart::kDarkGrey1,
                    };

                    inline constexpr ColourElement kBackgroundHoover = {
                        "PopupMenuButtonLikeBackgroundHoover",
                        ColourChart::kLightGrey2,
                        ColourChart::kLightGrey2
                    };

                    inline constexpr ColourElement kTextHoover = {
                        "PopupMenuButtonLikeTextHoover",
                        ColourChart::kBlack,
                        ColourChart::kDarkGrey1,
                    };

                    inline constexpr ColourElement kScrollbar = {
                        "PopupMenuButtonLikeScrollbar",
                        ColourChart::kDarkGrey3,
                        ColourChart::kDarkGrey3
                    };
                }
            }

            namespace NumberBox
            {
                inline constexpr ColourElement kText = {
                    "NumberBoxText",
                    ColourChart::kRed,
                    ColourChart::kRed,
                };

                inline constexpr ColourElement kDot = {
                    "NumberBoxDot",
                    ColourChart::kRed,
                    ColourChart::kRed,
                };

                inline constexpr ColourElement kEditorBackground = {
                    "NumberBoxEditorBackground",
                    ColourChart::kRed,
                    ColourChart::kRed,
                };

                inline constexpr ColourElement kEditorSelectionBackground = {
                    "NumberBoxEditorSelectionBackground",
                    ColourChart::kWhite,
                    ColourChart::kWhite,
                };

                inline constexpr ColourElement kEditorText = {
                    "NumberBoxEditorText",
                    ColourChart::kBlack,
                    ColourChart::kBlack,
                };
            }

            namespace EnvelopeDisplay
            {
                inline constexpr ColourElement kBackground = {
                    "EnvelopeDisplayBackground",
                    ColourChart::kBlack,
                    ColourChart::kBlack
                };

                inline constexpr ColourElement kBorder = {
                    "EnvelopeDisplayBorder",
                    ColourChart::kDarkGrey3,
                    ColourChart::kDarkGrey4
                };

                inline constexpr ColourElement kEnvelope = {
                    "EnvelopeDisplayEnvelope",
                    ColourChart::kGreen4,
                    ColourChart::kGreen4,
                };
            }

            namespace PatchNameDisplay
            {
                inline constexpr ColourElement kBackground = {
                    "PatchNameDisplayBackground",
                    ColourChart::kBlack,
                    ColourChart::kBlack
                };

                inline constexpr ColourElement kBorder = {
                    "PatchNameDisplayBorder",
                    ColourChart::kDarkGrey3,
                    ColourChart::kDarkGrey4
                };

                inline constexpr ColourElement kText = {
                    "PatchNameDisplayText",
                    ColourChart::kRed,
                    ColourChart::kRed,
                };
            }

            namespace TrackGeneratorDisplay
            {
                inline constexpr ColourElement kBackground = {
                    "TrackGeneratorDisplayBackground",
                    ColourChart::kBlack,
                    ColourChart::kBlack
                };

                inline constexpr ColourElement kBorder = {
                    "TrackGeneratorDisplayBorder",
                    ColourChart::kDarkGrey3,
                    ColourChart::kDarkGrey4
                };

                inline constexpr ColourElement kShaper = {
                    "TrackGeneratorDisplayShaper",
                    ColourChart::kGreen4,
                    ColourChart::kGreen4,
                };
            }
        }
    }
}
