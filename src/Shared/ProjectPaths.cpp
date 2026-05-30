#include "Shared/ProjectPaths.h"

namespace
{
    constexpr const char* kProjectNameToken { "project(Matrix-Control" };
    constexpr const char* kRootMarkerFileName { ".matrix-control-root" };
    constexpr const char* kCmakeListsFileName { "CMakeLists.txt" };
    constexpr const char* kEnvVarProjectRoot { "MATRIX_CONTROL_PROJECT_ROOT" };
    constexpr const char* kCompanyFolderName { "Ten Square Software" };
    constexpr const char* kProductFolderName { "Matrix-Control" };
    constexpr const char* kLogsFolderName { "logs" };
    constexpr const char* kMidiLogsFolderName { "midi" };
    constexpr const char* kApvtsLogsFolderName { "apvts" };

    struct RootCache
    {
        juce::File root;
        bool usedFallback { false };
        bool resolved { false };
    };

    RootCache& getRootCache()
    {
        static RootCache cache;
        return cache;
    }

    bool directoryContainsProjectMarker(const juce::File& directory)
    {
        if (!directory.isDirectory())
            return false;

        if (directory.getChildFile(kRootMarkerFileName).existsAsFile())
            return true;

        const juce::File cmakeFile = directory.getChildFile(kCmakeListsFileName);
        if (!cmakeFile.existsAsFile())
            return false;

        return cmakeFile.loadFileAsString().contains(kProjectNameToken);
    }

    juce::File findRootFromStartDirectory(const juce::File& startDirectory)
    {
        juce::File current = startDirectory;

        while (current != juce::File())
        {
            if (directoryContainsProjectMarker(current))
                return current;

            current = current.getParentDirectory();
        }

        return {};
    }

    juce::File getFallbackRoot()
    {
        return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
            .getChildFile(kCompanyFolderName)
            .getChildFile(kProductFolderName);
    }

    juce::File resolveRootFromEnvironment()
    {
        const juce::String envValue = juce::SystemStats::getEnvironmentVariable(kEnvVarProjectRoot, {});
        if (envValue.isEmpty())
            return {};

        const juce::File envRoot(envValue);
        if (directoryContainsProjectMarker(envRoot))
            return envRoot;

        return {};
    }

    juce::Array<juce::File> collectWalkUpRoots()
    {
        const juce::Array<juce::File> startPoints
        {
            juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory(),
            juce::File::getCurrentWorkingDirectory()
        };

        juce::Array<juce::File> discoveredRoots;

        for (const auto& startPoint : startPoints)
        {
            const juce::File foundRoot = findRootFromStartDirectory(startPoint);
            if (foundRoot.exists() && !discoveredRoots.contains(foundRoot))
                discoveredRoots.add(foundRoot);
        }

        return discoveredRoots;
    }

    juce::File pickPreferredWalkUpRoot(const juce::Array<juce::File>& discoveredRoots)
    {
        if (discoveredRoots.isEmpty())
            return {};

        const juce::File executableFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile);

        for (const auto& root : discoveredRoots)
        {
            if (executableFile.isAChildOf(root))
                return root;
        }

        return discoveredRoots.getReference(0);
    }

    void resolveProjectRootIfNeeded()
    {
        auto& cache = getRootCache();
        if (cache.resolved)
            return;

        cache.resolved = true;

        const juce::File walkUpRoot = pickPreferredWalkUpRoot(collectWalkUpRoots());
        if (walkUpRoot.exists())
        {
            cache.root = walkUpRoot;
            return;
        }

        const juce::File envRoot = resolveRootFromEnvironment();
        if (envRoot.exists())
        {
            cache.root = envRoot;
            return;
        }

        cache.root = getFallbackRoot();
        cache.usedFallback = true;
    }
}

juce::File ProjectPaths::getProjectRoot()
{
    resolveProjectRootIfNeeded();
    return getRootCache().root;
}

juce::File ProjectPaths::getLogsDirectory(LogCategory category)
{
    const juce::String subfolder = category == LogCategory::kMidi
        ? kMidiLogsFolderName
        : kApvtsLogsFolderName;

    return getProjectRoot()
        .getChildFile(kLogsFolderName)
        .getChildFile(subfolder);
}

bool ProjectPaths::isUsingFallbackRoot()
{
    resolveProjectRootIfNeeded();
    return getRootCache().usedFallback;
}

juce::String ProjectPaths::getFallbackRootWarning()
{
    return "Project root not found — logs written to user data folder: "
        + getProjectRoot().getFullPathName();
}
