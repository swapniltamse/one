/* -------------------------------------------------------------------------- */
/* Copyright 2002-2011, OpenNebula Project Leads (OpenNebula.org)             */
/*                                                                            */
/* Licensed under the Apache License, Version 2.0 (the "License"); you may    */
/* not use this file except in compliance with the License. You may obtain    */
/* a copy of the License at                                                   */
/*                                                                            */
/* http://www.apache.org/licenses/LICENSE-2.0                                 */
/*                                                                            */
/* Unless required by applicable law or agreed to in writing, software        */
/* distributed under the License is distributed on an "AS IS" BASIS,          */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   */
/* See the License for the specific language governing permissions and        */
/* limitations under the License.                                             */
/* -------------------------------------------------------------------------- */

#ifndef VMTEMPLATE_H_
#define VMTEMPLATE_H_

#include "PoolObjectSQL.h"
#include "VirtualMachineTemplate.h"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

/**
 *  The VMTemplate class.
 */
class VMTemplate : public PoolObjectSQL
{
public:

    /**
     *  Function to write a VMTemplate on an output stream
     */
    friend ostream& operator<<(ostream& os, VMTemplate& u);

    /**
     * Function to print the VMTemplate object into a string in XML format
     *  @param xml the resulting XML string
     *  @return a reference to the generated string
     */
    string& to_xml(string& xml) const;

    /**
     *  Returns true if the object is public
     *     @return true if the Virtual Network is public
     */
    bool isPublic()
    {
        return (public_template == 1);
    };

    /**
     *  Publish or unpublish an object
     *    @param pub true to publish the object
     *    @return 0 on success
     */
    bool publish(bool pub)
    {
        if (pub == true)
        {
            public_template = 1;
        }
        else
        {
            public_template = 0;
        }

        return true;
    };

    // ------------------------------------------------------------------------
    // Template Contents
    // ------------------------------------------------------------------------

    VirtualMachineTemplate * get_template_contents() const
    {
        return new VirtualMachineTemplate(*template_contents);
    }

    /**
     *  Gets the values of a template attribute
     *    @param name of the attribute
     *    @param values of the attribute
     *    @return the number of values
     */
    int get_template_attribute(
        string& name,
        vector<const Attribute*>& values) const
    {
        return template_contents->get(name,values);
    };

    /**
     *  Gets the values of a template attribute
     *    @param name of the attribute
     *    @param values of the attribute
     *    @return the number of values
     */
    int get_template_attribute(
        const char *name,
        vector<const Attribute*>& values) const
    {
        string str=name;
        return template_contents->get(str,values);
    };

    /**
     *  Gets a string based attribute
     *    @param name of the attribute
     *    @param value of the attribute (a string), will be "" if not defined
     */
    void get_template_attribute(
        const char *    name,
        string&         value) const
    {
        string str=name;
        template_contents->get(str,value);
    };

    /**
     *  Gets a string based attribute
     *    @param name of the attribute
     *    @param value of the attribute (an int), will be 0 if not defined
     */
    void get_template_attribute(
        const char *    name,
        int&            value) const
    {
        string str=name;
        template_contents->get(str,value);
    };

    /**
     *  Removes an attribute
     *    @param name of the attribute
     */
    int remove_template_attribute(const string&   name)
    {
        return template_contents->erase(name);
    };

    /**
     *  Adds a new attribute to the template (replacing it if
     *  already defined), the object's mutex SHOULD be locked
     *    @param name of the new attribute
     *    @param value of the new attribute
     *    @return 0 on success
     */
    int replace_template_attribute(
        const string& name,
        const string& value)
    {
        SingleAttribute * sattr;

        template_contents->erase(name);

        sattr = new SingleAttribute(name,value);
        template_contents->set(sattr);

        return 0;
    };

private:
    // -------------------------------------------------------------------------
    // Friends
    // -------------------------------------------------------------------------

    friend class VMTemplatePool;

    // -------------------------------------------------------------------------
    // VMTemplate Attributes
    // -------------------------------------------------------------------------

    /**
     *  Owner's name
     */
    string      user_name;

    /**
     *  The Virtual Machine template, holds the VM attributes.
     */
    VirtualMachineTemplate* template_contents;

    /**
     *  Public scope of the VMTemplate
     */
    int         public_template;

    /**
     *  Registration time
     */
    time_t      regtime;

    // *************************************************************************
    // DataBase implementation (Private)
    // *************************************************************************

    /**
     *  Execute an INSERT or REPLACE Sql query.
     *    @param db The SQL DB
     *    @param replace Execute an INSERT or a REPLACE
     *    @return 0 one success
     */
    int insert_replace(SqlDB *db, bool replace);

    /**
     *  Bootstraps the database table(s) associated to the VMTemplate
     */
    static void bootstrap(SqlDB * db)
    {
        ostringstream oss(VMTemplate::db_bootstrap);

        db->exec(oss);
    };

    /**
     *  Rebuilds the object from an xml formatted string
     *    @param xml_str The xml-formatted string
     *
     *    @return 0 on success, -1 otherwise
     */
    int from_xml(const string &xml_str);

protected:

    // *************************************************************************
    // Constructor
    // *************************************************************************
    VMTemplate(int id, int uid, string _user_name,
                   VirtualMachineTemplate * _template_contents);

    ~VMTemplate();

    // *************************************************************************
    // DataBase implementation
    // *************************************************************************

    static const char * db_names;

    static const char * db_bootstrap;

    static const char * table;

    /**
     *  Writes the VMTemplate in the database.
     *    @param db pointer to the db
     *    @return 0 on success
     */
    int insert(SqlDB *db, string& error_str);

    /**
     *  Writes/updates the VMTemplate data fields in the database.
     *    @param db pointer to the db
     *    @return 0 on success
     */
    int update(SqlDB *db)
    {
        return insert_replace(db, true);
    };
};

#endif /*VMTEMPLATE_H_*/
