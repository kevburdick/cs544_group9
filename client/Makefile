JC = javac

.SUFFIXES: .java .class
.java.class:
	$(JC) $*.java

CLASSES = \
		Client.java \
		Transport.java \
      CTP.java \
		CTPResponse.java \
		StateMachine.java \
		ResponseTimer.java \
      message/Ack.java \
		message/AckConnect.java \
      message/Close.java \
      message/Connect.java \
      message/Contents.java \
      message/Edit.java \
      message/Error.java \
		message/Message.java \
      message/Move.java \
      message/Open.java \
		message/Release.java \
		message/ReqLock.java \
		message/ReqContents.java \
      message/Status.java \
      message/Sync.java \
		test/TCPSock.java \
		test/Server.java   
        
classes: $(CLASSES:.java=.class)

clean:
	$(RM) *.class
	$(RM) ./*/*.class

