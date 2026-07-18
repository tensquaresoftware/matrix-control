#pragma once

#include <juce_core/juce_core.h>

namespace Core
{
    /** Pure predicate for Device Inquiry debounce after MIDI port sync.
        Returns true when both ports are available and the open pair differs from the
        last pair for which an inquiry was started — avoids re-spamming inquiry on
        every repeated sync of the same open ports. */
    inline bool shouldStartDeviceInquiry(bool bothPortsAvailable,
                                         const juce::String& openInputId,
                                         const juce::String& openOutputId,
                                         const juce::String& lastInquiryInputId,
                                         const juce::String& lastInquiryOutputId) noexcept
    {
        if (! bothPortsAvailable)
            return false;

        if (openInputId.isEmpty() || openOutputId.isEmpty())
            return false;

        return openInputId != lastInquiryInputId || openOutputId != lastInquiryOutputId;
    }
}
