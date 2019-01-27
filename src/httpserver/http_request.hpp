/*
     This file is part of libhttpserver
     Copyright (C) 2011-2019 Sebastiano Merlino

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 2.1 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
     USA
*/

#if !defined (_HTTPSERVER_HPP_INSIDE_) && !defined (HTTPSERVER_COMPILATION)
#error "Only <httpserver.hpp> or <httpserverpp> can be included directly."
#endif

#ifndef _HTTP_REQUEST_HPP_
#define _HTTP_REQUEST_HPP_

#include <map>
#include <vector>
#include <string>
#include <utility>
#include <iosfwd>

struct MHD_Connection;

namespace httpserver
{

class webserver;

namespace http
{
    class header_comparator;
    class arg_comparator;
};

/**
 * Class representing an abstraction for an Http Request. It is used from classes using these apis to receive information through http protocol.
**/
class http_request
{
    public:
        static const std::string EMPTY;

        /**
         * Method used to get the username eventually passed through basic authentication.
         * @return string representation of the username.
        **/
        const std::string& get_user();

        /**
         * Method used to get the username extracted from a digest authentication
         * @return the username
        **/
        const std::string& get_digested_user();

        /**
         * Method used to get the password eventually passed through basic authentication.
         * @return string representation of the password.
        **/
        const std::string& get_pass();

        /**
         * Method used to get the path requested
         * @return string representing the path requested.
        **/
        const std::string& get_path() const
        {
            return this->path;
        }

        /**
         * Method used to get all pieces of the path requested; considering an url splitted by '/'.
         * @return a vector of strings containing all pieces
        **/
        const std::vector<std::string>& get_path_pieces() const
        {
            return this->post_path;
        }

        /**
         * Method used to obtain a specified piece of the path; considering an url splitted by '/'.
         * @param index the index of the piece selected
         * @return the selected piece in form of string
        **/
        const std::string& get_path_piece(int index) const
        {
            if(((int)(this->post_path.size())) > index)
                return this->post_path[index];
            return EMPTY;
        }

        /**
         * Method used to get the METHOD used to make the request.
         * @return string representing the method.
        **/
        const std::string& get_method() const
        {
            return this->method;
        }

        /**
         * Method used to get all headers passed with the request.
         * @param result a map<string, string> > that will be filled with all headers
         * @result the size of the map
        **/
        const std::map<std::string, std::string, http::header_comparator>& get_headers();

        /**
         * Method used to get all footers passed with the request.
         * @param result a map<string, string> > that will be filled with all footers
         * @result the size of the map
        **/
        const std::map<std::string, std::string, http::header_comparator>& get_footers();

        /**
         * Method used to get all cookies passed with the request.
         * @param result a map<string, string> > that will be filled with all cookies
         * @result the size of the map
        **/
        const std::map<std::string, std::string, http::header_comparator>& get_cookies();

        /**
         * Method used to get all args passed with the request.
         * @param result a map<string, string> > that will be filled with all args
         * @result the size of the map
        **/
        const std::map<std::string, std::string, http::arg_comparator>& get_args();

        /**
         * Method used to get a specific header passed with the request.
         * @param key the specific header to get the value from
         * @return the value of the header.
        **/
        const std::string& get_header(const std::string& key);

        const std::string& get_cookie(const std::string& key);

        /**
         * Method used to get a specific footer passed with the request.
         * @param key the specific footer to get the value from
         * @return the value of the footer.
        **/
        const std::string& get_footer(const std::string& key);

        /**
         * Method used to get a specific argument passed with the request.
         * @param ket the specific argument to get the value from
         * @return the value of the arg.
        **/
        const std::string& get_arg(const std::string& key);

        /**
         * Method used to get the content of the request.
         * @return the content in string representation
        **/
        const std::string& get_content() const
        {
            return this->content;
        }

        /**
         * Method to check whether the size of the content reached or exceeded content_size_limit.
         * @return boolean
        **/
        bool content_too_large() const
        {
            return content.size()>=content_size_limit;
        }
        /**
         * Method used to get the content of the query string..
         * @return the query string in string representation
        **/
        const std::string& get_querystring();

        /**
         * Method used to get the version of the request.
         * @return the version in string representation
        **/
        const std::string& get_version() const
        {
            return this->version;
        }

        /**
         * Method used to get the requestor.
         * @return the requestor
        **/
        const std::string& get_requestor();

        /**
         * Method used to get the requestor port used.
         * @return the requestor port
        **/
        unsigned short get_requestor_port();

        bool check_digest_auth(const std::string& realm,
                const std::string& password,
                int nonce_timeout, bool& reload_nonce
        );

        friend std::ostream &operator<< (std::ostream &os, http_request &r);

    private:
        /**
         * Default constructor of the class. It is a specific responsibility of apis to initialize this type of objects.
        **/
        http_request():
            content(""),
            content_size_limit(static_cast<size_t>(-1)),
            underlying_connection(0x0),
            headers_loaded(false),
            footers_loaded(false),
            cookies_loaded(false),
            args_loaded(false),
            basic_auth_loaded(false),
            digest_auth_loaded(false),
            requestor_loaded(false),
            unescaper(0x0)
        {
        }

