#ifndef SJTU_TICKET_HPP
#define SJTU_TICKET_HPP

namespace cay {

class ticket_system {
  public:
    ticket_system () {}

    void buy_ticket ( char* key[], char* arg[], int len ) {}
    void query_order ( char* key[], char* arg[], int len ) {}
    void refund_ticket ( char* key[], char* arg[], int len ) {}
    void clear () {}
  private:
};

}

#endif