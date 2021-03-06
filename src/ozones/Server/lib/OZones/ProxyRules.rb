# -------------------------------------------------------------------------- #
# Copyright 2002-2011, OpenNebula Project Leads (OpenNebula.org)             #
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

module OZones
    
    class ProxyRules 
        def initialize(type, file_path) 
            @type      = type
            if file_path
                @file_path = file_path
            else 
                if !ENV["ONE_LOCATION"]
                    @file_path="/usr/lib/one/ozones/htaccess"
                else
                    @file_path=ENV["ONE_LOCATION"]+"/lib/ozones/htaccess"    
                end
            end
        end
        
        def update
            case @type
                when "apache"
                    apWritter = OZones::ApacheWritter.new @file_path
                    apWritter.update
            end
        end
    end
    
end
