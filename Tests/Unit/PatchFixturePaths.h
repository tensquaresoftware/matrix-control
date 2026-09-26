#pragma once

#include <juce_core/juce_core.h>

namespace PatchTestFixtures
{
    inline juce::File fixturesRoot()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR);
    }

    inline juce::File matrixControlRoot()
    {
        return fixturesRoot().getChildFile("Matrix-Control");
    }

    inline juce::File matrix1000EditorRoot()
    {
        return fixturesRoot().getChildFile("Matrix-1000 Editor");
    }

    inline juce::File initFixturesDir()
    {
        return matrixControlRoot().getChildFile("Init");
    }

    inline juce::File matrixControlMastersDir()
    {
        return matrixControlRoot().getChildFile("Masters");
    }

    inline juce::File matrix1000EditorMastersDir()
    {
        return matrix1000EditorRoot().getChildFile("Masters");
    }

    inline juce::File patchFixturesRoot()
    {
        return matrixControlRoot().getChildFile("Patches");
    }

    // Resolves unit-test reference patches under Patches/UnitTests/, then legacy User/ and root.
    inline juce::File resolvePatchFixtureFile(const juce::String& fileName)
    {
        const auto root = patchFixturesRoot();
        const auto unitTestsFile = root.getChildFile("UnitTests").getChildFile(fileName);
        if (unitTestsFile.existsAsFile())
            return unitTestsFile;

        const auto userFile = root.getChildFile("User").getChildFile(fileName);
        if (userFile.existsAsFile())
            return userFile;

        const auto legacy = root.getChildFile(fileName);
        if (legacy.existsAsFile())
            return legacy;

        return unitTestsFile;
    }

    // Resolves a path under Tests/Fixtures/. Accepts Matrix-Control/... paths and legacy
    // Init|Patches|Masters prefixes (remapped under Matrix-Control/). ROM/ remaps to Factory/.
    inline juce::File resolveFixtureFile(const juce::String& relativePath)
    {
        const auto root = fixturesRoot();
        const auto direct = root.getChildFile(relativePath);
        if (direct.existsAsFile())
            return direct;

        const auto underMatrixControl = matrixControlRoot().getChildFile(relativePath);
        if (underMatrixControl.existsAsFile())
            return underMatrixControl;

        auto remapped = relativePath;
        if (remapped.contains("Patches/ROM/"))
            remapped = remapped.replace("Patches/ROM/", "Patches/Factory/");

        if (remapped != relativePath)
        {
            const auto remappedDirect = root.getChildFile(remapped);
            if (remappedDirect.existsAsFile())
                return remappedDirect;

            const auto remappedMc = matrixControlRoot().getChildFile(remapped);
            if (remappedMc.existsAsFile())
                return remappedMc;
        }

        if (relativePath.startsWith("Init/")
            || relativePath.startsWith("Patches/")
            || relativePath.startsWith("Masters/"))
            return underMatrixControl;

        return direct;
    }
}
