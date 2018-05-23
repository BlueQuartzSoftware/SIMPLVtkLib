# Request C++11 standard, using new CMake variables.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

if(${CMAKE_MAJOR_VERSION} STREQUAL "3")
  # CMake 3.1 introduces if() policies on dereferencing variables in quotes
  cmake_policy(SET CMP0054 NEW)
endif()

cmake_minimum_required(VERSION 3.8.0)

#------------------------------------------------------------------------------
# Create the Top level Project
#
project(SIMPLVtkLib VERSION 1.0.0)
set(PROJECT_PREFIX "SIMPLVtkLib" CACHE STRING "The Prefix to be used for Preprocessor definitions")

# ---------- Setup output Directories -------------------------
if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${SIMPLVtkLibProj_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all Libraries"
    )
endif()

# --------- Setup the Executable output Directory -------------
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${SIMPLVtkLibProj_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all Executables."
    )
endif()

# --------- Setup the Executable output Directory -------------
if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${SIMPLVtkLibProj_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all static libraries."
    )
endif()

#------------------------------------------------------------------------------
# Find the Git Package for Versioning. It should be ok if Git is NOT found
# Find_package(Git)

# get_filename_component(SIMPLVtkLib_PARENT ${SIMPLVtkLibProj_SOURCE_DIR} DIRECTORY CACHE)

#------------------------------------------------------------------------------
# If the developer has set another CMP directory then use that, otherwise look
# for the CMP directory at the same level as the SIMPL directory
if("${CMP_SOURCE_DIR}" STREQUAL "")
  set(CMP_SOURCE_DIR ${SIMPLVtkLib_PARENT}/CMP)
  message(STATUS "CMP_SOURCE_DIR: ${CMP_SOURCE_DIR}")
endif()

if(NOT EXISTS "${CMP_SOURCE_DIR}")
  message(FATAL_ERROR "CMP_SOURCE_DIR does not exist at '${CMP_SOURCE_DIR}'")
endif()

#------------------------------------------------------------------------------
set(SIMPLVtkLib_BUILT_AS_DYNAMIC_LIB 0)
if(BUILD_SHARED_LIBS)
  set(SIMPLVtkLib_BUILT_AS_DYNAMIC_LIB 1)
endif(BUILD_SHARED_LIBS)

set(GENERATE_GIT_VERSION 1)

if(NOT PROJECT_RESOURCES_DIR)
  set(PROJECT_RESOURCES_DIR ${SIMPLVtkLib_SOURCE_DIR}/Resources)
endif()
if(NOT DEFINED CMP_SOURCE_DIR)
  set(CMP_SOURCE_DIR ${SIMPLVtkLib_SOURCE_DIR}/Support/cmp)
endif()

