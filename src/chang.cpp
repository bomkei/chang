#include <iostream>
#include <stdexcept>
#include "Driver.h"

int main(int argc, char** argv) {
    return Driver().main(argc, argv);
    
  try {
    return Driver().main(argc, argv);
  }
  catch( std::exception const& ex ) {
    std::cerr
      << std::endl
      << std::string(60, '!') << std::endl
      << "unhandled exception has occurred, please report to developer." << std::endl
      << "what(): " << ex.what() << std::endl;
  }
}