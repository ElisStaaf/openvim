#ifdef __linux__

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <termios.h>
#include <dirent.h>
#include <limits.h>
#include <locale.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <pty.h>
#include <openvim/io.h>
#include <openvim/theme.h>

static struct termios old_termios;
static int mouse_down = 0;

const char *io_config = "~/.openvim.cfg";

void io_init(void) {
  tcgetattr(STDIN_FILENO, &old_termios);
  struct termios new_termios = old_termios;
  
  new_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  new_termios.c_oflag &= ~(OPOST);
  new_termios.c_cflag |= (CS8);
  new_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  new_termios.c_cc[VMIN] = 0;
  new_termios.c_cc[VTIME] = 1;
  
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);
  
  setlocale(LC_ALL, "C.UTF-8");
  printf(IO_CLEAR_SCREEN "\x1B[?1000;1002;1006;1015h");
  
  size_t buffer_size = 1048576;
  setvbuf(stdout, malloc(buffer_size), _IOFBF, buffer_size);
  
  fflush(stdout);
}

void io_exit(void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_termios);
  printf("\x1B[?25h\x1B[?1000;1002;1006;1015l" IO_CLEAR_SCREEN);
  
  fflush(stdout);
}

io_file_t io_fopen(const char *path, int write_mode) {
  if (strstr(path, "://")) {
    char buffer[strlen(path) + 32];
    sprintf(buffer, "wget -q -O- '%s'\n", path);
    
    if (write_mode) {
      return (io_file_t) {
        .type = io_file_file,
        .read_only = 1,
        .data = NULL,
      };
    } else {
      return (io_file_t) {
        .type = io_file_clipboard,
        .read_only = 1,
        .data = popen(buffer, "r"),
      };
    }
  }
  
  char buffer[8192];
  io_dsolve(path, buffer);
  
  return (io_file_t) {
    .type = io_file_file,
    .read_only = 0,
    .data = fopen(buffer, write_mode ? "wb+" : "rb"),
  };
}

time_t io_ftime(const char *path) {
  struct stat attrib;
  stat(path, &attrib);
  
  return attrib.st_mtime;
}

int io_fvalid(io_file_t file) {
  return !!file.data;
}

void io_fclose(io_file_t file) {
  if (file.type == io_file_clipboard) {
    io_cclose(file);
  } else {
    fclose(file.data);
  }
}

ssize_t io_fwrite(io_file_t file, void *buffer, size_t count) {
  if (file.read_only || !io_fvalid(file)) {
    return 0;
  }
  
  if (file.type == io_file_terminal) {
    return write((size_t)(file.data) / 1048576, buffer, count);
  }
  
  return fwrite(buffer, 1, count, file.data);
}

ssize_t io_fread(io_file_t file, void *buffer, size_t count) {
  if (!io_fvalid(file)) {
    return 0;
  }
  
  if (file.type == io_file_terminal) {
    return read((size_t)(file.data) / 1048576, buffer, count);
  }
  
  return fread(buffer, 1, count, file.data);
}

void io_frewind(io_file_t file) {
  rewind(file.data);
}

int io_feof(io_file_t file) {
  return feof(file.data);
}

void io_dsolve(const char *path, char *buffer) {
  if (path[0] == '~') {
    const char *home = getenv("HOME");
    strcpy(buffer, home);
    
    io_dsolve(path + 1, buffer + strlen(home));
  } else {
    realpath(path, buffer);
  }
}

io_file_t io_dopen(const char *path) {
  return (io_file_t) {
    .type = io_file_directory,
    .read_only = 1,
    .data = opendir(path),
  };
}

int io_dvalid(io_file_t file) {
  return !!file.data;
}

void io_dclose(io_file_t file) {
  closedir(file.data);
}

int io_dread(io_file_t file, char *buffer) {
  struct dirent *entry = readdir(file.data);
  
  if (!entry) {
    return 0;
  }
  
  strcpy(buffer, entry->d_name);
  return 1;
}

void io_drewind(io_file_t file) {
  rewinddir(file.data);
}

io_file_t io_copen(int write_mode) {
  if (write_mode) {
    return (io_file_t) {
      .type = io_file_clipboard,
      .read_only = 0,
      .data = popen("xclip -selection clipboard -i", "w"),
    };
  } else {
    return (io_file_t) {
      .type = io_file_clipboard,
      .read_only = 1,
      .data = popen("xclip -selection clipboard -o", "r"),
    };
  }
}

void io_cclose(io_file_t file) {
  pclose(file.data);
}

io_file_t io_topen(const char *path) {
  int term_fd;
  pid_t pid = forkpty(&term_fd, NULL, NULL, NULL);
  
  if (pid < 0) {
    return (io_file_t) {
      .type = io_file_file,
      .read_only = 1,
      .data = NULL,
    };
  }
  
  if (!pid) {
    execl(path, path, NULL);
    exit(0);
  }
  
  fcntl(term_fd, F_SETFL, FNDELAY);
  
  return (io_file_t) {
    .type = io_file_terminal,
    .read_only = 0,
    .data = (void *)((size_t)(term_fd * 1048576 + pid)),
  };
}

