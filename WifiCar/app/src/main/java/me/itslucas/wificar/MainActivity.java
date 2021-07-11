//Author u18301012 2019.07.09
//WifiCar app based on Android
package me.itslucas.wificar;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.view.View;
import android.view.WindowManager;
import android.webkit.WebView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

public class MainActivity extends AppCompatActivity {

    public boolean connected = false;
    //Widget declare
    public Button upButton;
    public Button backButton;
    public Button leftButton;
    public Button rightButton;
    public Button stopButton;
    public Button connectButton;
    public Button disConnectButton;
    public Button cameraUp;
    public Button cameraDown;
    public Button cameraLeft;
    public Button cameraRight;
    public Button cameraReset;
    public Button autoDrive;
    public Button takePhoto;
    public Button gasButton;
    public Button brakeButton;
    public Button ringButton;
    public Button photoButton;
    //Speed bar, to show the speed
    public ProgressBar speedBar;
    //Status texts
    public TextView ipView;
    public TextView portView;
    public TextView statusView;
    //public TextView packetView;
    //Stream view
    public WebView streamView;
    //connect informations
    public EditText ipEdit;
    public EditText portEdit;

    /**
     * send message
     *
     * @param msg message
     */
    public void sendMessage(String msg) {
        //packetView.setText(msg);
        TCPClient.sharedCenter().send(msg.getBytes());
    }
    /**
     * connect to server
     */
    public void connect() {
        Toast.makeText(MainActivity.this,
                ipEdit.getText().toString() + portEdit.getText().toString(),
                Toast.LENGTH_SHORT).show();
        TCPClient.sharedCenter().connect(ipEdit.getText().toString(),
                Integer.parseInt(portEdit.getText().toString()));
        connected = true;
    }
    /**
     * disconnect from server
     */
    public void disconnect() {
        TCPClient.sharedCenter().disconnect();
    }

