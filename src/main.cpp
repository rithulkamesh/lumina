#include "lumina.hpp"
#include <iostream>

using namespace std;
int main() {
  Lumina engine;
  engine.window_options = (WindowOptions){{1280, 720}, "lumina"};
  engine.Run();

  return 0;
}
