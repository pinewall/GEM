#include "xml_base.h"

int main (int argc, char ** argv)
{
    if (argc < 3)
    {
        printf ("Usage: ./xml_test input.xml output.xml\n");
        return -1;
    }
    XMLElement * root = new XMLElement ("root", 2);
    XMLElement * date = new XMLElement ("date", 0);
    XMLElement * time = new XMLElement ("time", 0);
    root->addElement (date);
    root->addElement (time);
    date->addElementText ("2011-12-29");
    time->addElementText ("19:10:32");
    root->writeToFile (argv[2]);

    int size = 10;
    char str_size[10];
    sprintf (str_size, "%d", size);
    printf ("str_size: %s\n", str_size);

    XMLElement * cdf = new XMLElement (argv[1]);
    cdf->writeToFile (argv[2]);
    return 0;
}
