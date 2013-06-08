/*
 * This class defines the Edit message
 * client → server or server → client indicating an edit
 * @author Kevin Burdick 
 */
 
package message;

import java.nio.ByteBuffer;

public class Edit extends Message
{

    /**
    *  Constructor
    */
   public Edit(short pClientID, byte pAction, int pPosition, byte[] data)
   {
      super(Message.EDIT_TYPE,(short)(header_size+data.length));
      fileData = new byte[data.length];
      clientID = pClientID;
      sectionLength = data.length;
      filePosition = pPosition;
      action = pAction;
      //copy file data
      System.arraycopy(data, 0,fileData, 0,data.length);    
   }
	/**
    *  Constructor
    */
   public Edit(byte[] pMsg)
   {
	  super(Message.EDIT_TYPE,header_size);   
      fileData = new byte[pMsg.length-14];
      type = (short)(((pMsg[0]<<8) & 0xff00)|(pMsg[1] & 0xff)); 
      length = (short)(((pMsg[2]<<8) & 0xff00)|(pMsg[3] & 0xff));
	  clientID = (short)(((pMsg[4]<<8) & 0xff00)|(pMsg[5] & 0xff));
      action = pMsg[6]; 
	  sectionLength =  (int)(((pMsg[7]<<8) & 0xff000000)|((pMsg[8]<<8) & 0xff0000)|((pMsg[9]<<8) & 0xff00)|(pMsg[10] & 0xff));  
      filePosition = (int)(((pMsg[11]<<8) & 0xff000000)|((pMsg[12]<<8) & 0xff0000)|((pMsg[13]<<8) & 0xff00)|(pMsg[14] & 0xff)); 
      //copy file data
      System.arraycopy(pMsg, 15,fileData, 0,pMsg.length-15); 
   }
    
   /**
    *  Returns the byte array of the message 
    */
	public byte[] getbyteArray() {
      byte[] output = new byte[header_size+fileData.length];
      output[0] = (byte)((type & 0xff00)>> 8);
      output[1] = (byte)(type & 0xff);
      output[2] = (byte)((length & 0xff00)>> 8);
	  output[3] = (byte)(length & 0xff);
      output[4] = (byte)((clientID & 0xff00)>> 8);
      output[5] = (byte)(clientID & 0xff);
      output[6] = action;
      byte[] temp2 = ByteBuffer.allocate(4).putInt(sectionLength).array();
      for(int i=0;i<4;i++) {
          output[7+i] = temp2[i];  
      }
      temp2 = ByteBuffer.allocate(4).putInt(filePosition).array();
      for(int j=0;j<4;j++) {
          output[11+j] = temp2[j];  
      }
      //copy file data
      System.arraycopy(fileData, 0,output, 15,fileData.length); 
      return output;
   }
	
   public short getClientID() {
       return clientID;
   }
   public int getFilePosition() {
      return filePosition;
   }

   public int getSectionLen() {
      return sectionLength;
   }

	public byte[] getFileData() {
       return fileData;
   }
   
   public byte getAction() {
      return action;
   }

   public void dumpMsg() {
      System.out.println("message");
      System.out.println("type = "+type);
      System.out.println("length = "+length);
      System.out.println("clientID = "+clientID);
      System.out.println("action = "+action); 
      System.out.println("filePosition = "+filePosition);
      System.out.println("sectionLength = "+sectionLength);
      System.out.println("data size = "+fileData.length);
	}
    

   //file data 
   private  byte[] fileData;

	//this is the size of the header, does not include data field
   private static final short header_size= 15; 

   private short clientID;
   private int filePosition;
   private int sectionLength;
   private byte action;
}
