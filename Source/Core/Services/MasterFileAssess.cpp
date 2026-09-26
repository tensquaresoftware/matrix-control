#include "Core/Services/MasterFileAssess.h"

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/Services/MasterM1kmCodec.h"

namespace Core
{
    namespace MasterFileAssess
    {
        namespace
        {
            constexpr const char* kSyxExtension = ".syx";

            Assessment assessM1km(const juce::File& file)
            {
                Assessment assessment;
                assessment.format = Format::kM1km;

                if (file.getSize() != static_cast<juce::int64>(MasterM1kmCodec::kFileByteSize))
                    return assessment;

                juce::MemoryBlock data;
                if (! file.loadFileAsData(data) || ! MasterM1kmCodec::isValidContents(data))
                    return assessment;

                assessment.isValidMaster = true;
                return assessment;
            }

            Assessment assessSyx(const juce::File& file, SysExDecoder& decoder)
            {
                Assessment assessment;
                assessment.format = Format::kSyx;

                if (file.getSize() != static_cast<juce::int64>(SysExConstants::kMasterMessageLength))
                    return assessment;

                juce::MemoryBlock data;
                if (! file.loadFileAsData(data))
                    return assessment;

                juce::uint8 packed[SysExConstants::kMasterPackedDataSize] = {};
                if (! decoder.decodeMasterSysEx(data, packed))
                    return assessment;

                assessment.isValidMaster = true;
                return assessment;
            }
        }

        bool hasSyxExtension(const juce::File& file) noexcept
        {
            return file.getFileExtension().equalsIgnoreCase(kSyxExtension);
        }

        Assessment assess(const juce::File& file, SysExDecoder& decoder)
        {
            if (! file.existsAsFile())
                return {};

            if (MasterM1kmCodec::hasExtension(file))
                return assessM1km(file);

            if (hasSyxExtension(file))
                return assessSyx(file, decoder);

            return {};
        }
    }
} // namespace Core
