#include <string.h>
#include <openvim.h>
#include <openvim/theme.h>

static const char *__format_theme_17[] = {
  "\x1B[49m",
  "\x1B[40m",
  "\x1B[100m",
  
  "\x1B[97m",
  "\x1B[30m",
  "\x1B[31m",
  "\x1B[32m",
  "\x1B[33m",
  "\x1B[34m",
  "\x1B[35m",
  "\x1B[36m",
  "\x1B[37m",
  "\x1B[90m",
  
  "\x1B[7m",
};

static const char *__format_theme_16[] = {
  "\x1B[49m",
  "\x1B[100m",
  "\x1B[47m",
  
  "\x1B[97m",
  "\x1B[30m",
  "\x1B[31m",
  "\x1B[32m",
  "\x1B[33m",
  "\x1B[34m",
  "\x1B[35m",
  "\x1B[36m",
  "\x1B[37m",
  "\x1B[90m",
  
  "\x1B[7m",
};

static const char *__format_theme_16_tty[] = {
  "\x1B[49;25m",
  "\x1B[100;5m",
  "\x1B[47;25m",
  
  "\x1B[97;1m",
  "\x1B[30;22m",
  "\x1B[31;1m",
  "\x1B[32;1m",
  "\x1B[33;1m",
  "\x1B[34;1m",
  "\x1B[35;1m",
  "\x1B[36;1m",
  "\x1B[37;1m",
  "\x1B[90;1m",
  
  "\x1B[49;7;5m",
};

static const char *__format_theme_cga_black[] = {
  "\x1B[38;5;231m\x1B[48;5;16m\x1B[1m",
  "\x1B[38;5;231m\x1B[48;5;59m\x1B[1m",
  "\x1B[38;5;16m\x1B[48;5;145m\x1B[1m",
  
  "\x1B[38;5;231m\x1B[1m",
  "\x1B[38;5;16m\x1B[1m",
  "\x1B[38;5;196m\x1B[1m",
  "\x1B[38;5;46m\x1B[1m",
  "\x1B[38;5;226m\x1B[1m",
  "\x1B[38;5;21m\x1B[1m",
  "\x1B[38;5;201m\x1B[1m",
  "\x1B[38;5;51m\x1B[1m",
  "\x1B[38;5;145m\x1B[1m",
  "\x1B[38;5;59m\x1B[1m",
  
  "\x1B[38;5;16m\x1B[48;5;231m\x1B[1m",
};

static const char *__format_theme_cga_blue[] = {
  "\x1B[38;5;231m\x1B[48;5;20m\x1B[1m",
  "\x1B[38;5;231m\x1B[48;5;63m\x1B[1m",
  "\x1B[38;5;16m\x1B[48;5;145m\x1B[1m",
  
  "\x1B[38;5;231m\x1B[1m",
  "\x1B[38;5;63m\x1B[1m",
  "\x1B[38;5;203m\x1B[1m",
  "\x1B[38;5;83m\x1B[1m",
  "\x1B[38;5;227m\x1B[1m",
  "\x1B[38;5;147m\x1B[1m",
  "\x1B[38;5;207m\x1B[1m",
  "\x1B[38;5;87m\x1B[1m",
  "\x1B[38;5;145m\x1B[1m",
  "\x1B[38;5;63m\x1B[1m",
  
  "\x1B[38;5;16m\x1B[48;5;231m\x1B[1m",
};

static const char *__format_theme_crt_green[] = {
  "\x1B[38;5;46m\x1B[48;5;16m",
  "\x1B[38;5;46m\x1B[48;5;22m",
  "\x1B[38;5;16m\x1B[48;5;34m",
  
  "\x1B[38;5;46m",
  "\x1B[38;5;16m",
  "\x1B[38;5;34m",
  "\x1B[38;5;46m",
  "\x1B[38;5;46m",
  "\x1B[38;5;28m",
  "\x1B[38;5;34m",
  "\x1B[38;5;46m",
  "\x1B[38;5;34m",
  "\x1B[38;5;22m",
  
  "\x1B[38;5;16m\x1B[48;5;46m",
};

static const char *__format_theme_crt_amber[] = {
  "\x1B[38;5;208m\x1B[48;5;16m",
  "\x1B[38;5;208m\x1B[48;5;52m",
  "\x1B[38;5;16m\x1B[48;5;130m",
  
  "\x1B[38;5;208m",
  "\x1B[38;5;16m",
  "\x1B[38;5;130m",
  "\x1B[38;5;208m",
  "\x1B[38;5;208m",
  "\x1B[38;5;94m",
  "\x1B[38;5;130m",
  "\x1B[38;5;208m",
  "\x1B[38;5;130m",
  "\x1B[38;5;52m",
  
  "\x1B[38;5;16m\x1B[48;5;208m",
};

static const char *__format_theme_solarized_dark[] = {
  "\x1B[38;5;230m\x1B[48;5;234m",
  "\x1B[38;5;230m\x1B[48;5;241m",
  "\x1B[38;5;234m\x1B[48;5;245m",
  
  "\x1B[38;5;230m",
  "\x1B[38;5;235m",
  "\x1B[38;5;160m",
  "\x1B[38;5;64m",
  "\x1B[38;5;136m",
  "\x1B[38;5;33m",
  "\x1B[38;5;125m",
  "\x1B[38;5;37m",
  "\x1B[38;5;245m",
  "\x1B[38;5;241m",
  
  "\x1B[38;5;234m\x1B[48;5;230m",
};

const char *theme_names[] = {
  "16 colors + background (terminal)",
  "16 colors (terminal)",
  "16 colors + blink attr. (terminal)",
  "CGA black (256-color)",
  "CGA blue (256-color)",
  "Monochromatic green (256-color)",
  "Monochromatic amber (256-color)",
  "Solarized dark (256-color)",
};

static const char **__format_themes[] = {
  __format_theme_17,
  __format_theme_16,
  __format_theme_16_tty,
  __format_theme_cga_black,
  __format_theme_cga_blue,
  __format_theme_crt_green,
  __format_theme_crt_amber,
  __format_theme_solarized_dark,
};

void theme_apply(char *buffer) {
  int length = strlen(buffer);
  
  for (int i = 0; i < length; i++) {
    if ((buffer[i] >= 0x0E && buffer[i] <= 0x1A) ||
        (buffer[i] >= 0x1C && buffer[i] <= 0x1E)) {
      const char *str;
      
      if (buffer[i] == 0x1D) {
        if (bd_config.theme == theme_16_tty) {
          str = "";
        } else {
          str = "\x1B[1m";
        }
      } else if (buffer[i] == 0x1E) {
        if (bd_config.theme == theme_16_tty) {
          str = "";
        } else {
          str = "\x1B[22m";
        }
      } else {
        if (buffer[i] == 0x1C) {
          buffer[i]--;
        }
        
        str = __format_themes[bd_config.theme][buffer[i] - 0x0E];
      }
      
      int str_length = strlen(str);
      
      if (str_length == 0) {
        for (int j = i; j < length; j++) {
          buffer[j] = buffer[j + 1];
        }
        
        length--, i--;
        continue;
      }
      
      for (int j = length; j >= i + 1; j--) {
        buffer[j + (str_length - 1)] = buffer[j];
      }
      
      memcpy(buffer + i, str, str_length);
      
      length += str_length - 1;
      i += str_length - 1;
    }
  }
}
