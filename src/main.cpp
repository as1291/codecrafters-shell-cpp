#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

std::vector<std::string> commands = {"exit", "echo", "type"};

void callType(std::string param)
{
  auto it = std::find(std::begin(commands), std::end(commands), param);
  if (it != std::end(commands))
    std::cout << param << " is a shell builtin" << std::endl;
  else
  {
    std::string pathvar = std::getenv("PATH");
    std::istringstream path_stream(pathvar);
    std::string pathsplit;
    while (std::getline(path_stream, pathsplit, ':'))
    {
      std::string filepath = pathsplit + '/' + param;
      if (access(filepath.c_str(), X_OK) == 0)
      {
        std::cout << param << " is " << filepath << std::endl;
        return;
      }
    }
    std::cout << param << ": not found" << std::endl;
  }
}

void callEcho(std::string param)
{
  std::cout << param << std::endl;
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
  else
    std::cout << input << ": command not found" << std::endl;
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
