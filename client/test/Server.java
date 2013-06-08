/*
 * This is the main entry into the test server application
 *
 */
//package test; 

import java.net.*;
import java.io.*;

import message.Close;
import message.Error;
import message.Open;
import message.Connect;
import message.Contents;
import message.Edit;
import message.Message;
import message.AckConnect;
import message.Ack;
import message.Move;
import message.Release;
import message.ReqLock;
import message.Status;
import message.ReqContents;
import message.Sync;
import test.TCPSock;



/** Main program class for the test server application 
 */
public class Server
{
	static TCPSock server;
    public static void main(String[] args)
    {
    	InputStreamReader input = new InputStreamReader(System.in) ;
    	BufferedReader inputBuff = new BufferedReader(input) ;
    	String menuSelection; 
        int port = 5449;
    	//setup socket
    	try {
    		System.out.println("Server connection waiting");
		     ServerSocket listenSock = new ServerSocket(port);
			 server = new TCPSock(listenSock.accept());
			 System.out.println("Server connection made");
    	}
    	catch(Exception e) {
             System.out.println("Exception"+e.toString());
  		}
    	 
    	 
    	while(true) {
    		DisplayMainMenu();
    		try {
	    		menuSelection = inputBuff.readLine();
	    		if(menuSelection.equals("1"))
	    			SendMessage();
	    		else if(menuSelection.equals("2"))
	    			ReadMessage();
	    		else if(menuSelection.equals("3"))
	    			RegressionTest();
	    		else if(menuSelection.equals("4"))
					DFATest();	
	    		System.out.println("Press return key to continue");
	    		menuSelection = inputBuff.readLine();
	    		//roll screen
	    		for(int i=0;i<6;i++)
	    			System.out.print("\n"); 	
	    		}
    		catch (IOException err) {
    			System.out.println("Error reading line");	
    		}
    		
    	}
    }
    static void SendMessage()
    {
        short clientID = 2100;
        String menuSelection; 
        InputStreamReader input = new InputStreamReader(System.in) ;
    	BufferedReader inputBuff = new BufferedReader(input) ;
        
    
        try {
		    // ServerSocket listenSock = new ServerSocket(port);
			//  server = new TCPSock(listenSock.accept()); 
			  DisplaySelectMessageMenu();
			  menuSelection = inputBuff.readLine();
			  int selection = Integer.parseInt(menuSelection);
			  
			  switch(selection) 
			  {
				  case(1): {
					  System.out.println("1)ERROR");
					  Error msg = new Error((short)1);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  case (2): {
					  System.out.println("2)CONNECT");
					  Connect msg = new Connect((short)2);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  case (3): {
					  System.out.println("3)ACKCONNECT");
					  AckConnect msg = new AckConnect((short)2, clientID);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  case (4): {
					  System.out.println("4)OPEN");
					  Open msg = new Open(clientID, 2100);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  case (5): {
					  System.out.println("5)ACKOPEN");
					  Ack msg = new Ack(Message.ACKOPEN_TYPE);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  case (6): {
					  System.out.println("6)ACKLOCK");
					  Ack msg = new Ack(Message.ACKLOCK_TYPE);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  case (7): {
					  System.out.println("7)ACKEDIT"); 
					  Ack msg = new Ack(Message.ACKEDIT_TYPE);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  case (8): {
					  System.out.println("8)SERVRELEASE");
					  Ack msg = new Ack(Message.SERVRELEASE_TYPE);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  case (9): {
					  System.out.println("9)REQCONTENTS");
					  ReqContents msg = new ReqContents(clientID,2100,2100);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  case (10): {
					  System.out.println("10)CONTENTS");
					  byte[] data = new byte[] {(byte)1,(byte)2,(byte)3,(byte)4,(byte)5,(byte)6,(byte)7,(byte)8,(byte)9,(byte)10};
			          Contents msg = new Contents(257,10,data);
			          server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  case (11): {
					  System.out.println("11)MOVE");
					  Move msg = new Move(clientID,2100);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  case (12): {
					  System.out.println("12)REQLOCK");
					  ReqLock msg = new ReqLock(clientID);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  case (13): {
					  System.out.println("13)RELEASE");
					  Release msg = new Release(clientID);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }	
				  case (14): {
					  System.out.println("14)SYNC");
					  Sync msg = new Sync(clientID);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }	
				  case (15): {
					  System.out.println("15)STATUS");
					  Status msg = new Status();
					  byte[] checksum = new byte[32];
			          msg.setChecksum(checksum);
			          server.sendResponse(msg.getbyteArray());
					  break;
				  }	
				  case (16): {
					  System.out.println("16)EDIT");
					  byte[] data = new byte[] {(byte)1,(byte)2,(byte)3,(byte)4,(byte)5,(byte)6,(byte)7,(byte)8,(byte)9,(byte)10};
					  Edit msg = new Edit(clientID,(byte)1,2100, data);
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  case (17): {
					  System.out.println("17)CLOSE");
					  Close msg = new Close();
					  server.sendResponse(msg.getbyteArray());
					  break;
				  }
				  default: {
					  System.out.println("Invalid Message type selected");
				  }
			  }//end switch	  
		}
        catch(Exception e) {
           System.out.println("Exception"+e.toString());
		}
    }
    
    static void DisplaySelectMessageMenu()
    {
    	System.out.println("*******************");
    	System.out.println("Select Message Menu");
    	System.out.println("*******************");
    	System.out.println("Enter Selection");
    	System.out.println("1)ERROR");
    	System.out.println("2)CONNECT");
    	System.out.println("3)ACKCONNECT");
    	System.out.println("4)OPEN");
    	System.out.println("5)ACKOPEN");
    	System.out.println("6)ACKLOCK");
    	System.out.println("7)ACKEDIT"); 
    	System.out.println("8)SERVRELEASE");
    	System.out.println("9)REQCONTENTS");
    	System.out.println("10)CONTENTS");
    	System.out.println("11)MOVE");
    	System.out.println("12)REQLOCK");
    	System.out.println("13)RELEASE");
    	System.out.println("14)SYNC");
    	System.out.println("15)STATUS");
    	System.out.println("16)EDIT");
    	System.out.println("17)CLOSE");
    }
    
    static void ReadMessage()
    {
    	//System.out.println("Not implemented yet");
    	byte[] buff = server.getRequest();
		Message msg = Message.makeMessage(buff);
		System.out.println("Message: "+msg.getType()+" received");
    	
    }
    
    static void DisplayMainMenu()
    {
    	System.out.println("******");
    	System.out.println("Menu");
    	System.out.println("******");
    	System.out.println("Enter Selection");
    	System.out.println("1) Send Message");
    	System.out.println("2) Receive Message");
    	System.out.println("3) Run regression test");
    	System.out.println("4) DFA test");
    }
    
    /*
     * This method executes a set of request/response cases with the client and is a quick test to see if 
     * everything is working. This test is not a complete white-box test 
     */
    static void RegressionTest()
    {
        short version =1;
        short id =1025;

        //This is one black box test that corresponds to scenario outline the client file.
        //there is no checking in this file since its just a test stub, if it does not complete 
        //it fails
        try {
		   byte[] buff = server.getRequest();
		   Message msg;
           AckConnect ack = new AckConnect(version, id);
           ack.dumpMsg();
           byte[] test = ack.getbyteArray();
           for(int i=0;i<8;i++)
           System.out.println("hre"+test[i]);
           server.sendResponse(ack.getbyteArray());
           //receive open request
           buff = server.getRequest();
           msg = Message.makeMessage(buff);
           Ack ackMsg = new Ack(Message.ACKOPEN_TYPE);
           server.sendResponse(ackMsg.getbyteArray());
           //receive lock request
           buff = server.getRequest();
           msg = Message.makeMessage(buff);
           Ack ackMsg2 = new Ack(Message.ACKLOCK_TYPE);
           server.sendResponse(ackMsg2.getbyteArray());
           //receive release lock request
           buff = server.getRequest();
           msg = Message.makeMessage(buff);
           Ack ackMsg3 = new Ack(Message.SERVRELEASE_TYPE);
           server.sendResponse(ackMsg3.getbyteArray());
           //receive move request
           buff = server.getRequest();
           msg = Message.makeMessage(buff);
           //receive request contents request
           buff = server.getRequest();
           msg = Message.makeMessage(buff);   
           byte[] data = new byte[] {(byte)1,(byte)2,(byte)3,(byte)4,(byte)5,(byte)6,(byte)7,(byte)8,(byte)9,(byte)10};
           Contents contents = new Contents(255,10,data);
           server.sendResponse(contents.getbyteArray());
           //receive sync request
           buff = server.getRequest();
           msg = Message.makeMessage(buff);
           byte[] checksum = new byte[32];
           Status status = new Status();
           status.setChecksum(checksum);
           server.sendResponse(status.getbyteArray());
           //receive edit request
           buff = server.getRequest();
           msg = Message.makeMessage(buff);
           Ack ackMsg4 = new Ack(Message.ACKEDIT_TYPE);
           server.sendResponse(ackMsg4.getbyteArray());
           //receive close request
           buff = server.getRequest();
           msg = Message.makeMessage(buff);
          
           //close the socket
           server.close();
  
		  }
        catch(Exception e) {
           System.out.println("Exception"+e.toString());
		  }

        System.out.println("test server closed");
 
    }
    
    /*
     * This method test the state machine in the client and verifies that it is working correctly
     */
    static void DFATest()
    {
    	System.out.println("Not implemented yet");
    }
   
}
    
