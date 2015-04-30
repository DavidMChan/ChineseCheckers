include_directories(${PROJECT_SOURCE_DIR}/include/gflags)

set (PUBLIC_HDRS
  "gflags.h"
  "gflags_declare.h"
  "gflags_completions.h"
)


if (GFLAGS_NAMESPACE_SECONDARY)
  set (INCLUDE_GFLAGS_NS_H "// Import gflags library symbols into alternative/deprecated namespace(s)")
  foreach (ns IN LISTS GFLAGS_NAMESPACE_SECONDARY)
    string (TOUPPER "${ns}" NS)
    set (gflags_ns_h "${PROJECT_BINARY_DIR}/include/${GFLAGS_INCLUDE_DIR}/gflags_${ns}.h")
    configure_file ("${PROJECT_SOURCE_DIR}/src/gflags_ns.h.in" "${gflags_ns_h}" @ONLY)
    list (APPEND PUBLIC_HDRS "${gflags_ns_h}")
    set (INCLUDE_GFLAGS_NS_H "${INCLUDE_GFLAGS_NS_H}\n#include \"gflags_${ns}.h\"")
  endforeach ()
else ()
  set (INCLUDE_GFLAGS_NS_H)
endif ()

set (PRIVATE_HDRS
  "config.h"
  "util.h"
  "mutex.h"
)

set (GFLAGS_SRCS
  "gflags.cc"
  "gflags_reporting.cc"
  "gflags_completions.cc"
)

set_source_files_properties(${GFLAGS_SRCS} PROPERTIES COMPILE_FLAGS "-w -Wno-missing-variable-declarations -Wno-implicit-fallthrough")
set_source_files_properties(${PRIVATE_HDRS} PROPERTIES COMPILE_FLAGS "-w -Wno-missing-variable-declarations -Wno-implicit-fallthrough")
set_source_files_properties(${PUBLIC_HDRS} PROPERTIES COMPILE_FLAGS "-w -Wno-missing-variable-declarations -Wno-implicit-fallthrough")

add_library(gflags ${TYPE} ${GFLAGS_SRCS} ${PRIVATE_HDRS} ${PUBLIC_HDRS})
