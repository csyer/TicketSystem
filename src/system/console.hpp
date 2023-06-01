#ifndef SJTU_CONSOLE_HPP
#define SJTU_CONSOLE_HPP

#include "vector.hpp"
#include "list.hpp"

#include "system.hpp"
#include "train.hpp"
#include "user.hpp"

namespace cay {

struct order_info {
    int type;
    user_name user;
    Time startTime;
    int pos, needSeat, from, to, deltaTime;
    order_info () {}
    order_info ( int _type, const user_name& _user, const Time& _startTime,
                 int _pos, int _needSeat, int _from, int _to, int _deltaTime ):
        type(_type), user(_user), startTime(_startTime), 
        pos(_pos), needSeat(_needSeat), from(_from), to(_to), deltaTime(_deltaTime) {}

    order_info ( const order_info& obj ) {
        type=obj.type;
        user=obj.user;
        startTime=obj.startTime;
        pos=obj.pos;
        needSeat=obj.needSeat;
        from=obj.from;
        to=obj.to;
        deltaTime=obj.deltaTime;
    }
    order_info ( const order_info&& obj ) {
        type=obj.type;
        user=obj.user;
        startTime=obj.startTime;
        pos=obj.pos;
        needSeat=obj.needSeat;
        from=obj.from;
        to=obj.to;
        deltaTime=obj.deltaTime;
    }

    order_info& operator= ( const order_info& obj ) {
        if ( &obj==this ) return *this;
        type=obj.type;
        user=obj.user;
        startTime=obj.startTime;
        pos=obj.pos;
        needSeat=obj.needSeat;
        from=obj.from;
        to=obj.to;
        deltaTime=obj.deltaTime;
        return *this;
    }
    order_info& operator= ( const order_info&& obj ) {
        if ( &obj==this ) return *this;
        type=obj.type;
        user=obj.user;
        startTime=obj.startTime;
        pos=obj.pos;
        needSeat=obj.needSeat;
        from=obj.from;
        to=obj.to;
        deltaTime=obj.deltaTime;
        return *this;
    }
};
const std::string STATUS[]={"[success]", "[pending]", "[refunded]"};

class console : public system {
  public:
    console () {
        user_order.open("order/list");
        order_list.open("data/order/", "list.dat");
        waiting_list.open("order/waiting_list");
    }
    ~console () {
        order_list.close();
    }

    bool solve () {
        static char input[32768];
        std::cin.getline(input, 32768);

        int arg_num=0;
        char* id=strtok(input, " ");
        char* cmd=strtok(nullptr, " ");
        char* key[15];
        char* arg[15];
        for ( ; ; ++arg_num ) {
            key[arg_num]=strtok(nullptr, " ");
            if ( key[arg_num]==nullptr ) break;
            arg[arg_num]=strtok(nullptr, " ");
        }
        
        std::cout << id <<' ';
        if ( !strcmp(cmd, "add_user") ) {
            std::cout << user_sys.add_user(key, arg, arg_num) <<'\n';
        }
        if ( !strcmp(cmd, "login") ) {
            std::cout << user_sys.login(key, arg, arg_num) <<'\n';
        }
        if ( !strcmp(cmd, "logout") ) {
            std::cout << user_sys.logout(key, arg, arg_num) <<'\n';
        }
        if ( !strcmp(cmd, "query_profile") ) {
            user_sys.query_profile(key, arg, arg_num);
        }
        if ( !strcmp(cmd, "modify_profile") ) {
            user_sys.modify_profile(key, arg, arg_num);
        }

        if ( !strcmp(cmd, "add_train") ) {
            std::cout << train_sys.add_train(key, arg, arg_num) <<'\n';
        }
        if ( !strcmp(cmd, "delete_train") ) {
            std::cout << train_sys.delete_train(key, arg, arg_num) <<'\n';
        }
        if ( !strcmp(cmd, "release_train") ) {
            std::cout << train_sys.release_train(key, arg, arg_num) <<'\n';
        }
        if ( !strcmp(cmd, "query_train") ) {
            train_sys.query_train(key, arg, arg_num);
        }
        if ( !strcmp(cmd, "query_ticket") ) {
            train_sys.query_ticket(key, arg, arg_num);
        }
        if ( !strcmp(cmd, "query_transfer") ) {
            train_sys.query_transfer(key, arg, arg_num);
        }

        if ( !strcmp(cmd, "buy_ticket") ) {
            buy_ticket(key, arg, arg_num);
        }
        if ( !strcmp(cmd, "query_order") ) {
            query_order(key, arg, arg_num);
        }
        if ( !strcmp(cmd, "refund_ticket") ) {
            refund_ticket(key, arg, arg_num);
        }
        if ( !strcmp(cmd, "clean") ) {
            user_sys.clear();
            train_sys.clear();
        }
        if ( !strcmp(cmd, "exit") ) { 
            std::cout <<"bye\n";
            return false; 
        }
        return true;
    }
  private:
    user_system user_sys;
    train_system train_sys;

