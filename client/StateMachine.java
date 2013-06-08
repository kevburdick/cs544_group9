/*
 * This class is the state machine for CTP protocol. It checks the sending and receiving messages 
 * based on the state. 
 * @author Kevin Burdick 
 */
 
import message.Message;
import message.AckConnect;
import message.Error;

/** Main program class for the client application 
 */
public class StateMachine
{
    static short state;
    static Transport transport;
    static CTPResponse ctpResponse; 
    public final int SENDING =1;
    public final int RECEIVING =0; 
    //Response values
    private final int INVALID = -1;//invalid request from client user
    private final int NO_RESPONSE = 0;//no response from server

    public StateMachine(Transport pTransport) {
        state = INIT_STATE;
        transport = pTransport; 
	 }
    
    /** 
     * “STATEFUL” this method implements the DFA. It takes in a message whether 
     * sending or receiving and acts on it based on both the state and message type 
     * There are two message types, synchronous and asynchronous supported
     * @return status and response message information from server
     *    Case 1: Valid to send sync message - we send and wait for specific(correct) response
     *    Case 2: Response received we forward up the stack
     *    Case 3: Invalid to send - we create a error response and INVALID status and send back up stack
     *    Case 4: Invalid response received we create Error message and send to server
     *    Case 5: No response from server we create a error response and NO_RESPONSE status and send back up stack
     *    Case 5: Valid to send async message - we send and forget
     *    Case 6: Valid async response received - we store in CTPResponse and continue to wait   
     *    
     */
    public CTPResponse Process(int direction, Message msg)
    {
	    switch (state) {
	    	case INIT_STATE: {
	    		if (msg.getType() == Message.CONNECT_TYPE && direction == SENDING) {
	    			sendRequest(msg);			
	    			state = VERSION_STATE;
	    			return getResponse(Message.ACKCONNECT_TYPE);
	    		}
	    		else {//invalid or bad type should never be reached
	    			//error case from client user
	    			if(direction == SENDING) {
	    				ctpResponse = new CTPResponse(Message.ERROR_TYPE,INVALID);
	    				return ctpResponse;
	    			}
	    			else {//error case from server	
	    				Error errMsg = new Error(Error.BAD_MESSAGE_ERR);
	    				sendRequest(errMsg);	
	    				ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
	    			}
	    		}
	    			
	    	}
	    	case VERSION_STATE: {	
	    		if (msg.getType() == Message.ACKCONNECT_TYPE) {
	    			AckConnect ackMsg = (AckConnect)msg;
	                CTP.getInstance().setClientID(ackMsg.getClientID());
	                state = CONNECTED_STATE;
	                ctpResponse.LoadReponse(msg);
    				return ctpResponse;
	    		}
	    		else if(msg.getType() == Message.CLOSE_TYPE) {
		    		if(direction == SENDING) {
		    			sendRequest(msg);
		    			state = INIT_STATE;
		    			//when we send close we do not wait for response just close and return 
		    			ctpResponse = new CTPResponse(Message.INVALID_TYPE);
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    		else {//received close msg
		    			state = INIT_STATE;
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    	}
	    		else if(msg.getType() == Message.ERROR_TYPE) {
	    			//received error msg
	    			if(direction == RECEIVING) {
	    				ctpResponse = new CTPResponse(Message.INVALID_TYPE);
	    				ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    	}
	    		else {//invalid or bad type should never be reached
	    			//error case from client user
	    			if(direction == SENDING) {
	    				ctpResponse = new CTPResponse(Message.ERROR_TYPE,INVALID);
	    				return ctpResponse;
	    			}
	    			else {//error case from server		
	    				Error errMsg = new Error(Error.BAD_MESSAGE_ERR);
	    				sendRequest(errMsg);
	    				ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
	    			}
	    		}
	        }
	    	case CONNECTED_STATE: {	
		    	if (msg.getType() == Message.OPEN_TYPE) {
		    		sendRequest(msg);
	    			state = OPENING_STATE;
	    			return getResponse(Message.ACKOPEN_TYPE);
		    	}
		    	else if(msg.getType() == Message.CLOSE_TYPE) {
		    		if(direction == SENDING) {
		    			sendRequest(msg);
		    			state = INIT_STATE;
		    			//when we send close we do not wait for response just close and return 
		    			ctpResponse = new CTPResponse(Message.INVALID_TYPE);
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    		else {//received close msg
		    			state = INIT_STATE;
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    	}
		    	else if(msg.getType() == Message.ERROR_TYPE) {
	    			//received error msg
	    			if(direction == RECEIVING) {
	    				ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    	}
	    		else {//invalid or bad type should never be reached
	    			//error case from client user
	    			if(direction == SENDING) {
	    				ctpResponse = new CTPResponse(Message.ERROR_TYPE,INVALID);
	    				return ctpResponse;
	    			}
	    			else {//error case from server		
	    				Error errMsg = new Error(Error.BAD_MESSAGE_ERR);
	    				sendRequest(errMsg);
	    				ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
	    			}
	    		}
		    } 
	    	case OPENING_STATE: {	
	    		if (msg.getType() == Message.ACKOPEN_TYPE) {
	    			state = OPEN_STATE;
	    			ctpResponse.LoadReponse(msg);
    				return ctpResponse;
	    		}
	    		else if(msg.getType() == Message.CLOSE_TYPE) {
	    			if(direction == SENDING) {
		    			sendRequest(msg);
		    			state = INIT_STATE;
		    			//when we send close we do not wait for response just close and return 
		    			ctpResponse = new CTPResponse(Message.INVALID_TYPE);
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    		else {//received close msg
		    			state = INIT_STATE;
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    	}
	    		else if(msg.getType() == Message.ERROR_TYPE) {
	    			//received error msg
	    			if(direction == RECEIVING) {
	    				ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    	}
	    		else {//invalid or bad type should never be reached
	    			//error case from client user
	    			if(direction == SENDING) {
	    				ctpResponse = new CTPResponse(Message.ERROR_TYPE,INVALID);
	    				return ctpResponse;
	    			}
	    			else {//error case from server		
	    				Error errMsg = new Error(Error.BAD_MESSAGE_ERR);
	    				sendRequest(errMsg);
	    				ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
	    			}
	    		}	
	        }
	    	case OPEN_STATE: {	
		    	if (msg.getType() == Message.REQLOCK_TYPE) {
		    		sendRequest(msg);
		    		state = WAIT_LOCK_STATE;
	    			return getResponse(Message.ACKLOCK_TYPE);
		    	}
		    	else if(msg.getType() == Message.CLOSE_TYPE) {
		    		if(direction == SENDING) {
		    			sendRequest(msg);
		    			state = INIT_STATE;
		    			//when we send close we do not wait for response just close and return 
		    			ctpResponse = new CTPResponse(Message.INVALID_TYPE);
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    		else {//received close msg
		    			state = INIT_STATE;
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    	}
		    	else if(msg.getType() == Message.MOVE_TYPE) {
		    		if(direction == SENDING) {
		    			//when we send move we do not wait for response just return 
		    			sendRequest(msg);
		    			ctpResponse = new CTPResponse(Message.INVALID_TYPE);
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    		else { //received move msg
		    			ctpResponse.LoadReponse(msg);
    			    	return ctpResponse;
		    		}
		    	}
		    	else if(msg.getType() == Message.REQCONTENTS_TYPE) {
		    		sendRequest(msg);
		    		return getResponse(Message.CONTENTS_TYPE);
		    	}
		    	else if(msg.getType() == Message.CONTENTS_TYPE) {
		    		ctpResponse.LoadReponse(msg);
    				return ctpResponse;
		    	}
		    	else if(msg.getType() == Message.STATUS_TYPE) {
		    		ctpResponse.LoadReponse(msg);
    				return ctpResponse;
		    	}
		    	else if(msg.getType() == Message.SYNC_TYPE) {
		    		sendRequest(msg);
		    		return getResponse(Message.STATUS_TYPE);
		    	}
		    	else if(msg.getType() == Message.EDIT_TYPE) {
		    		//do not have lock this is invalid from client user
	        		if(direction == SENDING) {
	        			ctpResponse = new CTPResponse(Message.ERROR_TYPE,INVALID);
    				    return ctpResponse;
	        		}
		    		else{ //received edit message from server
		    			ctpResponse.LoadReponse(msg);
    				    return ctpResponse;
		    		}
		    	}
		    	else if(msg.getType() == Message.ERROR_TYPE) {
	    			//received error msg
	    			if(direction == RECEIVING) {
	    				ctpResponse = new CTPResponse(Message.INVALID_TYPE);
	    				ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    	}
	    		else {//invalid or bad type should never be reached
	    			//error case from client user
	    			if(direction == SENDING) {
	    				ctpResponse = new CTPResponse(Message.ERROR_TYPE,INVALID);
	    				return ctpResponse;
	    			}
	    			else {//error case from server		
	    				Error errMsg = new Error(Error.BAD_MESSAGE_ERR);
	    				sendRequest(errMsg);
	    				ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
	    			}
	    		}

		    } 
	        case WAIT_LOCK_STATE: {	
	        	if (msg.getType() == Message.ACKLOCK_TYPE) {
	        		state = LOCKED_STATE;
	        		ctpResponse.LoadReponse(msg);
    				return ctpResponse;
		    	}
	        	else if(msg.getType() == Message.CLOSE_TYPE) {
	        		if(direction == SENDING) {
		    			sendRequest(msg);
		    			state = INIT_STATE;
		    			//when we send close we do not wait for response just close and return 
		    			ctpResponse = new CTPResponse(Message.INVALID_TYPE);
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    		else {//received close msg
		    			state = INIT_STATE;
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    	}
	        	else if(msg.getType() == Message.MOVE_TYPE) {
	        		if(direction == SENDING) {
		    			//when we send move we do not wait for response just return 
		    			sendRequest(msg);
		    			ctpResponse = new CTPResponse(Message.INVALID_TYPE);
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    		else { //received move msg
		    			ctpResponse.LoadReponse(msg);
    			    	return ctpResponse;
		    		}
		    	}
	        	else if(msg.getType() == Message.REQCONTENTS_TYPE) {
	        		sendRequest(msg);
		    		return getResponse(Message.CONTENTS_TYPE);
		    	}
		    	else if(msg.getType() == Message.CONTENTS_TYPE) {
		    		ctpResponse.LoadReponse(msg);
    				return ctpResponse;
		    	}
		    	else if(msg.getType() == Message.SYNC_TYPE) {
		    		sendRequest(msg);
		    		return getResponse(Message.STATUS_TYPE);
		    	}
		    	else if(msg.getType() == Message.STATUS_TYPE) {
		    		ctpResponse.LoadReponse(msg);
    				return ctpResponse;
		    	}
		    	else if(msg.getType() == Message.EDIT_TYPE) {
		    		//do not have lock this is invalid from client user
	        		if(direction == SENDING) {
	        			ctpResponse = new CTPResponse(Message.ERROR_TYPE,INVALID);
    				    return ctpResponse;
	        		}
		    		else{ //received edit message from server
		    			ctpResponse.LoadReponse(msg);
    				    return ctpResponse;
		    		}
		    	}
		    	else if(msg.getType() == Message.ERROR_TYPE) {
	    			//received error msg
	    			if(direction == RECEIVING) {
	    				ctpResponse = new CTPResponse(Message.INVALID_TYPE);
	    				ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    	}
	    		else {//invalid or bad type should never be reached
	    			//error case from client user
	    			if(direction == SENDING) {
	    				ctpResponse = new CTPResponse(Message.ERROR_TYPE,INVALID);
	    				return ctpResponse;
	    			}
	    			else {//error case from server		
	    				Error errMsg = new Error(Error.BAD_MESSAGE_ERR);
	    				sendRequest(errMsg);
	    				ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
	    			}
	    		}	
		    }
	        case LOCKED_STATE: {	
	        	if (msg.getType() == Message.RELEASE_TYPE) {
	        		sendRequest(msg);
	        		return getResponse(Message.SERVRELEASE_TYPE);
		    	}
	        	else if(msg.getType() == Message.CLOSE_TYPE) {
	        		if(direction == SENDING) {
		    			sendRequest(msg);
		    			state = INIT_STATE;
		    			//when we send close we do not wait for response just close and return 
		    			ctpResponse = new CTPResponse(Message.INVALID_TYPE);
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    		else {//received close msg
		    			state = INIT_STATE;
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    	}
	        	else if (msg.getType() == Message.SERVRELEASE_TYPE) {
	        		state = OPEN_STATE;
	        		ctpResponse.LoadReponse(msg);
    				return ctpResponse;
	        	}
	        	else if(msg.getType() == Message.MOVE_TYPE) {
	        		if(direction == SENDING) {
		    			//when we send move we do not wait for response just return 
		    			sendRequest(msg);
		    			ctpResponse = new CTPResponse(Message.INVALID_TYPE);
		    			ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    		else { //received move msg
		    			ctpResponse.LoadReponse(msg);
    			    	return ctpResponse;
		    		}
		    	}
	        	else if(msg.getType() == Message.SYNC_TYPE) {
	        		sendRequest(msg);
		    		return getResponse(Message.STATUS_TYPE);
		    	}
	        	else if(msg.getType() == Message.EDIT_TYPE) {
	        		//we have lock 
	        		if(direction == SENDING) {
	        			sendRequest(msg);
			    		return getResponse(Message.ACKLOCK_TYPE);
	        		}
		    		else{ //received edit message from server
		    			ctpResponse.LoadReponse(msg);
    				    return ctpResponse;
		    		}
		    	}
	        	else if (msg.getType() == Message.ACKEDIT_TYPE) {
	        		ctpResponse.LoadReponse(msg);
				    return ctpResponse;
	        	}
	        	else if(msg.getType() == Message.STATUS_TYPE) {
	        		ctpResponse.LoadReponse(msg);
				    return ctpResponse;
		    	}
	        	else if(msg.getType() == Message.ERROR_TYPE) {
	    			//received error msg
	    			if(direction == RECEIVING) {
	    				ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
		    		}
		    	}
	    		else {//invalid or bad type should never be reached
	    			//error case from client user
	    			if(direction == SENDING) {
	    				ctpResponse = new CTPResponse(Message.ERROR_TYPE,INVALID);
	    				return ctpResponse;
	    			}
	    			else {//error case from server		
	    				Error errMsg = new Error(Error.BAD_MESSAGE_ERR);
	    				sendRequest(errMsg);
	    				ctpResponse.LoadReponse(msg);
	    				return ctpResponse;
	    			}
	    		}
		    }
	        default:
	        {//should never be reached Invalid State 
	    		System.out.println("Invalid State in Client");
	    		ctpResponse = new CTPResponse(Message.ERROR_TYPE,INVALID);
				return ctpResponse;
	    	}			
	    }//end switch
    }
    
    
    /** 
     * Method sends message to server
     */
    void sendRequest(Message msg)
    {
    	transport.sendRequest(msg.getbyteArray());
    	
    }
    
    /** 
     * Method waits for a message from the server
     */
    CTPResponse getResponse(int msgType)
    {
    	//store the response type expected
		ctpResponse = new CTPResponse(msgType);
		Message msg;
        //there are async messages also so keep pulling messages until
    	//we get the response to our request, a close, or an error 
    	do { 
    		byte[] buff = transport.getResponse(); 
    		//check that we got something
    		if (buff.length != 0) {
	    		msg = Message.makeMessage(buff);
	    		Process(RECEIVING, msg);
    		}
    		else //this is the case where we have heard nothing back from server
    			//throw a response up to user
    		{
    			ctpResponse = new CTPResponse(Message.ERROR_TYPE,NO_RESPONSE);
    			return ctpResponse;
    		}
    			
    	} while ((msg.getType() != ctpResponse.getWaitResponseType())&&
    			(msg.getType() != Message.CLOSE_TYPE)                &&
    			(msg.getType() != Message.ERROR_TYPE));
    	return ctpResponse;
    }
    
    
    //These are the states for the state machine that match the document 
    private static final short INIT_STATE = 1;
    private static final short VERSION_STATE = 2; 
  	private static final short CONNECTED_STATE = 3;
	private static final short OPENING_STATE = 4;
	private static final short OPEN_STATE = 5;
	private static final short WAIT_LOCK_STATE = 6;
    private static final short LOCKED_STATE = 7;
}
    
