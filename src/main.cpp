#include "lumina.hpp"
#include <iostream>

using namespace std;
int main() {
  Lumina engine;
  engine.window_options = (WindowOptions){{640, 480}, "lumina"};
  engine.Run();

  return 0;
}
