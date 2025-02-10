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

class Command
{
public:
  std::vector<std::string> paths;

  Command(std::vector<std::string> _paths)
  {
    paths = _paths;
  }

  virtual int execute(std::string &input)
  {
    return 0;
  }

  virtual bool should_exit()
  {
    return false;
  }
};

class Exit : public Command
{
public:
  using Command::Command;
  virtual int execute(std::string &input)
  {
    return 0;
  }
  virtual bool should_exit()
  {
    return true;
  }
};

class Echo : public Command
{
public:
  using Command::Command;
  virtual int execute(std::string &input)
  {
    std::cout << input.substr(6) << std::endl;
    return 0;
  }
};

class Type : public Command
{
public:
  using Command::Command;
  virtual int execute(std::string &input)
  {
    std::string cmd = input.substr(6, input.length() - 6);
    std::string cmdpath = "";
    bool found = false;
    /*
    // check for shell built ins
    if (commands.find(cmd) != commands.end())
    {
      std::cout << cmd << " is a shell builtin" << std::endl;
      return 0;
    }
    */

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
    return 0;
  }
};

class Unknown : public Command
{
public:
  using Command::Command;
  virtual int execute(std::string &input)
  {
    std::vector<std::string> splitcmds = split(input, ' ');
    std::string cmdtype = splitcmds.at(0);
    std::string cmdpath = "";
    bool found = false;

    // check in PATH dirs
    for (std::string p : paths)
    {
      std::filesystem::path search_path = p;
      if (exists(search_path, cmdtype))
      {
        cmdpath = p;
        break;
      }
    }

    if (!cmdpath.empty())
    {
      // std::string fullcmd = cmdpath + '/' + cmdtype;
      // splitcmds[0] = fullcmd;

      std::string fullcmd_args = join(splitcmds, " ");
      system(fullcmd_args.c_str());
    }
    else
    {
      std::cout << input << ": command not found" << std::endl;
    }
    return 0;
  }
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

Command getCmd(std::string cmdtype, std::unordered_map<std::string, Command &> cmds)
{
  return (Command)cmds[cmdtype];
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

std::string join(const std::vector<std::string> &vec, std::string delimeter)
{
  std::ostringstream stream;
  for (auto &elem : vec)
  {
    stream << elem << delimeter;
  }
  return stream.str();
}

int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::unordered_map<std::string, Command &> commands;
  const char *path = std::getenv("PATH");
  std::vector<std::string> paths = split(path, ':');

  commands["exit"] = Exit(paths);
  commands["echo"] = Echo(paths);
  commands["type"] = Type(paths);
  commands["unknown"] = Unknown(paths);

  while (true)
  {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> splitcmds = split(input, ' ');
    std::string cmdtype = splitcmds.at(0);

    Command c = getCmd(cmdtype, commands);
    c.execute(input);
    if (c.should_exit()){
      return 0;
    }

  }
}
