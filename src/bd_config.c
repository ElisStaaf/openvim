#include <openvim/theme.h>
#include <openvim/io.h>
#include <openvim.h>

bd_config_t bd_config = (bd_config_t) {
  .indent_width = 2,
  .indent_spaces = 1,
  .indent_guides = 1,
  .scroll_step = 2,
  .scroll_image_step = 4,
  .scroll_width_margin = 0,
  .undo_edit_count = 48,
  .undo_depth = 64,
  .theme = theme_17,
  .xterm_colors = 1,
  .terminal_count = 2048,
  .column_guide = 0,
  .column_tiny = 0,
  
  .shell_path = "/usr/bin/sh",
  
  .syntax_colors = {
    "",             // st_color_none
    IO_WHITE,       // st_color_default
    IO_BOLD_PURPLE, // st_color_keyword
    IO_YELLOW,      // st_color_number
    IO_BLUE,        // st_color_function
    IO_BOLD_CYAN,   // st_color_type
    IO_GREEN,       // st_color_string
    IO_GRAY,        // st_color_comment
    IO_BOLD_RED,    // st_color_symbol
  },
};
