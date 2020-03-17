package com.example.bleblinktcolor;

import androidx.appcompat.app.AppCompatActivity;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.widget.SeekBar;
import android.widget.TextView;

import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    private final static String TAG = MainActivity.class.getSimpleName();

    private BluetoothLeScanner scanner;
    private BluetoothDevice device;
    private BluetoothGatt blegatt;
    private BluetoothGattCharacteristic characteristic;
    private boolean running = false;
    private boolean founddevice = false;

    // These are hardcoded from the gatt service on the Pi Zero
    // TODO change UUIDs and get better strategy on how to get hold of the device
    private final static UUID UUID_COLOR_SERVICE = UUID.fromString("00001802-0000-1000-8000-00805f9b34fb");
    private final static UUID UUID_COLOR_CHARACTERISTIC = UUID.fromString("00002a06-0000-1000-8000-00805f9b34fb");
    private final static UUID UUID_COLOR_DESCR = UUID.fromString("8260c653-1a54-426b-9e36-e84c238bc669");
    private final static int REQUEST_ENABLE_BT = 1;

    private TextView textView;
    private int curr_red = 0x00;
    private int curr_green = 0xFF;
    private int curr_blue = 0x00;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        BluetoothManager manager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        BluetoothAdapter adapter = manager.getAdapter();
        scanner = adapter.getBluetoothLeScanner();

        if (!adapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        }
        if (!running) {
            Log.i(TAG, "startScanning");
            startScanning();
            running = true;
        }

        textView = findViewById(R.id.textView);
        textView.setBackgroundColor((0xff) << 24 | 0xff << 16);

        SeekBar sBarR = findViewById(R.id.seekBarR);
        sBarR.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            int pval = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                pval = progress;
                curr_red = pval;
                setCol();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                Log.d(TAG, "ValR: " + pval);
            }
        });
        SeekBar sBarG = findViewById(R.id.seekBarG);
        sBarG.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            int pval = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                pval = progress;
                curr_green = pval;
                setCol();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                Log.d(TAG, "ValG: " + pval);
            }
        });
        SeekBar sBarB = findViewById(R.id.seekBarB);
        sBarB.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            int pval = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                pval = progress;
                curr_blue = pval;
                setCol();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                Log.d(TAG, "ValB: " + pval);
            }
        });
    }

    private final BluetoothGattCallback gattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.i(TAG, "Connected to GATT server.");
                // Attempts to discover services after successful connection.
                Log.i(TAG, "Attempting to start service discovery:" +
                        blegatt.discoverServices());
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                Log.i(TAG, "Disconnected from GATT server.");
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            BluetoothGattService service = gatt.getService(UUID_COLOR_SERVICE);

            if (service != null) {
                Log.i(TAG, "Found service" + service.toString());
                characteristic = service.getCharacteristic(UUID_COLOR_CHARACTERISTIC);
                Log.i(TAG, "Found characteristic" + characteristic.toString());
            }
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt,
                                         BluetoothGattCharacteristic characteristic,
                                         int status) {
            Log.i(TAG, "onCharacteristicRead");
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt,
                                          BluetoothGattCharacteristic characteristic,
                                          int status) {
            Log.i(TAG, "onCharacteristicWrite");
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt,
                                            BluetoothGattCharacteristic characteristic) {
            Log.i(TAG, "onCharacteristicChanged");
        }
    };

    private final ScanCallback leScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            if (result.getDevice().getName() != null) {
                if (result.getDevice().getName().equals("raspberryp")) {
                    Log.d(TAG, "Found raspberries ");
                    device = result.getDevice();
                    setDevice();
                    stopScanning();
                    setCol();
                }
            }
        }
    };

    private void startScanning() {
        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                scanner.startScan(leScanCallback);
            }
        });
    }

    private void stopScanning() {
        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                scanner.stopScan(leScanCallback);
            }
        });
    }

    private void setDevice() {
        blegatt = device.connectGatt(this, false, gattCallback);
        Log.i(TAG, "Found Device" + blegatt.toString());
        founddevice = true;
    }

    private byte[] color2Byte(int r, int g, int b) {
        return new byte[]{(byte) (7), (byte) (r), (byte) (g), (byte) (b),};
    }

    private void setCol() {
        if (founddevice) {
            textView.setBackgroundColor((0xff) << 24 | (curr_red & 0xff) << 16 | (curr_green & 0xff) << 8 | (curr_blue & 0xff));
            if (characteristic != null) {
                BluetoothGattDescriptor desc = characteristic.getDescriptor(UUID_COLOR_DESCR);
                desc.setValue(color2Byte(curr_red, curr_green, curr_blue));
                blegatt.writeDescriptor(desc);
            }
        } else
            textView.setBackgroundColor((0xff) << 24 | 0xff << 16);
    }
}
