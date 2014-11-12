Exidx Coverage Design Document
==============================


Overall Algorithm
-----------------

1. Before the section merge

   1. Collect all region fragments of .ARM.exidx and .ARM.extab and build the
      mapping data structure (including the corresponding .text, .rel.ARM.exidx,
      and .rel.ARM.extab sections.)

   2. Sort the .rel.ARM.exidx entries by the offset of the fixup.

   3. Check that .ARM.exidx is well-formed.
      1. The size of region fragment is multiple of 8.
      2. The odd words are relocations to the corresponding text section.
      3. The even words are relocations to the corresponding .ARM.exidx section.

2. After the section merge

   1. Build a text region fragment to index map.

   2. Sort the .ARM.exidx region fragments by their corresponding text region
      fragment index.

   3. Iterate each text and .ARM.exidx region fragments.

      1. If text and .ARM.exidx region fragments mismatch, then create a new
         region fragment with EXIDX_CANTUNWIND if `is_exidx_required_` is `true`
         and `is_prev_cant_unwind_` is `false`.  Set `is_prev_cant_unwind_` to
         `true`.

      2. Compute the size of the compressed .ARM.exidx region fragment.

         1. Create temporary copy: `is_exidx_required` and
            `is_prev_cant_unwind`.

         2. Initialize the counter `num_entries` to 0.

         3. For each entry in this .ARM.exidx region fragment

            1. If this entry is not EXIDX_CANTUNWIND, then set
               `is_exidx_required` to `true`, and `is_prev_cant_unwind` to
               `false`.  Increase `num_entries`.

            2. If this entry is EXIDX_CANTUNWIND and `is_exidx_required` is
               `false`, then skip this entry.

            3. If this entry is EXIDX_CANTUNWIND, `is_exidx_required` is
               `true`, and `is_prev_cant_unwind` is `true`, then skip this
               entry.

            4. If this entry is EXIDX_CANTUNWIND, `is_exidx_required` is
               `true`, and `is_prev_cant_unwind` is `false`, then we have to
               emit this EXIDX_CANTUNWIND entry.  Set `is_prev_cant_unwind` to
               `true`, and increase `num_entries`.

         4. Return `num_entries`.

      3. If `num_entries` is equal to zero, then remove the region fragment,
         and delete all relocations.

      4. If `num_entries` is unchanged, then flush the `is_prev_cant_unwind`
         and `is_exidx_required`, and continue on next region fragment.

      5. If `num_entries` is greater than zero and changed, then build new
         exidx region fragment data.

         1. Allocate new byte array with `num_entries * 8`.

         2. Create temporary copy: `is_exidx_required` and
            `is_prev_cant_unwind`.

         3. Initialize `new_offset` to 0.

         4. For each entry in this .ARM.exidx region fragment

            1. If this entry is not EXIDX_CANTUNWIND, then set
               `is_exidx_required` to `true`, and `is_prev_cant_unwind` to
               `false`.  Copy the data from the old content.  Update the offset
               of the relocation fixups.  Increase `new_offset` by 8.

            2. If this entry is EXIDX_CANTUNWIND and `is_exidx_required` is
               `false`, then skip this entry.  Remove the related relocations.

            3. If this entry is EXIDX_CANTUNWIND, `is_exidx_required` is
               `true`, and `is_prev_cant_unwind` is `true`, then skip this
               entry.  Remove the related relocations.

            4. If this entry is EXIDX_CANTUNWIND, `is_exidx_required` is
               `true`, and `is_prev_cant_unwind` is `false`, then we have to
               emit this EXIDX_CANTUNWIND entry.  Copy the data from the old
               content.  Update the offset of the relocation fixups.  Set
               `is_prev_cant_unwind` to true, and increase `new_offset` by 8.

         5. Replace the content in this region fragment (Update `StringRef`)

         6. Copy temporary back to `is_exidx_required_` and
            `is_prev_cant_unwind_`.


Data Structure
--------------

If (and only if) the .ARM.exidx, .ARM.extab, and the corresponding text
sections are not ignored (aka. GC'ed), then there will be one `ARMExData` for
this combination.

- `ARMModuleExData` -- The class that holds all exception related data of a
  module.

  - `input_map_` -- A map from `Input` to `ARMInputExData`.

  - `exidx_map_` -- A map from .ARM.exidx region fragments to `ARMExData`.

  - `region_data_allocator_` -- An allocator for new region fragment data.

- `ARMInputExData` -- The class that holds input-specific exception data.

  - `name_map_` -- A map from canonical name to `ARMExData`.  For example,
    the canonical name for `.ARM.exidx.dog` is `.dog` and `.rel.ARM.extab.pig`
    is `.pig`.

- `ARMExData` -- The class that holds the corresponding sections, e.g. `.pig`,
  `.ARM.exidx.pig`, `.rel.ARM.exidx.pig`, `.ARM.extab.pig`, and
  `.rel.ARM.extab.pig` will form an `ARMExData`.

  - `text_frag_` -- The pointer to text fragment (must be available.)

  - `exidx_frag_` -- The pointer to .ARM.exidx region fragment
    (must be available.)

  - `extab_frag_` -- The pointer to .ARM.extab region fragment.

  - `exidx_reloc_` -- Relocation data for .ARM.exidx region fragment
    (must be available.)

  - `extab_reloc_` -- Relocation data for .ARM.extab region fragment.
