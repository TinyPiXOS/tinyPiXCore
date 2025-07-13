
#include "tpSocketNotifier.h"
#include "tpSocketNotifierManager.h"
#include "tpSocket.h"
#include <unistd.h>
#include <iostream>
#include <map>
#include <vector>
#include <mutex>

/*tpSocketNotifier::tpSocketNotifier(int fd, Type type, std::function<void()> callback)
    : fd_(fd), type_(type), callback_(callback), enabled_(true) {
    tpSocketNotifierManager::instance().registerNotifier(this);
}*/
//callback_(std::move(callback)),
//      hangupCallback_(std::move(hangupCb)) , // 默认空
tpSocketNotifier::tpSocketNotifier(int fd, Type type, std::function<void()> callback,std::function<void()> hangupCb)
    : fd_(fd), type_(type),
      callback_(std::move(callback)),
      hangupCallback_(std::move(hangupCb)) , // 默认空
      enabled_(true) {
     tpSocketNotifierManager::instance().registerNotifier(this);
 }

tpSocketNotifier::~tpSocketNotifier() {
    tpSocketNotifierManager::instance().unregisterNotifier(this);
}

void tpSocketNotifier::setEnabled(bool enable) {
    enabled_ = enable;
}

bool tpSocketNotifier::isEnabled() const {
    return enabled_;
}

int tpSocketNotifier::socket() const {
    return fd_;
}

tpSocketNotifier::Type tpSocketNotifier::type() const {
    return type_;
}