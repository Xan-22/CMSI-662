#include <atomic>
#include <mutex>
#include <thread>
#include <iostream>
#include <vector>

struct Deck {
  std::atomic<unsigned int> cards = 52;
  int draw() {
    return (cards > 0) ? (--cards, 1) : -1;
  }
};

class Participant {
  static std::atomic<unsigned int> globalId;
  const unsigned int id;
  unsigned int hand = 0;
public:
  std::mutex drawMutex;
  Participant() = delete;
  explicit Participant(const Deck &cards) : id(globalId++) {}
  unsigned int get_id() const { return id; }
  bool draw(int cards) { 
    if (cards <= 0) return false;
    hand += cards;
    return true;
  }
};

std::atomic<unsigned int> Participant::globalId(1);

void synchronizeDraw(Participant *p1, Participant *p2, Deck &deck) {
  std::mutex *first;
  std::mutex *second;

  while(deck.cards > 0) {
    // Lock the mutexes in a consistent order to avoid deadlocks
    std::vector<std::mutex*> locks = {&p1->drawMutex, &p2->drawMutex};
    // Ensure proper ordering for locking.
    if (p1->get_id() < p2->get_id()) {
      first = &p1->drawMutex;
      second = &p2->drawMutex;
    } else {
      first = &p2->drawMutex;
      second = &p1->drawMutex;
    }

    std::lock_guard<std::mutex> firstLock(*locks[0]);
    std::lock_guard<std::mutex> secondLock(*locks[1]);
    std::cout << "There are " << deck.cards.load() << " cards left in the deck\n";
    if (p1->draw(deck.draw())) {
      std::cout << "Participant " << p1->get_id() << " drew a card.\n";
    }
    if (p2->draw(deck.draw())) {
      std::cout << "Participant " << p2->get_id() << " drew a card.\n";
    }
  }
}

int main(void) {
  Deck cards;
  Participant p1(cards); Participant p2(cards);
  std::thread thr1(synchronizeDraw, &p1, &p2, std::ref(cards));
  std::thread thr2(synchronizeDraw, &p2, &p1, std::ref(cards));
  thr1.join(); thr2.join();
  std::cout << "Game Over\n";
}