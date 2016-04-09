set_source_files_properties(
    ParallelMovingAverage/ParallelMovingAverage_dfe.o PROPERTIES
    EXTERNAL_OBJECT TRUE  # Identifies this as an object file
    GENERATED TRUE  # Avoids need for file to exist at configure-time
)

set_source_files_properties(
    OptionPricing/OptionPricing_dfe.o PROPERTIES
    EXTERNAL_OBJECT TRUE  # Identifies this as an object file
    GENERATED TRUE  # Avoids need for file to exist at configure-time
)

add_library(Bitstreams
  ParallelMovingAverage/ParallelMovingAverage.cpp ParallelMovingAverage/ParallelMovingAverage_dfe.o
  OptionPricing/OptionPricing.cpp OptionPricing/OptionPricing_dfe.o)

target_link_libraries(Bitstreams maxeleros slic m pthread)