        http_request(MHD_Connection* underlying_connection, unescaper_ptr unescaper):
            content(""),
            content_size_limit(static_cast<size_t>(-1)),
            underlying_connection(underlying_connection),
            headers_loaded(false),
            footers_loaded(false),
            cookies_loaded(false),
            args_loaded(false),
            basic_auth_loaded(false),
            digest_auth_loaded(false),
            requestor_loaded(false),
            unescaper(unescaper)
        {
        }

        /**
         * Copy constructor.
         * @param b http_request b to copy attributes from.
        **/
        http_request(const http_request& b):
            user(b.user),
            pass(b.pass),
            path(b.path),
            digested_user(b.digested_user),
            method(b.method),
            post_path(b.post_path),
            headers(b.headers),
            footers(b.footers),
            cookies(b.cookies),
            args(b.args),
            querystring(b.querystring),
            content(b.content),
            content_size_limit(b.content_size_limit),
            version(b.version),
            requestor(b.requestor),
            underlying_connection(b.underlying_connection),
            headers_loaded(b.headers_loaded),
            footers_loaded(b.footers_loaded),
            cookies_loaded(b.cookies_loaded),
            args_loaded(b.args_loaded),
            basic_auth_loaded(b.basic_auth_loaded),
            digest_auth_loaded(b.digest_auth_loaded),
            requestor_loaded(b.requestor_loaded),
            unescaper(b.unescaper)
        {
        }

        http_request(http_request&& b) noexcept:
            user(std::move(b.user)),
            pass(std::move(b.pass)),
            path(std::move(b.path)),
            digested_user(std::move(b.digested_user)),
            method(std::move(b.method)),
            post_path(std::move(b.post_path)),
            headers(std::move(b.headers)),
            footers(std::move(b.footers)),
            cookies(std::move(b.cookies)),
            args(std::move(b.args)),
            querystring(std::move(b.querystring)),
            content(std::move(b.content)),
            content_size_limit(b.content_size_limit),
            version(std::move(b.version)),
            requestor(std::move(b.requestor)),
            underlying_connection(std::move(b.underlying_connection))
        {
        }

        http_request& operator=(const http_request& b)
        {
            if (this == &b) return *this;

            this->user = b.user;
            this->pass = b.pass;
            this->path = b.path;
            this->digested_user = b.digested_user;
            this->method = b.method;
            this->post_path = b.post_path;
            this->headers = b.headers;
            this->footers = b.footers;
            this->cookies = b.cookies;
            this->args = b.args;
            this->querystring = b.querystring;
            this->content = b.content;
            this->content_size_limit = b.content_size_limit;
            this->version = b.version;
            this->requestor = b.requestor;
            this->underlying_connection = b.underlying_connection;

            return *this;
        }

        http_request& operator=(http_request&& b)
        {
            if (this == &b) return *this;

            this->user = std::move(b.user);
            this->pass = std::move(b.pass);
            this->path = std::move(b.path);
            this->digested_user = std::move(b.digested_user);
            this->method = std::move(b.method);
            this->post_path = std::move(b.post_path);
            this->headers = std::move(b.headers);
            this->footers = std::move(b.footers);
            this->cookies = std::move(b.cookies);
            this->args = std::move(b.args);
            this->querystring = std::move(b.querystring);
            this->content = std::move(b.content);
            this->content_size_limit = b.content_size_limit;
            this->version = std::move(b.version);
            this->requestor = std::move(b.requestor);
            this->underlying_connection = std::move(b.underlying_connection);

            return *this;
        }

        std::string user;
        std::string pass;
        std::string path;
        std::string digested_user;
        std::string method;
        std::vector<std::string> post_path;
        std::map<std::string, std::string, http::header_comparator> headers;
        std::map<std::string, std::string, http::header_comparator> footers;
        std::map<std::string, std::string, http::header_comparator> cookies;
        std::map<std::string, std::string, http::arg_comparator> args;
        std::string querystring;
        std::string content;
        size_t content_size_limit;
        std::string version;
        std::string requestor;

        unsigned short requestor_port;
        struct MHD_Connection* underlying_connection;

        bool headers_loaded;
        bool footers_loaded;
        bool cookies_loaded;
        bool args_loaded;

        bool basic_auth_loaded;
        bool digest_auth_loaded;

        bool requestor_loaded;

        unescaper_ptr unescaper;

        static int build_request_header(void *cls, enum MHD_ValueKind kind,
                const char *key, const char *value
        );

        static int build_request_footer(void *cls, enum MHD_ValueKind kind,
                const char *key, const char *value
        );

        static int build_request_cookie(void *cls, enum MHD_ValueKind kind,
                const char *key, const char *value
        );

        static int build_request_args(void *cls, enum MHD_ValueKind kind,
                const char *key, const char *value
        );

        /**
         * Method used to set an header value by key.
         * @param key The name identifying the header
         * @param value The value assumed by the header
        **/
        void set_header(const std::string& key, const std::string& value)
        {
            this->headers[key] = value;
        }

        void check_or_fill_headers();

