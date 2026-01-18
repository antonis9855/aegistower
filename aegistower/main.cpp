#include "sgg/graphics.h"
#include "globalstate.h"
// Entry point pou trexei to game
int main()
{
    Globalstate::getInstance()->Run();
    return 0;
}
