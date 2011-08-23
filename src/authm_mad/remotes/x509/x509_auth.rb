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

require 'openssl'
require 'base64'
require 'fileutils'

# X509 authentication class. It can be used as a driver for auth_mad
# as auth method is defined. It also holds some helper methods to be used
# by oneauth command
class X509Auth
    PROXY_PATH = ENV['HOME']+'/.one/one_x509'

    attr_reader :dn

    # Initialize x509Auth object
    #
    # @param [Hash] default options for path
    # @option options [String] :cert public cert for the user
    # @option options [String] :key private key for the user. Needed
    #                          to use login method
    # @option options [String] :ca_dir directory of trusted CA's. Optional.
    def initialize(options={})
        @options={
            :cert   => nil,
            :key    => nil,
            :ca_dir => nil
        }.merge!(options)

        @cert = OpenSSL::X509::Certificate.new(@options[:cert])
        @dn   = @cert.subject.to_s

        if @options[:key]
            @key  = OpenSSL::PKey::RSA.new(@options[:key])
        end
    end

    ###########################################################################
    # Client side
    ###########################################################################

    # Creates the login file for x509 authentication at ~/.one/one_x509.
    # By default it is valid for 1 hour but it can be changed to any number
    # of seconds with expire parameter (in seconds)
    def login(user, expire=3600)
        expire ||= 3600

        # Init proxy file path and creates ~/.one directory if needed
        # Set instance variables
        proxy_dir = File.dirname(PROXY_PATH)

        begin
            FileUtils.mkdir_p(proxy_dir)
        rescue Errno::EEXIST
        end

        #Create the x509 proxy
        time = Time.now.to_i+expire

        text_to_sign = "#{user}:#{@dn}:#{time}"
        signed_text  = encrypt(text_to_sign)

	    token   = "#{signed_text}:#{@cert.to_pem}"
	    token64 = Base64::encode64(token).strip.delete!("\n")

        proxy="#{user}:x509:#{token64}"

        file = File.open(PROXY_PATH, "w")
        file.write(proxy)
        file.close

        token64
    end

    ###########################################################################
    # Server side
    ###########################################################################
    # auth method for auth_mad
    def authenticate(user, pass, token)
        begin
            validate

            plain = decrypt(token)

            _user, subject, time_expire = plain.split(':')

            if (user != _user)
                return "User name missmatch"
            elsif ((subject != @dn) || (subject != pass))
                return "Certificate subject missmatch"
            elsif Time.now.to_i >= time_expire.to_i
                return "x509 proxy expired, login again to renew it"
            end

            return true
        rescue => e
            return e.message
        end
    end

private
    ###########################################################################
    #                       Methods to encrpyt/decrypt keys
    ###########################################################################
    # Encrypts data with the private key of the user and returns
    # base 64 encoded output in a single line
    def encrypt(data)
        return nil if !@key
        Base64::encode64(@key.private_encrypt(data)).delete!("\n").strip
    end

    # Decrypts base 64 encoded data with pub_key (public key)
    def decrypt(data)
        @cert.public_key.public_decrypt(Base64::decode64(data))
    end

    ###########################################################################
    # Validate the user certificate
    ###########################################################################
    def validate
 	    now    = Time.now
        failed = "Could not validate user credentials: "

        # Check start time and end time of certificate
        if @cert.not_before > now || @cert.not_after < now
            raise failed +  "Certificate not valid. Current time is " +
                  now.localtime.to_s + "."
        end

 	    # Check the rest of the certificate chain if specified
        if !@options[:ca_dir]
            return
        end

        begin
            signee = @cert

            begin
                ca_hash = signee.issuer.hash.to_s(16)
                ca_path = @options[:ca_dir] + '/' + ca_hash + '.0'

                ca_cert = OpenSSL::X509::Certificate.new(File.read(ca_path))

                if !((signee.issuer.to_s == ca_cert.subject.to_s) &&
                     (signee.verify(ca_cert.public_key)))
                    raise  failed + signee.subject.to_s + " with issuer " +
                           signee.issuer.to_s + " was not verified by " +
                           ca.subject.to_s + "."
                end

                signee = ca_cert
            end while ca_cert.subject.to_s != ca_cert.issuer.to_s
        rescue
            raise
        end
    end
end
