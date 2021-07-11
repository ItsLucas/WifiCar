//Author u18301012 2019.07.08
//Simple package of TCP connection class
package me.itslucas.wificar;

import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.charset.StandardCharsets;

public class TCPClient {
    private static final String TAG = "TCPClient";
    private static TCPClient instance;
    private Socket socket;
    private String ipAddress;
    private int port;
    private Thread thread;
    private OutputStream outputStream;
    private InputStream inputStream;
    private OnServerConnectedCallbackBlock connectedCallback;
    private OnServerDisconnectedCallbackBlock disconnectedCallback;
    private OnReceiveCallbackBlock receivedCallback;

    private TCPClient() {
        super();
    }

    public static TCPClient sharedCenter() {
        if (instance == null) {
            synchronized (TCPClient.class) {
                if (instance == null) {
                    instance = new TCPClient();
                }
            }
        }
        return instance;
    }

    /**
     * Connect via ip and port
     *
     * @param ipAddress ip
     * @param port      port
     */
    public void connect(final String ipAddress, final int port) {

        thread = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    socket = new Socket(ipAddress, port);
                    if (isConnected()) {
                        TCPClient.sharedCenter().ipAddress = ipAddress;
                        TCPClient.sharedCenter().port = port;
                        if (connectedCallback != null) {
                            connectedCallback.callback();
                        }
                        outputStream = socket.getOutputStream();
                        inputStream = socket.getInputStream();
                        receive();
                        Log.i(TAG, "连接成功");
                    } else {
                        Log.i(TAG, "连接失败");
                        if (disconnectedCallback != null) {
                            disconnectedCallback.callback(new IOException("连接失败"));
                        }
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                    Log.e(TAG, "连接异常");
                    if (disconnectedCallback != null) {
                        disconnectedCallback.callback(e);
                    }
                }
            }
        });
        thread.start();
    }

    /**
     * is it connected
     */
    public boolean isConnected() {
        return socket.isConnected();
    }

    /**
     * connect
     */
    public void connect() {
        connect(ipAddress, port);
    }

    /**
     * disconnect
     */
    public void disconnect() {
        if (isConnected()) {
            try {
                if (outputStream != null) {
                    outputStream.close();
                }
                socket.close();
                if (socket.isClosed()) {
                    if (disconnectedCallback != null) {
                        disconnectedCallback.callback(new IOException("断开连接"));
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * recieve
     */
    public void receive() {
        while (isConnected()) {
            try {
                /**resolv*/
                byte[] bt = new byte[1024];
//                get bytes,count
                int length = inputStream.read(bt);
//                get correct bytes
                byte[] bs = new byte[length];
                System.arraycopy(bt, 0, bs, 0, length);

                String str = new String(bs, StandardCharsets.UTF_8);
                if (str != null) {
                    if (receivedCallback != null) {
                        receivedCallback.callback(str);
                    }
                }
                Log.i(TAG, "接收成功");
            } catch (IOException e) {
                Log.i(TAG, "接收失败");
            }
        }
    }

    /**
     * 发送数据
     *
     * @param data 数据
     */
    public void send(final byte[] data) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                if (socket != null) {
                    try {
                        outputStream.write(data);
                        outputStream.flush();
                        Log.i(TAG, "发送成功");
                    } catch (IOException e) {
                        e.printStackTrace();
                        Log.i(TAG, "发送失败");
                    }
                } else {
                    connect();
                }
            }
        }).start();

    }

    public void setConnectedCallback(OnServerConnectedCallbackBlock connectedCallback) {
        this.connectedCallback = connectedCallback;
    }

    public void setDisconnectedCallback(OnServerDisconnectedCallbackBlock disconnectedCallback) {
        this.disconnectedCallback = disconnectedCallback;
    }

    public void setReceivedCallback(OnReceiveCallbackBlock receivedCallback) {
        this.receivedCallback = receivedCallback;
    }

    private void removeCallback() {
        connectedCallback = null;
        disconnectedCallback = null;
        receivedCallback = null;
    }

    public interface OnServerConnectedCallbackBlock {
        void callback();
    }

    public interface OnServerDisconnectedCallbackBlock {
        void callback(IOException e);
    }

    public interface OnReceiveCallbackBlock {
        void callback(String receicedMessage);
    }
}