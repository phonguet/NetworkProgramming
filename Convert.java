/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package client;

/**
 *
 * @author QUANG
 */
public class Convert {
    public static byte[] longToBytes(long l) {
        byte[] result = new byte[8];
        byte[] b = new byte[]{
            (byte) l,
            (byte) (l >> 8),
            (byte) (l >> 16),
            (byte) (l >> 24),
            (byte) (l >> 32),
            (byte) (l >> 40),
            (byte) (l >> 48),
            (byte) (l >> 56)};
        return b;
    }

    public static long bytesToLong(byte[] b) {

        long l = ((long) b[7] << 56)
                | ((long) b[6] & 0xff) << 48
                | ((long) b[5] & 0xff) << 40
                | ((long) b[4] & 0xff) << 32
                | ((long) b[3] & 0xff) << 24
                | ((long) b[2] & 0xff) << 16
                | ((long) b[1] & 0xff) << 8
                | ((long) b[0] & 0xff);
        return l;
    }

    public static byte[] intToBytes(int i) {
        byte[] result = new byte[4];

        result[3] = (byte) (i >> 24);
        result[2] = (byte) (i >> 16);
        result[1] = (byte) (i >> 8);
        result[0] = (byte) (i /*>> 0*/);

        return result;
    }

    public static int bytesToInt(byte[] b) {
        int MASK = 0xFF;
        int result = 0;
        result = b[0] & MASK;
        result = result + ((b[1] & MASK) << 8);
        result = result + ((b[2] & MASK) << 16);
        result = result + ((b[3] & MASK) << 24);
        return result;
    }
}
