project(SIMPLVtkLib VERSION 2.0)


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

include(${CMP_SOURCE_DIR}/cmpProject.cmake)

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
)

if(UNIX AND NOT APPLE)
    set_source_files_properties(${Project_SRCS} PROPERTIES COMPILE_FLAGS -fPIC)
endif()

# --------------------------------------------------------------------
# Look for Vtk 8.0 as we need it for the plugin GUI to be generated
# These are the required component libraries
set(VtkComponents
    vtkGUISupportQt
    vtkRenderingCore
    vtkRenderingFreeType
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
  )

include("${CMP_SOURCE_DIR}/ExtLib/VtkSupport.cmake")
CMP_FindVtkComponents(QT5_REQUIRED TRUE
                      COMPONENTS ${VtkComponents}
                      )

# Add in all the dependent link libraries
set(SIMPLVtkLib_LINK_LIBRARIES
  Qt5::Core
  SIMPLib
  Qt5::Gui
  Qt5::Widgets
  ${VtkComponents}
  ${SIMPLVtkLib_Extra_Link_Libraries}
)

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
)
if(WIN32 AND BUILD_SHARED_LIBS)
    target_compile_definitions(SIMPLVtkLib PUBLIC "-DSIMPLVtkLib_BUILT_AS_DYNAMIC_LIB")
endif()
target_compile_definitions(SIMPLVtkLib PRIVATE -DQT_CORE_LIB)

# LibraryProperties( SIMPLVtkLib ${EXE_DEBUG_EXTENSION})
target_link_libraries(SIMPLVtkLib ${SIMPLVtkLib_LINK_LIBRARIES} )

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

