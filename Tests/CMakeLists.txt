# Reserved for a future Matrix-Control_UnitTests console target (juce_add_console_app).
# Do not add_subdirectory(Tests) from the root CMakeLists — that creates Builds/.../Tests/
# and must not link Tests/Unit/*.cpp into AU/VST3 (static juce::UnitTest registrars crash hosts).
