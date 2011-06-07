#!/usr/bin/env ruby

require 'optparse'
require 'pp'

module CommandParser
    OPTIONS = [
        VERBOSE={
            :name  => "verbose",
            :short => "-v",
            :large => "--verbose",
            :description => "Verbose mode"
        },
        HELP={
            :name => "help",
            :short => "-h",
            :large => "--help",
            :description => "Show this message"
        }
    ]
    
    class CmdParser
        attr_reader :options, :args
        
        def initialize(args=[], &block)
            @opts = Array.new
            @commands = Hash.new
            @formats = Hash.new
            @script = nil
            @usage = ""
            
            @args = args
            @options = Hash.new
            
            set :format, :file, "" do |arg| format_file(arg) ; end
            set :format, :range, "" do |arg| format_range(arg) ; end
            set :format, :text, ""  do |arg| format_text(arg) ; end
            
            instance_eval(&block)
            
            self.run
        end
        
        def usage(str)
            @usage = "Usage: #{str}"
        end
        
        def set(e, *args, &block)
            case e
            when :option
                add_option(args[0])
            when :format
                add_format(args[0], args[1], block)
            end
        end
        
        def command(name, desc, *args_format, &block)
            cmd = Hash.new
            cmd[:desc] = desc
            cmd[:arity] = 0
            cmd[:options] = []
            cmd[:args_format] = Array.new
            args_format.each {|args|
                if args.instance_of?(Array)
                    cmd[:arity]+=1 unless args.include?(nil)
                    cmd[:args_format] << args
                elsif args.instance_of?(Hash) && args[:options]
                    cmd[:options] << args[:options]
                else
                    cmd[:arity]+=1
                    cmd[:args_format] << [args]
                end
            }
            cmd[:proc] = block
            @commands[name] = cmd
        end

        def script(*args_format, &block)
            @script=Hash.new
            @script[:args_format] = Array.new
            args_format.collect {|args|
                if args.instance_of?(Array)
                    @script[:arity]+=1 unless args.include?(nil)
                    @script[:args_format] << args
                elsif args.instance_of?(Hash) && args[:options]
                    @opts << args[:options]
                else
                    @script[:arity]+=1
                    @script[:args_format] << [args]
                end
            }
            
            @script[:proc] = block
        end
        
        def run
            comm_name=""
            if @script
                comm=@script
            elsif
                if @args[0] && !@args[0].match(/^-/)
                    comm_name=@args.shift.to_sym
                    comm=@commands[comm_name]
                end
            end
            
            if comm.nil?
                help
                exit -1
            end
            
            extra_options = comm[:options] if comm
            parse(extra_options)
            if comm
                check_args!(comm_name, comm[:arity], comm[:args_format])
                
                begin
                    rc = comm[:proc].call
                rescue Exception =>e
                    puts e.message
                    exit -1
                end
                
                if rc.instance_of?(Array)
                    puts rc[1]
                    exit rc.first
                else
                    exit rc
                end
            end
        end
        
        def help
            puts @usage
            puts
            print_options
            puts
            print_commands
            puts
            print_formatters
        end
        
        private
        
        def print_options
            puts "Options:"
            
            shown_opts = Array.new
            opt_format = "#{' '*5}%-25s %s"
            @commands.each{ |key,value|
                value[:options].flatten.each { |o|
                    if shown_opts.include?(o[:name])
                        next
                    else
                        shown_opts << o[:name]
                        short = o[:short].split(' ').first
                        printf opt_format, "#{short}, #{o[:large]}", o[:description]
                        puts
                    end
                }
            }
            
            @opts.each{ |o|
                printf opt_format, "#{o[:short]}, #{o[:large]}", o[:description]
                puts
            }
        end
        
        def print_commands
            puts "Commands:"
            
            cmd_format5 =  "#{' '*5}%s"
            cmd_format10 =  "#{' '*10}%s"
            @commands.each{ |key,value|
                printf cmd_format5, "* #{key}"
                puts
                
                args_str=value[:args_format].collect{ |a|
                    if a.include?(nil)
                        "[#{a.compact.join("|")}]"
                    else
                        a.join("|")
                    end
                }.join(' ')
                printf cmd_format10, "arguments: #{args_str}"
                puts
                
                value[:desc].split("\n").each { |l|
                    printf cmd_format10, l
                    puts
                }
                
                unless value[:options].empty?
                    opts_str=value[:options].flatten.collect{|o|
                        o[:name]
                    }.join(', ')
                    printf cmd_format10, "options: #{opts_str}"
                    puts
                end
                puts
            }
        end
        
        def print_formatters
            puts "argument formats:"
            
            cmd_format5 =  "#{' '*5}%s"
            cmd_format10 =  "#{' '*10}%s"
            @formats.each{ |key,value|
                printf cmd_format5, "* #{key}"
                puts
                
                value[:desc].split("\n").each { |l|
                    printf cmd_format10, l
                    puts
                }
            }
        end
        
        def add_option(option)
            if option.instance_of?(Array)
                option.each { |o| @opts << o }
            elsif option.instance_of?(Hash)
                @opts << option
            end
        end
        
        def add_format(format, description, block)
            @formats[format] = {
                :desc => description,
                :proc => block
            }
        end

        def parse(extra_options)
            @cmdparse=OptionParser.new do |opts|
                merge = @opts
                merge = @opts + extra_options if extra_options
                merge.flatten.each do |e|
                    opts.on(e[:short],e[:large], e[:format],e[:description]) do |o|
                        if e[:proc]
                            e[:proc].call
                        elsif e[:name]=="help"
                            help
                            #puts opts
                            exit
                        else
                            @options[e[:name].to_sym]=o
                        end
                    end
                end
            end

            begin
                @cmdparse.parse!(@args)
            rescue => e
                puts e.message
                exit -1
            end
        end
        
        def check_args!(name, arity, args_format)
            if @args.length < arity
                print "Command #{name} requires "
                if arity>1
                    puts "#{args_format.length} parameters to run."
                else
                    puts "one parameter to run"
                end
                exit -1
            else
                id=0
                @args.collect!{|arg|
                    format = args_format[id]
                    argument = nil
                    error_msg = nil
                    format.each { |f|
                        rc = @formats[f][:proc].call(arg) if @formats[f]
                        if rc[0]==0
                            argument=rc[1]
                            break
                        else
                            error_msg=rc[1]
                            next
                        end
                    }
                    
                    unless argument
                        puts error_msg if error_msg
                        puts "command #{name}: argument #{id} must be one of #{format.join(', ')}"
                        exit -1
                    end
                    
                    id+=1
                    argument
                }
            end
        end
        
        ########################################################################
        # Formatters for arguments
        ########################################################################
        def format_text(arg)
            arg.instance_of?(String) ? [0,arg] : [-1]
        end

        def format_file(arg)
            File.exists?(arg) ? [0,arg] : [-1]
        end
        
        REG_RANGE=/^(?:(?:\d+\.\.\d+|\d+),)*(?:\d+\.\.\d+|\d+)$/
        
        def format_range(arg)
            arg_s = arg.gsub(" ","").to_s
            return [-1] unless arg_s.match(REG_RANGE)

            ids = Array.new
            arg_s.split(',').each { |e|
                if e.match(/^\d+$/)
                    ids << e.to_i
                elsif m = e.match(/^(\d+)\.\.(\d+)$/)
                    ids += (m[1].to_i..m[2].to_i).to_a
                else
                    return [-1]
                end
            }
            
            return 0,ids.uniq
        end
    end
end


