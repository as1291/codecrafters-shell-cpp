#include <iostream>
#include <string>

int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage
  while (true)
  {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);
    std::string command = input.substr(0,input.find(' '));
    std::string param;

    const std::size_t paramIndex = input.find(' ')+1;
    if(paramIndex!=std::string::npos){
      param=input.substr(paramIndex);
    }

    if(command=="exit") break;
    else if(command=="type"){
      if(param=="echo" || param=="exit" || param=="type") {
        std::cout<<param<< " is a shell builtin" << std::endl;
      }else
      {
        std::cout << param << ": not found" << std::endl;
      }
      
    }
    else if(command=="echo") std::cout<<param << std::endl;
    else std::cout << input << ": command not found" << std::endl;
  }
}
