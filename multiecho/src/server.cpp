#include "server.h"

using namespace std;

vector<Client> Server::clients;

Server::Server(){

   Mythread::InitMutex();
 
   int yes = 1;

   serverSock = socket(AF_INET, SOCK_STREAM, 0);
   memset(&serverAddr, 0 , sizeof(sockaddr_in));
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_addr.s_addr = INADDR_ANY;
   serverAddr.sin_port = htons(PORT);

   setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

   if(bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(sockaddr_in))<0){
      cerr << "Failed to bind" << endl;
   }

   listen(serverSock, 5);
}

void Server::AcceptAndDispatch() {
  
  Client *c;
  Mythread *t;

  socklen_t cliSize = sizeof(sockaddr_in);

  while(1) {

          c = new Client();
	  t = new Mythread();

	  //Blocks here;
          c->sock = accept(serverSock, (struct sockaddr *) &clientAddr, &cliSize);

	  if(c->sock < 0) {
	    cerr << "Error on accept";
	  }
	  else {
	    t->Create((void *) Server::HandleClient, c);
	  }
  }
}

//Static
void *Server::HandleClient(void *args) {

  //Pointer to accept()'ed Client
  Client *c = (Client *) args;
  char buffer[1024], message[256];
  int index;
  int n;

  //Add client in Static clients <vector> (Critical section!)
  Mythread::LockMutex((const char *) c->name);
  
    //Before adding the new client, calculate its id. (Now we have the lock)
    c->SetId(Server::clients.size());
    sprintf(buffer, "Client n.%d", c->id);
    c->SetName(buffer);
    cout << "Adding client with id: " << c->id << endl;
    Server::clients.push_back(*c);

  Mythread::UnLockMutex((const char *) c->name);

  while(1) {
    memset(buffer, 0, sizeof buffer);
    n = recv(c->sock, buffer, sizeof buffer, 0);

    //Client disconnected?
    if(n == 0) {
      cout << "Client " << c->name << " diconnected" << endl;
      close(c->sock);
      
      //Remove client in Static clients <vector> (Critical section!)
      Mythread::LockMutex((const char *) c->name);

        index = Server::FindClientIndex(c);
        cout << "Erasing user in position " << index << " whose name id is: " 
	  << Server::clients[index].id << endl;
        Server::clients.erase(Server::clients.begin() + index);

      Mythread::UnLockMutex((const char *) c->name);

      break;
    }
    else if(n < 0) {
      cerr << "Error while receiving message from client: " << c->name << endl;
    }
    else {
      //Message received. Send to all clients.
      snprintf(message, sizeof message, "<%s>: %s", c->name, buffer); 
      cout << "Will send to all: " << message  << endl;
      Server::SendToAll(message);
    }
  }

  //End thread
  return NULL;
}

void Server::SendToAll(char *message) {
  int n;

  //Acquire the lock
  Mythread::LockMutex("'SendToAll()'");
 
    for(size_t i=0; i<clients.size(); i++) {
      n = send(Server::clients[i].sock, message, strlen(message), 0);
      cout << n << " bytes sent." << endl;
    }
   
  //Release the lock  
  Mythread::UnLockMutex("'SendToAll()'");
}

void Server::ListClients() {
  for(size_t i=0; i<clients.size(); i++) {
    cout << clients.at(i).name << endl;
  }
}

/*
  Should be called when vector<Client> clients is locked!
*/
int Server::FindClientIndex(Client *c) {
  for(size_t i=0; i<clients.size(); i++) {
    if((Server::clients[i].id) == c->id) return (int) i;
  }
  cerr << "Client id not found." << endl;
  return -1;
}