    /**
     * save image to gallery and external storage
     *
     * @param bmp the bitmap of the image
     */
    private void saveImageToPhotos(Bitmap bmp) {
        File appDir = new File(Environment.getExternalStorageDirectory(), "WifiCar");
        if (!appDir.exists()) {
            appDir.mkdir();
        }
        String fileName = System.currentTimeMillis() + ".jpg";
        File file = new File(appDir, fileName);
        try {
            FileOutputStream fos = new FileOutputStream(file);
            bmp.compress(Bitmap.CompressFormat.JPEG, 100, fos);
            fos.flush();
            fos.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        try {
            MediaStore.Images.Media.insertImage(MainActivity.this.getApplicationContext().getContentResolver(),
                    file.getAbsolutePath(), fileName, null);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return;
        }
        Intent intent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE);
        Uri uri = Uri.fromFile(file);
        intent.setData(uri);
        MainActivity.this.getApplicationContext().sendBroadcast(intent);
    }
    /**
     * hide bottom nav ui to avoid strange issues
     */
    protected void hideBottomUI() {
        if(Build.VERSION.SDK_INT>=19) {
            View decorView = getWindow().getDecorView();
            int uiOptions = View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_IMMERSIVE;
            decorView.setSystemUiVisibility(uiOptions);
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION);
        }
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TCPClient.sharedCenter().setConnectedCallback(new TCPClient.OnServerConnectedCallbackBlock() {
            @Override
            public void callback() {
                statusView.setText("Connected");
            }
        });
        TCPClient.sharedCenter().setDisconnectedCallback(new TCPClient.OnServerDisconnectedCallbackBlock() {
            @Override
            public void callback(IOException e) {
                statusView.setText("Not Connected");
            }
        });
        hideBottomUI();
        String[] PERMISSIONS = {
                "android.permission.READ_EXTERNAL_STORAGE",
                "android.permission.WRITE_EXTERNAL_STORAGE"
        };
        int permission = ContextCompat.checkSelfPermission(this,
                "android.permission.WRITE_EXTERNAL_STORAGE");
        if(permission != PackageManager.PERMISSION_GRANTED){
            ActivityCompat.requestPermissions(this,PERMISSIONS,1);
        }
        upButton = findViewById(R.id.upButton);
        upButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("F");
                }
            }
        });
        backButton = findViewById(R.id.backButton);
        backButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("B");
                }
            }
        });
        leftButton = findViewById(R.id.leftButton);
        leftButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("L");
                }
            }
        });
        rightButton = findViewById(R.id.rightButton);
        rightButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("R");
                }
            }
        });
        stopButton = findViewById(R.id.stopButton);
        stopButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("S");
                }
            }
        });
        connectButton = findViewById(R.id.connectButton);
        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                connect();
                streamView.loadUrl("http://" +
                        ipEdit.getText().toString() +
                        ":" +
                        "8080" +
                        "/stream_simple.html");
            }
        });
        disConnectButton = findViewById(R.id.disConnectButton);
        disConnectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                disconnect();
            }
        });
        cameraDown = findViewById(R.id.cameraDown);
        cameraDown.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("MO2D");
                }
            }
        });
        cameraUp = findViewById(R.id.cameraUp);
        cameraUp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("MO2U");
                }
            }
        });
        cameraLeft = findViewById(R.id.cameraLeft);
        cameraLeft.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("MO1U");
                }
            }
        });
        cameraRight = findViewById(R.id.cameraRight);
        cameraRight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("MO1D");
                }
            }
        });
        cameraReset = findViewById(R.id.cameraReset);
        cameraReset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("MOK");
                }
            }
        });
        autoDrive = findViewById(R.id.autoDrive);
        autoDrive.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("AUTO");
                }
            }
        });
        streamView = findViewById(R.id.streamView);
        takePhoto = findViewById(R.id.photoButton);
        takePhoto.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Toast.makeText(MainActivity.this,
                        "Work in progress",
                        Toast.LENGTH_SHORT).show();
            }
        });
        ringButton = findViewById(R.id.bellButton);
        ringButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("BELL");
                }
            }
        });
        speedBar = findViewById(R.id.speedBar);
        gasButton = findViewById(R.id.gasButton);
        gasButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("CU");
                    speedBar.setProgress(speedBar.getProgress()+10);
                    if(speedBar.getProgress() > 70) {
                        Toast.makeText(MainActivity.this,
                                "Speed can't be more than 70",
                                Toast.LENGTH_SHORT).show();
                        speedBar.setProgress(70);
                    }
                }
            }
        });
        brakeButton = findViewById(R.id.brakeButton);
        brakeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!connected) {
                    Toast.makeText(MainActivity.this,
                            "Not connected to server!",
                            Toast.LENGTH_SHORT).show();
                } else {
                    sendMessage("CD");
                    speedBar.setProgress(speedBar.getProgress()-10);
                    if(speedBar.getProgress() < 0) {
                        Toast.makeText(MainActivity.this,
                                "Speed can't be less than 0",
                                Toast.LENGTH_SHORT).show();
                        speedBar.setProgress(0);
                    }
                }
            }
        });
        photoButton = findViewById(R.id.photoButton);
        photoButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        Bitmap bp = returnBitMap("http://192.168.1.1:8080/?action=snapshot");
                        saveImageToPhotos(bp);
                    }
                }).start();
            }
        });
        ipView = findViewById(R.id.ipView);
        portView = findViewById(R.id.portView);
        statusView = findViewById(R.id.statusView);
        //packetView = findViewById(R.id.packetView);
        ipEdit = findViewById(R.id.ipEdit);
        portEdit = findViewById(R.id.portEdit);
    }
    /**
     * return img bitmap of the given url
     *
     * @param url image url
     */
    public final static Bitmap returnBitMap(String url) {
        URL myFileUrl;
        Bitmap bitmap = null;
        try {
            myFileUrl = new URL(url);
            HttpURLConnection conn;
            conn = (HttpURLConnection) myFileUrl.openConnection();
            conn.setDoInput(true);
            conn.connect();
            InputStream is = conn.getInputStream();
            bitmap = BitmapFactory.decodeStream(is);
        } catch (MalformedURLException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return bitmap;
    }
}
