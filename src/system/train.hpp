#ifndef SJTU_TRAIN_HPP
#define SJTU_TRAIN_HPP

#include "bplus_tree.hpp"
#include "console.hpp"
#include "map.hpp"
#include "system.hpp"
#include "utility.hpp"

namespace cay {

using trainID=string<20>;
using station_name=string<30>;
using DateRange=pair<Date, Date>;
struct train_info {
    trainID id;
    int stationNum, seatNum;
    station_name stations[100];
    int prices[100];
    Clock startTime;
    int travelingTimes[100], stopoverTimes[100];
    DateRange saleDate;
    char type;

    train_info () {
        stationNum=seatNum=0;
        memset(prices, 0, sizeof(prices));
        memset(travelingTimes, 0, sizeof(travelingTimes));
        memset(stopoverTimes, 0, sizeof(stopoverTimes));
    }

    train_info ( const train_info& obj ):
        id(obj.id), stationNum(obj.stationNum), seatNum(obj.seatNum), 
        startTime(obj.startTime), saleDate(obj.saleDate), type(obj.type) {
        memcpy(stations, obj.stations, sizeof(stations));
        memcpy(prices, obj.prices, sizeof(prices));
        memcpy(travelingTimes, obj.travelingTimes, sizeof(travelingTimes));
        memcpy(stopoverTimes, obj.stopoverTimes, sizeof(stopoverTimes));
    }
    train_info ( const train_info&& obj ):
        id(obj.id), stationNum(obj.stationNum), seatNum(obj.seatNum), 
        startTime(obj.startTime), saleDate(obj.saleDate), type(obj.type) {
        memcpy(stations, obj.stations, sizeof(stations));
        memcpy(prices, obj.prices, sizeof(prices));
        memcpy(travelingTimes, obj.travelingTimes, sizeof(travelingTimes));
        memcpy(stopoverTimes, obj.stopoverTimes, sizeof(stopoverTimes));
    }
    
    train_info& operator= ( const train_info& obj ) {
        if ( &obj==this ) return *this;
        id=obj.id, 
        stationNum=obj.stationNum, 
        seatNum=obj.seatNum, 
        startTime=obj.startTime, 
        saleDate=obj.saleDate, 
        type=obj.type;
        memcpy(stations, obj.stations, sizeof(stations));
        memcpy(prices, obj.prices, sizeof(prices));
        memcpy(travelingTimes, obj.travelingTimes, sizeof(travelingTimes));
        memcpy(stopoverTimes, obj.stopoverTimes, sizeof(stopoverTimes));
        return *this;
    }
    train_info& operator= ( const train_info&& obj ) {
        if ( &obj==this ) return *this;
        id=obj.id, 
        stationNum=obj.stationNum, 
        seatNum=obj.seatNum, 
        startTime=obj.startTime, 
        saleDate=obj.saleDate, 
        type=obj.type;
        memcpy(stations, obj.stations, sizeof(stations));
        memcpy(prices, obj.prices, sizeof(prices));
        memcpy(travelingTimes, obj.travelingTimes, sizeof(travelingTimes));
        memcpy(stopoverTimes, obj.stopoverTimes, sizeof(stopoverTimes));
        return *this;
    }
};
struct seat_info {
    int stationNum, seats[100];
    seat_info () { memset(seats, 0, sizeof(seats)); }
    seat_info ( int _stationNum, int _seatNum ):stationNum(_stationNum) {
        memset(seats, 0, sizeof(seats));
        for ( int i=0 ; i<stationNum-1 ; i++ ) 
            seats[i]=_seatNum;
    }

    seat_info ( const seat_info& obj ):stationNum(obj.stationNum) {
        memcpy(seats, obj.seats, sizeof(seats));
    }
    seat_info ( const seat_info&& obj ):stationNum(obj.stationNum) {
        memcpy(seats, obj.seats, sizeof(seats));
    }

    seat_info& operator= ( const seat_info& obj ) {
        if ( &obj==this ) return *this;
        stationNum=obj.stationNum;
        memcpy(seats, obj.seats, sizeof(seats));
        return *this;
    }
    seat_info& operator= ( const seat_info&& obj ) {
        if ( &obj==this ) return *this;
        stationNum=obj.stationNum;
        memcpy(seats, obj.seats, sizeof(seats));
        return *this;
    }
};
struct ticket_info {
    trainID id;
    Time leavingTime, arrivingTime;
    int price, time, seat;
    ticket_info () {}
    ticket_info ( const trainID& _id, 
                  const Time& _leavingTime, 
                  const Time& _arrivingTime, 
                  const int& _price, const int& _time, const int& _seat ):
        id(_id), leavingTime(_leavingTime), arrivingTime(_arrivingTime),
        price(_price), time(_time), seat(_seat) {}

