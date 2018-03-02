
set(${PROJECT_NAME}_Visualization_Controllers_HDRS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSConcurrentImport.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSController.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSFilterModel.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSFilterViewSettings.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSLookupTableController.h
)

set(${PROJECT_NAME}_Visualization_Controllers_SRCS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSConcurrentImport.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSController.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSFilterModel.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSFilterViewSettings.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Visualization/Controllers/VSLookupTableController.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/Controllers" "${${PROJECT_NAME}_Visualization_Controllers_HDRS}" "${${PROJECT_NAME}_Visualization_Controllers_SRCS}" "0")