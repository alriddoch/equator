// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include <Atlas/Codecs/XML.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Encoder.h>

#include <skstream.h>
#include <fstream>

#include "ClientConnection.h"

#define debug(prg) { if (debug_flag) { prg } }

static bool debug_flag = false;

using Atlas::Message::Object;

ClientConnection::ClientConnection() :
    client_fd(-1), encoder(NULL), serialNo(512)
{
}

ClientConnection::~ClientConnection()
{
    if (encoder != NULL) {
        delete encoder;
    }
}

void ClientConnection::operation(const RootOperation & op)
{
#if 0
    const std::string & from = op.GetFrom();
    if (from.empty()) {
        cerr << "ERROR: Operation with no destination" << endl << flush;
        return;
    }
    dict_t::iterator I = objects.find(from);
    if (I == objects.end()) {
        cerr << "ERROR: Operation with invalid destination" << endl << flush;
        return;
    }
    oplist res = I->second->message(op);
    oplist::iterator J = res.begin();
    for(J = res.begin(); J != res.end(); ++J) {
        (*J)->SetFrom(I->first);
        send(*(*J));
    }
#endif
}

void ClientConnection::ObjectArrived(const Error&op)
{
    debug(cout << "ERROR" << endl << flush;);
    push(op);
    error_flag = true;
}

void ClientConnection::ObjectArrived(const Info & op)
{
    debug(cout << "INFO" << endl << flush;);
    const std::string & from = op.GetFrom();
    if (from.empty()) {
        reply_flag = true;
        error_flag = false;
        try {
            Object ac = op.GetArgs().front();
            reply = ac.AsMap();
            // const std::string & acid = reply["id"].AsString();
            // objects[acid] = new ClientAccount(acid, *this);
        }
        catch (...) {
            cerr << "WARNING: Malformed account from server" << endl << flush;
        }
    } else {
        operation(op);
    }
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Action& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Appearance& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Combine& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Communicate& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Create& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Delete& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Disappearance& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Divide& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Feel& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Get& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Imaginary& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Listen& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Login& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Logout& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Look& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Move& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Perceive& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Perception& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::RootOperation& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Set& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Sight& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Smell& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Sniff& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Sound& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Talk& op)
{
    push(op);
}

void ClientConnection::ObjectArrived(const Atlas::Objects::Operation::Touch& op)
{
    push(op);
}

int ClientConnection::read() {
    if (ios.is_open()) {
        codec->Poll();
        return 0;
    } else {
        return -1;
    }
}

bool ClientConnection::connect(const std::string & server)
{
    debug(cout << "Connecting to " << server << endl << flush;);

    ios.open(server.c_str(), 6767);
    if (!ios.is_open()) {
        std::cerr << "ERROR: Could not connect to " << server << "."
                  << std::endl << std::flush;
        return false;
    }
    client_fd = ios.getSocket();

    Atlas::Net::StreamConnect conn("cyphesis_aiclient", ios, this);

    debug(cout << "Negotiating... " << flush;);
    while (conn.GetState() == Atlas::Net::StreamConnect::IN_PROGRESS) {
      conn.Poll();
    }
    debug(cout << "done" << endl;);
  
    if (conn.GetState() == Atlas::Net::StreamConnect::FAILED) {
        cerr << "Failed to negotiate" << endl;
        return false;
    }

    codec = conn.GetCodec();

    encoder = new Atlas::Objects::Encoder(codec);

    codec->StreamBegin();

    return true;
}

bool ClientConnection::login(const std::string & account,
                             const std::string & password)
{
    Atlas::Objects::Operation::Login l = Atlas::Objects::Operation::Login::Instantiate();
    Object::MapType acmap;
    acmap["id"] = account;
    acmap["password"] = password;

    l.SetArgs(Object::ListType(1,Object(acmap)));

    reply_flag = false;
    error_flag = false;
    send(l);
    return true;
}

bool ClientConnection::create(const std::string & account,
                              const std::string & password)
{
    Atlas::Objects::Operation::Create c = Atlas::Objects::Operation::Create::Instantiate();
    Object::MapType acmap;
    acmap["id"] = account;
    acmap["password"] = password;

    c.SetArgs(Object::ListType(1,Object(acmap)));

    reply_flag = false;
    error_flag = false;
    send(c);
    return true;
}

bool ClientConnection::wait()
// Waits for response from server. Used when we are expecting a login response
// Return whether or not an error occured
{
   error_flag = false;
   reply_flag = false;
   while (!reply_flag) {
      codec->Poll();
   }
   return error_flag;
}

void ClientConnection::send(Atlas::Objects::Operation::RootOperation & op)
{
    debug(std::fstream f(1);
          Atlas::Codecs::XML c(f, (Atlas::Bridge*)this);
          Atlas::Objects::Encoder enc(&c);
          enc.StreamMessage(&op);
          std::cout << std::endl << flush;);
    

    op.SetSerialno(++serialNo);
    encoder->StreamMessage(&op);
    ios << flush;
}

void ClientConnection::poll()
{
    fd_set infds;
    struct timeval tv;

    FD_ZERO(&infds);

    FD_SET(client_fd, &infds);

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int retval = select(client_fd+1, &infds, NULL, NULL, &tv);

    if (retval && (FD_ISSET(client_fd, &infds))) {
        if (ios.peek() == -1) {
            std::cerr << "Server disconnected" << std::endl << std::flush;
            return;
        }
        codec->Poll();
        return;
    }
    return;

}

RootOperation * ClientConnection::pop()
{
    poll();
    if (operationQueue.empty()) {
        return NULL;
    }
    RootOperation * op = operationQueue.front();
    operationQueue.pop_front();
    return op;
}

bool ClientConnection::pending()
{
    return !operationQueue.empty();
}

template<class O>
void ClientConnection::push(const O & op)
{
    reply_flag = true;
    RootOperation * new_op = new O(op); 
    operationQueue.push_back(new_op);
}