    ticket_info ( const ticket_info& obj ) {
        id=obj.id;
        leavingTime=obj.leavingTime;
        arrivingTime=obj.arrivingTime;
        price=obj.price;
        time=obj.time;
        seat=obj.seat;
    }
    ticket_info ( const ticket_info&& obj ) {
        id=obj.id;
        leavingTime=obj.leavingTime;
        arrivingTime=obj.arrivingTime;
        price=obj.price;
        time=obj.time;
        seat=obj.seat;
    }
    ticket_info& operator= ( const ticket_info& obj ) {
        if ( &obj==this ) return *this;
        id=obj.id;
        leavingTime=obj.leavingTime;
        arrivingTime=obj.arrivingTime;
        price=obj.price;
        time=obj.time;
        seat=obj.seat;
        return *this;
    }
    ticket_info& operator= ( const ticket_info&& obj ) {
        if ( &obj==this ) return *this;
        id=obj.id;
        leavingTime=obj.leavingTime;
        arrivingTime=obj.arrivingTime;
        price=obj.price;
        time=obj.time;
        seat=obj.seat;
        return *this;
    }
};
bool compare_with_price ( const ticket_info& a, const ticket_info& b ) {
    if ( a.price==b.price ) return a.id<b.id;
    return a.price<b.price;
}
bool compare_with_time ( const ticket_info& a, const ticket_info& b ) {
    if ( a.time==b.time ) return a.id<b.id;
    return a.time<b.time;
}
bool default_int_cmp_less ( const int& x, const int& y ) { return x<y; }

struct station_info {
    trainID id;
    int pos, price, seat;
    station_name from, to;
    Time leavingTime, arrivingTime;

    station_info () {}
    station_info ( const trainID& _id, int _pos, int _price, int _seat, 
                   const station_name& _from, const station_name& _to, 
                   const Time& _leavingTime, const Time& _arrivingTime ):
        id(_id), pos(_pos), price(_price), seat(_seat), 
        from(_from), to(_to), leavingTime(_leavingTime), arrivingTime(_arrivingTime) {}
};

class train_system : public system {
    friend class console;
  public:
    train_system () {
        train_list.open("data/train/", "train_list.dat");
        seat_list.open("data/train/", "seat_list.dat");
        date_seat.open("train/date_seat");
        all_train.open("train/all_train");
        released_train.open("train/released_train");
        station_list.open("train/station_list");
    }

    train_info get_train ( int pos ) {
        return train_list.read(pos);
    }

    int add_train ( char* key[], char* arg[], int len ) {
        trainID id(get(key, arg, len, "-i"));
        if ( all_train.count(id) ) return FAIL;

        train_info info;
        info.id=id;
        info.stationNum=atoi(get(key, arg, len, "-n"));
        info.seatNum=atoi(get(key, arg, len, "-m"));
        info.startTime=Clock(get(key, arg, len, "-x"));
        
        auto s_ptr=get(key, arg, len, "-s");
        for ( int i=0 ; i<info.stationNum ; i++ )
            info.stations[i]=strsep(&s_ptr, "|");
        auto o_ptr=get(key, arg, len, "-o");
        for ( int i=1 ; i<info.stationNum-1 ; i++ ) 
            info.stopoverTimes[i]=atoi(strsep(&o_ptr, "|"));
        auto p_ptr=get(key, arg, len, "-p");
        auto t_ptr=get(key, arg, len, "-t");
        for ( int i=1 ; i<info.stationNum ; i++ ) {
            info.prices[i]=atoi(strsep(&p_ptr, "|"));
            info.travelingTimes[i]=atoi(strsep(&t_ptr, "|"));

            info.prices[i]+=info.prices[i-1];
            info.travelingTimes[i]+=info.travelingTimes[i-1];
            if ( i>1 ) info.travelingTimes[i]+=info.stopoverTimes[i-1];
        }

        auto d_ptr=get(key, arg, len, "-d");
        auto left=strsep(&d_ptr, "|"), right=strsep(&d_ptr, "|");
        info.saleDate=DateRange(Date(left), Date(right));
        info.type=*get(key, arg, len, "-y");
        
        int pos=train_list.push_back(info);
        all_train.insert(id, pos);
        return SUCCESS;
    }
    int delete_train ( char* key[], char* arg[], int len ) {
        trainID id=get(key, arg, len, "-i");
        if ( released_train.count(id) ) return FAIL;
        return all_train.erase(id)?SUCCESS:FAIL;
    }
    int release_train ( char* key[], char* arg[], int len ) {
        trainID id=get(key, arg, len, "-i");
        auto pr=all_train.at(id);
        if ( !pr.second || released_train.count(id) ) return FAIL;
        train_info info=train_list.read(pr.first);
        for ( int i=0 ; i<info.stationNum ; i++ ) 
            station_list.insert({info.stations[i], pr.first}, 0);

        seat_info s_info(info.stationNum, info.seatNum);
        for ( Date i=info.saleDate.first ; i<=info.saleDate.second ; ++i ) {
            int s_pos=seat_list.push_back(s_info);
            date_seat.insert(pair<int, Date>(pr.first, i), s_pos);
        }

        released_train.insert(id, 0);
        return SUCCESS;
    }

