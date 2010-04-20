/* -------------------------------------------------------------------------- */
/* Copyright 2002-2010, OpenNebula Project Leads (OpenNebula.org)             */
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

#include "VirtualMachineManagerDriver.h"
#include "NebulaLog.h"
#include "LifeCycleManager.h"

#include "Nebula.h"
#include <sstream>

VirtualMachineManagerDriver::VirtualMachineManagerDriver(
    int                         userid,
    const map<string,string>&   attrs,
    bool                        sudo,
    VirtualMachinePool *        pool):
        Mad(userid,attrs,sudo),driver_conf(true),vmpool(pool)
{
    map<string,string>::const_iterator  it;
    char *          error_msg = 0;
    const char *    cfile;
    string          file;
    int             rc;

    it = attrs.find("DEFAULT");

    if ( it != attrs.end() )
    {
        if (it->second[0] != '/') //Look in ONE_LOCATION/etc or in "/etc/one"
        {
            Nebula& nd = Nebula::instance();

            file  = nd.get_defaults_location() + it->second;
            cfile = file.c_str();
        }
        else //Absolute Path
        {
            cfile = it->second.c_str();
        }

        rc = driver_conf.parse(cfile, &error_msg);

        if ( rc != 0 )
        {
            ostringstream   oss;

            if ( error_msg != 0 )
            {
                oss << "Error loading driver configuration file " << cfile <<
                    " : " << error_msg;

                free(error_msg);
            }
            else
            {
                oss << "Error loading driver configuration file " << cfile;
            }

            NebulaLog::log("VMM", Log::ERROR, oss);
        }
    }
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void VirtualMachineManagerDriver::get_default(
    const char *  name,
    const char *  vname,
    string&       value) const
{
    vector<const Attribute *>   attrs;
    string                      sn = name;

    if ( driver_conf.get(sn,attrs) == 1 )
    {
        const VectorAttribute * vattr;

        vattr = static_cast<const VectorAttribute *>(attrs[0]);

        value = vattr->vector_value(vname);
    }
    else
    {
        value = "";
    }
}

/* ************************************************************************** */
/* Driver ASCII Protocol Implementation                                       */
/* ************************************************************************** */

void VirtualMachineManagerDriver::deploy (
    const int     oid,
    const string& host,
    const string& conf) const
{
    ostringstream os;

    os << "DEPLOY " << oid << " " << host << " " << conf << " -" << endl;

    write(os);
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void VirtualMachineManagerDriver::shutdown (
    const int     oid,
    const string& host,
    const string& name) const
{
    ostringstream os;

    os << "SHUTDOWN " << oid << " " << host << " " << name << " -" << endl;

    write(os);
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void VirtualMachineManagerDriver::cancel (
    const int     oid,
    const string& host,
    const string& name) const
{
    ostringstream os;

    os << "CANCEL " << oid << " " << host << " " << name << " -" << endl;

    write(os);
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void VirtualMachineManagerDriver::checkpoint (
    const int     oid,
    const string& host,
    const string& name,
    const string& file) const
{
    ostringstream os;

    os<< "CHECKPOINT " << oid<< " "<< host<< " "<< name<< " "<< file<< endl;

    write(os);
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void VirtualMachineManagerDriver::save (
    const int     oid,
    const string& host,
    const string& name,
    const string& file) const
{
    ostringstream os;

    os<< "SAVE " << oid << " " << host << " " << name << " "<< file << endl;

    write(os);
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void VirtualMachineManagerDriver::restore (
    const int     oid,
    const string& host,
    const string& name,
    const string& file) const
{
    ostringstream os;

    os << "RESTORE " << oid << " " << host << " " << name << " " << file<< endl;

    write(os);
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void VirtualMachineManagerDriver::migrate (
    const int     oid,
    const string& shost,
    const string& name,
    const string& dhost) const
{
    ostringstream os;

    os<< "MIGRATE " << oid << " "<< shost<< " "<< name<< " "<< dhost<< endl;

    write(os);
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void VirtualMachineManagerDriver::poll (
    const int     oid,
    const string& host,
    const string& name) const
{
    ostringstream os;

    os << "POLL " << oid << " " << host << " " << name << " -" << endl;

    write(os);
};

/* ************************************************************************** */
/* MAD Interface                                                              */
/* ************************************************************************** */

void VirtualMachineManagerDriver::protocol(
    string&     message)
{
    istringstream           is(message);
    ostringstream           os;

    string                  action;
    string                  result;

    int                     id;
    VirtualMachine *        vm;

    os << "Message received: " << message;
    NebulaLog::log("VMM", Log::DEBUG, os);

    // Parse the driver message
    if ( is.good() )
        is >> action >> ws;
    else
        return;

    if ( is.good() )
        is >> result >> ws;
    else
        return;

    if ( is.good() )
    {
        is >> id >> ws;

        if ( is.fail() )
        {
            if ( action == "LOG" )
            {
                string info;

                is.clear();
                getline(is,info);
                NebulaLog::log("VMM",Log::INFO, info.c_str());
            }

            return;
        }
    }
    else
        return;

    // Get the VM from the pool
    vm = vmpool->get(id,true);

    if ( vm == 0 )
    {
        return;
    }

    if ( vm->get_lcm_state() == VirtualMachine::DELETE ||
         vm->get_lcm_state() == VirtualMachine::FAILURE||
         vm->get_lcm_state() == VirtualMachine::LCM_INIT )
    {
        os.str("");
        os << "Ignored: " << message;
        vm->log("VMM",Log::WARNING,os);

        vm->unlock();
        return;
    }

    // Driver Actions
    if ( action == "DEPLOY" )
    {
        Nebula              &ne = Nebula::instance();
        LifeCycleManager *  lcm = ne.get_lcm();

        if (result == "SUCCESS")
        {
            string              deploy_id;

            is >> deploy_id;

            vm->update_info(deploy_id);

            vmpool->update(vm);

            lcm->trigger(LifeCycleManager::DEPLOY_SUCCESS, id);
        }
        else
        {
            string info;

            getline(is,info);

            os.str("");
            os << "Error deploying virtual machine";
               
            if (info[0] != '-')
               os << ": " << info;

            vm->log("VMM",Log::ERROR,os);

            lcm->trigger(LifeCycleManager::DEPLOY_FAILURE, id);
        }
    }
    else if (action == "SHUTDOWN" )
    {
        Nebula              &ne  = Nebula::instance();
        LifeCycleManager    *lcm = ne.get_lcm();

        if (result == "SUCCESS")
        {
            lcm->trigger(LifeCycleManager::SHUTDOWN_SUCCESS, id);
        }
        else
        {
            string          info;

            getline(is,info);

            os.str("");
            os << "Error shuting down VM, " << info;

            vm->log("VMM",Log::ERROR,os);

            lcm->trigger(LifeCycleManager::SHUTDOWN_FAILURE, id);
        }
    }
    else if ( action == "CANCEL" )
    {
        Nebula              &ne  = Nebula::instance();
        LifeCycleManager    *lcm = ne.get_lcm();

        if (result == "SUCCESS")
        {
            lcm->trigger(LifeCycleManager::CANCEL_SUCCESS, id);
        }
        else
        {
            string  info;

            getline(is,info);

            os.str("");
            os << "Error canceling VM, " << info;

            vm->log("VMM",Log::ERROR,os);

            lcm->trigger(LifeCycleManager::CANCEL_FAILURE, id);
        }
    }
    else if ( action == "SAVE" )
    {
        Nebula              &ne  = Nebula::instance();
        LifeCycleManager    *lcm = ne.get_lcm();

        if (result == "SUCCESS")
        {
            lcm->trigger(LifeCycleManager::SAVE_SUCCESS, id);
        }
        else
        {
            string          info;

            getline(is,info);

            os.str("");
            os << "Error saving VM state, " << info;

            vm->log("VMM",Log::ERROR,os);

            lcm->trigger(LifeCycleManager::SAVE_FAILURE, id);
        }
    }
    else if ( action == "RESTORE" )
    {
        Nebula              &ne  = Nebula::instance();
        LifeCycleManager    *lcm = ne.get_lcm();

        if (result == "SUCCESS")
        {
            lcm->trigger(LifeCycleManager::DEPLOY_SUCCESS, id);
        }
        else
        {
            string          info;

            getline(is,info);

            os.str("");
            os << "Error restoring VM, " << info;

            vm->log("VMM",Log::ERROR,os);

            lcm->trigger(LifeCycleManager::DEPLOY_FAILURE, id);
        }
    }
    else if ( action == "MIGRATE" )
    {
        Nebula              &ne  = Nebula::instance();
        LifeCycleManager    *lcm = ne.get_lcm();

        if (result == "SUCCESS")
        {
            lcm->trigger(LifeCycleManager::DEPLOY_SUCCESS, id);
        }
        else
        {
            string          info;

            getline(is,info);

            os.str("");
            os << "Error live-migrating VM, " << info;

            vm->log("VMM",Log::ERROR,os);

            lcm->trigger(LifeCycleManager::DEPLOY_FAILURE, id);
        }
    }
    else if ( action == "POLL" )
    {
        if (result == "SUCCESS")
        {
            size_t          pos;

            string          tmp;
            string          var;
            ostringstream   os;
            istringstream   tiss;

            int             cpu    = -1;
            int             memory = -1;
            int             net_tx = -1;
            int             net_rx = -1;
            char            state  = '-';

            while(is.good())
            {
                is >> tmp >> ws;

                pos = tmp.find('=');

                if ( pos == string::npos )
                {
                    os.str("");
                    os << "Error parsing monitoring attribute: " << tmp;

                    vm->log("VMM",Log::ERROR,os);

                    break;
                }

                tmp.replace(pos,1," ");

                tiss.clear();

                tiss.str(tmp);

                tiss >> var;

                if (var == "USEDMEMORY")
                {
                    tiss >> memory;
                }
                else if (var == "USEDCPU")
                {
                    tiss >> cpu;
                }
                else if (var == "NETRX")
                {
                    tiss >> net_rx;
                }
                else if (var == "NETTX")
                {
                    tiss >> net_tx;
                }
                else if (var == "STATE")
                {
                    tiss >> state;
                }
                else
                {
                    string val;

                    os.str("");
                    os << "Unknown monitoring attribute (adding/updating"
                       << " template): " << tmp;

                    vm->log("VMM",Log::WARNING,os);

                    tiss >> val;

                    vmpool->update_template_attribute(vm,var,val);
                }
            }

            vm->update_info(memory,cpu,net_tx,net_rx);

            vmpool->update(vm);

            if (state != '-' && 
                (vm->get_lcm_state() == VirtualMachine::RUNNING ||
                 vm->get_lcm_state() == VirtualMachine::UNKNOWN))
            {
                Nebula              &ne  = Nebula::instance();
                LifeCycleManager *  lcm = ne.get_lcm();

                switch (state)
                {
                case 'a': // Still active, good!
                    os.str("");
                    os  << "Monitor Information:\n"
                        << "\tCPU   : "<< cpu    << "\n"
                        << "\tMemory: "<< memory << "\n"
                        << "\tNet_TX: "<< net_tx << "\n"
                        << "\tNet_RX: "<< net_rx;
                    vm->log("VMM",Log::DEBUG,os);

                    if ( vm->get_lcm_state() == VirtualMachine::UNKNOWN)
                    {
                        vm->log("VMM",Log::INFO,"VM was now found, new state is"
                                " RUNNING"); 
                        vm->set_state(VirtualMachine::RUNNING);
                        vmpool->update(vm);
                    }
                    break;

                case 'p': // It's paused
                    vm->log("VMM",Log::INFO,"VM running but new state "
                            "from monitor is PAUSED.");

                    lcm->trigger(LifeCycleManager::MONITOR_SUSPEND, id);
                    break;

                case 'e': //Failed
                    vm->log("VMM",Log::INFO,"VM running but new state "
                            "from monitor is ERROR.");

                    lcm->trigger(LifeCycleManager::MONITOR_FAILURE, id);
                    break;

                case 'd': //The VM was not found
                    vm->log("VMM",Log::INFO,"VM running but it was not found."
                            " Restart and delete actions available or try to"
                            " recover it manually");

                    lcm->trigger(LifeCycleManager::MONITOR_DONE, id);
                    break;
                }
            }
        }
        else
        {
            string          info;

            getline(is,info);

            os.str("");
            os << "Error monitoring VM, " << info;

            vm->log("VMM",Log::ERROR,os);
        }
    }
    else if (action == "LOG")
    {
        string info;

        getline(is,info);
        vm->log("VMM",Log::INFO,info.c_str());
    }

    vm->unlock();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void VirtualMachineManagerDriver::recover()
{
    NebulaLog::log("VMM",Log::INFO,"Recovering VMM drivers");
}

