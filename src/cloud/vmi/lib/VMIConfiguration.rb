
class VMIConfiguration

    NAME_REG=/[\w\d_-]+/
    VARIABLE_REG=/\s*(#{NAME_REG})\s*=\s*/
    SIMPLE_VARIABLE_REG=/#{VARIABLE_REG}([^\[]+?)(#.*)?/
    SINGLE_VARIABLE_REG=/^#{SIMPLE_VARIABLE_REG}$/
    ARRAY_VARIABLE_REG=/^#{VARIABLE_REG}\[(.*?)\]/m
    
    def initialize(file)
        @conf=parse_conf(file)
    end

    def add_value(conf, key, value)
        if conf[key]
            if !conf[key].kind_of?(Array)
                conf[key]=[conf[key]]
            end
            conf[key]<<value
        else
            conf[key]=value
        end
    end

    def parse_conf(file)
        conf_file=File.read(file)
    
        conf=Hash.new

        conf_file.scan(SINGLE_VARIABLE_REG) {|m|
            key=m[0].strip.upcase
            value=m[1].strip
        
            # hack to skip multiline VM_TYPE values
            next if %w{NAME TEMPLATE}.include? key.upcase
        
            add_value(conf, key, value)
        }
    
        conf_file.scan(ARRAY_VARIABLE_REG) {|m|
            master_key=m[0].strip.upcase
                
            pieces=m[1].split(',')
        
            vars=Hash.new
            pieces.each {|p|
                key, value=p.split('=')
                vars[key.strip.upcase]=value.strip
            }

            add_value(conf, master_key, vars)
        }

        conf
    end
    
    def conf
        @conf
    end
    
    def [](key)
        @conf[key.to_s.upcase]
    end
end

if $0 == __FILE__

    require 'pp'

    conf=VMIConfiguration.new('vmi-server.conf')
    pp conf.conf

end
