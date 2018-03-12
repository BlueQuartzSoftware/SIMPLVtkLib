
set(${PROJECT_NAME}_VtkWidget
  VSBoxWidget
  VSPlaneWidget
)

set(${PROJECT_NAME}_VtkWidget_NoUi
  VSAbstractWidget
)

# --------------------------------------------------------------------
# Loop through Visualization Filters with UI Files
# --------------------------------------------------------------------
foreach(VtkWidget ${${PROJECT_NAME}_VtkWidget})
  set(${PROJECT_NAME}_VtkWidget_SRCS 
    ${${PROJECT_NAME}_VtkWidget_SRCS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VtkWidgets/${VtkWidget}.cpp
  )

  set(${PROJECT_NAME}_VtkWidget_HDRS 
    ${${PROJECT_NAME}_VtkWidget_HDRS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VtkWidgets/${VtkWidget}.h
  )

  set(${PROJECT_NAME}_VtkWidget_UIS 
    ${${PROJECT_NAME}_VtkWidget_UIS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VtkWidgets/UI_Files/${VtkWidget}.ui
  )
endforeach(VtkWidget)

# --------------------------------------------------------------------
# Loop through Visualization Filters without UI Files
# --------------------------------------------------------------------
foreach(VtkWidget ${${PROJECT_NAME}_VtkWidget_NoUi})
  set(${PROJECT_NAME}_VtkWidget_SRCS 
    ${${PROJECT_NAME}_VtkWidget_SRCS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VtkWidgets/${VtkWidget}.cpp
  )

  set(${PROJECT_NAME}_VtkWidget_HDRS 
    ${${PROJECT_NAME}_VtkWidget_HDRS}
    ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VtkWidgets/${VtkWidget}.h
  )
endforeach(VtkWidget)


cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/VtkWidgets" "${${PROJECT_NAME}_VtkWidget_HDRS}" "${${PROJECT_NAME}_VtkWidget_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${PROJECT_NAME}/VtkWidgets/UI_Files" "${${PROJECT_NAME}_VtkWidget_UIS}" "")

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PROJECT_NAME}_VtkWidget_Generated_UI_HDRS ${${PROJECT_NAME}_VtkWidget_UIS} )
foreach(h ${${PROJECT_NAME}_VtkWidget_Generated_UI_HDRS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PROJECT_NAME}_VtkWidget_Generated_UI_HDRS}" "" "0")

include_directories(${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/VtkWidgets/UI_Files)
