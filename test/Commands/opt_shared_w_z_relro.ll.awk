#!/usr/bin/awk -f

{
  if ($1 == "There" && $2 == "are" && $5 == "headers,") {
    if ($4 == "section")
      SECTION_NUM = $3;
    else if ($4 == "program")
      SEGMENT_NUM = $3;
  }

  if ($2 == "Headers:") {
    if ($1 == "Section")
      SECTION_START_NL = NR + 2;
    else if ($1 == "Program")
      SEGMENT_START_NL = NR + 2;
  } else if ($1 == "Section" && $2 == "to" && \
             $3 == "Segment" && $4 == "mapping:") {
    MAPPING_START_NL = NR + 2;
  }

# -----  read section headers  -----
  if (SECTION_START_NL != 0 && \
      NR >= SECTION_START_NL && \
      NR < SECTION_START_NL + SECTION_NUM) {
    SECTION_IDX = NR - SECTION_START_NL;
    if (SECTION_IDX < 10) {
      section[SECTION_IDX, 0] = $3;
      section[SECTION_IDX, 1] = hexstr_to_decnum($5);
    } else {
      section[SECTION_IDX, 0] = $2;
      section[SECTION_IDX, 1] = hexstr_to_decnum($4);
    }
# -----  read program headers  -----
  } else if (SEGMENT_START_NL != 0 && \
             NR >= SEGMENT_START_NL && \
             NR < SEGMENT_START_NL + SEGMENT_NUM) {
    SEGMENT_IDX = NR - SEGMENT_START_NL;
    segment[SEGMENT_IDX, 0] = $1;
    segment[SEGMENT_IDX, 1] = hexstr_to_decnum($3);
    segment[SEGMENT_IDX, 2] = hexstr_to_decnum($6);
    segment[SEGMENT_IDX, 3] = hexstr_to_decnum($NF);
# -----  read section to segment mapping  -----
  } else if (MAPPING_START_NL != 0 && \
             NR >= MAPPING_START_NL && \
             NR < MAPPING_START_NL + SEGMENT_NUM) {
    MAPPING_IDX = NR - MAPPING_START_NL;
    mapping[MAPPING_IDX, 0] = $2;
    mapping[MAPPING_IDX, 1] = $NF;
  }
}

END {
# -----  1. check if there is one PT_GNU_RELRO  -----
  for (i = 0; i < SEGMENT_NUM; i++) {
    if (segment[i, 0] == "GNU_RELRO") {
      RELRO_SEG_IDX = i;
      break;
    }
  }
  if (i == SEGMENT_NUM)
    exit -1;

# -----  2. check if there is one PT_LOAD that covers the PT_GNU_RELRO  -----
  for (i = 0; i < SEGMENT_NUM; i++) {
    if (segment[i, 0] == "LOAD") {
      if (segment[i, 1] >= segment[RELRO_SEG_IDX, 1] && \
          segment[i, 1] + segment[i, 2] >= \
            segment[RELRO_SEG_IDX, 1] + segment[RELRO_SEG_IDX, 2] ) {
        LOAD_ALIGN = segment[i, 3];
        break;
      }
    }
  }
  if (i == SEGMENT_NUM)
    exit -2;

# -----  3. check if the PT_GNU_RELRO ends on a common page boundary  -----
  LAST_RELRO_SECTION = mapping[RELRO_SEG_IDX, 1];
  for (i = 0; i < SECTION_NUM; i++) {
    if (section[i, 0] == LAST_RELRO_SECTION) {
      FIRST_NON_RELRO_SECTION_ADDR = section[i + 1, 1];
      break;
    }
  }
  if (FIRST_NON_RELRO_SECTION_ADDR % LOAD_ALIGN != 0)
    exit -3;
}

# -----  function that converts a hex string to a dec number  -----
function hexstr_to_decnum(str)
{
  str = tolower(str);
  if (substr(str, 1, 2) == "0x")
    str = substr(str, 3);
  n = length(str);
  num = 0;
  for (i = 1; i <= n; i++) {
    c = substr(str, i, 1);
    if ((k = index("0123456789", c)) > 0)
      k--;
    else if ((k = index("abcdef", c)) > 0)
      k += 9;
    else
      exit -4;
    num = num * 16 + k;
  }
  return num;
}

