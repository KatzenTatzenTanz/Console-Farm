#include <sstream>
#include <time.h>
#include "StringTokenizer.h"

using namespace std;


/*####################################################*\
#......................................................#
#...CROP DATA..........................................#
#......here crops are handled, from storage to.........#
#......saving and creation.............................#
#......................................................#
\*####################################################*/

struct Crop {
    int ID;
    int stage;
    long long lastGrowTime;

    /**
     * Create a standard filler crop that will not appear on screen (ID = -1)
    */
    Crop() {
        ID = -1;
        stage = 0;
        lastGrowTime = 0;
    };

    /**
     * use this for planting a Crop
     * @param ID ID of a flower defined in CropData
    */
    Crop(int ID) {
        this->ID = ID;
        stage = 0;
        lastGrowTime = time(NULL);
    }

    /**
     * Create a new Crop object given the root token
     * @param token The root token of this object
    */
    Crop(Token root) {
        ID = stoi(root.getValue("ID")->value);
        stage = stoi(root.getValue("Stage")->value);
        lastGrowTime = stoi(root.getValue("Lastgrow")->value);
    }

    /**
     * Creates a json esque string of this object
     * @return a json esque string of this object
    */
    string toString() {
        string* objects = new string[3]
        {
            toPrimitive("ID",to_string(ID)),
            toPrimitive("Stage",to_string(stage)),
            toPrimitive("Lastgrow",to_string(lastGrowTime))
        };
        return toObjectInline(objects,3 );
    }
};


/*####################################################*\
#......................................................#
#...FIELD DATA.........................................#
#......here fields are handled, from storage to........#
#......saving and creation.............................#
#......................................................#
\*####################################################*/

struct Field {
    int w,h;
    Crop* data;
    string name;

    Field() {};

    /**
     * Creates an empty field (Crop.ID = -1)
     * @param w width of the Field
     * @param h height of the Field
    */
    Field(int w, int h) {
        this->w = w;
        this->h = h;
        data = new Crop[w*h];
    }

    /**
     * Create a new Field object given the root token
     * @param token The root token of this object
    */
    Field(Token root) {
        w = stoi(root.getValue("W")->value);
        h = stoi(root.getValue("H")->value);
        name = root.getValue("Name")->value;

        //get all crops in the field
        data = new Crop[w*h];
        Token* dataToken = root.getValue("Data");
        for(int i = 0; i < w*h; ++i) {
            data[i] = *new Crop(dataToken->data[i]);
        }
    }

    Crop getCrop(int x, int y) {
        return data[y*w+x];
    }
    void setCrop(int x, int y, Crop data) {
        (*this).data[y*w+x] = data;
    }

    /**
     * Creates a json esque string of this object
     * @return a json esque string of this object
    */
    string toString() {
        string* objects = new string[4]
        {
            toPrimitive("Name",name),
            toPrimitive("H",to_string(h)),
            toPrimitive("W",to_string(w))
        };

        //create array of crops
        string* dataStr = new string[w*h];
        for(int i = 0; i < w*h; ++i)
            dataStr[i] = data[i].toString();
        objects[3] = toArray("Data",dataStr,w*h);
        //combine and return
        return toObject(objects,4);
    }
};


/*####################################################*\
#......................................................#
#...GAME DATA..........................................#
#......Here we create and excecute the general Game....#
#......logic aswell as tokenizing the field saves......#
#......................................................#
\*####################################################*/


/*#...Crops Handling......................#...#..#.###*/

struct CropData {

    string stages;
    string* mods;
    int oModsLen;

    int stageUpMin;
    int stageUpMax;

    int value;
    int price;
    
    string name;

    CropData() {};

    /**
     * Create a new CropData object given the root token
     * @param token The root token of this object
    */
    CropData(Token root) {
        stageUpMin = stoi(root.getValue("MinTime")->value);
        stageUpMax = stoi(root.getValue("MaxTime")->value);
        value = stoi(root.getValue("Sell")->value);
        price = stoi(root.getValue("Buy")->value);
        name = root.getValue("Name")->value;
        stages = root.getValue("Stages")->value;

        //fill the array of mods that will be applied to [stages]
        Token* mo = root.getValue("Mods");
        mods = new string[stages.length()];
        oModsLen = mo->data.size();
        for(int i = 0; i < stages.length(); ++i) {
            mods[i] = (i < oModsLen)?mo->data[i].value:"0";
        }
    }

