#include "chang.h"

int main(int argc, char** argv) {
  try {
    return Driver().main(argc, argv);
  }
  catch( std::exception const& ex ) {
    std::cout
      << "unhandled exception has occurred, please report to developer." << std::endl
      << "what(): " << ex.what() << std::endl;
  }
}