        /**
         * Method used to set a footer value by key.
         * @param key The name identifying the footer
         * @param value The value assumed by the footer
        **/
        void set_footer(const std::string& key, const std::string& value)
        {
            this->footers[key] = value;
        }

        void check_or_fill_footers();

        /**
         * Method used to set a cookie value by key.
         * @param key The name identifying the cookie
         * @param value The value assumed by the cookie
        **/
        void set_cookie(const std::string& key, const std::string& value)
        {
            this->cookies[key] = value;
        }

        void check_or_fill_cookies();

        /**
         * Method used to set an argument value by key.
         * @param key The name identifying the argument
         * @param value The value assumed by the argument
        **/
        void set_arg(const std::string& key, const std::string& value)
        {
            this->args[key] = value.substr(0,content_size_limit);
        }

        /**
         * Method used to set an argument value by key.
         * @param key The name identifying the argument
         * @param value The value assumed by the argument
         * @param size The size in number of char of the value parameter.
        **/
        void set_arg(const char* key, const char* value, size_t size)
        {
            this->args[key] = std::string(value,
                                          std::min(size, content_size_limit));
        }

        void check_or_fill_args();

        /**
         * Method used to set the content of the request
         * @param content The content to set.
        **/
        void set_content(const std::string& content)
        {
            this->content = content.substr(0,content_size_limit);
        }

        /**
         * Method used to set the maximum size of the content
         * @param content_size_limit The limit on the maximum size of the content and arg's.
        **/
        void set_content_size_limit(size_t content_size_limit)
        {
            this->content_size_limit = content_size_limit;
        }

        /**
         * Method used to append content to the request preserving the previous inserted content
         * @param content The content to append.
         * @param size The size of the data to append.
        **/
        void grow_content(const char* content, size_t size)
        {
            this->content.append(content, size);
            if (this->content.size() > content_size_limit)
            {
                this->content.resize (content_size_limit);
            }
        }

        /**
         * Method used to set the path requested.
         * @param path The path searched by the request.
        **/
        void set_path(const std::string& path)
        {
            this->path = path;
            std::vector<std::string> complete_path = http::http_utils::tokenize_url(this->path);
            for(unsigned int i = 0; i < complete_path.size(); i++)
            {
                this->post_path.push_back(complete_path[i]);
            }
        }

        /**
         * Method used to set the request METHOD
         * @param method The method to set for the request
        **/
        void set_method(const std::string& method);

        /**
         * Method used to set the request http version (ie http 1.1)
         * @param version The version to set in form of string
        **/
        void set_version(const std::string& version)
        {
            this->version = version;
        }

        /**
         * Method used to set the requestor
         * @param requestor The requestor to set
        **/
        void set_requestor(const std::string& requestor)
        {
            this->requestor = requestor;
        }

        /**
         * Method used to set the requestor port
         * @param requestor The requestor port to set
        **/
        void set_requestor_port(unsigned short requestor_port)
        {
            this->requestor_port = requestor_port;
        }

        /**
         * Method used to remove an header previously inserted
         * @param key The key identifying the header to remove.
        **/
        void remove_header(const std::string& key)
        {
            this->headers.erase(key);
        }

        /**
         * Method used to set all headers of the request.
         * @param headers The headers key-value map to set for the request.
        **/
        void set_headers(const std::map<std::string, std::string>& headers)
        {
            std::map<std::string, std::string>::const_iterator it;
            for(it = headers.begin(); it != headers.end(); ++it)
                this->headers[it->first] = it->second;
        }

        /**
         * Method used to set all footers of the request.
         * @param footers The footers key-value map to set for the request.
        **/
        void set_footers(const std::map<std::string, std::string>& footers)
        {
            std::map<std::string, std::string>::const_iterator it;
            for(it = footers.begin(); it != footers.end(); ++it)
                this->footers[it->first] = it->second;
        }

        /**
         * Method used to set all cookies of the request.
         * @param cookies The cookies key-value map to set for the request.
        **/
        void set_cookies(const std::map<std::string, std::string>& cookies)
        {
            std::map<std::string, std::string>::const_iterator it;
            for(it = cookies.begin(); it != cookies.end(); ++it)
                this->cookies[it->first] = it->second;
        }

        /**
         * Method used to set all arguments of the request.
         * @param args The args key-value map to set for the request.
        **/
        void set_args(const std::map<std::string, std::string>& args)
        {
            std::map<std::string, std::string>::const_iterator it;
            for(it = args.begin(); it != args.end(); ++it)
                this->args[it->first] = it->second.substr(0,content_size_limit);
        }

        /**
         * Method used to set the username of the request.
         * @param user The username to set.
        **/
        void set_user(const std::string& user)
        {
            this->user = user;
        }

        void set_digested_user(const std::string& digested_user)
        {
            this->digested_user = digested_user;
        }

        /**
         * Method used to set the password of the request.
         * @param pass The password to set.
        **/
        void set_pass(const std::string& pass)
        {
            this->pass = pass;
        }

        void basic_auth_parse();
        void digest_auth_parse();
        void parse_requestor_info();

        friend class webserver;
};

std::ostream &operator<< (std::ostream &os, http_request &r);

};
#endif
