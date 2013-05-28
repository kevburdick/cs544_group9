/* * This is the main entry into the client application * */ import message.Connect;import message.Message;import message.AckConnect;import message.Open;/** Main program class for the client application  */public class Client{    static short state;    static short clientID;    public static void main(String[] args)    {        //This is one black box test that corresponds to scenario outline in the Server test file.        //This executes the API and the protocol code is run against the Server test stub        //An application layer would replace this or place similar calls                  //FIXME This should be commandline at least        String host = "Gateway";          System.out.println("client starting");        CTP ctp = new CTP(host);        System.out.println("client connect");        ctp.Connect();        System.out.println("client open");        ctp.Open();        System.out.println("client request lock");        ctp.ReqLock();        System.out.println("client release lock");        ctp.ReleaseLock();        System.out.println("client move");        ctp.Move(255);        System.out.println("client request Contents");        ctp.ReqContents(255,255);        System.out.println("client sync");        ctp.Sync();        System.out.println("client edit");        byte[] data = new byte[256];        ctp.Edit((byte)1,245,data);        System.out.println("client close");        ctp.Close();        System.out.println("client ending");    }}    