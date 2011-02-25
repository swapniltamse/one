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

require 'OpenNebulaJSON/JSONUtils'

module OpenNebulaJSON
    class ClusterJSON < OpenNebula::Cluster
        include JSONUtils

        def allocate(template_json)
            cluster_hash = parse_json(template_json,'cluster')
            if OpenNebula.is_error?(cluster_hash)
                return cluster_hash
            end

            super(cluster_hash['name'])
        end

        def perform_action(template_json)
            action_hash = parse_json(template_json, 'action')
            if OpenNebula.is_error?(action_hash)
                return action_hash
            end

            rc = case action_hash['perform']
                when "add_host"    then self.add_host(action_hash['params'])
                when "remove_host" then self.remove_host(action_hash['params'])
                else
                    error_msg = "#{action_hash['perform']} action not " <<
                                " available for this resource"
                    OpenNebula::Error.new(error_msg)
            end
        end

        def add_host(params=Hash.new)
            super(params['host_id'])
        end

        def remove_host(params=Hash.new)
            super(params['host_id'])
        end
    end
end