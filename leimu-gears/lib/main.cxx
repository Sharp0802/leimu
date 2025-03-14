#include <leimu/leimu.h>

int main(int, char* argv[]) {
  const leimu::Config config{
    leimu::config::VkConfig{}
  };

  leimu::App app(argv[0], config);
  if (!app) {
    return EXIT_FAILURE;
  }

  app.run();

  return EXIT_SUCCESS;
}
