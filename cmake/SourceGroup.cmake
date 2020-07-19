# Copyright (c) Matthew Rodusek
# Distributed under the OSI-approved MIT License. See accompanying
# file LICENSE.txt or https://opensource.org/licenses/MIT for details.

#.rst:
# SourceGroup
# -----------
#
# This module defines additional source_group functionality that is
# compatible with older versions of CMake

if (CMAKE_VERSION VERSION_LESS 3.5.0)
  include(CMakeParseArguments)
endif ()

#.rst
# .. command:: source_group_tree
#
# This command defines a grouping for source files in IDE project
# generation.
#
# This function is not supported in CMake 3.7 or before, and is
# added here for compatibility.
#
#   If this is executed in CMake 3.8 or later, it defaults to
#   source_group(TREE ...) instead to preserve functionality.
#
# .. code-block:: cmake
#
#   source_group_tree(<root> [PREFIX <prefix>] [FILES <src>])
#
# Creates groups recursively based on the specified ``<root>``
# along with their relative paths to the given source files.
#
# The options are:
#
#   ``PREFIX``
#     Source group and files located directly in <root> path, will
#     be placed in <prefix> source groups.
#
#   ``FILES``
#     Any source file specified explicitly will be placed in group
#     made up of the relative path between ``<root>`` and the file.
#     Relative paths are interpreted with respect to the
#     current source directory.
#     It is an error if any ``<src>`` is not prefixed by ``<prefix>``
function(source_group_tree root)

  cmake_parse_arguments(
    SOURCE_GROUP_TREE # Prefix
    ""      # Option args
    PREFIX  # Single args
    FILES   # Multi args
    ${ARGN}
  )

  if (SOURCE_GROUP_TREE_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "source_group: Unknown arguments specified: ${SOURCE_GROUP_TREE_UNPARSED_ARGUMENTS}")
  endif ()

  if (IS_ABSOLUTE "${root}")
    set(root_path "${root}")
  else ()
    set(root_path "${CMAKE_CURRENT_SOURCE_DIR}/${root}")
    get_filename_component(root_path "${root_path}" ABSOLUTE)
  endif ()

  # source_group(TREE ...) introduced in 3.8
  if (CMAKE_VERSION VERSION_GREATER 3.8)
    set(args)
    if (SOURCE_GROUP_TREE_PREFIX)
      set(args PREFIX "${SOURCE_GROUP_TREE_PREFIX}" ${args})
    endif ()
    if (SOURCE_GROUP_TREE_FILES)
        set(args FILES "${SOURCE_GROUP_TREE_FILES}" ${args})
    endif ()
    source_group( TREE "${root_path}" ${args} )
  else ()
    foreach (file IN LISTS SOURCE_GROUP_TREE_FILES)
      if (SOURCE_GROUP_TREE_PREFIX)
        set(group "${SOURCE_GROUP_TREE_PREFIX}")
      else ()
        set(group)
      endif ()

      if (IS_ABSOLUTE "${file}")
        set(absolute_file "${file}")
      else ()
        # All relative paths should be relative to the source directory
        set(absolute_file "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
        get_filename_component(absolute_file "${absolute_file}" ABSOLUTE)
      endif ()

      file(RELATIVE_PATH group_suffix "${root_path}" "${absolute_file}")
      if (group_suffix MATCHES "(\\.\\.).*")
        message(FATAL_ERROR "source_group ROOT: ${root_path} is not a prefix of file ${absolute_file}")
      endif ()
      get_filename_component(group_suffix "${group_suffix}" DIRECTORY)
      string(REPLACE "/" "\\" group_suffix "${group_suffix}")

      if (group AND group_suffix)
        set(group "${SOURCE_GROUP_TREE_PREFIX}\\${group_suffix}")
      elseif (group_suffix)
        set(group "${group_suffix}")
      endif ()
      source_group("${group}" FILES "${absolute_file}")
    endforeach ()
  endif ()
endfunction()
