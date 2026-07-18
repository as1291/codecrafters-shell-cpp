#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <filesystem>

std::vector<std::string> allCommands = {"exit", "echo", "type", "pwd"};

std::vector<std::string> tokenize(const std::string &input)
{
  std::vector<std::string> tokens;
  bool isSingleQuote = false;
  std::string current = "";
  for (auto &s : input)
  {
    if (s == '\'')
      isSingleQuote = !isSingleQuote;
    else if (s == ' ' && !isSingleQuote)
    {
      if (!current.empty())
      {
        tokens.push_back(current);
        current.clear();
      }
    }
    else
    {
      current += s;
    }
  }

  if (!current.empty())
  {
    tokens.push_back(current);
  }
  return tokens;
}

std::string findPath(std::string filename)
{
  std::string pathvar = std::getenv("PATH");
  std::istringstream path_stream(pathvar);
  std::string pathsplit;
  while (std::getline(path_stream, pathsplit, ':'))
  {
    std::string filepath = pathsplit + '/' + filename;
    if (access(filepath.c_str(), X_OK) == 0)
    {
      return filepath;
    }
  }
  return "";
}

bool callPath(std::vector<std::string> &tokens)
{
  if (tokens.empty())
    return false;
  std::string filepath = findPath(tokens[0]);
  if (filepath.empty())
  {
    return false;
  }

  std::vector<char *> args;
  for (auto &token : tokens)
  {
    args.push_back(const_cast<char *>(token.c_str()));
  }
  args.push_back(nullptr);
  pid_t pid = fork();
  if (pid < 0)
  {
    perror("fork");
    return false;
  }
  if (pid == 0)
  {
    execv(filepath.c_str(), args.data());
    perror("execv");
    exit(1);
  }

  waitpid(pid, nullptr, 0);
  return true;
}

void callPwd()
{
  std::cout << std::filesystem::current_path().string() << std::endl;
  return;
}

void callEcho(const std::vector<std::string> &tokens)
{
  for (size_t i = 1; i < tokens.size(); i++)
  {
    std::cout << tokens[i];

    if (i + 1 != tokens.size())
      std::cout << " ";
  }
  std::cout << std::endl;
  return;
}

void callCd(const std::vector<std::string> &tokens)
{
  if (tokens.size() != 2)
    return;
  std::string path = tokens[1];
  if (path == "~")
  {
    char *homeDir = std::getenv("HOME");

    if (homeDir != nullptr)
      path = homeDir;
  }
  if (chdir(path.c_str()) != 0)
  {
    std::cout << "cd: " << path << ": No such file or directory" << std::endl;
  }
  return;
}

void callType(const std::vector<std::string> &tokens)
{
  if (tokens.size() < 2)
    return;
  for (size_t i = 1; i < tokens.size(); i++)
  {
    std::string param = tokens[i];
    auto it = std::find(std::begin(allCommands), std::end(allCommands), param);
    if (it != std::end(allCommands))
      std::cout << param << " is a shell builtin" << std::endl;
    else
    {
      std::string filepath = findPath(param);
      if (!filepath.empty())
      {
        std::cout << param << " is " << filepath << std::endl;
      }
      else
        std::cout << param << ": not found" << std::endl;
    }
  }
}

int REP()
{
  std::cout << "$ ";
  std::string input;
  std::getline(std::cin, input);
  std::vector<std::string> tokens = tokenize(input);
  if (tokens.empty())
    return 0;

  std::string command = tokens[0];

  if (command == "exit")
    return -1;
  else if (command == "echo")
    callEcho(tokens);
  else if (command == "type")
    callType(tokens);
  else if (command == "pwd")
    callPwd();
  else if (command == "cd")
    callCd(tokens);
  else
  {
    if (!callPath(tokens))
    {
      std::cout << command << ": command not found" << std::endl;
    }
  }
  return 0;
}

int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (1)
  {
    int returnval;
    returnval = REP();
    if (returnval == -1)
    {
      break;
    }
  }
}
