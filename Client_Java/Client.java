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
import java.net.*;
import java.io.*;
import java.util.ArrayList;
import java.util.Scanner;

class ConnectThread extends Thread {

    static final int BUFSIZE = 8192;
//    private Socket conn;
    private DataInputStream in;
    private DataOutputStream out;
    int port;
    int type;
    private final String path = "E:\\Client\\Client\\ListFile";
    
    public ConnectThread(int port) {
        this.port = port;
    }

    @Override
    public void run() {
        byte[] socketBuffer = new byte[BUFSIZE];
        try {
            InetAddress addr = InetAddress.getByName("127.0.0.1");
            System.out.println("addr:"+addr);
            Socket socket = new Socket(addr, 6789);
//            System.out.println("socket: "+ socket);
            in = new DataInputStream(socket.getInputStream());
            out = new DataOutputStream(socket.getOutputStream());
            while (true) {
                type = Important.readType(in);
                if (type == 1) {
                    do {
                        Important.sendListFileMessage(out, path, port);
                        type = Important.readType(in);
                    } while (type != 1);
                    String fileName;
                    do {
                        System.out.println("Enter file name to download or 'QUIT' to close the process:");
                        Scanner sc = new Scanner(System.in);
                        long start = System.currentTimeMillis();
                        fileName = sc.nextLine();
                        if (!fileName.equalsIgnoreCase("QUIT")) {
                            ArrayList<Long> ports = new ArrayList<Long>();
                            ArrayList<InetAddress> ipAddrs = new ArrayList<InetAddress>();
                            Important.sendRequestDownloadMess(out, fileName);
                            type = Important.readType(in);
                            if (type == 4) {
                                byte[] longBuff = new byte[8];
                                in.read(longBuff, 0, 8);
                                long file_size = Convert.bytesToLong(longBuff);
                                byte[] intBuff = new byte[4];
                                in.read(intBuff, 0, 4);
                                int numberClients = Convert.bytesToInt(intBuff);
                                System.out.println("List client:");
                                for (int i = 0; i < numberClients; i++) {
                                    byte[] byteIpAddr = new byte[4];
                                    byte[] byte_port = new byte[8];
                                    in.read(byteIpAddr, 0, 4);
                                    in.read(byte_port, 0, 8);
                                    long port = Convert.bytesToLong(byte_port);
                                    InetAddress ip_addr = InetAddress.getByAddress(byteIpAddr);
                                    ports.add(port);
                                    ipAddrs.add(ip_addr);
                                    System.out.println("IP address: " + ip_addr.toString());
                                    System.out.println("port:"+port);

                                }
                                for (int i = 0; i < ipAddrs.size(); i++) {
                                    long port = ports.get(i);
                                    Socket downloadSocket = new Socket(ipAddrs.get(i), (int) port);
                                    DataInputStream inDownload = new DataInputStream(downloadSocket.getInputStream());
                                    DataOutputStream outDownload = new DataOutputStream(downloadSocket.getOutputStream());
                                    int type2;
                                    type2 = inDownload.readInt();
                                    if (type2 == 1) {
                                        Important.sendRequestToClientMess(outDownload, fileName);
                                        type2 = inDownload.readInt();
                                        if (type2 == 3) {
                                            boolean flag = Important.recvFile(inDownload,path+"/" + fileName, file_size, BUFSIZE);
                                            if(flag){
                                                long end = System.currentTimeMillis();
                                                long time = end - start;
                                                System.out.println("time for downloading file:"+time);
                                                type2 = 1;
                                                outDownload.writeInt(type2);

                                                outDownload.close();
                                                inDownload.close();
                                                downloadSocket.close();
                                                break;
                                            }
                                        }
                                    }
                                }
                                Important.sendListFileMessage(out, path, port);
                            }else if(type==7){
                                byte[] error_byte = new byte[4];
                                in.read(error_byte,0,4);
                                int errorCode = Convert.bytesToInt(error_byte);
                                if(errorCode==1){
                                    System.out.println("File not exist!");
                                }
                            }
                            
                        } else {
                            break;
                        }
                    } while (!fileName.equalsIgnoreCase("QUIT"));
                    System.out.println("Conection thread closed!");
                    break;
                } else {

                    System.out.println("Wrong type! " + type);
                }
                break;
            }
            in.close();
            out.close();
            socket.close();
        } catch (IOException ex) {}

    }

}

class ListenThread extends Thread {

    static final int BUFSIZE = 8192;
    private final Socket connSock;
    private DataInputStream in;
    private DataOutputStream out;

    public ListenThread(Socket socket) {
        this.connSock = socket;
    }

    @Override
    public void run() {
        byte[] buffer = new byte[BUFSIZE];
        try {
            in = new DataInputStream(connSock.getInputStream());
            out = new DataOutputStream(connSock.getOutputStream());
            int type;
            while (true) {
                out.writeInt(1);
                type = in.readInt();
                if (type == 5) {
                    String fileName = in.readUTF();
                    String path = ("E:\\Client\\Client\\ListFile" + fileName);
//                    System.out.println("path:"+path);
                    File file = new File(path);
                    if (file.exists()) {
                        Important.sendFile(out, file, BUFSIZE);
                        type = in.readInt();
                        if (type == 1) {
                            break;
                        }
                    }else{
                        type=7;
                        out.writeInt(type);
                        out.flush();
                        int errorCode = 1;
                        out.writeInt(errorCode);
                        out.flush();
                    }

                }
            }
            in.close();
            out.close();
            connSock.close();
        } catch (IOException ex) {}
    }

}

public class Client {

    public static void main(String[] args) {
        int port = 1098;
        Thread connThread = new ConnectThread(port);
        connThread.start();

        try {
            InetAddress addr = InetAddress.getByName("127.0.0.1");
            ServerSocket servSocket = new ServerSocket(port, 1, addr);
            System.out.println("Listening for connections on port " + servSocket.getLocalPort());
            while (true) {
                Socket conn = servSocket.accept();
                System.out.println("Connected with " + conn);
                Thread listenThread = new ListenThread(conn);
                listenThread.start();
            }
        } catch (IOException ex) {}
    }
    
}

