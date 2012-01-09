#include "xml_base.h"
#include <assert.h>
void XMLElement::scanNonLinedXMLElementName (char * elem_name, FILE * fp)
{
    assert (elem_name != (char *) 0);
    char line[XML_LINE];
    char * bra, * ket;

    fgets (line, XML_LINE, fp);
    assert (line != (char *) 0);
    //printf ("scanNonLinedXMLElementName==>line: %s\n", line);

    bra = strchr (line, '<');
    ket = strchr (line, '>');
    assert (bra != (char *) 0 && ket != (char *) 0);
    strncpy (elem_name, bra+1, ket-bra-1);
    elem_name[ket-bra-1] = '\0';
}

void XMLElement::scanLinedXMLElement (char * elem_name, char * elem_text, FILE * fp)
{
    assert (elem_name != (char *) 0 && elem_text != (char *) 0);
    char line[XML_LINE];
    char * bra, * ket;

    fgets (line, XML_LINE, fp);
    assert (line != (char *) 0);
    //printf ("scanLinedXMLElement==>line: %s\n", line);

    bra = strchr (line, '<');
    ket = strchr (line, '>');
    assert (bra != (char *) 0 && ket != (char *) 0);
    strncpy (elem_name, bra+1, ket-bra-1);
    elem_name[ket-bra-1] = '\0';

    bra = strchr (ket, '<');
    assert (bra != (char *) 0);
    strncpy (elem_text, ket+1, bra-ket-1);
    elem_text[bra-ket-1] = '\0';
    //printf ("scanLinedXMLElement==>elem_name: %s\n", elem_name);
    //printf ("scanLinedXMLElement==>elem_text: %s\n", elem_text);
}

void XMLElement::scanDimension (XMLElement * parent, FILE * fp)
{
    assert (parent != (XMLElement *) 0);
    int ndim = parent->num_of_children;
    char elem_name[XML_TEXTLEN];
    char elem_text[XML_TEXTLEN];

    XMLElement * node;
    for (int i = 0; i < ndim; i ++)
    {
        parent->addElement (new XMLElement ("dimension", 5));
        node = parent->getChildren ()[i];
        assert (node != (XMLElement *) 0);

        scanNonLinedXMLElementName (elem_name, fp);                         /** dimension **/

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* name */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* gname */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* size */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* action */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* state */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanNonLinedXMLElementName (elem_name, fp);                         /** dimension **/
    }
}

void XMLElement::scanVariable (XMLElement * parent, FILE * fp)
{
    assert (parent != (XMLElement *) 0);
    int nvar = parent->num_of_children;
    char elem_name[XML_TEXTLEN];
    char elem_text[XML_TEXTLEN];
    int size;

    XMLElement * node;
    XMLElement * subnode;
    XMLElement * subsubnode;
    for (int i = 0; i < nvar; i ++)
    {
        parent->addElement (new XMLElement ("variable", 9));
        node = parent->getChildren ()[i];
        assert (node != (XMLElement *) 0);

        scanNonLinedXMLElementName (elem_name, fp);                         /** variable **/

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* name */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* gname */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* type */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* ndim */
        node->addElement (new XMLElement (elem_name, elem_text));

        sscanf (elem_text, "%d", &size);
        scanNonLinedXMLElementName (elem_name, fp);                         /** dimensions_of_variable **/
        node->addElement (new XMLElement (elem_name, size));
        subnode = node->getChild (elem_name);
        assert (subnode != (XMLElement *) 0);
        for (int j = 0; j < size; j ++) 
        {
            scanLinedXMLElement (elem_name, elem_text, fp);
            subnode->addElement (new XMLElement (elem_name, elem_text));
        }
        scanNonLinedXMLElementName (elem_name, fp);                         /** dimensions_of_variable **/

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* natts */
        node->addElement (new XMLElement (elem_name, elem_text));

        sscanf (elem_text, "%d", &size);
        if (size != 0)
        {
            scanNonLinedXMLElementName (elem_name, fp);                     /** attributes_of_variable **/
            node->addElement (new XMLElement (elem_name, size));
            subnode = node->getChild (elem_name);
            assert (subnode != (XMLElement *) 0);
            for (int j = 0; j < size; j ++)
            {
                scanNonLinedXMLElementName (elem_name, fp);                         /** attribute **/
                subnode->addElement (new XMLElement (elem_name, 4));
                subsubnode = subnode->getChildren () [j];
                assert (subsubnode != (XMLElement *) 0);

                scanLinedXMLElement (elem_name, elem_text, fp);                     /* key */
                subsubnode->addElement (new XMLElement (elem_name, elem_text));
                scanLinedXMLElement (elem_name, elem_text, fp);                     /* value */
                subsubnode->addElement (new XMLElement (elem_name, elem_text));
                scanLinedXMLElement (elem_name, elem_text, fp);                     /* action */
                subsubnode->addElement (new XMLElement (elem_name, elem_text));
                scanLinedXMLElement (elem_name, elem_text, fp);                     /* state */
                subsubnode->addElement (new XMLElement (elem_name, elem_text));

                scanNonLinedXMLElementName (elem_name, fp);                         /** attribute **/
            }
            scanNonLinedXMLElementName (elem_name, fp);                     /** attributes_of_variable **/
        }
        else
        {
            scanLinedXMLElement (elem_name, elem_text, fp);                 /** attributes_of_variable (null) **/
            node->addElement (new XMLElement (elem_name, ""));
        }

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* action */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* state */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanNonLinedXMLElementName (elem_name, fp);                         /** variable **/
    }
}

