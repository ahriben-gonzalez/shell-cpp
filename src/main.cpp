#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <filesystem>
#include <set>

std::set<std::string> builtin = {"exit", "echo", "type", "pwd", "cd"};

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

class Command
{
public:
  std::vector<std::string> paths;
  Command() = default;
  Command(std::vector<std::string> _paths) : paths(_paths) {};

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
    std::cout << input.substr(5) << std::endl;
    return 0;
  }
};

class Type : public Command
{
public:
  using Command::Command;
  virtual int execute(std::string &input)
  {
    std::string cmd = input.substr(5, input.length() - 5);
    std::string cmdpath = "";
    bool found = false;

    // check for shell built ins
    if (builtin.find(cmd) != builtin.end())
    {
      std::cout << cmd << " is a shell builtin" << std::endl;
      return 0;
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

class Pwd : public Command
{
public:
  using Command::Command;
  virtual int execute(std::string &input)
  {
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::cout << currentPath.string() << std::endl;
    return 0;
  }
};

class Cd : public Command
{
public:
  using Command::Command;
  virtual int execute(std::string &input)
  {
    std::vector<std::string> splitcmds = split(input, ' ');
    std::string path = splitcmds.at(1);
    if (path == "~"){
      std::filesystem::current_path(getenv("HOME"));
      return 0;
    }

    std::filesystem::path path_to_check(path);

    if (!std::filesystem::exists(path_to_check))
    {
      std::cout << "cd: " << path << ": No such file or directory" << std::endl;
      return 0;
    }

    std::filesystem::current_path(path);

    return 0;
  }
};

void freecommands(std::unordered_map<std::string, Command *> commands)
{
  for (const auto &pair : commands)
  {
    delete pair.second;
  }
}

int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::unordered_map<std::string, Command *> commands;
  const char *path = std::getenv("PATH");
  std::vector<std::string> paths = split(path, ':');

  commands["exit"] = new Exit(paths);
  commands["echo"] = new Echo(paths);
  commands["type"] = new Type(paths);
  commands["unknown"] = new Unknown(paths);
  commands["pwd"] = new Pwd(paths);
  commands["cd"] = new Cd(paths);

  while (true)
  {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> splitcmds = split(input, ' ');
    std::string cmdtype = splitcmds.at(0);

    if (commands.find(cmdtype) == commands.end())
    {
      commands["unknown"]->execute(input);
      continue;
    }
    commands[cmdtype]->execute(input);
    if (commands[cmdtype]->should_exit())
    {
      freecommands(commands);
      return 0;
    }
  }
}
