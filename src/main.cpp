#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string exit = "exit";
  std::string echo = "echo";

  while (true){
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);
    
    if (input.compare(0, exit.size(), exit) == 0) {
        return 0;
    }
    
    if (input.compare(0, echo.size(), echo) == 0) {
        std::cout << input.substr(echo.length() + 1) << std::endl;
        continue;
    }
    
    std::cout << input << ": command not found" << std::endl;
  }
}