void io_tclose(io_file_t file) {
  pid_t pid = (size_t)(file.data) % 1048576;
  kill(pid, SIGKILL);
}

void io_tresize(io_file_t file, int width, int height) {
  struct winsize ws;
  
  ws.ws_col = width;
  ws.ws_row = height;
  
  ioctl((size_t)(file.data) / 1048576, TIOCSWINSZ, &ws);
}

void io_tsend(io_file_t file, unsigned int key) {
  char buffer[256];
  buffer[0] = '\0';
  
  if (key >= IO_ARROW_UP && key <= IO_ARROW_LEFT) {
    sprintf(buffer, "\x1B[%c", 'A' + (key - IO_ARROW_UP));
  }
  
  io_fwrite(file, buffer, strlen(buffer));
}

int io_techo(io_file_t file) {
  struct termios pty_termios;
  tcgetattr((size_t)(file.data) / 1048576, &pty_termios);
  
  return !!(pty_termios.c_lflag & ECHO);
}

void io_cursor(int x, int y) {
  if (x < 0 || y < 0) {
    printf("\x1B[?25l");
  } else {
    printf("\x1B[%d;%dH\x1B[?25h", y + 1, x + 1);
  }
}

void io_printf(const char *format, ...) {
  char buffer[32768];
  
  va_list args;
  va_start(args, format);
  
  vsprintf(buffer, format, args);
  va_end(args);
  
  theme_apply(buffer);
  printf("%s", buffer);
}

int io_printf_wrap(int x, int width, int y, const char *format, ...) {
  char buffer[8192];
  
  va_list args;
  va_start(args, format);
  
  vsprintf(buffer, format, args);
  va_end(args);
  
  theme_apply(buffer);
  
  int width_left = width;
  io_cursor(x, y);
  
  char word[256];
  int length = 0;
  
  for (int i = 0;; i++) {
    if (!buffer[i] || isspace(buffer[i])) {
      word[length] = '\0';
      
      if (width_left < length) {
        io_cursor(x, ++y);
        width_left = width;
      }
      
      printf("%s", word);
      
      width_left -= length;
      length = 0;
      
      if (buffer[i] == '\n') {
        io_cursor(x, ++y);
        width_left = width;
      } else if (buffer[i]) {
        if (width_left) {
          putchar(' ');
          width_left--;
        }
      } else {
        break;
      }
    } else {
      word[length++] = buffer[i];
    }
  }
  
  return y;
}

void io_flush(void) {
  fflush(stdout);
}

static unsigned char indirect_buffer[256];
static int indirect_read_head = 0, indirect_write_head = 0;

static int indirect_read(unsigned char *ptr, int count) {
  int done_count = 0;
  
  while (count && indirect_read_head != indirect_write_head) {
    *(ptr++) = indirect_buffer[indirect_read_head];
    
    indirect_read_head = (indirect_read_head + 1) % 256;
    done_count++, count--;
  }
  
  if (count) {
    done_count += read(STDIN_FILENO, ptr, count);
  }
  
  return done_count;
}

static void indirect_unread(unsigned char chr) {
  indirect_buffer[indirect_write_head] = chr;
  indirect_write_head = (indirect_write_head + 1) % 256;
}

