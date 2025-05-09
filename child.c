#include <io.h>
#include <stdio.h>

int main() {
  char req;
  char res_buf[65536] = {};
  for (int i = 0; i < 3; ++i) {
    int n_read = _read(4, &req, 1);
    if (n_read != 1) {
      return 1;
    }
    printf("C: req\n");
    int n_written = _write(3, res_buf, sizeof(res_buf));
    printf("C: res: %d\n", n_written);
  }
}
