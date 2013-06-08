/*
 * This is the main entry into the Transport Layer. It was originally developed and leveraged off of the example 
 * in Computer Networks by Forouzan and Mosharraf 
 */

package test; 

import java.net.*;
import java.io.*;

/** Main program class for the client application 
 */
public class TCPSock
{
	Socket socket;
   OutputStream sendStream;
   InputStream recStream;


	public TCPSock(Socket s)
   {
      try {
      socket = s;
      sendStream = socket.getOutputStream();
      recStream = socket.getInputStream();
		}
		catch(Exception ex) {
			 System.out.println("Exception "+ex.toString());
      }
	}
	 
	public void sendResponse(byte[] pBuffer)
   {
      System.out.println("sendResponse ");
		try {
			byte[] buffer = pBuffer;
			sendStream.write(buffer,0,buffer.length);
		}
		catch(IOException ex) {
			 System.out.println("Exception "+ex.toString());
      }
	}
   
   public byte[] getRequest()
   {
      System.out.println("getRequest ");
		try {
         int  dataSize;
         while ((dataSize=recStream.available())==0);
		 byte[] recvBuff = new byte[dataSize];
         recStream.read(recvBuff,0,dataSize);
         String response = new String(recvBuff,0,dataSize);
         return recvBuff;
		}
		catch(IOException ex) {
			System.out.println("Exception "+ex.toString());
         return null;
      }
	}
   
   public void close()
   {
      //System.out.println("close");
		try {
   		sendStream.close();
   		recStream.close();
         socket.close();

		}
		catch(IOException ex) {
			System.out.println("Exception "+ex.toString());
      }
	}


}
    
