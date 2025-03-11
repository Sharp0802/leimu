#include <leimu/leimu.h>

int main(int, char* argv[]) {
  leimu::App app(argv[0]);
  if (!app) {
    return EXIT_FAILURE;
  }

  app.run();

  return EXIT_SUCCESS;
}
