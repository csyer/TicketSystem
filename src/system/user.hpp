#ifndef SJTU_USER_HPP
#define SJTU_USER_HPP

#include "bplus_tree.hpp"
#include "console.hpp"
#include "map.hpp"
#include "system.hpp"
#include "utility.hpp"

namespace cay {

using user_name=string<20>;
struct user_info {
    string<30> password, mailAddr;
    string<15> name;
    int privilege;

    user_info () {}
    user_info ( const string<30> _password, 
                const string<30> _mailAddr, 
                const string<15> _name, 
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

class user_system : public system {
    friend class console;
  public:
    user_system () { user_list.open("user"); }

    bool empty () { return user_list.empty(); }

    int add_user ( char* key[], char* arg[], const int len ) {
        user_name new_user=get(key, arg, len, "-u");
        user_info new_info( get(key, arg, len, "-p"),
                            get(key, arg, len, "-m"),
                            get(key, arg, len, "-n"),
                            get(key, arg, len, "-g") );
        if ( user_list.count(new_user) ) return FAIL;
        if ( user_list.empty() ) {
            new_info.privilege=10;
            return add_user(new_user, new_info);
        }

        auto ptr=get(key, arg, len, "-c");
        if ( ptr==nullptr ) return add_user(new_user, new_info);
        user_name cur_user(ptr);

        if ( !user_log.count(cur_user) ) return FAIL;
        user_info cur_info=user_list.at(cur_user).first;
        if ( new_info>=cur_info ) return FAIL;
        return add_user(new_user, new_info);
    }
    int login ( char* key[], char* arg[], const int len ) {
        user_name user=get(key, arg, len, "-u");
        const char* pwd=get(key, arg, len, "-p");
        auto pr=user_list.at(user);
        user_info& info=pr.first;
        if ( !pr.second || info.password!=pwd ) return FAIL;
        return user_log.insert({user, 0}).second?SUCCESS:FAIL;
    }
    int logout ( char* key[], char* arg[], const int len ) {
        user_name user=get(key, arg, len, "-u");
        auto it=user_log.find(user);
        if ( it==user_log.end() ) return FAIL;
        user_log.erase(it);
        return SUCCESS;
    }
    void query_profile ( char* key[], char* arg[], const int len ) {
        user_name cur_user=get(key, arg, len, "-c");
        if ( !user_log.count(cur_user) ) {
            std::cout << FAIL <<'\n';
            return ;
        }
        auto cur_info=user_list.at(cur_user).first;

        user_name user=get(key, arg, len, "-u");
        if ( cur_user==user ) {
            std::cout << user.str() <<" "<< cur_info.show() <<'\n';
            return ;
        }

        auto pr=user_list.at(user);
        auto info=pr.first;

        if ( !pr.second ) {
            std::cout << FAIL <<'\n';
            return ;
        }
        if ( cur_info<=info ) {
            std::cout << FAIL <<'\n';
            return ;
        }
        std::cout << user.str() <<" "<< info.show() <<'\n';
        return ;
    }
    void modify_profile ( char* key[], char* arg[], const int len ) {
        user_name cur_user=get(key, arg, len, "-c");
        user_name user=get(key, arg, len, "-u");

        if ( !user_log.count(cur_user) ) {
            std::cout << FAIL <<'\n';
            return ;
        }
        auto pr=user_list.at(user);
        if ( !pr.second ) {
            std::cout << FAIL <<'\n';
            return ;
        }

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

        if ( cur_user==user ) {
            if ( ptr_g==nullptr ) {
                user_list.update(user, new_info);
                std::cout << user.str() <<" "<< new_info.show() <<'\n';
            }
            else std::cout << FAIL <<'\n';
            return ;
        }
        if ( new_info>=cur_info ) {
            std::cout << FAIL <<'\n';
            return ;
        }
        if ( cur_info<=info ) {
            std::cout << FAIL <<'\n';
            return ;
        }
        user_list.update(user, new_info);
        std::cout << user.str() <<" "<< new_info.show() <<'\n';
    }
    void clear () {
        user_list.clear();
        user_log.clear();
    }

  private:
    bplus_tree<user_name, user_info> user_list;
    map<user_name, char> user_log;

    int add_user ( const user_name& new_user, const user_info& new_info ) {
        user_list.insert(new_user, new_info);
        return SUCCESS;
    }
    bool is_login ( const user_name& user ) {
        return user_log.count(user);
    }
};

}

#endif