project(readedid)

set(CMAKE_OSX_SYSROOT macosx10.8)
set(CMAKE_OSX_ARCHITECTURES $(ARCHS_STANDARD_64_BIT))
set(CMAKE_C_FLAGS "-x objective-c")
set(CMAKE_CXX_FLAGS "-x objective-c++")
set(CMAKE_EXE_LINKER_FLAGS
    "-framework IOKit -framework Foundation -framework CoreGraphics"
)

cmake_minimum_required(VERSION 2.8.6)

set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.richardli.readEDID")
set(APP_TYPE MACOSX_BUNDLE)
set(CONFIGURATION_BUILD_DIR $(BUILD_DIR)/$(EFFECTIVE_PLATFORM_NAME))
add_subdirectory(readEDID)
