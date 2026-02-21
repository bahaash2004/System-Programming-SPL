package bgu.spl.net.impl.stomp;
import bgu.spl.net.srv.Server;
public class StompServer {

    public static void main(String[] args) {
        // TODO: implement this
        if (args.length < 2) {
            System.out.println("Usage: java StompServer <port> <server_type(tpc/reactor)>");
            System.exit(1);
            
        }

        int port = Integer.parseInt(args[0]);
        String serverType = args[1];

        if (serverType.equals("tpc")) {
            // Start thread-per-client server
            Server.threadPerClient(
                    port,
                    () -> new StompMessagingProtocolImpl(), //protocol factory
                    () -> new StompEncoderDecoder() //message encoder decoder factory
            ).serve();

        } else if (serverType.equals("reactor")) {
            // Start reactor server
            Server.reactor(
                    Runtime.getRuntime().availableProcessors(),
                    port,
                    () -> new StompMessagingProtocolImpl(), //protocol factory
                    () -> new StompEncoderDecoder() //message encoder decoder factory
            ).serve();
        } else {
            System.out.println("Unknown server type: " + serverType);
        }
    }
}