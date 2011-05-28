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

#ifndef REQUEST_H_
#define REQUEST_H_

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>

#include "RequestManager.h"
#include "AuthManager.h"

using namespace std;

/**
 *  The Request Class represents the basic abstraction for the OpenNebula
 *  XML-RPC API. This interface must be implemented by any XML-RPC API call
 */
class Request: public xmlrpc_c::method
{
public:
    /**
     *  Wraps the actual execution function by authorizing the user
     *  and calling the request_execute virtual function
     *    @param _paramlist list of XML parameters
     *    @param _retval value to be returned to the client
     */
    virtual void execute(
        xmlrpc_c::paramList const& _paramList,
        xmlrpc_c::value *   const  _retval);

    /**
     *  Error codes for the XML-RPC API
     */
    enum ErrorCode {
        SUCCESS        = 0x0000,
        AUTHENTICATION = 0x0100,
        AUTHORIZATION  = 0x0200,
        NO_EXISTS      = 0x0400,
        ACTION         = 0x0800,
        XML_RPC_API    = 0x1000,
        INTERNAL       = 0x2000,
    };

protected:

    /* ------------------- Attributes of the Request ---------------------- */

    int                 uid;  /** id of the user performing the request */
    
    int                 gid; /** id of the user performing the request  */

    PoolSQL *           pool; /** id of the user performing the request */

    string              method_name; /** The name of the XML-RPC method */

    AuthRequest::Object    auth_object; /** Auth object for the request */

    AuthRequest::Operation auth_op; /** Auth operation for the request  */


    /* -------------------- Constructors ---------------------------------- */

    Request(const string& mn, 
            const string& signature, 
            const string& help): uid(-1),gid(-1),pool(0),method_name(mn),retval(0)
    {
        _signature = signature;
        _help      = help;
    };

    virtual ~Request(){};

    /* -------------------------------------------------------------------- */
    /* -------------------------------------------------------------------- */

    /**
     *  Performs a basic autorization for this request using the uid/gid
     *  from the request. The function gets the object from the pool to get 
     *  the public attribute and its owner. The authorization is based on 
     *  object and type of operation for the request.
     *    @param oid of the object.
     */
    bool basic_authorization(int oid);
            
    /**
     *  Actual Execution method for the request. Must be implemented by the
     *  XML-RPC requests
     *    @param uid of the user making the request
     *    @param gid of the user making the request
     *    @param _paramlist of the XML-RPC call (complete list)
     */
    virtual void request_execute(xmlrpc_c::paramList const& _paramList) = 0;

    /**
     *  Builds an XML-RPC response updating retval. After calling this function
     *  the xml-rpc excute method should return
     *    @param val to be returned to the client
     */
    void success_response(int val);

    /**
     *  Builds an XML-RPC response updating retval. After calling this function
     *  the xml-rpc excute method should return
     *    @param val string to be returned to the client
     */
    void success_response(const string& val);

    /**
     *  Builds an XML-RPC response updating retval. After calling this function
     *  the xml-rpc excute method should return
     *    @param ec error code for this call
     *    @param val string representation of the error
     */
    void failure_response(ErrorCode ec, const string& val);

    /**
     *  Gets a string representation for the Auth object in the
     *  request.
     *    @param ob object for the auth operation
     *    @returns string equivalent of the object
     */
    static string object_name(AuthRequest::Object ob);

    /**
     *  Logs authorization errors
     *    @param action authorization action
     *    @param object object that needs to be authorized
     *    @param uid user that is authorized
     *    @param id id of the object, -1 for Pool
     *    @returns string for logging
     */
    string authorization_error (const string &action,
                                const string &object,
                                int   uid,
                                int   id);
    /**
     *  Logs authenticate errors
     *    @returns string for logging
     */
    string authenticate_error ();

    /**
     *  Logs get object errors
     *    @param object over which the get failed
     *    @param id of the object over which the get failed
     *    @returns string for logging
     */
    string get_error (const string &object,
                      int id);

    /**
     *  Logs action errors
     *    @param action that triggered the error
     *    @param object over which the action was applied
     *    @param id id of the object, -1 for Pool, -2 for no-id objects
     *              (allocate error, parse error)
     *    @param rc returned error code (NULL to ignore)
     *    @returns string for logging
     */
    string action_error (const string &action,
                         const string &object,
                         int id,
                         int rc);

private:
    /**
     *  Session token from the OpenNebula XML-RPC API
     */
    string             session;

    /**
     *  Return value of the request from libxmlrpc-c
     */
    xmlrpc_c::value * retval;
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

#endif //REQUEST_H_

