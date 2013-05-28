/* * This class defines the Request Contents message * client → server to request file contents * @author Kevin Burdick  */ package message;import java.util.ArrayList;import java.nio.ByteBuffer;import java.nio.ByteOrder;public class ReqContents extends Message{    /**    *  Constructor    */   public ReqContents(short pClientID, int pPosition, int pLength)   {      super(Message.REQCONTENTS_TYPE,header_size);      optionsList = new ArrayList<String>();         clientID = pClientID;      filePosition = pPosition;      sectionLength = pLength;   }	/**    *  Constructor    */   public ReqContents(byte[] pMsg)   {      //FIXME should throw exception		super(Message.REQCONTENTS_TYPE,header_size);      optionsList = new ArrayList<String>();           type = (short)((pMsg[0] & 0xff<<8)|(pMsg[1] & 0xff));       length = (short)((pMsg[2] & 0xff<<8)|(pMsg[3] & 0xff)); 		clientID = (short)((pMsg[4]& 0xff<<8)|(pMsg[5] & 0xff));      filePosition = (int)((pMsg[6]& 0xff<<8)|(pMsg[7] & 0xff<<8)|(pMsg[8]& 0xff<<8)|(pMsg[9] & 0xff<<0));         sectionLength = (int)((pMsg[10]& 0xff<<8)|(pMsg[11] & 0xff<<8)|(pMsg[12]& 0xff<<8)|(pMsg[13] & 0xff<<0));               }       /**    *  Returns the byte array of the message     */	public byte[] getbyteArray() {      byte[] temp = new byte[14];      temp[0] = (byte)(type >> 8);      temp[1] = (byte)type;      temp[2] = (byte)(length >> 8);		temp[3] = (byte)length;      temp[4] = (byte)(clientID >> 8);      temp[5] = (byte)clientID;      byte[] temp2 = ByteBuffer.allocate(4).putInt(filePosition).array();      for(int i=0;i<4;i++) {          temp[6+i] = temp2[i];        }      temp2 = ByteBuffer.allocate(4).putInt(sectionLength).array();      for(int j=0;j<4;j++) {          temp[10+j] = temp2[j];        }      return temp;   }	   public short getClientID() {       return clientID;   }   public int getFilePosition() {      return filePosition;   }   public int getSectionLen() {      return sectionLength;   }	public ArrayList<String> getOptions() {       return optionsList;   }   public void dumpMsg() {      System.out.println("message");      System.out.println("type = "+type);      System.out.println("length = "+length);      System.out.println("clientID = "+clientID);      System.out.println("filePosition = "+filePosition);      System.out.println("sectionLength = "+sectionLength);      System.out.println("options size = "+optionsList.size());	}        //options list   private  ArrayList<String> optionsList;	//this is the size of the header, does not include options field   private static final short header_size= 7;    private short clientID;   private int filePosition;   private int sectionLength;}