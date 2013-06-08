/*
 * This class defines the Release Lock message
 * requires that this client have ownership of the lock 
 * client → server releasing write lock
 * @author Kevin Burdick 
 */
 
package message;

import java.util.ArrayList;

public class Release extends Message
{

   /**
    *  Constructor
    */
   public Release(short pClientID)
   {
      super(Message.RELEASE_TYPE,header_size);
      optionsList = new ArrayList<String>();     
      clientID = pClientID;
 
   }
	/**
    *  Constructor
    */
   public Release(byte[] pMsg)
   {
	  super(Message.RELEASE_TYPE,header_size);
      optionsList = new ArrayList<String>();    
      type = (short)(((pMsg[0]<<8) & 0xff00)|(pMsg[1] & 0xff));  
      length = (short)(((pMsg[2]<<8) & 0xff00)|(pMsg[3] & 0xff)); 
	  clientID = (short)(((pMsg[4]<<8) & 0xff00)|(pMsg[5] & 0xff));                 
   }
    
   /**
    *  Returns the byte array of the message 
    */
	public byte[] getbyteArray() {
      byte[] temp = new byte[header_size];
      temp[0] = (byte)((type & 0xff00)>> 8);
      temp[1] = (byte)(type & 0xff);
      temp[2] = (byte)((length & 0xff00)>> 8);
      temp[3] = (byte)(length & 0xff);
      temp[4] = (byte)((clientID & 0xff00)>> 8);
      temp[5] = (byte)(clientID & 0xff);
      return temp;
   }
    
   public short getClientID() {
       return clientID;
   }

	public ArrayList<String> getOptions() {
       return optionsList;
   }

   public void dumpMsg() {
      System.out.println("message");
      System.out.println("type = "+type);
      System.out.println("length = "+length);
      System.out.println("clientID = "+clientID);
      System.out.println("options size = "+optionsList.size());
	}
    

    //options list
   private  ArrayList<String> optionsList;

	//this is the size of the header, does not include options field
   private static final short header_size= 6; 

   private short clientID;
}