    database<order_info> order_list;
    using Tuple=pair<pair<int, int>, int>;
    bplus_tree<Tuple, char> waiting_list;
    bplus_tree<user_name, int> user_order;

    void buy_ticket ( char* key[], char* arg[], int len ) {
        user_name user(get(key, arg, len, "-u"));
        if ( !user_sys.is_login(user) ) {
            std::cout << FAIL <<'\n';
            return ;
        }

        trainID id(get(key, arg, len, "-i"));
        if ( !train_sys.released_train.count(id) ) {
            std::cout << FAIL <<'\n';
            return ;
        }

        station_name fromStation(get(key, arg, len, "-f")),
                     toStation(get(key, arg, len, "-t"));
        Date startDate(get(key, arg, len, "-d"));

        int t_pos=train_sys.all_train.at(id).first;
        train_info t_info(train_sys.train_list.read(t_pos));
        int needSeat=atoi(get(key, arg, len, "-n"));
        if ( needSeat>t_info.seatNum ) {
            std::cout << FAIL <<'\n';
            return ;
        }

        int left, right;
        for ( left=0 ; left<t_info.stationNum ; left++ ) 
            if ( t_info.stations[left]==fromStation ) break;
        if ( left==t_info.stationNum ) {
            std::cout << FAIL <<'\n';
            return ;
        }

        Clock nowTime=t_info.startTime;
        int deltaDate=nowTime.add(t_info.travelingTimes[left]+t_info.stopoverTimes[left]);
        Date firstDate=startDate-deltaDate;

        auto& range=t_info.saleDate;
        if ( firstDate<range.first || firstDate>range.second ) {
            std::cout << FAIL <<'\n';
            return ;
        }
        Time startTime(firstDate, t_info.startTime);
        int dT=firstDate-range.first;

        int s_pos=train_sys.date_seat.at(t_pos).first+dT;
        seat_info s_info(train_sys.seat_list.read(s_pos));

        int maxSeat=1000000;
        for ( right=left ; right<t_info.stationNum ; right++ ) {
            if ( t_info.stations[right]==toStation ) break;
            maxSeat=std::min(maxSeat, s_info.seats[right]);
        }
        if ( right==t_info.stationNum ) {
            std::cout << FAIL <<'\n';
            return ;
        }

        if ( needSeat<=maxSeat ) {
            for ( int i=left ; i<right ; i++ ) 
                s_info.seats[i]-=needSeat;
            train_sys.seat_list.write(s_pos, s_info);
            int o_pos=order_list.push_back(order_info(0, user, startTime, t_pos, needSeat, left, right, dT));
            user_order.insert(user, o_pos);
            std::cout << (long long)needSeat*(long long)(t_info.prices[right]-t_info.prices[left]) <<'\n';
            return ;
        }

        auto q_ptr=get(key, arg, len, "-q");
        if ( q_ptr==nullptr || *q_ptr=='f' ) {
            std::cout << FAIL <<'\n';
            return ;
        }

        int o_pos=order_list.push_back(order_info(1, user, startTime, t_pos, needSeat, left, right, dT));
        user_order.insert(user, o_pos);

        // waiting_list.push_back(o_pos);
        waiting_list.insert(Tuple(pair<int, int>(t_pos, dT), o_pos), 0);

        std::cout <<"queue\n";
    }
    void query_order ( char* key[], char* arg[], int len ) {
        user_name user(get(key, arg, len, "-u"));
        if ( !user_sys.is_login(user) ) {
            std::cout << FAIL <<'\n';
            return ;
        }

        vector<int> all_order(user_order.find_range(user));
        if ( all_order.empty() ) {
            std::cout << 0 <<'\n';
            return ;
        }
        else std::cout << all_order.size() <<'\n';

        all_order.sort(default_int_cmp_less);
        for ( int i=0 ; i<all_order.size() ; i++ ) {
            order_info o_info(order_list.read(all_order[i]));
            train_info t_info(train_sys.train_list.read(o_info.pos));
            const int& o_from=o_info.from;
            const int& o_to=o_info.to;
            std::cout << STATUS[o_info.type] <<' ';
            t_info.id.print(); std::cout <<' ';;
            t_info.stations[o_from].print(); std::cout <<' ';;
            Time lTime=o_info.startTime+t_info.travelingTimes[o_from]+t_info.stopoverTimes[o_from];
            lTime.show();
            std::cout <<" -> ";
            t_info.stations[o_to].print(); std::cout <<' ';;
            Time aTime=o_info.startTime+t_info.travelingTimes[o_to];
            aTime.show();
            std::cout <<' '<< t_info.prices[o_to]-t_info.prices[o_from] <<' '<< o_info.needSeat <<'\n';
        }
    }
    void refund_ticket ( char* key[], char* arg[], int len ) {
        user_name user(get(key, arg, len, "-u"));
        if ( !user_sys.is_login(user) ) {
            std::cout << FAIL <<'\n';
            return ;
        }
        auto n_ptr=get(key, arg, len, "-n");
        int order_id;
        if ( n_ptr==nullptr ) order_id=1;
        else order_id=atoi(n_ptr);

        vector<int> all_order(user_order.find_range(user));
        if ( all_order.size()<order_id ) {
            std::cout << FAIL <<'\n';
            return ;
        }
        all_order.sort(default_int_cmp_less);
        int r_pos=all_order[order_id-1];
        order_info o_info=order_list.read(r_pos);
        if ( o_info.type==2 ) {
            std::cout << FAIL <<'\n';
            return ;
        }
        if ( o_info.type==0 ) {
            int s_pos=train_sys.date_seat.at(o_info.pos).first+o_info.deltaTime;
            seat_info s_info(train_sys.seat_list.read(s_pos));
            for ( int i=o_info.from ; i<o_info.to ; i++ ) 
                s_info.seats[i]+=o_info.needSeat;
            train_sys.seat_list.write(s_pos, s_info);
        }
        o_info.type=2;
        order_list.write(r_pos, o_info);

        pair<int, int> pr(o_info.pos, o_info.deltaTime);
        vector<int> waiting(waiting_list.find_range(Tuple(pr, -1), Tuple(pr, 1000000000)));
        waiting.sort(default_int_cmp_greater);
        for ( int i=0 ; i<waiting.size() ; i++ ) {
            int& o_pos=waiting[i];
            if ( o_pos==r_pos ) {
                waiting_list.erase(Tuple(pr, o_pos));
                continue;
            }
            order_info it(order_list.read(o_pos));
            int s_pos=train_sys.date_seat.at(it.pos).first+it.deltaTime;
            seat_info s_info(train_sys.seat_list.read(s_pos));
            int maxSeat=1000000;
            for ( int k=it.from ; k<it.to ; k++ ) 
                maxSeat=std::min(maxSeat, s_info.seats[k]);
            if ( it.needSeat<=maxSeat ) {
                for ( int k=it.from ; k<it.to ; k++ ) 
                    s_info.seats[k]-=it.needSeat;
                train_sys.seat_list.write(s_pos, s_info);
                waiting_list.erase(Tuple(pr, o_pos));
                it.type=0;
                order_list.write(o_pos, it);
            }
        }

        std::cout << SUCCESS <<'\n';
    }
};

}

#endif