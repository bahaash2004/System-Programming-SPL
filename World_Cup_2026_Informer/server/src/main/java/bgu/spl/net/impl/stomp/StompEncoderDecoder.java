package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageEncoderDecoder;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

public class StompEncoderDecoder implements MessageEncoderDecoder<String> {
private byte[] bytes = new byte[1 << 10]; // start with 1k
private int len = 0;

    @Override
    public String decodeNextByte(byte nextByte) {
        if (nextByte == '\u0000') {
            return popString();
        }

        pushByte(nextByte);
        return null;
        
    }

    @Override
    public byte[] encode(String message) {
        return (message + "\u0000").getBytes(StandardCharsets.UTF_8);
    }

    private void pushByte(byte nextByte) {
        if (len >= bytes.length) {
            bytes = Arrays.copyOf(bytes, len * 2);
        }
        bytes[len++] = nextByte;
    }

    private String popString() {
        String result = new String(bytes, 0, len, StandardCharsets.UTF_8);
        len = 0;
        return result;
    }
    
}