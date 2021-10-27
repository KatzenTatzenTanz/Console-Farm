#include "types.h"


/*####################################################*\
#......................................................#
#...GAME LOGIC.........................................#
#......Here we handle Game requests, such as drawing,..#
#......placing, selling and updating of the crops......#
#......................................................#
\*####################################################*/


void printField(Field f, CropData cropTypes[]) {

    /*
    design:
    
    ######
    #    #
    #    #
    #    #
    #    #
    #    #
    ######

    */

    /*#...Edge Creation...................#...#..#.###*/

    string* edge = new string[3]{"\033[48;2;24;24;24m    ","\033[48;2;24;24;24m\033[38;2;122;122;255m ###","\033[48;2;24;24;24m\033[38;2;122;122;255m #  "};
    for(int x = 0; x < f.w; ++x) {
        edge[0].push_back(' ');
        edge[1].push_back('#');
        edge[2].push_back(' ');
    }
    edge[0] += "    \033[0m\n";
    edge[1] += "### \033[0m\n";
    edge[2] += "  # \033[0m\n";
    cout << f.name << endl << edge[0] << edge[1] << edge[2];
    for(int y = 0; y < f.h; ++y)
        cout << edge[2];
    cout << edge[2] << edge[1] << edge[0] << "\033[" + to_string(f.h + 3) + "A" + "\033[4C";

    /*#...Field Creation..................#...#..#.###*/

    for(int y = 0; y < f.h; ++y) {
        for(int x = 0; x < f.w; ++x) {
            Crop c = f.getCrop(x,y);

            cout << ((((x+y)%2)==0)?"\033[48;2;36;36;36m":"\033[48;2;48;48;48m"); 

            if(c.ID != -1)
                cout << "\033[1;"+cropTypes[c.ID].mods[c.stage]+"m" << cropTypes[c.ID].stages[c.stage];
            else
                cout << " ";
        }
        cout << endl << "\033[4C";
    }
    cout << "\033[0m\n\n\n";
}

/**
 * @param field index of a field, 0 <= field < fieldsC
 * draws the field of the Game into the console 
*/
void Game::draw(int field) {
    cout << "$: \033[33m" << money << "\033[0m\n";
    Field f = fields[field];
    printField(f,cropTypes);
}

/**
 * @param w width of the to be created field
 * @param w height of the to be created field
 * @param name Identifying name of the to be created field (THIS IS NOT THE INDEX)
 * this will push a new field of width w, height h to the end of the field Stack.
*/
void Game::createField(int w, int h, string name) {
    cout << "Do you want to create a farm called \"" + name + "\" for " + to_string(w*h*fieldPrice) + "$ (press y to contiue, anything else to abort):\t";
    char s;
    cin >> s;
    if(s != 'y') {
        cout << "Aborted\n";
        return;
    }
    if(w > 40 || h > 30 || w < 0 || h < 0) {
        cerr << "OUT OF RANGE 0 < w < 40 & 0 < h < 30";
        return;
    }
    if(money < w*h*fieldPrice) {
        cerr << "TOO LITTLE MONEY";
        return;
    }
    money -= w*h*fieldPrice;
    Field* nf = new Field[fieldsC+1];
    for(int i = 0; i < fieldsC; ++i) {
        nf[i] = fields[i];
    }
    nf[fieldsC] = *new Field(w,h);
    nf[fieldsC].name = name;
    fields = nf;
    ++fieldsC;
}

void Game::showShop() {
    cout << "$: \033[33m" << money << "\033[0m\n";
    for (int i = 0; i < cropTypesC; ++i) {
        string* data = new string[5] {
            "\033[31mPrice\033[0m$" + to_string(cropTypes[i].price),
            "\033[38;2;255;255;0mValue\033[0m$"+to_string(cropTypes[i].value),
            "\033[32mStages\033[0m: "+to_string(cropTypes[i].stages.length()),
            
            "\033[35mMaxGrowLength\033[0m: "+to_string(cropTypes[i].stages.length() * cropTypes[i].stageUpMax/60.).substr(0,3)+" min",
            "\033[35mMinGrowLength\033[0m: "+to_string(cropTypes[i].stages.length() * cropTypes[i].stageUpMin/60.).substr(0,3)+" min",
        };
        cout << "\033[34m" + cropTypes[i].name + "\033[0m: " + toObject(data,5) << endl;
    }
    
}

void Game::setCrop(int field, int ID, int x, int y) {
    //TODO:
}