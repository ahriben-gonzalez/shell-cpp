#include <iostream>
#include <string>
#include <unordered_map>
#include <set>

int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string exit = "exit";
  std::string echo = "echo";
  std::string type = "type";
  std::set<std::string> commands = {"exit", "echo", "type"};

  while (true)
  {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);

    if (input.compare(0, exit.size(), exit) == 0)
    {
      return 0;
    }

    if (input.compare(0, echo.size(), echo) == 0)
    {
      std::cout << input.substr(echo.length() + 1) << std::endl;
      continue;
    }

    if (input.compare(0, type.size(), type) == 0)
    {
      std::string cmd = input.substr(echo.length() + 1, input.length() - (type.length() + 1));
      if (commands.find(cmd) != commands.end())
      {
        std::cout <<  cmd << " is a shell builtin" << std::endl;
      }
      else
      {
        std::cout <<  cmd << ": not found" << std::endl;
      }
      continue;
    }

    std::cout << input << ": command not found" << std::endl;
  }
}
