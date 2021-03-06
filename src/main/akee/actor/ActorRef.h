
#ifndef _AKEE_ACTOR_ACTORREF_H_
#define _AKEE_ACTOR_ACTORREF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <string>
#include <iostream>

#include "akee/basic/stddef.h"
#include "akee/actor/ActorPath.h"
#include "akee/actor/IActorRef.h"
#include "akee/actor/IActorRefScope.h"
#include "akee/actor/ISystemMessage.h"
#include "akee/actor/IInternalActorRef.h"

#include "akee/actor/IActorContext.h"
#include "akee/actor/ActorBase.h"
#include "akee/utils/StringUtils.h"

namespace akee {

class ActorRef;

class ILocalRef : public IActorRefScope {
public:
    bool isLocal() const { return true; }
};

class IRepointableRef : public IActorRefScope {
public:
    virtual bool isStarted() const = 0;
};

class IActorRefProvider;

class ActorRefBase : public IActorRef {
private:
    ActorPath * path_;

protected:
    virtual void tellInternal(MessageObject message, const IActorRef * sender = nullptr) = 0;

public:
    // ICanTell
    void tell(MessageObject message, const IActorRef * sender);

    // ISurrogated
    ISurrogated * toSurrogate(const ActorSystem * system) {
        return nullptr;
    }

    // IActorRef
    ActorPath * getPath() const {
        return path_;
    }

    const char * toString() {
        return StringUtils::format(path_->getLength() + 2, "[%s]", path_->toString());
    }
};

class InternalActorRefBase : public ActorRefBase,
                             public IInternalActorRef {
private:
    IInternalActorRef * parent_;
    IActorRefProvider * provider_;

public:
    InternalActorRefBase() {}

    // IInternalActorRef
    virtual IInternalActorRef * getParent() const { return parent_; }
    virtual IActorRefProvider * getProvider() const { return provider_; }

    virtual bool isTerminated() const = 0;
    virtual IActorRef * getChild(const std::string & name) const = 0;

    virtual void resume() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void restart() = 0;
    virtual void suspend() = 0;

    void sendSystemMessage(const ISystemMessage * message, const IActorRef * sender /* = nullptr */) {
        int msgType = message->getType();
        if (msgType == ISystemMessage::LocalMessage::Terminate) {
            stop();
        }
        else if (message != nullptr) {
            this->tell((MessageObject)nullptr, nullptr);
        }
    }

    // IActorRefScope
    virtual bool isLocal() const = 0;
};

class MinimalActorRef : public InternalActorRefBase, public ILocalRef {
public:
    // IInternalActorRef
    virtual bool isTerminated() const { return false; }
    virtual IActorRef * getChild(const std::string & name) const {
        return nullptr;
    }

    void resume() {};
    void start() {};
    void stop() {};
    void restart() {};
    void suspend() {};

    // For ICanTell
    void tellInternal(MessageObject message, const IActorRef * sender /* = nullptr */) {
        //
    }

    // IActorRefScope
    virtual bool isLocal() const { return true; }
};

class Nobody : public MinimalActorRef {
public:
    static Nobody * instance_;

private:
    ActorPath * path_;

private:
    Nobody() : path_(nullptr) {
        path_ = new RootActorPath("/Nobody");
    }

public:
    ~Nobody() { }

public:
    static void initNobody();

    static Nobody * getInstance() {
        return instance_;
    }

    ActorPath * getPath() const { return path_; }
    IActorRefProvider * getProvider() const { return nullptr; }
};

class ActorRefs {
public:
    static Nobody * nobody_;
    static IActorRef * nosender_;

public:
    static void staticInit() {
        nobody_ = Nobody::getInstance();
        nosender_ = nullptr;
    }

    static void staticFree() {
        nobody_ = nullptr;
        nosender_ = nullptr;
    }

    static Nobody * getNobody() {
        return nobody_;
    }

    static IActorRef * getNoSender() {
        return nosender_;
    }
};

class ActorRef : public ActorRefBase {
private:
    std::string name_;
    IActorContext * context_;

public:
    ActorRef() : ActorRefBase() {
        initActorRef("default");
    }

    ActorRef(const std::string & name) {
        initActorRef(name);
    }

    ActorRef(const ActorRef & src) {
        cloneActorRef(src);
    }

    ~ActorRef() {
    }

private:
    void initActorRef(const std::string & name) {
        name_ = name;
    }

protected:
    void cloneActorRef(const ActorRef & src) {
        this->name_ = src.name_;
    }

public:
    IActorRef * getSelf() const { return getContext()->getSelf(); }
    IActorRef * getSender() const { return getContext()->getSender(); }
    ActorSystem * getSystem() const { return getContext()->getSystem(); }
    IActorContext * getContext() const { return context_; }

    std::string getName() const {
        return name_;
    }

    void setName(const std::string & name) {
        name_ = name;
    }

    void tell(void * func, const IActorRef * result) const {
        std::cout << "ActorRef::tell();" << std::endl;
    }

    int start() {
        return 0;
    }
};

}  /* namespace akee */

#endif  /* _AKEE_ACTOR_ACTORREF_H_ */
