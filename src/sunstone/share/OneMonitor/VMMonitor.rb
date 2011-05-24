require 'OneMonitor'

class VMMonitor < OneMonitor

    VM_MONITORING_ELEMS = {
        :time => "LAST_POLL",
        :id => "ID",
        :name => "NAME",
        :lcm_state => "LCM_STATE",
        :memory => "MEMORY",
        :cpu => "CPU",
        :net_tx => "NET_TX",
        :net_rx => "NET_RX"
    }

    def initialize (log_file,monitoring_elems=VM_MONITORING_ELEMS)
        super log_file,monitoring_elems
    end

    def monitor
        super VirtualMachinePool
    end

    def snapshot
        super VirtualMachinePool
    end
end
