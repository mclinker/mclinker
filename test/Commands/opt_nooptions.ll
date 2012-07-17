; RUN: %MCLinker; MCLD_RESULT=$?; \
; RUN: %AloneLinker; ALONE_RESULT=$?; \
; RUN: test ${MCLD_RESULT} -eq 1
; RUN: test ${ALONE_RESULT} -eq 1
