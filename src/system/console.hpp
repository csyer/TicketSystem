#ifndef SJTU_CONSOLE_HPP
#define SJTU_CONSOLE_HPP

#include "ticket.hpp"
#include "train.hpp"
#include "user.hpp"

namespace cay {

class console {
  public:
    console () {}

    bool solve () {
        char input[4096];
        std::cin.getline(input, 4095);

        int arg_num=0;
        char* id=strtok(input, " ");
        char* cmd=strtok(nullptr, " ");
        char* key[10];
        char* arg[10];
        for ( ; ; ++arg_num ) {
            key[arg_num]=strtok(nullptr, " ");
            if ( key[arg_num]==nullptr ) break;
            arg[arg_num]=strtok(nullptr, " ");
            // std::cerr << key[arg_num] <<' '<< arg[arg_num] <<' ';
        }
        
        std::cout << id <<' ';
        if ( !strcmp(cmd, "add_user") ) {
            std::cout << user.add_user(key, arg, arg_num) <<'\n';
        }
        if ( !strcmp(cmd, "login") ) {
            std::cout << user.login(key, arg, arg_num) <<'\n';
        }
        if ( !strcmp(cmd, "logout") ) {
            std::cout << user.logout(key, arg, arg_num) <<'\n';
        }
        if ( !strcmp(cmd, "query_profile") ) {
            std::cout << user.query_profile(key, arg, arg_num) <<'\n';
        }
        if ( !strcmp(cmd, "modify_profile") ) {
            std::cout << user.modify_profile(key, arg, arg_num) <<'\n';
        }

        if ( !strcmp(cmd, "delete_train") ) {}
        if ( !strcmp(cmd, "release_train") ) {}
        if ( !strcmp(cmd, "query_train") ) {}
        if ( !strcmp(cmd, "query_ticket") ) {}
        if ( !strcmp(cmd, "query_transfer") ) {}

        if ( !strcmp(cmd, "buy_ticket") ) {}
        if ( !strcmp(cmd, "query_order") ) {}
        if ( !strcmp(cmd, "refund_ticket") ) {}
        if ( !strcmp(cmd, "clean") ) {
            user.clear();
            train.clear();
            ticket.clear();
        }
        if ( !strcmp(cmd, "exit") ) { 
            std::cout <<"bye\n";
            return false; 
        }
        return true;
    }
  private:
    user_system user;
    train_system train;
    ticket_system ticket;
};

}

#endif