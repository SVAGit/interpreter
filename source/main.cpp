#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"
#include "visitor.hpp"


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
    /* std::string input;
    std::getline(std::cin, input); */

    int file = open("code.txt", O_RDWR , 0666);
    off_t fileSize = lseek(file, 0, SEEK_END);
    lseek(file, 0, SEEK_SET);
    char input[fileSize + 1];
    ssize_t bytesRead = read(file, input, fileSize);
    input[bytesRead] = '\0';
    close(file);
    std::string str(input);
    std::cout << str <<std::endl;

    Lexer lexer(str);
    Parser parser(lexer.tokenize());
    parser.print_tokens();
    auto save = parser.parse();
    Printer printer;
    printer.print(save);
    Analyzer analyzer;
    analyzer.analyze(save);
    Executor executor;
    executor.execute(save);
    return 0;
}