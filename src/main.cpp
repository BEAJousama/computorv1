#include "Computor.hpp"

int main(int argc, char **argv)
{
    std::string equation;

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
            equation += argv[i];
    }
    else
    {
        std::cout << "Enter the equation:" << std::endl;
        std::getline(std::cin, equation);
    }

    try
    {
        Computor computor(equation);
        computor.solve();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
