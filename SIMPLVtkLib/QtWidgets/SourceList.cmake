
set(${PROJECT_NAME}_QtWidgets_HDRS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSColorButton.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSFilterView.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSInfoWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSMainWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSMainWidgetBase.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSTransformWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSViewWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSVisualizationWidget.h
)

set(${PROJECT_NAME}_QtWidgets_SRCS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSColorButton.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSFilterView.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSInfoWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSMainWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSMainWidgetBase.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSTransformWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSViewWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSVisualizationWidget.cpp
)

set(${PROJECT_NAME}_QtWidgets_UIS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSInfoWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSMainWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSTransformWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files/VSViewWidget.ui
)

cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/QtWidgets" "${${PROJECT_NAME}_QtWidgets_HDRS}" "${${PROJECT_NAME}_QtWidgets_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${PROJECT_NAME}/QtWidgets/UI_Files" "${${PROJECT_NAME}_QtWidgets_UIS}" "")

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PROJECT_NAME}_QtWidgets_Generated_UI_HDRS ${${PROJECT_NAME}_QtWidgets_UIS} )

cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PROJECT_NAME}_QtWidgets_Generated_UI_HDRS}" "" "0")

include_directories(${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/UI_Files)