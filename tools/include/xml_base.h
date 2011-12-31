#ifndef XML_BASE
#define XML_BASE

#include <string.h>
#include <stdio.h>
#define XML_TEXTLEN     256
#define XML_LINE        2048

struct StringPair
{
    char key [XML_TEXTLEN];
    char value [XML_TEXTLEN];

    StringPair (const char * _key, const char * _value)
    {
        strcpy (key, _key);
        strcpy (value, _value);
    }
};

typedef StringPair pair;

class XMLElement
{
    private:
        int level;
        XMLElement * parent;
        int num_of_children;
        int current_num_of_children;
        XMLElement ** children;

        char element_name[XML_TEXTLEN];
        char element_text[XML_TEXTLEN];
        int num_of_attributes;
        pair * attributes;

        static void scanDimension (XMLElement * parent, FILE * fp);
        static void scanVariable (XMLElement * parent, FILE * fp);
        static void scanAttribute (XMLElement * parent, FILE * fp);
        static void scanNonLinedXMLElementName (char * elem_name, FILE * fp);
        static void scanLinedXMLElement (char * elem_name, char * elem_text, FILE * fp);

    public:
        XMLElement (const char * xml_file);
        XMLElement (const char * _element_name, int _num_of_children);
        XMLElement (const char * _element_name, const char * _element_text);
        ~XMLElement ();
        XMLElement * getParent ();
        int getNumberOfChildren ();
        XMLElement * getChild (const char * _element_name);
        XMLElement ** getChildren ();

        char *      getElementName ();
        char *      getElementText ();
        int         getNumberOfAttributes ();
        pair *      getAttributes ();

        void        addElement (XMLElement * child);
        void        addElementText (const char * text);

        void        print (FILE * fp);
        bool        check ();
        void        writeToFile (const char * xml);
};

#endif
