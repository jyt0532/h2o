#include <iostream> 
#include <cstdlib>
#include <thread>
#include <mutex>
#include <vector>
using namespace std;
class H2O {
    public:
        H2O()
            : shared()
              , OQ(), HQ()
              , number_of_O(0), number_of_H(0){}

        bool can_produce_water(){
            return number_of_O>= 1 && number_of_H>=2;
        }
        void close_all_thread(){
            unique_lock<mutex> lk(shared);
            while(number_of_H){
                number_of_H--;
                HQ.notify_one();
            }
            while(number_of_O){
                number_of_O--;
                OQ.notify_one();
            }
        }
        void H(){
            unique_lock<mutex> lk(shared);
            number_of_H++;
            if(can_produce_water()){
                cout << "H2O is produced" << endl;
                number_of_O--;
                number_of_H-=2;
                OQ.notify_one();
                HQ.notify_one();
            }else{
                HQ.wait(lk);
                //HQ.wait(lk, bind(&H2O::can_produce_water, this));
            }
            lk.unlock();
        }
        void O(){
            unique_lock<mutex> lk(shared);
            number_of_O++;
            if(can_produce_water()){
                cout << "H2O is produced" << endl;
                number_of_O--;
                number_of_H-=2;
                HQ.notify_one();
                HQ.notify_one();
            }else{
                OQ.wait(lk);
                //OQ.wait(lk, bind(&H2O::can_produce_water, this));
            }
            lk.unlock();
        }

    private:
        std::mutex              shared;
        std::condition_variable HQ;
        std::condition_variable OQ;
        int                     number_of_O;
        int                     number_of_H;
};
void print_block(H2O &h2o, int i){
    if(i % 3 == 0){
        h2o.O();
    }else{
        h2o.H();
    }
}
int main(){
    H2O h2o;
    std::vector<std::thread> threads;
    for(int i = 0; i < 34; i++){
        threads.push_back(std::thread(print_block, ref(h2o), i));//Have to pass by reference
    }
    this_thread::sleep_for(chrono::milliseconds(3000));
    h2o.close_all_thread();
    for(int i = 0; i < threads.size() ; i++)
    {   
        threads[i].join();
    } 
    return 0;
}
