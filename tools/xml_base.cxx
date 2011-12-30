#include "xml_base.h"

XMLElement::XMLElement (const char * xml_file)
{
}

XMLElement::XMLElement (const char * _element_name, int _num_of_children)
{
    level = 0;
    parent = (XMLElement *) 0;
    num_of_children = _num_of_children;
    current_num_of_children = 0;
    children = new XMLElement * [num_of_children];
    strcpy (element_name, _element_name);
    element_text[0] = '\0';
    num_of_attributes = 0;
    attributes = (pair *) 0;
}

XMLElement::XMLElement (const char * _element_name, const char * _element_text)
{
    level = 0;
    parent = (XMLElement *) 0;
    num_of_children = 0;
    strcpy (element_name, _element_name);
    strcpy (element_text, _element_text);
    num_of_attributes = 0;
    attributes = (pair *) 0;
}

XMLElement::~XMLElement ()
{
    delete children;
}

XMLElement * XMLElement::getParent ()
{
    return parent;
}

int XMLElement::getNumberOfChildren ()
{
    return num_of_children;
}

XMLElement * XMLElement::getChild (const char * _element_name)
{
    for (int i = 0; i < current_num_of_children; i ++)
    {
        if (strcmp (_element_name, children[i]->element_name) == 0)
            return children[i];
    }
    return (XMLElement *) 0;
}

XMLElement ** XMLElement::getChildren ()
{
    return children;
}

char * XMLElement::getElementName ()
{
    return element_name;
}

char * XMLElement::getElementText ()
{
    return element_text;
}

pair * XMLElement::getAttributes ()
{
    return attributes;
}

void XMLElement::addElement (XMLElement * child)
{
    children[current_num_of_children++] = child;
    child->parent = this;
    child->level = this->level + 1;
}

void XMLElement::addElementText (const char * text)
{
    strcpy (element_text, text);
}

void XMLElement::print (FILE * fp)
{
    char format[XML_TEXTLEN];
    format[0] = '\0';
    for (int i = 0; i < level; i ++)
        strcat (format, "\t");
    if (num_of_children == 0)
    {
        fprintf (fp, "%s<%s>%s</%s>\n", format, element_name, element_text, element_name);
    }
    else
    {
        fprintf (fp, "%s<%s>\n", format, element_name);
        for (int i = 0; i < num_of_children; i ++)
            children[i]->print (fp);
        fprintf (fp, "%s</%s>\n", format, element_name);
    }
}

bool XMLElement::check ()
{
    if (this == (XMLElement *) 0)
        return false;
    if (num_of_children == 0)
        return true;
    for (int i = 0; i < num_of_children; i ++)
        if (children[i] == (XMLElement *) 0 || !children[i]->check())
        {
            printf ("level = %d\n", this->level);
            printf ("name = %s\n", this->element_name);
            return false;
        }
    return true;
}

void XMLElement::writeToFile (const char * xml)
{
    if (level != 0)
    {
        printf ("writeToFile must be called by root element\n");
        return;
    }
    FILE * fp;
    fp = fopen (xml, "w");
    fprintf (fp, "%s\n", "<?xml version=\"1.0\"?>");
    print (fp);
    fclose (fp);
}