io_event_t io_get_event(void) {
  static time_t old_time = 0;
  static int old_width = 0, old_height = 0;
  
  struct winsize ws;
  
  if (!(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)) {
    int width = ws.ws_col;
    int height = ws.ws_row;
    
    if (width != old_width || height != old_height) {
      old_width = width;
      old_height = height;
      
      return (io_event_t) {
        .type = IO_EVENT_RESIZE,
        .size = {width, height},
      };
    }
  }
  
  time_t new_time = time(NULL);
  
  if (new_time != old_time) {
    old_time = new_time;
    
    return (io_event_t) {
      .type = IO_EVENT_TIME_SECOND,
      .time = new_time,
    };
  }
  
  unsigned char ansi_buffer[256];
  unsigned char chr;
  
  if (indirect_read(&chr, 1) > 0) {
    unsigned int key = chr;
    
    if (chr == '\x1B') {
      indirect_read(ansi_buffer, 1);
      
      if (ansi_buffer[0] == '[') {
        indirect_read(ansi_buffer + 1, 1);
      }
      
      if (ansi_buffer[0] < 32) {
        key = IO_ALT(ansi_buffer[0]);
      } else if (ansi_buffer[1] == 'A') {
        key = IO_ARROW_UP;
      } else if (ansi_buffer[1] == 'B') {
        key = IO_ARROW_DOWN;
      } else if (ansi_buffer[1] == 'C') {
        key = IO_ARROW_RIGHT;
      } else if (ansi_buffer[1] == 'D') {
        key = IO_ARROW_LEFT;
      } else if (isdigit(ansi_buffer[1])) {
        indirect_read(ansi_buffer + 2, 1);
        
        if (ansi_buffer[2] == ';') {
          indirect_read(ansi_buffer + 3, 2);
          
          if (ansi_buffer[4] == 'A') {
            key = IO_ARROW_UP;
          } else if (ansi_buffer[4] == 'B') {
            key = IO_ARROW_DOWN;
          } else if (ansi_buffer[4] == 'C') {
            key = IO_ARROW_RIGHT;
          } else if (ansi_buffer[4] == 'D') {
            key = IO_ARROW_LEFT;
          }
          
          if (ansi_buffer[3] == '5' || ansi_buffer[3] == '6') {
            key = IO_CTRL(key);
          }
          
          if (ansi_buffer[3] == '2' || ansi_buffer[3] == '6' || ansi_buffer[3] == '4') {
            key = IO_SHIFT(key);
          }
          
          if (ansi_buffer[3] == '3' || ansi_buffer[3] == '4') {
            key = IO_ALT(key);
          }
        } else if (ansi_buffer[2] == '~') {
          if (ansi_buffer[1] == '3') {
            key = '\x7F';
          } else if (ansi_buffer[1] == '5') {
            key = IO_PAGE_UP;
          } else if (ansi_buffer[1] == '6') {
            key = IO_PAGE_DOWN;
          }
        } else {
          indirect_unread(ansi_buffer[1]);
          indirect_unread(ansi_buffer[2]);
          
          goto mouse_fix_for_kitty;
        }
      } else if (ansi_buffer[1] == 'H') {
        key = IO_HOME;
      } else if (ansi_buffer[1] == 'F') {
        key = IO_END;
      } else if (ansi_buffer[1] == 'Z') {
        key = IO_SHIFT('\t');
      } else if (ansi_buffer[1] == '<') {
mouse_fix_for_kitty:
        int mouse_mask = 0, mouse_x = 0, mouse_y = 0;
        char chr;
        
        for (;;) {
          indirect_read(&chr, 1);
          
          if (!isdigit(chr)) {
            break;
          }
          
          mouse_mask = mouse_mask * 10 + (chr - '0');
        }
        
        if (isalpha(chr)) {
          goto mouse_parse_end;
        }
        
        for (;;) {
          indirect_read(&chr, 1);
          
          if (!isdigit(chr)) {
            break;
          }
          
          mouse_x = mouse_x * 10 + (chr - '0');
        }
        
        if (mouse_x > 0) {
          mouse_x--;
        }
        
        if (isalpha(chr)) {
          goto mouse_parse_end;
        }
        
        for (;;) {
          indirect_read(&chr, 1);
          
          if (!isdigit(chr)) {
            break;
          }
          
          mouse_y = mouse_y * 10 + (chr - '0');
        }
        
        if (mouse_y > 0) {
          mouse_y--;
        }
        
mouse_parse_end:
        bool is_down = !(bool)(mouse_mask & 1);
        bool is_right = (bool)(mouse_mask & 2);
        
        int move_type = (mouse_mask / 32) % 4;
        
        if (move_type == 1) {
          is_down = (is_down && isupper(chr));
          
          if (mouse_down && is_down) {
            move_type = 2;
          } else {
            mouse_down = is_down;
            
            return (io_event_t) {
              .type = (is_down ? IO_EVENT_MOUSE_DOWN : IO_EVENT_MOUSE_UP),
              .mouse = {.x = mouse_x, .y = mouse_y},
            };
          }
        }
        
        if (move_type == 2) {
          return (io_event_t) {
            .type = IO_EVENT_MOUSE_MOVE,
            .mouse = {.x = mouse_x, .y = mouse_y},
          };
        }
        
        if (move_type == 3) {
          return (io_event_t) {
            .type = IO_EVENT_SCROLL,
            .scroll = (is_down ? -1 : 1),
          };
        }
      }
    } else if (chr == '\x7F') {
      key = IO_CTRL('H');
    }
    
    int utf_8_size = 1;
    
    if (chr >= 0xF0) {
      utf_8_size = 4;
    } else if (chr >= 0xE0) {
      utf_8_size = 3;
    } else if (chr >= 0xC0) {
      utf_8_size = 2;
    }
    
    if (utf_8_size > 1) {
      indirect_read(ansi_buffer, utf_8_size);
      key = chr | 0x80000000;
      
      for (int i = 0; i < utf_8_size - 1; i++) {
        key |= ((unsigned int)(ansi_buffer[i]) << (i * 8 + 8));
      }
    }
    
    return (io_event_t) {
      .type = IO_EVENT_KEY_PRESS,
      .key = key,
    };
  }
  
  return (io_event_t) {
    .type = 0,
  };
}

#endif
