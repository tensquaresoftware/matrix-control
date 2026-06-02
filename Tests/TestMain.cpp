#include <juce_core/juce_core.h>

// Console entry point for the Matrix-Control unit test runner. Every UnitTest
// subclass registers itself statically, so running all tests requires no manual
// registration here. Returns a non-zero exit code when any test fails so the
// runner can gate CI / local builds.
int main()
{
    juce::UnitTestRunner runner;
    runner.setAssertOnFailure(false);
    runner.runAllTests();

    int failures = 0;
    for (int i = 0; i < runner.getNumResults(); ++i)
        failures += runner.getResult(i)->failures;

    return failures == 0 ? 0 : 1;
}
