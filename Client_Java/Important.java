/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package client;

/**
 *
 * @author QUANG
 * 
 */
import java.io.*;

public class Important {
    public static void sendAcceptMessage(DataOutputStream out) throws IOException {
        int type=1;
        byte[] bytes = Convert.intToBytes(type);
        out.write(bytes);
        out.flush();
    }
    
    public static void sendListFileMessage(DataOutputStream out,String path, int port) throws IOException {
        int type = 2;
        byte[] byte_type = Convert.intToBytes(type);
        out.write(byte_type);
        out.flush();
        File dir = new File(path);
        File[] listFile = dir.listFiles();
        byte[] byte_port = Convert.longToBytes(port);
        out.write(byte_port);
        byte[] numberFile = Convert.intToBytes(listFile.length);
        out.write(numberFile);
        out.flush();
        for(File file:listFile){
            byte[] fileNameLength = Convert.intToBytes(file.getName().length());
            out.write(fileNameLength);
            out.flush();
            byte[] fileName = file.getName().getBytes();
            out.write(fileName, 0, file.getName().length());
            out.flush();
            byte[] fileSize = Convert.longToBytes(file.length());
            out.write(fileSize);
            out.flush();
        }
    }
    
    public static void sendRequestDownloadMess(DataOutputStream out,String fileName) throws IOException {
        int type = 6;
        byte[] byte_type = Convert.intToBytes(type);
        out.write(byte_type);
        out.flush();
        byte[] fileNameLength = Convert.intToBytes(fileName.length());
        out.write(fileNameLength);
        out.flush();
        System.out.println("file name:"+fileName);
        byte[] name = fileName.getBytes();
        out.write(name, 0, fileName.length());
        out.flush();
    }
    
    public static void sendRequestToClientMess(DataOutputStream out, String fileName) throws IOException {
        int type = 5;
        out.writeInt(type);
        out.flush();
        out.writeUTF(fileName);
        out.flush();
    }
    
    public static void sendFile(DataOutputStream out,File file,int BUFSIZE) throws IOException {
        byte[] buffer = new byte[BUFSIZE];
        int type = 3;
        out.writeInt(type);
        long sizeToRead = file.length();
        BufferedInputStream bis = new BufferedInputStream(new FileInputStream(file));
        while(sizeToRead>0){
            if(sizeToRead>BUFSIZE){
                bis.read(buffer, 0, BUFSIZE);
                out.write(buffer, 0, BUFSIZE);
                out.flush();
                sizeToRead-=BUFSIZE; 
            }else{
                bis.read(buffer, 0, (int)sizeToRead);
                out.write(buffer, 0, (int)sizeToRead);
                out.flush();
                sizeToRead-=sizeToRead;
            }
        }
        bis.close();
    }
    
    public static boolean recvFile(DataInputStream in,String path,long fileSize,int BUFSIZE) throws FileNotFoundException, IOException{
        File file = new File(path);
        BufferedOutputStream bos =new BufferedOutputStream(new FileOutputStream(file));
        byte[] buffer =new byte[BUFSIZE];
        while(fileSize>0){
            if(fileSize>BUFSIZE){
                in.read(buffer, 0, BUFSIZE);
                bos.write(buffer, 0, BUFSIZE);
                bos.flush();
                fileSize-=BUFSIZE;
            }else{
                in.read(buffer, 0, (int)fileSize);
                bos.write(buffer, 0, (int) fileSize);
                bos.flush();
                fileSize-=fileSize;
            }
        }
        bos.close();
        return fileSize==0;
    }
    
    public static int readType(DataInputStream in) throws IOException{
        int type;
        byte[] bytes = new byte[4];
        in.read(bytes);
        type=Convert.bytesToInt(bytes);
        return type;
    }
}