set(PROJECT_PREFIX "SIMPLVtkLib" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(SIMPLVtkLib_INSTALL_FILES 0)
set(PROJECT_INSTALL_HEADERS 0)
# -----------------------------------------------------------------------
# The project generally does NOT want to install headers. See if another
# project has over ridden that property
get_property(CMP_PROJECT_INSTALL_HEADERS GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS)
if("${CMP_PROJECT_INSTALL_HEADERS}" STREQUAL "")
  set_property(GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS "0")
else()
  set(SIMPLVtkLib_INSTALL_FILES "${CMP_PROJECT_INSTALL_HEADERS}")
endif()
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
set(CMP_HEADER_DIR ${SIMPLVtkLib_BINARY_DIR})
set(CMP_CONFIGURATION_FILE_NAME "SIMPLVtkLibConfiguration.h")
set(CMP_TYPES_FILE_NAME "SIMPLVtkLibTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "SIMPLVtkLibVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "SIMPLVtkLibVersion.cpp")
set(CMP_TOP_HEADER_FILE "SIMPLVtkLib.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_GENERATE_VERSION_STRING ${GENERATE_GIT_VERSION})

# Extra Variables that need to be set before all the configured files are generated.
set(SIMPLVtkLib_HDF5_SUPPORT 1)
option(SIMPLVtkLib_ENABLE_HDF5 "Enable the use of HDF5 functionality" ON)
mark_as_advanced(SIMPLVtkLib_ENABLE_HDF5)

# --------------------------------------------------------------------
# Find and Use the Qt5 Libraries
include(${CMP_SOURCE_DIR}/cmpProject.cmake)
include(${CMP_SOURCE_DIR}/ExtLib/Qt5Support.cmake)
set(SIMPLVtkLib_Qt5_Components Core Widgets Network Gui Concurrent Svg Xml OpenGL PrintSupport )
CMP_AddQt5Support( "${SIMPLVtkLib_Qt5_Components}"
                    "${SIMPL_USE_QtWebEngine}"
                    "${SIMPLProj_BINARY_DIR}"
                    "SIMPLVtkLib")

set(CMP_MODULES_SOURCE_DIR ${CMP_SOURCE_DIR}/Modules CACHE INTERNAL "")
set(CMAKE_MODULE_PATH ${CMP_MODULES_SOURCE_DIR} ${CMAKE_MODULE_PATH})

configure_file(${SIMPLVtkLib_SOURCE_DIR}/SIMPLVtkLibConfiguration.h.in
               ${SIMPLVtkLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE})

set(Project_SRCS "")

set(${PROJECT_NAME}_Generated_HDRS
        ${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME}
        ${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME}
        ${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}
        ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE}
)

set(${PROJECT_NAME}_Generated_SRCS
        ${CMP_HEADER_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
)

cmp_IDE_GENERATED_PROPERTIES("Generated" "${${PROJECT_NAME}_Generated_HDRS}" "${${PROJECT_NAME}_Generated_SRCS}")


# -- Run MOC and UIC on the necessary files
QT5_ADD_RESOURCES( SIMPLVtkLib_Generated_RC_SRCS "${SIMPLVtkLib_SOURCE_DIR}/icons/images/Icons.qrc" "${SIMPLVtkLib_SOURCE_DIR}/fonts/fonts.qrc" "${SIMPLVtkLib_SOURCE_DIR}/Resources/VtkSIMPL.qrc")
foreach(h ${SIMPLVtkLib_Generated_RC_SRCS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

include(${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/SourceList.cmake)
include(${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/SIMPLBridge/SourceList.cmake)
include(${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/SourceList.cmake)

  # -- Include the Dialogs sources
include(${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Dialogs/SourceList.cmake)

#------------------------------------------------------------------------------
# Clear these variables and look for any extra Widget Directories that the
# developer would like to add
set(SIMPLVtkLib_Extra_Sources)
set(SIMPLVtkLib_Extra_Headers)
set(SIMPLVtkLib_Extra_Link_Libraries)

# foreach(SVWidgetsExtraPath ${SVWidgetsExtraPaths})
#   include( ${SVWidgetsExtraPath}/SourceList.cmake )
# endforeach()

set(Project_SRCS

  ${${PROJECT_NAME}_Generated_HDRS}
  ${${PROJECT_NAME}_Generated_SRCS}
  ${SIMPLVtkLib_Generated_RC_SRCS}

  #---------------
  ${VS_SIMPLBridge_HDRS}
  ${VS_SIMPLBridge_SRCS}
  
  ${VS_VisualFilters_SRCS}
  ${VS_VisualFilters_HDRS}

  ${VS_VisualFilterWidgets_SRCS}
  ${VS_VisualFilterWidgets_HDRS}
  ${VS_VisualFilterWidgets_UIS}
  ${${PROJECT_NAME}_VisualFilterWidgets_Generated_UI_HDRS}

  ${${PROJECT_NAME}_VtkWidget_HDRS}
  ${${PROJECT_NAME}_VtkWidget_SRCS}
  ${${PROJECT_NAME}_VtkWidget_Generated_UI_HDRS}

  ${${PROJECT_NAME}_Visualization_Controllers_HDRS}
  ${${PROJECT_NAME}_Visualization_Controllers_SRCS}

  ${${PROJECT_NAME}_QtWidgets_HDRS}
  ${${PROJECT_NAME}_QtWidgets_SRCS}
  ${${PROJECT_NAME}_QtWidgets_Generated_UI_HDRS}

  ${${PROJECT_NAME}_Dialogs_Moc_HDRS}
  ${${PROJECT_NAME}_Dialogs_HDRS}
  ${${PROJECT_NAME}_Dialogs_SRCS}
  ${${PROJECT_NAME}_Dialogs_Generated_UI_HDRS}

  ${${PROJECT_NAME}_Dialogs_Utilities_Moc_HDRS}
  ${${PROJECT_NAME}_Dialogs_Utilities_HDRS}
  ${${PROJECT_NAME}_Dialogs_Utilities_SRCS}
)

if(UNIX AND NOT APPLE)
    set_source_files_properties(${Project_SRCS} PROPERTIES COMPILE_FLAGS -fPIC)
endif()

# --------------------------------------------------------------------
# Look for Vtk 8.0 as we need it for the plugin GUI to be generated
# These are the required component libraries
set(SIMPLVtk_VtkComponents
		vtkFiltersGeometry
    vtkFiltersModeling
    vtkGUISupportQt
    vtkRenderingCore
    vtkRenderingFreeType
    vtkRenderingImage
    vtkRenderingOpenGL2
    vtkRenderingVolumeOpenGL2
    vtkRenderingContextOpenGL2
    vtkRenderingAnnotation
    vtkInteractionWidgets
    vtkInteractionStyle
    vtkIOLegacy
    vtkIOImage
    vtkIOGeometry
    vtkChartsCore
    vtkViewsContext2D
    CACHE INTERNAL "VTK Components for SIMPLVtkLib" FORCE
  )

include("${CMP_SOURCE_DIR}/ExtLib/VtkSupport.cmake")
CMP_FindVtkComponents(QT5_REQUIRED TRUE
                      COMPONENTS ${SIMPLVtk_VtkComponents}
                      )

# Add in all the dependent link libraries
set(SIMPLVtkLib_LINK_LIBRARIES
  Qt5::Core
  SIMPLib
  SVWidgetsLib
  Qt5::Gui
  Qt5::Widgets
  ${SIMPLVtk_VtkComponents}
  ${SIMPLVtkLib_Extra_Link_Libraries}
)

get_filename_component(SIMPLVtkLib_BINARY_PARENT_DIR ${SIMPLVtkLib_BINARY_DIR} DIRECTORY)
add_library(SIMPLVtkLib ${LIB_TYPE} ${Project_SRCS})
CMP_AddDefinitions(TARGET SIMPLVtkLib)
target_include_directories(SIMPLVtkLib
                            PUBLIC
                              ${SIMPLProj_SOURCE_DIR}/Source
                              ${SIMPLProj_BINARY_DIR}
                              ${SIMPLVtkLib_BINARY_DIR}
                              ${SIMPLVtkLib_SOURCE_DIR}/SIMPLVtkLib
                              ${SIMPLVtkLib_SOURCE_DIR}/SIMPLVtkLib/QtWidgets
                              ${SIMPLVtkLib_SOURCE_DIR}/SIMPLVtkLib/SIMPLBridge
                              ${SIMPLVtkLib_SOURCE_DIR}/SIMPLVtkLib/Visualization
                              ${SIMPLVtkLib_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers
                              ${SIMPLVtkLib_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilters
                              ${SIMPLVtkLib_SOURCE_DIR}/SIMPLVtkLib/Visualization/VtkWidgets
                              ${SIMPLVtkLib_BINARY_PARENT_DIR})
if(WIN32 AND BUILD_SHARED_LIBS)
    target_compile_definitions(SIMPLVtkLib PUBLIC "-DSIMPLVtkLib_BUILT_AS_DYNAMIC_LIB")
endif()
target_compile_definitions(SIMPLVtkLib PRIVATE -DQT_CORE_LIB)

# LibraryProperties( SIMPLVtkLib ${EXE_DEBUG_EXTENSION})
target_link_libraries(SIMPLVtkLib ${SIMPLVtkLib_LINK_LIBRARIES} )

set_target_properties(SIMPLVtkLib
                          PROPERTIES FOLDER SIMPLVtkLib)

set(install_dir "tools")
set(lib_install_dir "lib")
if(WIN32)
    set(install_dir ".")
    set(lib_install_dir "")
endif()


if(BUILD_SHARED_LIBS AND NOT APPLE)
    INSTALL(TARGETS ${PROJECT_NAME}
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}
        LIBRARY DESTINATION ${lib_install_dir}
        ARCHIVE DESTINATION ${lib_install_dir}
        BUNDLE DESTINATION "."
    )
endif()

