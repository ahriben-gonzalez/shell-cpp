#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

typedef enum CMDS
{
  EXIT,
  ECHO,
  TYPE,
  UNKNOWN
};

std::vector<std::string> split(const std::string &s, char delim)
{
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim))
  {
    elems.push_back(item);
  }
  return elems;
}

CMDS getCmd(std::string cmdtype, std::unordered_map<std::string, int> cmds)
{
  if (cmds.find(cmdtype) == cmds.end())
  {
    return (CMDS)cmds[cmdtype];
  }
  return UNKNOWN;
}

int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::unordered_map<std::string, int> commands;
  commands["exit"] = EXIT;
  commands["echo"] = ECHO;
  commands["type"] = TYPE;

  while (true)
  {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> splitcmds = split(input, ' ');
    std::string cmdtype = splitcmds.at(0);
    std::cout << cmdtype << ": not found" << std::endl;
    CMDS c = getCmd(cmdtype, commands);

    switch (c)
    {
    case EXIT:
    {
      return 0;
    }
    case ECHO:
    {
      std::cout << input.substr(cmdtype.length() + 1) << std::endl;
      break;
    }
    case TYPE:
    {
      std::string cmd = input.substr(cmdtype.length() + 1, input.length() - (cmdtype.length() + 1));
      if (commands.find(cmd) != commands.end())
      {
        std::cout << cmd << " is a shell builtin" << std::endl;
      }
      else
      {
        std::cout << cmd << ": not found" << std::endl;
      }
      break;
    }
    case UNKNOWN:
    {
      std::cout << input << ": command not found" << std::endl;
      break;
    }
    default:
    {
      break;
    }
    }
  }
}
