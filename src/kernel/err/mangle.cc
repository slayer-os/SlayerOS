#include <klib/string.h>
#include <klib/memory.h>

#include <dbg/log.h>

bool is_mangled(const char *name) {
  return name[0] == '_' && name[1] == 'Z';
}

void copy_unmangled_name(char *buffer, const char *name) {
  memcpy(buffer, name, strlen(name));
}

u32 parse_function_length(const char *name, u32 *i, char *length_str) {
  u32 length_i = 0;
  while (name[*i] >= '0' && name[*i] <= '9') {
    length_str[length_i++] = name[(*i)++];
  }
  length_str[length_i] = '\0';
  return atoi(length_str);
}

void unmangle_name(char *buffer, const char *name, u32 *i) {
  bool ns = false;
  char size_str[16];
  u32 j = 0;
  if (name[*i] == 'N') {
    ns = true;
    (*i)++;
  }
  u32 length = parse_function_length(name, i, size_str);
  memcpy(buffer, name + *i, length);
  *i += length;
  j += length;
  if (ns) {
    buffer[j++] = ':';
    buffer[j++] = ':';
    while (name[*i] != 'E') {
      length = parse_function_length(name, i, size_str);
      memcpy(buffer + j, name + *i, length);
      j += length;
      *i += length;
      if (name[*i] != 'E') {
        buffer[j++] = ':';
        buffer[j++] = ':';
      }
    }
    (*i)++;
  }
  buffer[j] = '\0';
}

void copy_function_name(char *buffer, const char *name, u32 *i, u32 *j, u32 length) {
  memcpy(buffer, name + *i, length);
  buffer[length] = '(';
  *j = length + 1;
  *i += length;
}

void append_remaining_name(char *buffer, const char *name, u32 *i, u32 *j) {
  while (name[*i] != '\0') {
    buffer[(*j)++] = name[(*i)++];
  }
}

bool is_end_of_parameters(char c) {
  return c == 'E' || c == '\0' || c == ' ';
}


void append_to_buffer(char *buffer, u32 *j, char *latest_type, u32 *latest_size, const char *str) {
  while (*str) {
    buffer[(*j)++] = *str;
    latest_type[(*latest_size)++] = *str++;
  }
}

void handle_repeated_type(char *buffer, u32 *j, const char *name, u32 *i, char old_params[64][16], char *latest_type, u32 *latest_size) {
  (*i)++;
  char type_i[16];
  u32 type_i_size = 0;
  while (name[*i] != '_') {
    type_i[type_i_size++] = name[(*i)++];
  }
  type_i[type_i_size] = '\0';
  u32 repeat = type_i_size > 0 ? atoi(type_i) : 0;
  char *old_param = old_params[repeat];
  while (*old_param && *old_param != ' ') {
    buffer[(*j)++] = *old_param;
    latest_type[(*latest_size)++] = *old_param++;
  }
}

void handle_objstruct(char *buffer, const char *name, u32 *i, u32 *j, char *latest_type, u32 *latest_size) {
  char name_str[128];
  unmangle_name(name_str, name, i);
  append_to_buffer(buffer, j, latest_type, latest_size, name_str);
}

void demangle_single_parameter(char *buffer, const char *name, u32 *i, u32 *j, char old_params[64][16], char *latest_type, u32 *latest_size) {
  switch (name[*i]) {
    case 'P': append_to_buffer(buffer, j, latest_type, latest_size, "*"); break;
    case 'v': append_to_buffer(buffer, j, latest_type, latest_size, "void"); break;
    case 'm': append_to_buffer(buffer, j, latest_type, latest_size, "u64"); break;
    case 'j': append_to_buffer(buffer, j, latest_type, latest_size, "u32"); break;
    case 't': append_to_buffer(buffer, j, latest_type, latest_size, "u16"); break;
    case 'h': append_to_buffer(buffer, j, latest_type, latest_size, "u8"); break;
    case 'y': append_to_buffer(buffer, j, latest_type, latest_size, "u64"); break;
    case 'x': append_to_buffer(buffer, j, latest_type, latest_size, "long long"); break;
    case 'i': append_to_buffer(buffer, j, latest_type, latest_size, "int"); break;
    case 'l': append_to_buffer(buffer, j, latest_type, latest_size, "long"); break;
    case 'c': append_to_buffer(buffer, j, latest_type, latest_size, "char"); break;
    case 'b': append_to_buffer(buffer, j, latest_type, latest_size, "bool"); break;
    case 'f': append_to_buffer(buffer, j, latest_type, latest_size, "float"); break;
    case 'd': append_to_buffer(buffer, j, latest_type, latest_size, "double"); break;
    case 's': append_to_buffer(buffer, j, latest_type, latest_size, "short"); break;
    case 'K': append_to_buffer(buffer, j, latest_type, latest_size, "const "); break;
    case 'z': append_to_buffer(buffer, j, latest_type, latest_size, "..."); break;
    case 'S': handle_repeated_type(buffer, j, name, i, old_params, latest_type, latest_size); break;
    default: {
      if ((name[*i] >= '0' && name[*i] <= '9') || name[*i] == 'N') {
        handle_objstruct(buffer, name, i, j, latest_type, latest_size);
        break;
      }
      append_to_buffer(buffer, j, latest_type, latest_size, name + *i); break;
    }
  }
}

void add_separator(char *buffer, u32 *j, const char *name, u32 i) {
  if (!is_end_of_parameters(name[i])) {
    buffer[(*j)++] = ',';
    buffer[(*j)++] = ' ';
  }
}

void demangle_parameters(char *buffer, const char *name, u32 *i, u32 *j, char old_params[64][16], u32 *old_params_i, char *latest_type, u32 *latest_size) {
  for (;;) {
    if (is_end_of_parameters(name[*i])) break;
    demangle_single_parameter(buffer, name, i, j, old_params, latest_type, latest_size);
    if (name[*i] == 'P' || name[*i] == 'K') { (*i)++; continue; }
    (*i)++;
    old_params[*old_params_i][*latest_size] = '\0';
    memcpy(old_params[(*old_params_i)++], latest_type, *latest_size);
    *latest_size = 0;
    add_separator(buffer, j, name, *i);
  }
}

char *demangle_alloc_symbol(const char *name) {
  char *buffer = (char*)kmalloc(128);
  if (!is_mangled(name)) {
    copy_unmangled_name(buffer, name);
    return buffer;
  }

  u32 i = 2, j = 0;
  
  if (name[i] == 'N') {
    unmangle_name(buffer, name, &i);
    j = strlen(buffer);
    buffer[j++] = '(';
  } else {
    char length_str[16];
    u32 length = parse_function_length(name, &i, length_str);
    copy_function_name(buffer, name, &i, &j, length);
  }

  char old_params[64][16];
  u32 old_params_i = 0;
  char latest_type[64];
  u32 latest_size = 0;

  demangle_parameters(buffer, name, &i, &j, old_params, &old_params_i, latest_type, &latest_size);

  buffer[j++] = ')';
  append_remaining_name(buffer, name, &i, &j);

  return buffer;
}
