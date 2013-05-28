/* * This is the main entry into the Transport Layer * */ import java.net.*;import java.io.*;/** Main program class for the client application  */public class Transport{	Socket socket;   OutputStream sendStream;   InputStream recStream;	Transport(String server, int port)   {      //System.out.println("Transport ");      try {      socket = new Socket(server,port);      sendStream = socket.getOutputStream();      recStream = socket.getInputStream();		}		catch(Exception ex) {			 System.out.println("Exception "+ex.toString());      }	}	 	public void sendRequest(byte[] pBuffer)   {      //System.out.println("sendRequest ");		try {			byte[] buffer = pBuffer;			sendStream.write(buffer,0,buffer.length);		}		catch(IOException ex) {			 System.out.println("Exception "+ex.toString());      }	}      public byte[] getResponse()   {      //System.out.println("getResponse ");		try {         int  dataSize;         while ((dataSize=recStream.available())==0);			byte[] recvBuff = new byte[dataSize];         recStream.read(recvBuff,0,dataSize);         String response = new String(recvBuff,0,dataSize);		   //System.out.println("Message Recieved "+response);         return recvBuff;		}		catch(IOException ex) {			System.out.println("Exception "+ex.toString());          return null;      }	}      public void close()   {      //System.out.println("close");		try {   		sendStream.close();   		recStream.close();         socket.close();		}		catch(IOException ex) {			System.out.println("Exception "+ex.toString());      }	} }    