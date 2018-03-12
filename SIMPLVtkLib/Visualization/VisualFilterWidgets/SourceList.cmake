
set(VSVisualFilterWidgets
  VSClipFilterWidget
  VSCropFilterWidget
  VSDataSetFilterWidget
  VSMaskFilterWidget
  VSSIMPLDataContainerFilterWidget
  VSSliceFilterWidget
  VSThresholdFilterWidget
)

set(VSVisualFilterWidgets_NoUi
  VSAbstractFilterWidget
)

# --------------------------------------------------------------------
# Loop through Visualization Filters with UI Files
# --------------------------------------------------------------------
foreach(VisFilterWidget ${VSVisualFilterWidgets})
  set(VS_VisualFilterWidgets_SRCS 
    ${VS_VisualFilterWidgets_SRCS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilterWidgets/${VisFilterWidget}.cpp
  )

  set(VS_VisualFilterWidgets_HDRS 
    ${VS_VisualFilterWidgets_HDRS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilterWidgets/${VisFilterWidget}.h
  )

  set(VS_VisualFilterWidgets_UIS 
    ${VS_VisualFilterWidgets_UIS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilterWidgets/UI_Files/${VisFilterWidget}.ui
  )
endforeach(VisFilterWidget)

# --------------------------------------------------------------------
# Loop through Visualization Filters without UI Files
# --------------------------------------------------------------------
foreach(VisFilterWidget ${VSVisualFilterWidgets_NoUi})
  set(VS_VisualFilterWidgets_SRCS 
    ${VS_VisualFilterWidgets_SRCS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilterWidgets/${VisFilterWidget}.cpp
  )

  set(VS_VisualFilterWidgets_HDRS 
    ${VS_VisualFilterWidgets_HDRS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilterWidgets/${VisFilterWidget}.h
  )
endforeach(VisFilterWidget)


cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/VisualFilterWidgets" "${VS_VisualFilterWidgets_HDRS}" "${VS_VisualFilterWidgets_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${PROJECT_NAME}/VisualFilterWidgets/UI_Files" "${VS_VisualFilterWidgets_UIS}" "")

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PROJECT_NAME}_VisualFilterWidgets_Generated_UI_HDRS ${VS_VisualFilterWidgets_UIS} )
foreach(h ${${PROJECT_NAME}_VisualFilterWidgets_Generated_UI_HDRS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PROJECT_NAME}_VisualFilterWidgets_Generated_UI_HDRS}" "" "0")

include_directories(${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VisualFilterWidgets/UI_Files)
