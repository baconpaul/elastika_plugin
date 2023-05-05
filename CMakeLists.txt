cmake_minimum_required(VERSION 3.20)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11 CACHE STRING "Build for 10.11")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(elastika_plugin VERSION 0.9.0 LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

if (NOT ${ELASTIKA_COPY_PLUGIN_AFTER_BUILD})
  set(ELASTIKA_COPY_PLUGIN_AFTER_BUILD FALSE)
endif()
if (${ELASTIKA_COPY_PLUGIN_AFTER_BUILD})
  message(STATUS "Elastika Plugin will be copied after build")
else()
  message(STATUS "Elastika Plugin will require manual copy after build")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  # Any Clang or any GCC
  add_compile_options(
          -Wno-multichar
          -fvisibility=hidden
          $<$<AND:$<NOT:$<BOOL:${WIN32}>>,$<COMPILE_LANGUAGE:CXX>>:-fvisibility-inlines-hidden>
  )
endif()

set(ELASTIKA_DIR libs/sapphire/src)
add_library(elastika-dsp STATIC
        ${ELASTIKA_DIR}/mesh_hex.cpp
        ${ELASTIKA_DIR}/mesh_physics.cpp
        )
target_include_directories(elastika-dsp PUBLIC ${ELASTIKA_DIR} libs/simde)
target_compile_definitions(elastika-dsp PUBLIC NO_RACK_DEPENDENCY)
if (WIN32)
  target_compile_definitions(elastika-dsp PUBLIC _USE_MATH_DEFINES)
endif()

add_subdirectory(libs/JUCE)
add_subdirectory(libs/clap-juce-extensions)

file(GLOB ELASTIKA_RESOURCES_GLOB
    ${CMAKE_SOURCE_DIR}/libs/sapphire/res/*.svg
    ${CMAKE_SOURCE_DIR}/res/*.svg
    )

juce_add_binary_data(elastika-binary
    NAMESPACE ElastikaBinary
    HEADER_NAME ElastikaBinary.h
    SOURCES ${ELASTIKA_RESOURCES_GLOB}
    )

juce_add_plugin(elastika-filter
    PRODUCT_NAME "Elastika"
    COMPANY_NAME "Sapphire"
    BUNDLE_ID "org.cosinekitty.sapphire.elastika"
    PLUGIN_MANUFACTURER_CODE CSkT
    PLUGIN_CODE ElsK

    IS_SYNTH FALSE

    COPY_PLUGIN_AFTER_BUILD ${ELASTIKA_COPY_PLUGIN_AFTER_BUILD}

    FORMATS AU VST3 Standalone
)

clap_juce_extensions_plugin(TARGET elastika-filter
        CLAP_ID "org.cosinekitty.sapphire.elastika"
        CLAP_PROCESS_EVENTS_RESOLUTION_SAMPLES 32
        CLAP_FEATURES effect filter)



target_sources(elastika-filter PRIVATE
  src/sapphire_lnf.cc
  src/ElastikaProcessor.cpp
  src/ElastikaEditor.cpp
)

target_compile_definitions(elastika-filter PUBLIC
    JUCE_ALLOW_STATIC_NULL_VARIABLES=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1

    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0

    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0

    JUCE_ALSA=1
    JUCE_JACK=1
)

target_link_libraries(elastika-filter PRIVATE
    juce::juce_core
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    elastika-binary
    elastika-dsp
)
