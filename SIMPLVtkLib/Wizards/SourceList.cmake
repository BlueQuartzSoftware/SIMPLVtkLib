set(WIZARDS_SOURCE_DIR "${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Wizards")

set(${PROJECT_NAME}_Wizards_HDRS
  ${WIZARDS_SOURCE_DIR}/AbstractMontageWizard.h
)

set(${PROJECT_NAME}_Wizards_SRCS
  ${WIZARDS_SOURCE_DIR}/AbstractMontageWizard.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/Wizards" "${${PROJECT_NAME}_Wizards_HDRS}" "${${PROJECT_NAME}_Wizards_SRCS}" "0")

  # -- Include the Execute Pipeline sources
include(${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Wizards/ExecutePipeline/SourceList.cmake)

  # -- Include the Execute Pipeline sources
include(${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Wizards/PerformMontage/SourceList.cmake)