    /**
     * Creates a json esque string of this object
     * @return a json esque string of this object
    */
    string toString() {
        string* objects = new string[7]
        {
            toPrimitive("Name",name),
            toPrimitive("Stages",stages),
            "",
            toPrimitive("MinTime",to_string(stageUpMin)),
            toPrimitive("MaxTime",to_string(stageUpMin)),
            toPrimitive("Sell",to_string(value)),
            toPrimitive("Buy",to_string(price)),
        };

        //create array of mods
        string* dataStr = new string[oModsLen];
        for(int i = 0; i < oModsLen; ++i)
            dataStr[i] = mods[i];
        objects[3] = toArray("Data",dataStr,oModsLen);
        //combine and return
        return toObject(objects,4);
    }
};


/*#...Game Backend........................#...#..#.###*/

struct Game {

    string cropTypePath;
    int money;

    int fieldPrice;

    Field* fields;
    int fieldsC;
    
    CropData* cropTypes;
    int cropTypesC;

    Game();

    /**
     * Create a new Game object given the root of the object
     * @param token The root token of this object
    */
    Game(ifstream& str) {
        ObjectToken root = tokenize(str);
        //ok c++ you are the special child
        new (this) Game(root.data.data());
    }

    /**
     * Create a new Game object given the root token
     * in most cases Game(ifstream& str) is recomended
     * @param token The root token of this object
    */
    Game(Token* root) {
        cropTypePath = root->getValue("CropPath")->value;
        createCropTypes();
        money = stoi(root->getValue("Money")->value);
        fieldPrice = stoi(root->getValue("FieldPrice")->value);
        //Create all fields
        Token* FieldSelector = root->getValue("Fields");
        fieldsC = FieldSelector->data.size();
        fields = new Field[fieldsC];
        int j = -1;
        for(Token t : FieldSelector->data) {
            fields[++j] = *new Field(t);
        }
    }

    void createCropTypes() {
        ifstream savefile;
        savefile.open(cropTypePath);
        if(savefile.is_open()) {
            //Create different types of crops
            ObjectToken DT = tokenize(savefile);
            Token* data = DT.getValue("Plants");
            cropTypesC = data->data.size();
            cout << data->value;
            cropTypes = new CropData[cropTypesC];
            int i = -1;
            for(Token t : data->data)
                cropTypes[++i] = *new CropData(t);
        } else {
            cerr << "Could not open CropPath at: " << cropTypePath << endl;
            terminate();
        }
    }

    /**
     * use Game::toString() and toArray() to save multiple objects into one file
     * @param fileName Name of the file that this object should be stored to
    */
    void save(string fileName) {
        ofstream savefile;
        savefile.open(fileName);
        if(savefile.is_open()) {
            savefile << toString();
        }
        savefile.flush();
        savefile.close();
    }

    /**
     * Creates a json esque string of this object
     * @return a json esque string of this object
    */
    string toString() {
        string* objects = new string[4]
        {
            toPrimitive("CropPath",cropTypePath),
            toPrimitive("Money",to_string(money)),
            toPrimitive("FieldPrice",to_string(fieldPrice)),
        };

        //create array of all Fields
        string* dataStr = new string[fieldsC];
        for(int i = 0; i < fieldsC; ++i)
            dataStr[i] = fields[i].toString();
        objects[3] = toArray("Fields",dataStr,fieldsC);

        //combine all and return
        return toObject(objects,4);
    }

    /* Anything below this is dependant on the implementation
    into the game and is therefore undefined in this file */

    void draw(int field);

    void update(int field);

    void createField(int w, int h, string name);
    void removeField(int field);

    void setCrop(int field, int ID, int x, int y);
    void setCrop(int field, int ID, int x, int y, int w, int h);

    void harvestCrop(int field, int ID, int x, int y);
    void harvestCrop(int field, int ID, int x, int y, int w, int h);

    void showShop();
};