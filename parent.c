#include <uv.h>

static uv_loop_t *loop;
static uv_process_t *child;
static uv_pipe_t *in_pipe;
static uv_pipe_t *out_pipe;

static char req = 'X';
static uv_buf_t uv_req_buf = {.len = 1, .base = &req};

static void write_cb(uv_write_t *req, int status) {
  free(req);
}

static void write_req(uv_timer_t *handle) {
  uv_write_t *req = malloc(sizeof(uv_write_t));
  uv_write(req, (uv_stream_t *)out_pipe, &uv_req_buf, 1, write_cb);
  printf("P: req\n");
}

static void alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
  buf->base = malloc(size);
  buf->len = (unsigned long)size;
}

static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
  free(buf->base);
  if (nread < 0) {
    uv_close((uv_handle_t *)stream, NULL);
  } else if (nread > 0) {
    printf("P: res: %zd\n", nread);
    uv_timer_t *timer = malloc(sizeof(uv_timer_t));
    uv_timer_init(loop, timer);
    uv_timer_start(timer, write_req, 0, 0);
  }
}

int main(int argc, char *argv[]) {
  child = malloc(sizeof(uv_process_t));
  in_pipe = malloc(sizeof(uv_pipe_t));
  out_pipe = malloc(sizeof(uv_pipe_t));

  char *args[] = {"child.exe", NULL};
  uv_stdio_container_t stdio[] = {
      [0] = {.flags = UV_INHERIT_FD, .data.fd = 0},
      [1] = {.flags = UV_INHERIT_FD, .data.fd = 1},
      [2] = {.flags = UV_INHERIT_FD, .data.fd = 2},
      [3] = {.flags = UV_CREATE_PIPE | UV_READABLE_PIPE | UV_WRITABLE_PIPE,
             .data.stream = (uv_stream_t *)in_pipe},
      [4] = {.flags = UV_CREATE_PIPE | UV_READABLE_PIPE | UV_WRITABLE_PIPE,
             .data.stream = (uv_stream_t *)out_pipe},
  };
  uv_process_options_t options = {
      .file = "child.exe",
      .args = args,
      .stdio_count = sizeof(stdio) / sizeof(stdio[0]),
      .stdio = stdio,
  };

  loop = uv_default_loop();
  uv_pipe_init(loop, in_pipe, 0);
  uv_pipe_init(loop, out_pipe, 0);
  uv_spawn(loop, child, &options);
  uv_read_start((uv_stream_t *)in_pipe, alloc_cb, read_cb);
  write_req(NULL);
  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);
}