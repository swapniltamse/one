# -------------------------------------------------------------------------- #
# Copyright 2002-2010, OpenNebula Project Leads (OpenNebula.org)             #
#                                                                            #
# Licensed under the Apache License, Version 2.0 (the "License"); you may    #
# not use this file except in compliance with the License. You may obtain    #
# a copy of the License at                                                   #
#                                                                            #
# http://www.apache.org/licenses/LICENSE-2.0                                 #
#                                                                            #
# Unless required by applicable law or agreed to in writing, software        #
# distributed under the License is distributed on an "AS IS" BASIS,          #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   #
# See the License for the specific language governing permissions and        #
# limitations under the License.                                             #
#--------------------------------------------------------------------------- #

require 'OpenNebula'

include OpenNebula

class VirtualMachinePoolOCCI < VirtualMachinePool
    OCCI_VM_POOL = %q{
        <COMPUTE_COLLECTION>
        <% if pool_hash['VM_POOL'] && pool_hash['VM_POOL']['VM'] %>
            <% vmlist=[pool_hash['VM_POOL']['VM']].flatten %>
            <% vmlist.each{ |vm|  %>  
            <COMPUTE href="<%= base_url %>/compute/<%= vm['ID'] %>"/>
            <% } %>
        <% end %>
        </COMPUTE_COLLECTION>       
    }
    
    
    # Creates the OCCI representation of a Virtual Machine Pool
    def to_occi(base_url)
        pool_hash = self.to_hash
        return pool_hash, 500 if OpenNebula.is_error?(pool_hash)

        begin
            occi = ERB.new(OCCI_VM_POOL)
            occi_text = occi.result(binding) 
        rescue Exception => e
            error = OpenNebula::Error.new(e.message)
            return error
        end    

        return occi_text.gsub(/\n\s*/,'')
    end
end

