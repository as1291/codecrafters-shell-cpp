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
// #include <windows.h>
#include <filesystem>

std::vector<std::string> commands = {"exit", "echo", "type", "pwd"};

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

bool callPath(std::string param)
{
  std::vector<std::string> tokens;
  std::istringstream input(param);
  std::string word;
  while (input >> word)
  {
    tokens.push_back(word);
  }
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
  else
    waitpid(pid, nullptr, 0);
  return true;
}

void callType(std::string param)
{
  auto it = std::find(std::begin(commands), std::end(commands), param);
  if (it != std::end(commands))
    std::cout << param << " is a shell builtin" << std::endl;
  else
  {
    // std::string pathvar = std::getenv("PATH");
    // std::istringstream path_stream(pathvar);
    // std::string pathsplit;
    // while (std::getline(path_stream, pathsplit, ':'))
    // {
    //   std::string filepath = pathsplit + '/' + param;
    //   if (access(filepath.c_str(), X_OK) == 0)
    //   {
    //     std::cout << param << " is " << filepath << std::endl;
    //     return;
    //   }
    // }
    std::string filepath = findPath(param);
    if (!filepath.empty())
    {
      std::cout << param << " is " << filepath << std::endl;
      return;
    }
    else
      std::cout << param << ": not found" << std::endl;
    return;
  }
}

void callEcho(std::string param)
{
  std::cout << param << std::endl;
}

void callPwd()
{
  std::cout << std::filesystem::current_path().string() << std::endl;
}

void callCd(std::string path)
{
  // if(path.empty())
  // std::cout<<"empty file name"
  if(chdir(path.c_str())!=0){
    std::cout << "cd: " << path << ": No such file or directory" << std::endl;
  }
}

int REP()
{
  std::cout << "$ ";
  std::string input;
  std::getline(std::cin, input);
  if (input == "exit")
    return -1;

  std::string command = input.substr(0, input.find(' '));
  std::size_t paramIndex = input.find(' ');
  std::string param;
  if (paramIndex != std::string::npos)
    param = input.substr(paramIndex + 1);

  if (command == "exit")
    return -1;
  else if (command == "echo")
    callEcho(param);
  else if (command == "type")
    callType(param);
  else if (command == "pwd")
    callPwd();
  else if (command == "cd")
    callCd(param);
  else
  {
    if (!callPath(input))
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