    void query_train ( char* key[], char* arg[], int len ) {
        trainID id(get(key, arg, len, "-i"));

        int is_released=0;
        if ( released_train.count(id) ) is_released=1;

        auto pr=all_train.at(id);
        if ( !pr.second ) {
            std::cout << FAIL <<'\n';
            return ;
        }
        int t_pos=pr.first;
        train_info t_info=train_list.read(t_pos);

        Date startDate(get(key, arg, len, "-d"));
        auto range=t_info.saleDate;
        if ( startDate<range.first || startDate>range.second ) {
            std::cout << FAIL <<'\n';
            return ;
        }

        int s_pos;
        seat_info s_info;
        if ( is_released ) {
            s_pos=date_seat.at(pair<int, Date>(t_pos, startDate)).first;
            s_info=seat_list.read(s_pos);
        }

        std::cout << id.str() <<' '<< t_info.type <<'\n';
        Time startTime(startDate, t_info.startTime);
        for ( int i=0 ; i<t_info.stationNum ; i++ ) {
            std::cout << t_info.stations[i].str() <<' ';
            Time nowTime=startTime+t_info.travelingTimes[i];
            if ( i==0 ) std::cout <<"xx-xx xx:xx";
            else std::cout << nowTime.show();
            std::cout <<" -> ";
            if ( i==t_info.stationNum-1 ) std::cout <<"xx-xx xx:xx";
            else std::cout << (nowTime+t_info.stopoverTimes[i]).show();
            std::cout <<' '<< t_info.prices[i] <<' ';
            if ( i==t_info.stationNum-1 ) std::cout <<"x\n";
            else {
                if ( is_released ) std::cout << s_info.seats[i] <<"\n";
                else std::cout << t_info.seatNum <<'\n';
            }
        }
        return ;
    }
    void query_ticket ( char* key[], char* arg[], int len ) {
        vector<ticket_info> ret;

        station_name fromStation(get(key, arg, len, "-s")),
                     toStation(get(key, arg, len, "-t"));
        vector<int> from_train(station_list.find_range({fromStation, -1}, {fromStation, 100000000})),
                    to_train(station_list.find_range({toStation, -1}, {toStation, 100000000}));
        Date startDate(get(key, arg, len, "-d"));
        from_train.sort(default_int_cmp_less);
        to_train.sort(default_int_cmp_less);
        int size_from=from_train.size(), size_to=to_train.size();
        for ( int i=0, j=0 ; j<size_to && i<size_from ; i++ ) {
            while ( j<size_to && from_train[i]>to_train[j] ) j++;
            if ( j==size_to || from_train[i]!=to_train[j] ) continue;

            train_info t_info(train_list.read(from_train[i]));
            int left=0, right=0;
            for ( left=0 ; left<t_info.stationNum ; left++ ) 
                if ( t_info.stations[left]==fromStation ) break;

            Clock nowTime=t_info.startTime;
            int deltaDate=nowTime.add(t_info.travelingTimes[left]+t_info.stopoverTimes[left]);
            Date firstDate=startDate-deltaDate;

            auto& range=t_info.saleDate;
            if ( firstDate<range.first || firstDate>range.second ) continue;
            Time startTime(firstDate, t_info.startTime);

            for ( right=left ; right<t_info.stationNum ; right++ )
                if ( t_info.stations[right]==toStation ) break;
            if ( right==t_info.stationNum ) continue;

            int s_pos=date_seat.at(pair<int, Date>(from_train[i], firstDate)).first;
            seat_info s_info(seat_list.read(s_pos));
            int maxSeat=1000000;
            for ( int i=left ; i<right ; i++ ) maxSeat=std::min(maxSeat, s_info.seats[i]);

            Time lTime=startTime+t_info.travelingTimes[left]+t_info.stopoverTimes[left],
                 aTime=startTime+t_info.travelingTimes[right];
            int sumTime=t_info.travelingTimes[right]-t_info.travelingTimes[left]-t_info.stopoverTimes[left],
                sumPrice=t_info.prices[right]-t_info.prices[left];
            ret.push_back(ticket_info(t_info.id, lTime, aTime, sumPrice, sumTime, maxSeat));

            j++;
        }
        auto p_ptr=get(key, arg, len, "-p");
        if ( p_ptr==nullptr || *p_ptr=='t' ) ret.sort(compare_with_time);
        else ret.sort(compare_with_price);

        std::cout << ret.size() <<'\n';
        for ( auto i=0 ; i<ret.size() ; i++ ) {
            ticket_info& ticket=ret[i];
            std::cout << ticket.id.str() <<' ';
            std::cout << fromStation.str() <<' ';
            std::cout << ticket.leavingTime.show() <<' ';
            std::cout <<"-> ";
            std::cout << toStation.str() <<' ';
            std::cout << ticket.arrivingTime.show() <<' ';
            std::cout << ticket.price <<' '<< ticket.seat <<'\n';
        }
        return ;
    }
    void query_transfer ( char* key[], char* arg[], int len ) {
        auto show=[]( const station_info& train ) -> void {
            std::cout << train.id.str() <<' ';
            std::cout << train.from.str() <<' ';
            std::cout << train.leavingTime.show() <<' ';
            std::cout <<"-> ";
            std::cout << train.to.str() <<' ';
            std::cout << train.arrivingTime.show() <<' ';
            std::cout << train.price <<' '<< train.seat <<'\n';
        };

        station_name fromStation(get(key, arg, len, "-s")),
                     toStation(get(key, arg, len, "-t"));
        vector<int> fromTrain(station_list.find_range({fromStation, -1}, {fromStation, 100000000})),
                    toTrain(station_list.find_range({toStation, -1}, {toStation, 100000000}));
        Date startDate(get(key, arg, len, "-d"));

        vector<station_info> trans_list;
        for ( int i=0 ; i<fromTrain.size() ; i++ ) {
            train_info t_info(train_list.read(fromTrain[i]));

            int left, right;
            for ( left=0 ; left<t_info.stationNum ; left++ ) 
                if ( t_info.stations[left]==fromStation ) break;
                
            Clock nowTime=t_info.startTime;
            int deltaDate=nowTime.add(t_info.travelingTimes[left]+t_info.stopoverTimes[left]);
            Date firstDate=startDate-deltaDate;

            auto& range=t_info.saleDate;
            if ( firstDate<range.first || firstDate>range.second ) continue;
            Time startTime(firstDate, t_info.startTime);

            int s_pos=date_seat.at(pair<int, Date>(fromTrain[i], firstDate)).first;
            seat_info s_info(seat_list.read(s_pos));
            
            int maxSeat=1000000;
            for ( right=left+1 ; right<t_info.stationNum ; right++ ) { 
                Time lTime=startTime+t_info.travelingTimes[left]+t_info.stopoverTimes[left],
                     aTime=startTime+t_info.travelingTimes[right];
                station_name from=t_info.stations[left], to=t_info.stations[right];
                maxSeat=std::min(maxSeat, s_info.seats[right-1]);
                int sumPrice=t_info.prices[right]-t_info.prices[left];

                trans_list.push_back(station_info(t_info.id, fromTrain[i], sumPrice, maxSeat, from, to, lTime, aTime));
            }
        }
        
        int flg=0;
        auto p_ptr=get(key, arg, len, "-p");
        if ( p_ptr==nullptr || *p_ptr=='t' ) flg=1;

        station_info train1, train2;
        int minTime=100000000, minPrice=100000000;
        for ( int i=0 ; i<toTrain.size() ; i++ ) {
            train_info t_info(train_list.read(toTrain[i]));

            int left, right;
            for ( right=0 ; right<t_info.stationNum ; right++ ) 
                if ( t_info.stations[right]==toStation ) break;

            for ( left=right-1 ; left>=0 ; left-- ) {
                for ( int i=0 ; i<trans_list.size() ; i++ ) {
                    auto& trans_info=trans_list[i];
                    if ( trans_info.pos==toTrain[i] ) continue;
                    if ( trans_info.to!=t_info.stations[left] ) continue;

                    Clock nowTime=t_info.startTime;
                    int deltaDate=nowTime.add(t_info.travelingTimes[left]+t_info.stopoverTimes[left]);
                    Time lTime, aTime;
                    Date firstDate;
                    if ( trans_info.arrivingTime.get_clock()<=nowTime ) {
                        Date startDate=trans_info.arrivingTime.get_date();
                        firstDate=startDate-deltaDate;
                        auto& range=t_info.saleDate;
                        if ( firstDate>range.second ) continue;
                        if ( firstDate<range.first ) firstDate=range.first;
                    }
                    else {
                        Date startDate=trans_info.arrivingTime.get_date()+1;
                        firstDate=startDate-deltaDate;
                        auto& range=t_info.saleDate;
                        if ( firstDate>range.second ) continue;
                        if ( firstDate<range.first ) firstDate=range.first;
                    }
                    Time startTime(firstDate, t_info.startTime);
                    lTime=startTime+t_info.travelingTimes[left]+t_info.stopoverTimes[left];
                    aTime=startTime+t_info.travelingTimes[right];

                    int s_pos=date_seat.at(pair<int, Date>(toTrain[i], firstDate)).first, maxSeat=1000000;
                    seat_info s_info=seat_list.read(s_pos);
                    for ( int k=left ; k<right ; k++ ) 
                        maxSeat=std::min(maxSeat, s_info.seats[k]);

                    int sumTime=aTime-trans_info.leavingTime,
                        sumPrice=trans_info.price+t_info.prices[right]-t_info.prices[left],
                        sPrice=t_info.prices[right]-t_info.prices[left];
                    station_name from=trans_info.to, to=toStation;
                    trainID nowId=t_info.id;
                    station_info second_train(station_info(nowId, toTrain[i], sPrice, maxSeat, from, to, lTime, aTime));

                    int solved=0;
                    if ( flg ) {
                        if ( sumTime!=minTime ) {
                            solved=1;
                            if ( sumTime<minTime ) {
                                train1=trans_info, 
                                train2=second_train,
                                minTime=sumTime, minPrice=sumPrice;
                            }
                        }
                        else {
                            if ( sumPrice!=minPrice ) {
                                solved=1;
                                if ( sumPrice<minPrice ) {
                                    train1=trans_info, 
                                    train2=second_train;
                                    minTime=sumTime, minPrice=sumPrice;
                                }
                            }
                        }
                    }
                    else {
                        if ( sumPrice!=minPrice ) {
                            solved=1;
                            if ( sumPrice<minPrice ) {
                                train1=trans_info, 
                                train2=second_train;
                                minTime=sumTime, minPrice=sumPrice;
                            }
                        }
                        else {
                            if ( sumTime!=minTime ) {
                                solved=1;
                                if ( sumTime<minTime ) {
                                    train1=trans_info, 
                                    train2=second_train;
                                    minTime=sumTime, minPrice=sumPrice;
                                }
                            }
                        }
                    }

                    if ( !solved ) {
                        if ( trans_info.id!=train1.id ) {
                            if ( trans_info.id<train1.id ) {
                                train1=trans_info, 
                                train2=station_info(nowId, toTrain[i], sPrice, maxSeat, from, to, lTime, aTime);
                            }
                        }
                        else {
                            if ( nowId<train2.id ) {
                                train1=trans_info, 
                                train2=station_info(nowId, toTrain[i], sPrice, maxSeat, from, to, lTime, aTime);
                            }
                        }
                    }
                }
            }


        }

        if ( minTime==100000000 ) {
            std::cout << 0 <<'\n';
            return ;
        }

        show(train1);
        show(train2);
    }

    void clear () {
        train_list.clear();
        seat_list.clear();
        date_seat.clear();
        all_train.clear();
        released_train.clear();
        station_list.clear();
    }
  private:
    database<train_info> train_list;
    database<seat_info> seat_list;
    bplus_tree<pair<int, Date>, int> date_seat;
    bplus_tree<trainID, int> all_train;
    bplus_tree<trainID, int> released_train;
    bplus_tree<pair<station_name, int>, char> station_list;
};

}

#endif