#include "QueueManager.h"

#include <iostream>

template <typename T> T QueueManager<T>::receive() {
  std::unique_lock<std::mutex> lck(_mtx);
  _condition.wait(lck, [this] {
    return !_output.empty();
  }); // pass unique lock to condition variable

  // remove last vector element from queue
  T img = std::move(_output.back());
  _output.pop_back();

  return img; // will not be copied due to return value optimization (RVO) in
              // C++
}

template <typename T> void QueueManager<T>::send(T &&img) {
  std::lock_guard<std::mutex> lck(_mtx);
  std::cout << "   Message # will be added to the queue" << std::endl;
  _output.push_back(std::move(img));
  _condition
      .notify_one(); // notify client after pushing new Vehicle into vector
}
