#include "xml_base.h"

int main ()
{
    XMLElement * root = new XMLElement ("root", 2);
    XMLElement * date = new XMLElement ("date", 0);
    XMLElement * time = new XMLElement ("time", 0);
    root->addElement (date);
    root->addElement (time);
    date->addElementText ("2011-12-29");
    time->addElementText ("19:10:32");
    root->writeToFile ("sample.xml");

    int size = 10;
    char str_size[10];
    sprintf (str_size, "%d", size);
    printf ("str_size: %s\n", str_size);
    return 0;
}
