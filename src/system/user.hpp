#ifndef SJTU_USER_HPP
#define SJTU_USER_HPP

#include <cstring>

#include "bplus_tree.hpp"
#include "map.hpp"
#include "utility.hpp"

namespace cay {

using user_name=string<20>;
struct user_info {
    string<30> password, mailAddr;
    string<20> name;
    int privilege;

    user_info () {}
    user_info ( const string<30> _password, 
                const string<30> _mailAddr, 
                const string<20> _name, 
                const char* _privilege="10" ):
        password(_password), 
        mailAddr(_mailAddr), 
        name(_name),
        privilege(atoi(_privilege)) {
    }

    const std::string show () {
        return name.str()+" "+mailAddr.str()+" "+to_string(privilege);
    }

    bool operator< ( const user_info& obj ) const {
        return privilege<obj.privilege; 
    }
    bool operator> ( const user_info& obj ) const {
        return privilege>obj.privilege; 
    }
    bool operator<= ( const user_info& obj ) const {
        return privilege<=obj.privilege; 
    }
    bool operator>= ( const user_info& obj ) const {
        return privilege>=obj.privilege; 
    }
};
class user {
  public:
    user () {}
  private:
    user_name username;
    user_info userinfo;
};

class user_system {
  public:
    user_system () { user_list.open("user/list"); }

    bool empty () { return user_list.empty(); }

    int add_user ( char* key[], char* arg[], const int len ) {
        user_name new_user=get(key, arg, len, "-u");
        user_info new_info( get(key, arg, len, "-p"),
                            get(key, arg, len, "-m"),
                            get(key, arg, len, "-n"),
                            get(key, arg, len, "-g") );
        if ( user_list.empty() ) {
            new_info.privilege=10;
            return add_user(new_user, new_info);
        }

        auto ptr=get(key, arg, len, "-c");
        if ( ptr==nullptr ) return add_user(new_user, new_info);
        user_name cur_user=ptr;

        if ( !user_log.count(cur_user) ) return -1;
        user_info cur_info=user_list.at(cur_user).first;
        if ( new_info>=cur_info ) return -1;
        return add_user(new_user, new_info);
    }

    int login ( char* key[], char* arg[], const int len ) {
        user_name user=get(key, arg, len, "-u");
        const char* pwd=get(key, arg, len, "-p");
        auto pr=user_list.at(user);
        user_info& info=pr.first;
        if ( !pr.second || info.password!=pwd ) return -1;
        return user_log.insert({user, 0}).second?0:-1;
    }
    int logout ( char* key[], char* arg[], const int len ) {
        user_name user=get(key, arg, len, "-u");
        auto it=user_log.find(user);
        if ( it==user_log.end() ) return -1;
        user_log.erase(it);
        return 0;
    }
    std::string query_profile ( char* key[], char* arg[], const int len ) {
        user_name cur_user=get(key, arg, len, "-c");
        if ( !user_log.count(cur_user) ) return "-1";
        auto cur_info=user_list.at(cur_user).first;

        user_name user=get(key, arg, len, "-u");
        if ( cur_user==user ) return user.str()+" "+cur_info.show();

        auto pr=user_list.at(user);
        auto info=pr.first;
        if ( cur_info<=info ) return "-1";
        return user.str()+" "+info.show();
    }
    std::string modify_profile ( char* key[], char* arg[], const int len ) {
        user_name cur_user=get(key, arg, len, "-c");
        user_name user=get(key, arg, len, "-u");

        if ( !user_log.count(cur_user) ) return "-1";
        auto pr=user_list.at(user);
        if ( !pr.second ) return "-1";

        auto info=pr.first;
        auto cur_info=user_list.at(cur_user).first;
        auto new_info=info;

        auto ptr_p=get(key, arg, len, "-p");
        if ( ptr_p!=nullptr ) new_info.password=ptr_p;
        auto ptr_m=get(key, arg, len, "-m");
        if ( ptr_m!=nullptr ) new_info.mailAddr=ptr_m;
        auto ptr_n=get(key, arg, len, "-n");
        if ( ptr_n!=nullptr ) new_info.name=ptr_n;
        auto ptr_g=get(key, arg, len, "-g");
        if ( ptr_g!=nullptr ) new_info.privilege=atoi(ptr_g);

        if ( new_info>=cur_info ) return "-1";
        if ( cur_user==user ) {
            user_list.update(user, new_info);
            return user.str()+" "+new_info.show();
        }
        if ( cur_info<=info ) return "-1";
        user_list.update(user, new_info);
        return user.str()+" "+new_info.show();
    }

    void clear () {}

  private:
    bplus_tree<user_name, user_info> user_list;
    map<user_name, char> user_log;

    int add_user ( const user_name& new_user, const user_info& new_info ) {
        user_list.insert(new_user, new_info);
        return 0;
    }
    char* get ( char* key[], char* arg[], int len, const char* src ) {
        for ( int i=0 ; i<len ; i++ )
            if ( !strcmp(key[i], src) ) return arg[i];
        return nullptr;
    }
};

}

#endif