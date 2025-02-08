#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <filesystem>

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
    return UNKNOWN;
  }
  return (CMDS)cmds[cmdtype];
}

bool exists(const std::filesystem::path &path, const std::string &filename)
{
  if (!std::filesystem::exists(path))
  {
    return false;
  }

  for (const auto &entry : std::filesystem::recursive_directory_iterator(path))
  {
    if (entry.is_regular_file() && entry.path().filename() == filename)
    {
      return true;
    }
  }

  return false;
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

  const char *path = std::getenv("PATH");
  std::vector<std::string> paths = split(path, ':');

  while (true)
  {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> splitcmds = split(input, ' ');
    std::string cmdtype = splitcmds.at(0);

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
      std::string cmdpath = "";
      bool found = false;

      // check for shell built ins
      if (commands.find(cmd) != commands.end())
      {
        std::cout << cmd << " is a shell builtin" << std::endl;
        break;
      }

      // check in PATH dirs
      for (std::string p : paths)
      {
        std::filesystem::path search_path = p;
        if (exists(search_path, cmd))
        {
          cmdpath = p;
          break;
        }
      }

      if (!cmdpath.empty())
      {
        std::cout << cmd << " is " << cmdpath << "/" << cmd << std::endl;
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
