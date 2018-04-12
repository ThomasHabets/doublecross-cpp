#include<iostream>
#include<unistd.h>
#include<thread>
#include"channel.h"

int
main()
{
  Channel<int> ich(0);
  std::thread th(
      [](ReadChannel<int> *ch){
        std::cout << "started thread\n";
        std::cout << "getting\n";
        std::cout << ch->get() << std::endl;
        std::cout << "… getting done\n";
        std::cout << "thread done\n";
      },
      &ich
  );

  std::cout << "Putting\n";
  //ich.put(1);
  std::cout << "… putting done\n";
  ich.close();
  sleep(1);
  ich.put(2);
  th.join();
}
