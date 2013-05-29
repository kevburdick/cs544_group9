#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include "CTP_server.h"
void *ServerInit();
void *send_back();
int check_file_valid(int );



void *Version(ConnectMessage *connect_msg)
{
    printf("Version state\n");
//check if the version is supported
if(connect_msg->version<3)//for example, only support version 1&2
  {printf("version %d supported\n",connect_msg->version);
  AckconnectMessage ackconnect_msg;
  ackconnect_msg.version=connect_msg->version;
  ackconnect_msg.type=3;
  //ackconnect_msg.client_id=assign_id();//need definition
  //string buffer=msg_to_buffer();
  //send_back(buffer);
  }
else//unsupported
{printf("version %d unsupported\n",connect_msg->version);
  // error message
 ErrorMessage error_msg;
 error_msg.type=1;
 error_msg.length=48;
 error_msg.error_code=2;
//string buffer=msg_to_buffer();
//send_back(buffer);
}


}

void *Opening(OpenMessage *open_msg)
{
printf("Opening state\n");
int flag;
int fileID;
fileID=open_msg->file_id;
flag=check_file_valid(fileID);

if(flag==1)
{printf("file #%d valid\n",open_msg->file_id);
AckMessage ackopen_msg;
ackopen_msg.type=5;
ackopen_msg.length=32;


}

else
{printf("file #%d invalid\n",open_msg->file_id);
 ErrorMessage error_msg;
 error_msg.type=1;
 error_msg.length=48;
 error_msg.error_code=3;
}
}


void *Locked(ReleaseMessage *release_msg)
{
AckMessage server_release_msg;
server_release_msg.type=8;
server_release_msg.length=32;


}

void *Wait_lock()
{

}

int main()
{
//test
ConnectMessage msg1={2,48,2};//type;length;version
ConnectMessage *p1;
p1=&msg1;
Version (p1);
///////////////////


}