void XMLElement::scanAttribute (XMLElement * parent, FILE * fp)
{
    assert (parent != (XMLElement *) 0);
    int natts = parent->num_of_children;
    char elem_name[XML_TEXTLEN];
    char elem_text[XML_TEXTLEN];

    XMLElement * node;
    for (int i = 0; i < natts; i ++)
    {
        parent->addElement (new XMLElement ("global_attribute", 4));
        node = parent->getChildren ()[i];
        assert (node != (XMLElement *) 0);

        scanNonLinedXMLElementName (elem_name, fp);                         /** global_attribute **/

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* key */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* value */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* action */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanLinedXMLElement (elem_name, elem_text, fp);                     /* state */
        node->addElement (new XMLElement (elem_name, elem_text));

        scanNonLinedXMLElementName (elem_name, fp);                         /** global_attribute **/
    }
}

XMLElement::XMLElement (const char * xml_file)
{
    XMLElement * node, * subnode, * subsubnode;
    int ndim, nvar, natts;
    char line[XML_LINE];
    char elem_name[XML_TEXTLEN];
    char elem_text[XML_TEXTLEN];

    FILE * fp = fopen (xml_file, "r");
    if (fp == (FILE *) 0)
        printf ("Can not open file %s\n", xml_file);
    else
    {
        level = 0;
        parent = (XMLElement *) 0;
        num_of_children = 3;
        current_num_of_children = 0;
        children = new XMLElement * [num_of_children];

        // read XML version
        fgets (line, XML_LINE, fp);                                     /* <?xml version="1.0"?> */
        //printf ("line: %s\n", line);
        // read root
        scanNonLinedXMLElementName (elem_name, fp);
        //printf ("netcdf: %s\n", elem_name);
        strcpy (element_name, elem_name);
        element_text[0] = '\0';

        // read dimensions
        scanNonLinedXMLElementName (elem_name, fp);                     /** dimension_list **/
        //printf ("dimension_list: %s\n", elem_name);
        scanLinedXMLElement (elem_name, elem_text, fp);                 /* ndim */
        sscanf (elem_text, "%d", &ndim);
        //printf ("number of dimensions is %d\n", ndim);
        this->addElement (new XMLElement ("dimension_list", ndim));
        node = this->getChildren ()[0];
        scanDimension (node, fp);                                       /* for each dimension; use scanLinedXMLElement */        
        scanNonLinedXMLElementName (elem_name, fp);                     /** dimension_list **/

        // read variables
        scanNonLinedXMLElementName (elem_name, fp);                     /** variable_list **/
        //printf ("variable_list: %s\n", elem_name);
        scanLinedXMLElement (elem_name, elem_text, fp);                 /* nvar */
        sscanf (elem_text, "%d", &nvar);
        //printf ("number of variables is %d\n", nvar);
        this->addElement (new XMLElement ("variable_list", nvar));
        node = this->getChildren ()[1];
        scanVariable (node, fp);                                        /* for each variable; use scanLinedXMLElement and scanNonLinedXMLElementName */
        scanNonLinedXMLElementName (elem_name, fp);                     /** variable_list **/
        
        // read global attributes
        scanNonLinedXMLElementName (elem_name, fp);                     /** global_attribute_list **/
        //printf ("global_attribute_list: %s\n", elem_name);
        scanLinedXMLElement (elem_name, elem_text, fp);                 /* natts */
        sscanf (elem_text, "%d", &natts);
        //printf ("number of global attributes is %d\n", natts);
        this->addElement (new XMLElement ("global_attribute_list", natts));
        node = this->getChildren ()[2];
        scanAttribute (node, fp);                                       /* for each attribute; use scanLinedXMLElement and scanNonLinedXMLElementName */
        scanNonLinedXMLElementName (elem_name, fp);                     /** global_attribute_list **/

        // finish root
        scanNonLinedXMLElementName (elem_name, fp);
        //printf ("netcdf: %s\n", elem_name);
        fclose (fp);
    }
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
    if (this == (XMLElement *) 0)
        printf ("Current XMLElement is not allocated !\n");
    assert (this != (XMLElement *) 0 && current_num_of_children < num_of_children);
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
