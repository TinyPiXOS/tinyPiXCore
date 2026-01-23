
#include "TpSocketNotifier.h"
#include "TpAbstractEventDispatcher.h"
#include <unistd.h>
#include <iostream>
#include <map>
#include <vector>
#include <mutex>

/*TpSocketNotifier::TpSocketNotifier(int fd, Type type, std::function<void()> callback)
    : fd_(fd), type_(type), callback_(callback), enabled_(true) {
    TpAbstractEventDispatcher::instance().registerNotifier(this);
}*/
//callback_(std::move(callback)),
//      hangupCallback_(std::move(hangupCb)) , // 默认空
TpSocketNotifier::TpSocketNotifier(int fd, Type type, std::function<void()> callback,std::function<void()> hangupCb)
    : fd_(fd), type_(type),
      callback_(std::move(callback)),
      hangupCallback_(std::move(hangupCb)) , // 默认空
      enabled_(true) {
     TpAbstractEventDispatcher::instance().registerNotifier(this);
 }

TpSocketNotifier::~TpSocketNotifier() {
    TpAbstractEventDispatcher::instance().unregisterNotifier(this);
}

void TpSocketNotifier::setEnabled(bool enable) {
    enabled_ = enable;
}

bool TpSocketNotifier::isEnabled() const {
    return enabled_;
}

int TpSocketNotifier::socket() const {
    return fd_;
}

TpSocketNotifier::Type TpSocketNotifier::type() const {
    return type_;
}
