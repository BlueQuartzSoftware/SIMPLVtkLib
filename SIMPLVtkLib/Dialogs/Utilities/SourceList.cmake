# --------------------------------------------------------------------
# Any Class that inherits from QObject, either directly or through the heirarchy needs to have its header listed here
set(${PROJECT_NAME}_Dialogs_Utilities_Moc_HDRS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Dialogs/Utilities/DREAM3DFileItemDelegate.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Dialogs/Utilities/DREAM3DFileTreeModel.h
  )
# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
set_source_files_properties( ${${PROJECT_NAME}_Dialogs_Utilities_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${${PROJECT_NAME}_Dialogs_Utilities_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

set(${PROJECT_NAME}_Dialogs_Utilities_HDRS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Dialogs/Utilities/DREAM3DFileItem.h
)

set(${PROJECT_NAME}_Dialogs_Utilities_SRCS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Dialogs/Utilities/DREAM3DFileItem.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Dialogs/Utilities/DREAM3DFileItemDelegate.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Dialogs/Utilities/DREAM3DFileTreeModel.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "Utilities" "${${PROJECT_NAME}_Dialogs_Utilities_HDRS};${${PROJECT_NAME}_Dialogs_Utilities_Moc_HDRS}" "${${PROJECT_NAME}_Dialogs_Utilities_SRCS}" "${PROJECT_INSTALL_HEADERS}")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Utilities" "" "${${PROJECT_NAME}_Dialogs_Utilities_Generated_MOC_SRCS}" "0")

#-- Put the generated files into their own group for IDEs
cmp_IDE_GENERATED_PROPERTIES("Generated/moc" "${${PROJECT_NAME}_Dialogs_Utilities_Generated_MOC_SRCS}" "")

set(${PROJECT_NAME}_Dialogs_Utilities_HDRS
  ${${PROJECT_NAME}_Dialogs_Utilities_HDRS}
  ${${PROJECT_NAME}_Dialogs_Utilities_Moc_HDRS}  # Add the headers that get Moc'ed here so they show up in solutions/IDEs/Project files
)

set(${PROJECT_NAME}_Dialogs_Utilities_SRCS
  ${${PROJECT_NAME}_Dialogs_Utilities_SRCS}
  ${${PROJECT_NAME}_Dialogs_Utilities_Generated_MOC_SRCS}
)

