#include "Game.h"

using namespace std;

int main(int argc, char const *argv[])
{
    ifstream savefile;
    savefile.open("testfile");
    if(savefile.is_open()) {
        Game g = *new Game(savefile);
        g.showShop();
        //g.createField(5,5,"UwU TestFarm");
        //g.draw(0);
        //g.save("testfile");
    } else {
        cerr << "UWU ERROR";
    }
    return 0;
}
