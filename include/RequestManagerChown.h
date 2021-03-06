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

#ifndef REQUEST_MANAGER_CHOWN_H_
#define REQUEST_MANAGER_CHOWN_H_

#include "Request.h"
#include "Nebula.h"

using namespace std;

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class RequestManagerChown : public Request
{
protected:
    RequestManagerChown(const string& method_name,
                        const string& help,
                        const string& params = "A:siii")
        :Request(method_name,params,help)
    {
        auth_op = AuthRequest::CHOWN;
    };

    ~RequestManagerChown(){};

    /* -------------------------------------------------------------------- */

    virtual void request_execute(xmlrpc_c::paramList const& _paramList,
                                 RequestAttributes& att);
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class VirtualMachineChown : public RequestManagerChown
{
public:
    VirtualMachineChown():
        RequestManagerChown("VirtualMachineChown",
                            "Changes ownership of a virtual machine")
    {    
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_vmpool();
        auth_object = AuthRequest::VM;
    };

    ~VirtualMachineChown(){};
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class TemplateChown : public RequestManagerChown
{
public:
    TemplateChown():
        RequestManagerChown("TemplateChown",
                            "Changes ownership of a virtual machine template")
    {    
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_tpool();
        auth_object = AuthRequest::TEMPLATE;
    };

    ~TemplateChown(){};
};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */


class VirtualNetworkChown: public RequestManagerChown
{
public:
    VirtualNetworkChown():
        RequestManagerChown("VirtualNetworkChown",
                           "Changes ownership of a virtual network")
    {    
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_vnpool();
        auth_object = AuthRequest::NET;
    };

    ~VirtualNetworkChown(){};

};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class ImageChown: public RequestManagerChown
{
public:
    ImageChown():
        RequestManagerChown("ImageChown",
                            "Changes ownership of an image")
    {    
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_ipool();
        auth_object = AuthRequest::IMAGE;
    };

    ~ImageChown(){};

};

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

class UserChown : public RequestManagerChown
{
public:
    UserChown():
        RequestManagerChown("UserChown",
                            "Changes ownership of a user",
                            "A:sii")
    {
        Nebula& nd  = Nebula::instance();
        pool        = nd.get_upool();
        auth_object = AuthRequest::USER;
    };

    ~UserChown(){};

    /* -------------------------------------------------------------------- */

    virtual void request_execute(xmlrpc_c::paramList const& _paramList,
                                 RequestAttributes& att);
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

#endif
