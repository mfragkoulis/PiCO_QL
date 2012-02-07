#include "SuperAccount.h"

using namespace std;

SuperAccount::SuperAccount(){
    iba = 0;
}

SuperAccount::SuperAccount(int ib){

    iba = ib;

}

int SuperAccount::get_iba(){

    return iba;

}
